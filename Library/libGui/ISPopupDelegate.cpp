#include "ISPopupDelegate.h"
//-----------------------------------------------------------------------------
ISPopupDelegate::ISPopupDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}
//-----------------------------------------------------------------------------
ISPopupDelegate::~ISPopupDelegate()
{

}
//-----------------------------------------------------------------------------
void ISPopupDelegate::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyledItemDelegate::paint(Painter, Option, Index);

    

	QStyleOptionViewItemV4 OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);
}
//-----------------------------------------------------------------------------
QSize ISPopupDelegate::sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QSize SizeHint = QStyledItemDelegate::sizeHint(Option, Index);
	SizeHint.setHeight(30);
	return SizeHint;
}
//-----------------------------------------------------------------------------
