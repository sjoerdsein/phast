/* Copyright (c) 2020 Stijn Hinterding, Utrecht University
 * Modifications (c) 2025 Sjoerd Seinhorst, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#ifndef CHAN_TRIGGER_SETTINGS_H
#define CHAN_TRIGGER_SETTINGS_H

#include <stdint.h>

struct chan_trigger_settings
{
public:
    enum trigger_edge {
        FALLING = 0,
        RISING = 1,
    };

    uint64_t ID {0};
    uint64_t sync_divider {1};
    int64_t delay_time {0}; // ps
    double voltage_threshold {-0.4}; // V
    trigger_edge edge {RISING};
};

#endif // CHAN_TRIGGER_SETTINGS_H
