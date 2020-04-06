#pragma once
#ifndef _GAMAINWINDOW_H_INCLUDED
#define _GAMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtCore/QUuid>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QInputDialog>
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
	void Log2();
	void LogN();
	void Destroy();
	void Timeout();

private:
	int LastValue;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
