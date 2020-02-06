#pragma once
#ifndef _ISDEFINESGUI_H_INCLUDED
#define _ISDEFINESGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDefinesGui
{
public:
	static ISDefinesGui& Instance();
	void Init(); //Инициализация

private:
	ISDefinesGui();
	~ISDefinesGui();
	ISDefinesGui(const ISDefinesGui &);
	ISDefinesGui& operator=(const ISDefinesGui &);

public:
	QStringList AVIABLE_IMAGE_EXTENSION;
	QMargins MARGINS_LAYOUT_NULL;
	QMargins MARGINS_LAYOUT_1_PX;
	QMargins MARGINS_LAYOUT_2_PX;
	QMargins MARGINS_LAYOUT_4_PX;
	QMargins MARGINS_LAYOUT_5_PX;
	QMargins MARGINS_LAYOUT_10_PX;
	QSize SIZE_MAIN_WINDOW;
	QSize SIZE_MAIN_WINDOW_MINIMUM;
	QSize SIZE_TASK_OBJECT_FORM;
	QSize SIZE_AUTHORIZATION_FORM;
	QSize SIZE_PROGRESS_FORM;
	QSize SIZE_640_480;
	QSize SIZE_500_90;
	QSize SIZE_450_450;
	QSize SIZE_550_300;
	QSize SIZE_200_200;
	QSize SIZE_100_100;
	QSize SIZE_64_64;
	QSize SIZE_45_45;
	QSize SIZE_32_32;
	QSize SIZE_25_25;
	QSize SIZE_24_24;
	QSize SIZE_22_22;
	QSize SIZE_20_20;
	QSize SIZE_18_18;
	QSize SIZE_16_16;
	QSize SIZE_10_10;
	QColor EDIT_WIDGET_COLOR_RED; //Цвет рамки вокруг незаполненного поля реадктирования информации
	QColor COLOR_BACKGROUND_INTERFACE; //Цвет фона форм интерфейса
	QColor COLOR_MAIN_MENU_BAR;
	QColor COLOR_WIDGET_BOX_TITLE;
	QColor COLOR_CALENDAR_SELECTED_DATE_BACKGROUND;
	QColor COLOR_CALENDAR_EVENT_FORM_FLASH;
	QColor COLOR_CALENDAR_CELL_BACKGROUP;
	QColor COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH;
	QColor COLOR_SPLASH_SCREEN_TEXT;
	QColor COLOR_MAIN_WINDOW_SHADOW;
	QColor COLOR_POPUP_BRUSH;
	QColor COLOR_FLASH_INCOMING_FORM;
	QColor COLOR_WHITE;
	QColor COLOR_RED;
	QColor COLOR_BLUE;
	QColor COLOR_BLACK;
	QColor COLOR_DARK_GREEN;
	QColor COLOR_LIGHT_GRAY;
	QColor COLOR_MONITOR_USER;
	QColor COLOR_NULL;
	QFont FONT_COURIER_12;
	QFont FONT_TAHOMA_35;
	QFont FONT_TAHOMA_18;
	QFont FONT_TAHOMA_15;
	QFont FONT_TAHOMA_15_BOLD;
	QFont FONT_TAHOMA_14;
	QFont FONT_TAHOMA_14_BOLD;
	QFont FONT_TAHOMA_12;
	QFont FONT_TAHOMA_12_BOLD;
	QFont FONT_TAHOMA_11;
	QFont FONT_TAHOMA_10_BOLD;
	QFont FONT_TAHOMA_10;
	QFont FONT_TAHOMA_9;
	QFont FONT_TAHOMA_9_BOLD;
	QFont FONT_TAHOMA_8;
	QFont FONT_APPLICATION;
	QFont FONT_APPLICATION_BOLD;
};
//-----------------------------------------------------------------------------
#define DEFINES_GUI ISDefinesGui::Instance()
//-----------------------------------------------------------------------------
#endif
