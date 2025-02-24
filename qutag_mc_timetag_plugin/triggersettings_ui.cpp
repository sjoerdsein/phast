/* Copyright (c) 2020 Stijn Hinterding, Utrecht University
 * Modifications (c) 2025 Sjoerd Seinhorst, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#include "triggersettings_ui.h"
#include <QVariant>
#include "ui_triggersettings_ui.h"

#include <bit>
#include <iostream>

#include "chan_trigger_settings.h"

/// Add the labels above the columns in the table of trigger settings
void triggersettings_ui::init_ui_table()
{
    QLabel* lbl_chan = new QLabel("Channel no.", this);
    QLabel* lbl_threshold = new QLabel("Threshold (V)", this);
    QLabel* lbl_delay_time = new QLabel("Delay time (ps)", this);
    QLabel* lbl_trigger_edge = new QLabel("Trigger edge", this);
    QLabel* lbl_divider = new QLabel("Sync divider", this);

    // Add the very first row
    // *Widget, row, column, rowspan, colspan
    this->ui->gridLayout->addWidget(lbl_chan,         0, 0, 1, 1, Qt::AlignTop);
    this->ui->gridLayout->addWidget(lbl_threshold,    0, 1, 1, 1, Qt::AlignTop);
    this->ui->gridLayout->addWidget(lbl_delay_time,   0, 2, 1, 1, Qt::AlignTop);
    this->ui->gridLayout->addWidget(lbl_trigger_edge, 0, 3, 1, 1, Qt::AlignTop);
    if (tt_comm->DeviceDescriptor() == "quTAG HR")
        this->ui->gridLayout->addWidget(lbl_divider,  0, 4, 1, 1, Qt::AlignTop);
}

/// Add a new row with the ID supplied in `chan_info`
void triggersettings_ui::add_channel_widgets(chan_trigger_settings chan_info)
{
    std::string const & device_descriptor = tt_comm->DeviceDescriptor();

    // Define the widgets with their settings
    chan_widgets cw;
    cw.channel_number = chan_info.ID;
    cw.chan_num = new QLabel(QString::number(chan_info.ID), this);

    // Input delay
    cw.delay_time = new QSpinBox(this);
    if      (device_descriptor == "quTAG MC") { cw.delay_time->setRange(- 50000,   50000); }
    else if (device_descriptor == "quTAG HR") { cw.delay_time->setRange(-100000,  100000); }
    else                                      { cw.delay_time->setRange(INT_MIN, INT_MAX); }
    cw.delay_time->setValue(chan_info.delay_time);

    // Trigger edge direction
    cw.combo_trigger_edge = new QComboBox(this);
    cw.combo_trigger_edge->addItem("Rising", QVariant(chan_trigger_settings::RISING));
    cw.combo_trigger_edge->addItem("Falling", QVariant(chan_trigger_settings::FALLING));
    switch (chan_info.edge) {
        case chan_trigger_settings::RISING:
            cw.combo_trigger_edge->setCurrentIndex(0); break;
        case chan_trigger_settings::FALLING:
            cw.combo_trigger_edge->setCurrentIndex(1); break;
    }

    // Trigger threshold voltage
    cw.voltage_threshold = new QDoubleSpinBox(this);
    cw.voltage_threshold->setRange(-3.0, 3.0);
    cw.voltage_threshold->setDecimals(4);
    cw.voltage_threshold->setValue(chan_info.voltage_threshold);

    // Sync divider (only on the start channel of quTAG HR)
    bool sync_divider_supported = device_descriptor == "quTAG HR" && chan_info.ID == 0;
    cw.sync_divider = new QComboBox(this);
    cw.sync_divider->addItems({"1", "2", "4", "8"});
    cw.sync_divider->setCurrentIndex(std::bit_width(chan_info.sync_divider)-1);
    if (not sync_divider_supported) {
        cw.sync_divider->setEnabled(false);
    }

    // Add the supported widgets to the grid
    int row = this->ui->gridLayout->rowCount() + 1;
    this->ui->gridLayout->addWidget(cw.chan_num,           row, 0, 1, 1, Qt::AlignTop);
    this->ui->gridLayout->addWidget(cw.voltage_threshold,  row, 1, 1, 1, Qt::AlignTop);
    this->ui->gridLayout->addWidget(cw.delay_time,         row, 2, 1, 1, Qt::AlignTop);
    this->ui->gridLayout->addWidget(cw.combo_trigger_edge, row, 3, 1, 1, Qt::AlignTop);
    if (sync_divider_supported)
        this->ui->gridLayout->addWidget(cw.sync_divider,   row, 4, 1, 1, Qt::AlignTop);


    this->channels_widgets[chan_info.ID] = cw;

    // Connect the signals and the slots, to call the correct function when one
    // of the widgets updates
    connect(cw.combo_trigger_edge, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=, this](int index){this->trigger_edge_changed(chan_info.ID, index);});

    connect(cw.voltage_threshold, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [=, this](double value){this->voltage_threshold_changed(chan_info.ID, value);});

    connect(cw.delay_time, QOverload<int>::of(&QSpinBox::valueChanged),
            [=, this](int value){this->delay_time_changed(chan_info.ID, value);});

    connect(cw.sync_divider, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=, this](int index){this->sync_divider_changed(chan_info.ID, index);});
}

/// Create the UI for changing the trigger settings
triggersettings_ui::triggersettings_ui(QWidget *parent, qutag_mc_communicator *tt_comm, const std::map<chan_id, chan_trigger_settings> &chan_settings) :
    QDialog((QWidget*)parent),
    ui(new Ui::triggersettings_ui),
    chan_info(chan_settings),
    original_chan_info(chan_settings),
    tt_comm(tt_comm)
{
    ui->setupUi(this);

    connect(this->ui->btn_OK, &QPushButton::clicked, this, &QDialog::accept);
    connect(this->ui->btn_cancel, &QPushButton::clicked, this, &QDialog::reject);

    this->init_ui_table();

    // Add the file info
    for (auto const & [id, ci] : this->chan_info) {
        this->add_channel_widgets(ci);
    }
}

/// Delete the UI for changing the trigger settings
triggersettings_ui::~triggersettings_ui()
{
    delete ui;
}

/// Update the channel info when the trigger edge widget is updated
void triggersettings_ui::trigger_edge_changed(chan_id chan_ID, int64_t /* combobox_index */)
{
  chan_info[chan_ID].edge = channels_widgets[chan_ID]
                                .combo_trigger_edge->currentData()
                                .value<chan_trigger_settings::trigger_edge>();
}

/// Update the channel info when the voltage threshold widget is updated
void triggersettings_ui::voltage_threshold_changed(chan_id chan_ID, double new_val)
{
    this->chan_info[chan_ID].voltage_threshold = new_val;
}

/// Update the channel info when the delay time widget is updated
void triggersettings_ui::delay_time_changed(chan_id chan_ID, int new_val)
{
    this->chan_info[chan_ID].delay_time = new_val;
}

/// Update the channel info when the sync divider widget is updated
void triggersettings_ui::sync_divider_changed(chan_id chan_ID, int64_t combobox_index)
{
    this->chan_info[chan_ID].sync_divider = 1 << combobox_index;
}

/// Upload the selected trigger settings for channel `chan_ID` to the device
void triggersettings_ui::push_to_device(chan_id chan_ID)
{
    std::cout << "Setting threshold of channel " << chan_ID << " to " << this->chan_info[chan_ID].voltage_threshold << " V\n";

    chan_trigger_settings checked_val = this->tt_comm->UpdateSignalConditioning(chan_ID, this->chan_info[chan_ID]);

    std::cout << "The device has set the threshold at " << checked_val.voltage_threshold << " V\n";

    // The actual voltage set by the device is slightly different than what we set it to. If the difference is small, ignore it
    double const threshold_setpoint = chan_info[chan_ID].voltage_threshold;
    double const threshold_difference = std::abs(checked_val.voltage_threshold - threshold_setpoint);
    this->chan_info[chan_ID] = checked_val;
    if (threshold_difference < voltage_threshold_resolution) {
        this->chan_info[chan_ID].voltage_threshold = threshold_setpoint;
    }
}

/// Return the current channel info map (trigger settings for each channel)
const std::map<chan_id, chan_trigger_settings> &triggersettings_ui::ChannelInfo() const
{
    return this->chan_info;
}

/// When the Apply button is clicked, upload all trigger settings to the device
void triggersettings_ui::on_btn_apply_clicked()
{
    // Disable all channels just to be sure
    tt_comm->SetEnabledChannels({});

    // Update settings
    std::vector<chan_id> enabled_channels {};
    enabled_channels.reserve(chan_info.size());
    for (auto const & [id, _] : chan_info) {
        this->push_to_device(id);
        enabled_channels.push_back(id);
    }

    // Enable the selected channels
    tt_comm->SetEnabledChannels(enabled_channels);

    // Update the old settings map, because new settings were accepted
    original_chan_info = chan_info;
}

/// When the Cancel button is clicked, restore the previous channel info
void triggersettings_ui::on_btn_cancel_clicked()
{
    this->chan_info = this->original_chan_info;
    this->setResult(QDialog::Rejected);
}

/// When the OK button is clicked, apply the settings and close the window
void triggersettings_ui::on_btn_OK_clicked()
{
    this->on_btn_apply_clicked();
    this->setResult(QDialog::Accepted);
    this->close();
}
