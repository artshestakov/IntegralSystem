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
#include <QtWidgets/QTextEdit>
//-----------------------------------------------------------------------------
#include "PTThreadCarat.h"
//-----------------------------------------------------------------------------
class PTMainWindow : public QWidget
{
	Q_OBJECT

public:
	PTMainWindow(QWidget *parent = 0);
	virtual ~PTMainWindow();

protected:
	void closeEvent(QCloseEvent *CloseEvent);

private:
	void Listen();
	void Unlisten();
	void Connect();
	void Disconnect();

private:
	void ServerStateChanged(bool Running);

private:
	PTThreadCarat *ThreadCarat;

private:
	QSpinBox *EditListenPort;
	QPushButton *ButtonListen;
	QPushButton *ButtonUnlisten;
	QGroupBox *GroupBoxClient;
	QLineEdit *EditLogin;
	QLineEdit *EditPassword;
	QPushButton *ButtonConnect;
	QPushButton *ButtonDisconnect;
	QTextEdit *TextEdit;
};
//-----------------------------------------------------------------------------
#endif
