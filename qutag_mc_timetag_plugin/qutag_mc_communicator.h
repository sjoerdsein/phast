/* Copyright (c) 2020 Stijn Hinterding, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#ifndef QUTAU_COMMUNICATOR_H
#define QUTAU_COMMUNICATOR_H

#include <vector>
#include <stdint.h>

#include <phast_gui/support/photon_event.h>
#include <phast_gui/interfaces/itimetaggercommunicator.h>

#include "qutag_mc_inc/tdcbase.h"

#include "chan_trigger_settings.h"

class qutag_mc_communicator : public ITimeTaggerCommunicator
{
private:
    bool have_device;
    TDC_DevType device_type;

    std::vector<chan_id> enabled_channels;

    double time_unit_seconds;
    uint64_t timestamp_buffer_size;
    bool reset_buffer_after_event_retrieval;

    std::string device_descriptor;

private:
    void update_channels_enabled();
    virtual void EnableChannel(chan_id channel_id);
    virtual void DisableChannel(chan_id channel_id);
    virtual void SetChannelEnabled(chan_id channel_id, bool enabled);

    // void check_for_events();

public:
    explicit qutag_mc_communicator(uint64_t buffer_size=1000000);
    virtual ~qutag_mc_communicator() override;

    virtual bool AnyDeviceAvailable() const override;
    virtual bool TryToConnect(int64_t device_ID) override;
    virtual bool Disconnect() override;

    virtual bool IsRealDevice() const override;
    virtual bool ConnectedToDevice() override;
    virtual uint64_t GetNumDevicesConnected() override;
    virtual double TimeUnit() const override;
    virtual const std::string& DeviceDescriptor() const override;
    // bool DevicePresent() const;

    virtual uint64_t GetSyncDivider(chan_id channel_id) override;
    virtual uint64_t UpdateSyncDivider(uint64_t value);
    virtual chan_trigger_settings GetSignalConditioning(chan_id chan_ID);
    virtual chan_trigger_settings UpdateSignalConditioning(chan_id chan_ID, chan_trigger_settings new_values);

    virtual bool DataLossSinceLastCall() override;
    virtual uint64_t ReceiveData(std::vector<int64_t>* timestamps, std::vector<uint8_t>* channel_IDs) override;
};

#endif // QUTAU_COMMUNICATOR_H
