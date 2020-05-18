#pragma once
#ifndef _PTMAINWINDOW_H_INCLUDED
#define _PTMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
//-----------------------------------------------------------------------------
class PTMainWindow : public QWidget
{
	Q_OBJECT

public:
	PTMainWindow(QWidget *parent = 0);
	virtual ~PTMainWindow();
};
//-----------------------------------------------------------------------------
#endif
