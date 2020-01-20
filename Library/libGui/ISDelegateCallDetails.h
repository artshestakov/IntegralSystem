#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISDelegateCallDetails : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateCallDetails(QObject *parent);
	virtual ~ISDelegateCallDetails();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;

private:
	QIcon IconDetailsNotNull;
	QIcon IconDetailsNull;
};
//-----------------------------------------------------------------------------
