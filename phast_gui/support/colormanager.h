/* Copyright (c) 2025 Sjoerd Seinhorst, Utrecht University
 * This sofware is licensed under the MIT license (see the LICENSE file)
*/

#include <array>
#include <QColor>

// This is a singleton class
class ColorManager
{
private:
    std::array<QColor, 32> const colors { // Must be unique
        QColor::fromHsv(120, 215, 255),
		QColor::fromHsv(300, 215, 255),
		QColor::fromHsv( 39, 252, 255),
		QColor::fromHsv(199, 204, 255),
		QColor::fromHsv(  0, 238, 255),
		QColor::fromHsv(120, 225, 140),
		QColor::fromHsv(300,  27, 255),
		QColor::fromHsv( 60, 250, 255),
		QColor::fromHsv(240, 126, 255),
		QColor::fromHsv( 28, 122, 153),
		QColor::fromHsv(180,  91, 255),
		QColor::fromHsv(331, 237, 227),
		QColor::fromHsv(174, 158, 139),
		QColor::fromHsv( 13,  99, 255),
		QColor::fromHsv( 61, 247, 166),
		QColor::fromHsv(300,  94, 255),
		QColor::fromHsv( 56,  40, 255),
		QColor::fromHsv(200, 240, 189),
		QColor::fromHsv(284,  49, 128),
		QColor::fromHsv(153, 154, 213),
		QColor::fromHsv( 56, 198, 113),
		QColor::fromHsv(223,  68, 255),
		QColor::fromHsv(276, 204, 227),
		QColor::fromHsv(349, 140, 255),
		QColor::fromHsv( 36,  82, 199),
		QColor::fromHsv(157,  46, 102),
		QColor::fromHsv(  3, 139, 156),
		QColor::fromHsv(185, 109, 193),
		QColor::fromHsv( 84, 106, 255),
		QColor::fromHsv(225, 186, 202),
		QColor::fromHsv(266,  21, 166),
		QColor::fromHsv( 97,  79, 153),
    };
    static constexpr QColor default_color {QColorConstants::White}; // Must not be in `colors`
    std::array<bool, std::tuple_size_v<decltype(colors)>> in_use {};

    ColorManager() = default;

public:
    // Prevent copy and assignment
    ColorManager(ColorManager const &) = delete;
    ColorManager & operator=(ColorManager const &) = delete;

    // Obtain the singleton instance
    static ColorManager& instance();

    // Managing colors
    QColor allocate_color();
    void release_color(QColor);

};
