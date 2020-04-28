#include "ISDefinesGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
QStringList ISDefines::Gui::AVIABLE_IMAGE_EXTENSION;
QMargins ISDefines::Gui::MARGINS_LAYOUT_NULL;
QMargins ISDefines::Gui::MARGINS_LAYOUT_1_PX;
QMargins ISDefines::Gui::MARGINS_LAYOUT_2_PX;
QMargins ISDefines::Gui::MARGINS_LAYOUT_4_PX;
QMargins ISDefines::Gui::MARGINS_LAYOUT_5_PX;
QMargins ISDefines::Gui::MARGINS_LAYOUT_10_PX;
QSize ISDefines::Gui::SIZE_MAIN_WINDOW;
QSize ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM;
QSize ISDefines::Gui::SIZE_TASK_OBJECT_FORM;
QSize ISDefines::Gui::SIZE_AUTHORIZATION_FORM;
QSize ISDefines::Gui::SIZE_PROGRESS_FORM;
QSize ISDefines::Gui::SIZE_640_480;
QSize ISDefines::Gui::SIZE_500_90;
QSize ISDefines::Gui::SIZE_450_450;
QSize ISDefines::Gui::SIZE_550_300;
QSize ISDefines::Gui::SIZE_200_200;
QSize ISDefines::Gui::SIZE_100_100;
QSize ISDefines::Gui::SIZE_64_64;
QSize ISDefines::Gui::SIZE_45_45;
QSize ISDefines::Gui::SIZE_32_32;
QSize ISDefines::Gui::SIZE_25_25;
QSize ISDefines::Gui::SIZE_24_24;
QSize ISDefines::Gui::SIZE_22_22;
QSize ISDefines::Gui::SIZE_20_20;
QSize ISDefines::Gui::SIZE_18_18;
QSize ISDefines::Gui::SIZE_16_16;
QSize ISDefines::Gui::SIZE_10_10;
QColor ISDefines::Gui::EDIT_WIDGET_COLOR_RED;
QColor ISDefines::Gui::COLOR_BACKGROUND_INTERFACE;
QColor ISDefines::Gui::COLOR_MAIN_MENU_BAR;
QColor ISDefines::Gui::COLOR_WIDGET_BOX_TITLE;
QColor ISDefines::Gui::COLOR_CALENDAR_SELECTED_DATE_BACKGROUND;
QColor ISDefines::Gui::COLOR_CALENDAR_EVENT_FORM_FLASH;
QColor ISDefines::Gui::COLOR_CALENDAR_CELL_BACKGROUP;
QColor ISDefines::Gui::COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH;
QColor ISDefines::Gui::COLOR_SPLASH_SCREEN_TEXT;
QColor ISDefines::Gui::COLOR_MAIN_WINDOW_SHADOW;
QColor ISDefines::Gui::COLOR_POPUP_BRUSH;
QColor ISDefines::Gui::COLOR_FLASH_INCOMING_FORM;
QColor ISDefines::Gui::COLOR_WHITE;
QColor ISDefines::Gui::COLOR_RED;
QColor ISDefines::Gui::COLOR_BLUE;
QColor ISDefines::Gui::COLOR_BLACK;
QColor ISDefines::Gui::COLOR_DARK_GREEN;
QColor ISDefines::Gui::COLOR_LIGHT_GRAY;
QColor ISDefines::Gui::COLOR_MONITOR_USER;
QColor ISDefines::Gui::COLOR_NULL;
QFont ISDefines::Gui::FONT_COURIER_12;
QFont ISDefines::Gui::FONT_TAHOMA_35;
QFont ISDefines::Gui::FONT_TAHOMA_18;
QFont ISDefines::Gui::FONT_TAHOMA_15;
QFont ISDefines::Gui::FONT_TAHOMA_15_BOLD;
QFont ISDefines::Gui::FONT_TAHOMA_14;
QFont ISDefines::Gui::FONT_TAHOMA_14_BOLD;
QFont ISDefines::Gui::FONT_TAHOMA_12;
QFont ISDefines::Gui::FONT_TAHOMA_12_BOLD;
QFont ISDefines::Gui::FONT_TAHOMA_11;
QFont ISDefines::Gui::FONT_TAHOMA_10_BOLD;
QFont ISDefines::Gui::FONT_TAHOMA_10;
QFont ISDefines::Gui::FONT_TAHOMA_9;
QFont ISDefines::Gui::FONT_TAHOMA_9_BOLD;
QFont ISDefines::Gui::FONT_TAHOMA_8;
QFont ISDefines::Gui::FONT_TAHOMA_8_BOLD;
QFont ISDefines::Gui::FONT_APPLICATION;
QFont ISDefines::Gui::FONT_APPLICATION_BOLD;
//-----------------------------------------------------------------------------
void ISDefines::Gui::Init()
{
	AVIABLE_IMAGE_EXTENSION = QStringList() << EXTENSION_PNG << EXTENSION_JPEG << EXTENSION_JPG << EXTENSION_DDS << EXTENSION_GIF << EXTENSION_ICNS << EXTENSION_ICO << EXTENSION_SVG << EXTENSION_TGA << EXTENSION_TIFF << EXTENSION_WBMP << EXTENSION_BMP << EXTENSION_WEBP;
	MARGINS_LAYOUT_NULL = QMargins();
	MARGINS_LAYOUT_1_PX = QMargins(1, 1, 1, 1);
	MARGINS_LAYOUT_2_PX = QMargins(2, 2, 2, 2);
	MARGINS_LAYOUT_4_PX = QMargins(4, 4, 4, 4);
	MARGINS_LAYOUT_5_PX = QMargins(5, 5, 5, 5);
	MARGINS_LAYOUT_10_PX = QMargins(10, 10, 10, 10);
	SIZE_MAIN_WINDOW = QSize(800, 559);
	SIZE_MAIN_WINDOW_MINIMUM = QSize(700, 500);
	SIZE_TASK_OBJECT_FORM = QSize(880, 520);
	SIZE_AUTHORIZATION_FORM = QSize(350, 450);
	SIZE_PROGRESS_FORM = QSize(550, 100);
	SIZE_640_480 = QSize(640, 480);
	SIZE_500_90 = QSize(500, 90);
	SIZE_450_450 = QSize(450, 450);
	SIZE_550_300 = QSize(550, 300);
	SIZE_200_200 = QSize(200, 200);
	SIZE_100_100 = QSize(100, 100);
	SIZE_64_64 = QSize(64, 64);
	SIZE_45_45 = QSize(45, 45);
	SIZE_32_32 = QSize(32, 32);
	SIZE_25_25 = QSize(25, 25);
	SIZE_24_24 = QSize(24, 24);
	SIZE_22_22 = QSize(22, 22);
	SIZE_20_20 = QSize(20, 20);
	SIZE_18_18 = QSize(18, 18);
	SIZE_16_16 = QSize(16, 16);
	SIZE_10_10 = QSize(10, 10);
	EDIT_WIDGET_COLOR_RED = QColor(236, 99, 99);
	COLOR_BACKGROUND_INTERFACE = QColor(230, 230, 230);
	COLOR_MAIN_MENU_BAR = QColor(81, 145, 189);
	COLOR_WIDGET_BOX_TITLE = QColor(235, 235, 235);
	COLOR_CALENDAR_SELECTED_DATE_BACKGROUND = QColor(215, 229, 240);
	COLOR_CALENDAR_EVENT_FORM_FLASH = QColor(223, 225, 255);
	COLOR_CALENDAR_CELL_BACKGROUP = QColor(245, 245, 245);
	COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH = QColor(245, 245, 245);
	COLOR_SPLASH_SCREEN_TEXT = QColor(37, 77, 158);
	COLOR_MAIN_WINDOW_SHADOW = QColor(0, 0, 0, 130);
	COLOR_POPUP_BRUSH = QColor(0, 0, 0, 180);
	COLOR_FLASH_INCOMING_FORM = QColor(226, 235, 255);
	COLOR_WHITE = QColor(Qt::white);
	COLOR_RED = QColor(Qt::red);
	COLOR_BLUE = QColor(Qt::blue);
	COLOR_BLACK = QColor(Qt::black);
	COLOR_DARK_GREEN = QColor(Qt::green);
	COLOR_LIGHT_GRAY = QColor(Qt::lightGray);
	COLOR_MONITOR_USER = QColor(210, 210, 210);
	COLOR_NULL = QColor();
	FONT_COURIER_12 = QFont("Courier", 12);
	FONT_TAHOMA_35 = QFont("Tahoma", 35);
	FONT_TAHOMA_18 = QFont("Tahoma", 18);
	FONT_TAHOMA_15 = QFont("Tahoma", 15);
	FONT_TAHOMA_15_BOLD = QFont("Tahoma", 15, QFont::Bold);
	FONT_TAHOMA_14 = QFont("Tahoma", 14);
	FONT_TAHOMA_14_BOLD = QFont("Tahoma", 14, QFont::Bold);
	FONT_TAHOMA_12 = QFont("Tahoma", 12);
	FONT_TAHOMA_12_BOLD = QFont("Tahoma", 12, QFont::Bold);
	FONT_TAHOMA_11 = QFont("Tahoma", 11);
	FONT_TAHOMA_10_BOLD = QFont("Tahoma", 10, QFont::Bold);
	FONT_TAHOMA_10 = QFont("Tahoma", 10);
	FONT_TAHOMA_9 = QFont("Tahoma", 9);
	FONT_TAHOMA_9_BOLD = QFont("Tahoma", 9, QFont::Bold);
	FONT_TAHOMA_8 = QFont("Tahoma", 8);
	FONT_TAHOMA_8_BOLD = QFont("Tahoma", 8, QFont::Bold);
	FONT_APPLICATION = FONT_TAHOMA_9;
	FONT_APPLICATION_BOLD = FONT_TAHOMA_9_BOLD;
}
//-----------------------------------------------------------------------------