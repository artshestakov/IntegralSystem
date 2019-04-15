#include "StdAfx.h"
#include "ISDelegateSearchField.h"
//-----------------------------------------------------------------------------
ISDelegateSearchField::ISDelegateSearchField(QObject *parent) : QStyledItemDelegate(parent)
{

}
//-----------------------------------------------------------------------------
ISDelegateSearchField::~ISDelegateSearchField()
{

}
//-----------------------------------------------------------------------------
QSize ISDelegateSearchField::sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &ModelIndex) const
{
	QSize Size = QStyledItemDelegate::sizeHint(Option, ModelIndex);
	Size.setHeight(30);
	return Size;
}
//-----------------------------------------------------------------------------
