/* Copyright (c) 2020 Stijn Hinterding, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#include "qutag_mc_communicator.h"
#include "qutag_mc_inc/tdcbase.h"
#include "qutag_mc_inc/tdcmultidev.h"

#include <phast_gui/support/photon_event.h>

#include "chan_trigger_settings.h"

#include <iostream>

#define MAX_BUF_SIZE 1000000
#define EXPOSURE_TIME 100       // in ms

/// Initialize the communicator and close all existing device connections.
qutag_mc_communicator::qutag_mc_communicator(uint64_t buffer_size) :
    have_device(false),
    device_type(DEVTYPE_NONE),
    enabled_channels(),
    time_unit_seconds(0.0),
    timestamp_buffer_size(buffer_size),
    reset_buffer_after_event_retrieval(true),
    device_descriptor("None")
{
    if (buffer_size > MAX_BUF_SIZE)
        this->timestamp_buffer_size = MAX_BUF_SIZE;

    unsigned int temp = 0;
    TDC_discover(&temp);
}

/// Destruct the object after closing the open connection
qutag_mc_communicator::~qutag_mc_communicator()
{
    if (this->have_device)
        TDC_deInit();
}

/// Test if any device can be found by connecting and disconnecting any
/// available device, if none are connected already.
bool qutag_mc_communicator::AnyDeviceAvailable() const
{
    if (this->have_device)
        return true;

    // We pass -1 to detect any device
    int rc = TDC_init(-1);

    if (rc == TDC_Ok) {
        TDC_deInit();

        return true;
    }

    return false;
}

/// If no device is connected, connect to the device specified by the
/// `device_ID`, where `-1` means any device.  Sets the timestamp buffer size,
/// enables all channels, reads the timebase and (for quTAG HR) resets the sync
/// divider.  Returns true if a new device was connected or if there was a device
/// connected already. Returns false if the connection failed.
bool qutag_mc_communicator::TryToConnect(int64_t device_ID)
{
    if (this->have_device)
        return true;

    int rc = TDC_connect(device_ID);

    if (rc != TDC_Ok) {
        printf("Could not connect to device. Error code: %i\n", rc);
        this->have_device = false;
        return false;
    } else {
        this->have_device = true;
    }

    this->device_type = TDC_getDevType();

    switch (this->device_type) {
        case DEVTYPE_QUTAG_MC:
            this->device_descriptor = "quTAG MC";
            break;
        case DEVTYPE_QUTAG_HR:
            this->device_descriptor = "quTAG HR";
            break;
        case DEVTYPE_QUTAG_QTI:
            this->device_descriptor = "quTAG QTI";
            break;
        case DEVTYPE_NONE:
        default:
            this->device_descriptor = "None";
            this->have_device = false;
            TDC_deInit();
            return false;
            break;
    }

    TDC_setTimestampBufferSize(this->timestamp_buffer_size);

    // By default we enable ALL channels
    TDC_enableChannels(true, 0xFF);

    // We can also set a 'sync divider'. This tells
    // the timetagger box to skip every n-th event in
    // channel 0 (ALWAYS channel zero, cannot be done
    // on other channels). This can be useful when using
    // high laser pulse frequencies.
    //  For now, we disable this.
    if (this->device_type == DEVTYPE_QUTAG_HR) { // Is this supported on quTAG QTI?
        rc = TDC_configureSyncDivider(1, false);
    }

    // Get the time unit
    TDC_getTimebase(&this->time_unit_seconds);

    return true;
}

/// De-initialize the device
bool qutag_mc_communicator::Disconnect()
{
    TDC_deInit();
    this->have_device = false;

    return true;
}

/// Yes, this is a real device
bool qutag_mc_communicator::IsRealDevice() const
{
    return true;
}

/// Returns whether a device is connected and initialized
bool qutag_mc_communicator::ConnectedToDevice()
{
    return this->have_device;
}

/// Returns the number of devices that are connected to this instance of tdcbase
uint64_t qutag_mc_communicator::GetNumDevicesConnected()
{
    if (this->have_device)
        return 0;

    uint64_t ret = 0;
    TDC_discover((unsigned int*)&ret);

    return ret;
}

/// Returns the time unit (cq time tag bin size) of this device
double qutag_mc_communicator::TimeUnit() const
{
    double result = 0;

    TDC_getTimebase(&result);

    return result;
}

/// Return the name (device type) of this device
const std::string& qutag_mc_communicator::DeviceDescriptor() const
{
    return this->device_descriptor;
}

/// Set the channels enabled on the device according to the `enabled_channels`
/// member variable. The start channel is always enabled.
void qutag_mc_communicator::update_channels_enabled()
{
    int32_t channels_mask = 0;

    for (chan_id id : this->enabled_channels) {
        channels_mask &= id;
    }

    TDC_enableChannels(true, channels_mask);  // TODO Should enStart always be true?
}

/// Enable the specified channel. Channel 0 is the start channel.
void qutag_mc_communicator::EnableChannel(chan_id channel_id)
{
    for (chan_id id : this->enabled_channels) {
        if (id == channel_id)
            return;
    }

    this->enabled_channels.push_back(channel_id);
    this->update_channels_enabled();
}

/// Disable the specified channel. Channel 0 is the start channel.
void qutag_mc_communicator::DisableChannel(chan_id channel_id)
{
    bool found = false;
    uint64_t index = 0;

    for (uint64_t i = 0; i < this->enabled_channels.size(); i++) {
        if (this->enabled_channels.at(i) == channel_id) {
            found = true;
            index = i;
            break;
        }
    }

    if (!found)
        return;

    this->enabled_channels.erase(this->enabled_channels.begin() + index);
    this->update_channels_enabled();
}

/// The the specified channel to the specified state. Channel 0 is the start channel.
void qutag_mc_communicator::SetChannelEnabled(chan_id channel_id, bool enabled)
{
    return (enabled) ? this->EnableChannel(channel_id) : this->DisableChannel(channel_id);
}

/// quTAG HR and only channel 0 (start) are supported. Return the sync divider
/// rate on the specified channel. E.g. 8 means only every eighth time tag is
/// passed to the computer.
uint64_t qutag_mc_communicator::GetSyncDivider(chan_id channel_id)
{
    if (this->device_type != DEVTYPE_QUTAG_HR)
        return 1;

    if (channel_id != 0)
        return 1;

    Int32 divider = 1;
    Bln32 reconstruct = false;

    TDC_getSyncDivider(&divider, &reconstruct);

    return (uint64_t)divider;
}

/// quTAG HR only. Set the sync divider rate on the start channel. Only the
/// values 1, 2, 4 or 8 are allowed. E.g. 8 means only every eighth time tag is
/// passed to the computer.
uint64_t qutag_mc_communicator::UpdateSyncDivider(uint64_t value)
{
    if (this->device_type != DEVTYPE_QUTAG_HR)
        return 1;

    Int32 divider = value;

    TDC_configureSyncDivider(divider, false);

    return this->GetSyncDivider(0);
}

/// Return the trigger settings for the specified channel ID, where channel 0 is
/// the start channel.  Returns a `chan_trigger_settings` instance, including
/// the sync divider rate.
chan_trigger_settings qutag_mc_communicator::GetSignalConditioning(uint64_t chan_ID)
{
    Int32 chan = (Int32)chan_ID;
    Bln32 on = 0;
    Bln32 edge = 1;
    double threshold = 0;

    int error_val = TDC_getSignalConditioning(chan, &edge, &threshold);

    std::cout << "qutag mc getSignalConditioning error value: " << error_val << std::endl;

    chan_trigger_settings ret;
    ret.ID = chan_ID;
    ret.edge = (edge == 0) ? chan_trigger_settings::FALLING : chan_trigger_settings::RISING;
    ret.signal_conditioning_enabled = on;
    ret.voltage_threshold = threshold;

    ret.delay_time = 0;

    if (chan_ID != 0) {
        ret.sync_divider = 1;
    } else {
        ret.sync_divider = this->GetSyncDivider(0);

    }
    return ret;
}

/// Set the trigger settings for the specified channel. Trigger settings are
/// supplied through a `chan_trigger_settings` object. Returns the resulting
/// trigger settings as a new `chan_trigger_settings` object.
chan_trigger_settings qutag_mc_communicator::UpdateSignalConditioning(uint64_t chan_ID, chan_trigger_settings new_values)
{
    Bln32 edge = (new_values.edge == chan_trigger_settings::RISING) ? 1 : 0;
    double threshold = new_values.voltage_threshold;
    TDC_SignalCond cond = (new_values.signal_conditioning_enabled) ? SCOND_MISC : SCOND_LVTTL;
    Int32 channel = (Int32)chan_ID;

    TDC_configureSignalConditioning(channel, cond, edge, threshold);

    return this->GetSignalConditioning(chan_ID);
}

/// Return whether there has been any data loss.
bool qutag_mc_communicator::DataLossSinceLastCall()
{
    Bln32 ret = 1;
    Bln32 ret2 = 1;
    TDC_getDataLost(&ret);
    TDC_getDataLost(&ret2);

    if (ret == 0 && ret2 == 0)
        return false;

    return true;
}

/// Receive the latest timestamp data from the device and place the data into
/// the `timestamps` and `chan_IDs` out-parameters. These are resized to number
/// of returned time tags.  For maximum performance, ensure that the capacity of
/// these vectors is at least `timestamp_buffer_size`.
uint64_t qutag_mc_communicator::ReceiveData(std::vector<int64_t>* timestamps,
                                     std::vector<uint8_t>* chan_IDs)
{
    timestamps->resize(this->timestamp_buffer_size);
    chan_IDs->resize(this->timestamp_buffer_size);

    uint64_t num_valid_events = 0;

    TDC_getLastTimestamps(this->reset_buffer_after_event_retrieval,
                          (int64_t*)&(*timestamps)[0],
                          (Uint8*)&(*chan_IDs)[0],
                          (Int32*)&num_valid_events);

    timestamps->resize(num_valid_events);
    chan_IDs->resize(num_valid_events);

    return num_valid_events;
}
