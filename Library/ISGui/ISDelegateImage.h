#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDelegateImage : public QStyledItemDelegate
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDelegateImage(QObject *parent);
	virtual ~ISDelegateImage();

protected:
	QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	void initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
