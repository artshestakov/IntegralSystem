#pragma once
//-----------------------------------------------------------------------------
#include <QStyledItemDelegate>
//-----------------------------------------------------------------------------
class BCComboDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	BCComboDelegate(QObject *parent = 0);
	virtual ~BCComboDelegate();

protected:
	virtual void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};
//-----------------------------------------------------------------------------
