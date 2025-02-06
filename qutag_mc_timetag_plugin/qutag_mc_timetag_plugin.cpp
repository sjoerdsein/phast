/* Copyright (c) 2020 Stijn Hinterding, Utrecht University
 * Modifications (c) 2025 Sjoerd Seinhorst, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#include "qutag_mc_timetag_plugin.h"

#include "qutag_mc_communicator.h"
#include "triggersettings_ui.h"

#include <qwidget.h>

/// Construct the quTAG MC time tag plugin, including the quTAG MC communicator
Qutag_mc_timetag_plugin::Qutag_mc_timetag_plugin() :
    comm(new qutag_mc_communicator()),
    chan_settings()
{
}

/// Destroy the plugin and the communicator
Qutag_mc_timetag_plugin::~Qutag_mc_timetag_plugin()
{
    delete this->comm;
}

/// Return this plugin name
QString Qutag_mc_timetag_plugin::TimeTagPluginName() const
{
    return "Qutag_mc_timetag_plugin";
}

/// Return this plugin descriptor
QString Qutag_mc_timetag_plugin::TimeTagPluginDescriptor() const
{
    return "plugin to communicate with qutools quTAG MC devices";
}

/// Return the menu label text
QString Qutag_mc_timetag_plugin::TimeTagMenuDescriptor() const
{
    return "quTAG MC trigger settings";
}

/// This plugin has a settings menu
bool Qutag_mc_timetag_plugin::TimeTagHasSettingsMenu() const
{
    return true;
}

/// Return the communicator by pointer
ITimeTaggerCommunicator* Qutag_mc_timetag_plugin::GetComm() const
{
    return this->comm;
}

/// NOT IMPLEMENTED! Disable a channel by `ID`
void Qutag_mc_timetag_plugin::DisableChan(chan_id ID)
{
    //this->comm->DisableChannel(ID);
}

/// There is no init dialog for this plugin
void Qutag_mc_timetag_plugin::ShowInitDialog(QWidget * /* parent */)
{
    return;
}

/// Show the trigger settings menu with the specified channels
void Qutag_mc_timetag_plugin::ShowTriggerSettingsUI(QWidget *parent, const std::vector<chan_id>& chan_IDs)
{
    this->update_chan_settings_map(chan_IDs);
    triggersettings_ui ui(parent, this->comm, this->chan_settings);
    ui.exec();
    chan_settings = ui.ChannelInfo();
}

/// Update the `chan_settings` member to contain exactly the channels listed in `active_channels`
void Qutag_mc_timetag_plugin::update_chan_settings_map(std::vector<chan_id> const & active_channels) {
    std::map<chan_id, chan_trigger_settings> new_settings{}; // Create a new map to replace the old one

    for (chan_id a : active_channels) { // Only fill the new map with active channels
        if (auto node = chan_settings.extract(a)) { // If it exists in the old map, move it over
            node.key() = a;
            new_settings.insert(std::move(node));
        } else { // If this channel was not in the old map, initialize with default settings
            new_settings.emplace(a, chan_trigger_settings {.ID = a});
        }
    }

    // Replace the old map with the new one
    chan_settings = std::move(new_settings);
}
