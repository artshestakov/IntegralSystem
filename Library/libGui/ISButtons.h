#pragma once
#ifndef _ISBUTTONS_H_INCLUDED
#define _ISBUTTONS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISPushButton : public QPushButton
{
	Q_OBJECT

public:
	ISPushButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent = 0);
	ISPushButton(const QIcon &Icon, const QString &Text, QWidget *parent = 0);
	ISPushButton(const QString &Text, QWidget *parent = 0);
	ISPushButton(const QIcon &Icon, QWidget *parent = 0);
	ISPushButton(QWidget *parent = 0);
	virtual ~ISPushButton();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISButtonClear : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClear(QWidget *parent = 0);
	virtual ~ISButtonClear();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISButtonClose : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClose(QWidget *parent = 0);
	virtual ~ISButtonClose();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISServiceButton : public ISPushButton
{
	Q_OBJECT

public:
	ISServiceButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent = 0);
	ISServiceButton(const QIcon &Icon, const QString &Text, QWidget *parent = 0);
	ISServiceButton(const QString &Text, QWidget *parent = 0);
	ISServiceButton(QWidget *parent = 0);
	virtual ~ISServiceButton();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISToolButton : public QToolButton
{
	Q_OBJECT

public:
	ISToolButton(QWidget *parent = 0);
	virtual ~ISToolButton();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
