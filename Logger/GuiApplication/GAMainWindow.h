#pragma once
#ifndef _GAMAINWINDOW_H_INCLUDED
#define _GAMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
//-----------------------------------------------------------------------------
class GAMainWindow : public QWidget
{
	Q_OBJECT

public:
	GAMainWindow(QWidget *parent = 0);
	virtual ~GAMainWindow();

private:
	void Start();
	void Log();
	void Destroy();
};
//-----------------------------------------------------------------------------
#endif
