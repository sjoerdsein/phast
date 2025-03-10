/* Copyright (c) 2025 Sjoerd Seinhorst, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#include <stdint.h>

#ifndef ITIMETRACESYNC_H
#define ITIMETRACESYNC_H

class ITimeTraceSync
{
public:
    virtual ~ITimeTraceSync() {}
    virtual bool ready_for_sync() const = 0;
    virtual int64_t get_sync_parameter() const = 0;
    virtual void apply_sync_parameter(int64_t param) = 0;
};

#endif // ITIMETRACESYNC_H