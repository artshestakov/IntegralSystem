#include "StdAfx.h"
#include "BCComboDelegate.h"
//-----------------------------------------------------------------------------
BCComboDelegate::BCComboDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}
//-----------------------------------------------------------------------------
BCComboDelegate::~BCComboDelegate()
{

}
//-----------------------------------------------------------------------------
void BCComboDelegate::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyledItemDelegate::paint(Painter, Option, Index);

	QStyleOptionViewItemV4 OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);
}
//-----------------------------------------------------------------------------
QSize BCComboDelegate::sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QSize SizeHint = QStyledItemDelegate::sizeHint(Option, Index);
	SizeHint.setHeight(30);
	return SizeHint;
}
//-----------------------------------------------------------------------------
