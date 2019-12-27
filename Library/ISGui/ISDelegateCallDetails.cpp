#include "StdAfx.h"
#include "ISDelegateCallDetails.h"
#include "ISDefines.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISDelegateCallDetails::ISDelegateCallDetails(QObject *parent) : QStyledItemDelegate(parent)
{
	IconDetailsNotNull = BUFFER_ICONS("CallDetails.NotNull");
	IconDetailsNull = BUFFER_ICONS("CallDetails.Null");
}
//-----------------------------------------------------------------------------
ISDelegateCallDetails::~ISDelegateCallDetails()
{
	
}
//-----------------------------------------------------------------------------
void ISDelegateCallDetails::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyleOptionViewItem OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);

	const QWidget *Widget = OptionViewItem.widget;
	QStyle *Style = Widget ? Widget->style() : QApplication::style();
	Style->drawControl(QStyle::CE_ItemViewItem, &OptionViewItem, Painter, Widget);

	QIcon IconDetails;
	bool CallDetails = Index.data().toBool();
	if (CallDetails)
	{
		IconDetails = IconDetailsNotNull;
	}
	else
	{
		IconDetails = IconDetailsNull;
	}

	QImage Image = IconDetails.pixmap(SIZE_18_18).toImage();
	if (Image.isNull())
	{
		return;
	}

	const int x = Option.rect.center().x() - Image.rect().width() / 2;
	const int y = Option.rect.center().y() - Image.rect().height() / 2;
	Painter->drawImage(QRect(x, y, Image.rect().width(), Image.rect().height()), Image);
}
//-----------------------------------------------------------------------------
void ISDelegateCallDetails::initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const
{
	QVariant Value = Index.data(Qt::DisplayRole);
	if (Value.isValid() && !Value.isNull())
	{
		Option->features |= QStyleOptionViewItem::HasDisplay;
	}
}
//-----------------------------------------------------------------------------
