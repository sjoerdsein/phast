/* Copyright (c) 2020 Stijn Hinterding, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#ifndef TRIGGERSETTINGS_UI_H
#define TRIGGERSETTINGS_UI_H

#include <QMainWindow>

#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDialog>

#include "qutag_mc_communicator.h"

class ISettingsContainer;

namespace Ui {
class triggersettings_ui;
}


class triggersettings_ui : public QDialog
{
    Q_OBJECT
private:
    struct chan_widgets {
        uint64_t channel_number;
        QLabel* chan_num;
        QComboBox* combo_trigger_edge;
        QDoubleSpinBox* voltage_threshold;
        QComboBox* sync_divider;
        QSpinBox* delay_time;
    };

public:
    triggersettings_ui(const triggersettings_ui&) = delete;
    triggersettings_ui& operator=(const triggersettings_ui&) = delete;

    explicit triggersettings_ui(QWidget *parent,
                                qutag_mc_communicator* tt_comm,
                                const std::map<chan_id, chan_trigger_settings>& chan_settings);
    ~triggersettings_ui();

    const std::map<chan_id, chan_trigger_settings> &ChannelInfo() const;

private slots:
    void on_btn_apply_clicked();
    void on_btn_cancel_clicked();
    void on_btn_OK_clicked();

private:
    Ui::triggersettings_ui *ui;
    std::map<chan_id, chan_trigger_settings> chan_info;
    const std::map<chan_id, chan_trigger_settings> original_chan_info;
    std::map<chan_id, chan_widgets> channels_widgets;

    qutag_mc_communicator* tt_comm;

    void init_ui_table();
    void add_channel_widgets(chan_trigger_settings chan_info);

    void trigger_edge_changed(uint64_t chan_ID, int64_t combobox_index);
    void voltage_threshold_changed(uint64_t chan_ID, double new_val);
    void delay_time_changed(uint64_t chan_ID, int new_val);
    void sync_divider_changed(uint64_t chan_ID, int64_t combobox_index);

    void push_to_device(uint64_t chan_ID);
};

#endif // TRIGGERSETTINGS_UI_H
