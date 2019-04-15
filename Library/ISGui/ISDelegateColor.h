#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDelegateColor : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateColor(QObject *parent);
	virtual ~ISDelegateColor();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
