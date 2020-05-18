#pragma once
#ifndef _PTMAINWINDOW_H_INCLUDED
#define _PTMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
//-----------------------------------------------------------------------------
#include "ISTcpServerCarat.h"
//-----------------------------------------------------------------------------
class PTMainWindow : public QWidget
{
	Q_OBJECT

public:
	PTMainWindow(QWidget *parent = 0);
	virtual ~PTMainWindow();

private:
	void Listen();
	void Unlisten();

private:
	ISTcpServerCarat *Server;

private:
	QSpinBox *EditListenPort;
	QPushButton *ButtonListen;
	QPushButton *ButtonUnlisten;
};
//-----------------------------------------------------------------------------
#endif
