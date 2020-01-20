#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISDelegateFile : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateFile(QWidget *parent = 0);
	virtual ~ISDelegateFile();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;

private:
	QImage ImageNotSelected;
	QImage ImageSelected;
};
//-----------------------------------------------------------------------------
