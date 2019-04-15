#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISPopupDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ISPopupDelegate(QObject *parent = 0);
	virtual ~ISPopupDelegate();

protected:
	virtual void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
