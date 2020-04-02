#pragma once
#ifndef _GAMAINWINDOW_H_INCLUDED
#define _GAMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QWidget>
//-----------------------------------------------------------------------------
class GAMainWindow : public QWidget
{
	Q_OBJECT

public:
	GAMainWindow(QWidget *parent = 0);
	virtual ~GAMainWindow();
};
//-----------------------------------------------------------------------------
#endif
