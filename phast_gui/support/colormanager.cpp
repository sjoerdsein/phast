/* Copyright (c) 2025 Sjoerd Seinhorst, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#include "colormanager.h"

#include <algorithm>

ColorManager& ColorManager::instance()
{
    static ColorManager instance;
    return instance;
}

QColor ColorManager::allocate_color()
{
    auto first_unused = std::ranges::find(in_use, false);

    if (first_unused != in_use.cend()) {
        // There is an unallocated color
        *first_unused = true;
        return colors[std::distance(in_use.begin(), first_unused)];
    } else {
        // All colors are allocated
        return default_color;
    }
}

void ColorManager::release_color(QColor const color)
{
    auto it = std::ranges::find(colors, color);
    if (it != colors.end()) {
        in_use[std::distance(colors.cbegin(), it)] = false;
    }
}
