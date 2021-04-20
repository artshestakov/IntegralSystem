#pragma once
#ifndef _ISDEFINESGUI_H_INCLUDED
#define _ISDEFINESGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
namespace ISDefines
{
    class ISGUI_EXPORT Gui
    {
    public:
        static void Init(); //Инициализация

        static QStringList AVIABLE_IMAGE_EXTENSION;
        static QMargins MARGINS_LAYOUT_NULL;
        static QMargins MARGINS_LAYOUT_1_PX;
        static QMargins MARGINS_LAYOUT_2_PX;
        static QMargins MARGINS_LAYOUT_4_PX;
        static QMargins MARGINS_LAYOUT_5_PX;
        static QMargins MARGINS_LAYOUT_10_PX;
        static QSize SIZE_MAIN_WINDOW;
        static QSize SIZE_MAIN_WINDOW_MINIMUM;
        static QSize SIZE_PROGRESS_FORM;
        static QSize SIZE_640_480;
        static QSize SIZE_100_100;
        static QSize SIZE_64_64;
        static QSize SIZE_45_45;
        static QSize SIZE_32_32;
        static QSize SIZE_25_25;
        static QSize SIZE_22_22;
        static QSize SIZE_20_20;
        static QSize SIZE_18_18;
        static QSize SIZE_16_16;
        static QColor EDIT_WIDGET_COLOR_RED; //Цвет рамки вокруг незаполненного поля реадктирования информации
        static QColor COLOR_BACKGROUND_INTERFACE; //Цвет фона форм интерфейса
        static QColor COLOR_MAIN_MENU_BAR;
        static QColor COLOR_CALENDAR_SELECTED_DATE_BACKGROUND;
        static QColor COLOR_CALENDAR_EVENT_FORM_FLASH;
        static QColor COLOR_CALENDAR_CELL_BACKGROUP;
        static QColor COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH;
        static QColor COLOR_SPLASH_SCREEN_TEXT;
        static QColor COLOR_POPUP_BRUSH;
        static QColor COLOR_FLASH_INCOMING_FORM;
        static QColor COLOR_WHITE;
        static QColor COLOR_RED;
        static QColor COLOR_BLACK;
        static QColor COLOR_DARK_GRAY;
        static QColor COLOR_BLUE;
        static QColor COLOR_MONITOR_USER;
        static QColor COLOR_NULL;
        static QFont FONT_COURIER_12;
        static QFont FONT_TAHOMA_35;
        static QFont FONT_TAHOMA_18;
        static QFont FONT_TAHOMA_15;
        static QFont FONT_TAHOMA_15_BOLD;
        static QFont FONT_TAHOMA_14;
        static QFont FONT_TAHOMA_14_BOLD;
        static QFont FONT_TAHOMA_12;
        static QFont FONT_TAHOMA_12_BOLD;
        static QFont FONT_TAHOMA_11;
        static QFont FONT_TAHOMA_10_BOLD;
        static QFont FONT_TAHOMA_10;
        static QFont FONT_TAHOMA_9;
        static QFont FONT_TAHOMA_9_BOLD;
        static QFont FONT_TAHOMA_8;
        static QFont FONT_TAHOMA_8_BOLD;
        static QFont FONT_APPLICATION;
        static QFont FONT_APPLICATION_BOLD;
    };
}
//-----------------------------------------------------------------------------
#endif
