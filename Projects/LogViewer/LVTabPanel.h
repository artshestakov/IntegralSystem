#pragma once
#ifndef _LVTABWIDGET_H_INCLUDED
#define _LVTABWIDGET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "LVLogModel.h"
//-----------------------------------------------------------------------------
class LVTabPanel : public QWidget
{
	Q_OBJECT

public:
	LVTabPanel(const QString &FilePath, QWidget *parent);
	virtual ~LVTabPanel();

private:
	LVLogModel *Model;
};
//-----------------------------------------------------------------------------
#endif
