#pragma once
#ifndef _ISCONSTANTSGUI_H_INCLUDED
#define _ISCONSTANTSGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtGui/QFont>
//-----------------------------------------------------------------------------
const QSize   SIZE_MAIN_WINDOW_MINIMUM = QSize(700, 500);
//-----------------------------------------------------------------------------
const QColor  COLOR_MAIN_MENU_BAR = QColor(81, 145, 189);
const QColor  COLOR_CALENDAR_SELECTED_DATE_BACKGROUND = QColor(215, 229, 240);
//-----------------------------------------------------------------------------
const QFont   FONT_TAHOMA_12_BOLD = QFont("Tahoma", 12, QFont::Bold);
const QFont   FONT_TAHOMA_10 = QFont("Tahoma", 10);
const QFont   FONT_TAHOMA_9 = QFont("Tahoma", 9);
const QFont   FONT_TAHOMA_9_BOLD = QFont("Tahoma", 9, QFont::Bold);
const QFont   FONT_APPLICATION = FONT_TAHOMA_9;
const QFont   FONT_APPLICATION_BOLD = FONT_TAHOMA_9_BOLD;

//-----------------------------------------------------------------------------
#endif
