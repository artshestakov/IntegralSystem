#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDelegateBoolean : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateBoolean(QObject *parent);
	virtual ~ISDelegateBoolean();

protected:
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;

private:
	QImage ImageNotSelected;
	QImage ImageSelected;
};
//-----------------------------------------------------------------------------
