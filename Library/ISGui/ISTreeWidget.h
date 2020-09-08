#pragma once
#ifndef _ISTREEWIDGET_H_INCLUDED
#define _ISTREEWIDGET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	ISTreeWidget(QWidget *parent = 0);
	virtual ~ISTreeWidget();

	void Clear();

	void RemoveBeginItem();
	void RemoveLastItem();
	void RemoveItem(int Index, int Column);
	void RemoveItem(QTreeWidgetItem *TreeWidgetItem); //Удалить элемент
	void ClearRecursive(QTreeWidgetItem *TreeWidgetItem);
};
//-----------------------------------------------------------------------------
#endif
