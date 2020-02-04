#include "ISDelegateImage.h"
#include "EXDefines.h"
#include "ISTrace.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
ISDelegateImage::ISDelegateImage(QObject *parent) : QStyledItemDelegate(parent)
{

}
//-----------------------------------------------------------------------------
ISDelegateImage::~ISDelegateImage()
{

}
//-----------------------------------------------------------------------------
QSize ISDelegateImage::sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QSize Size = QStyledItemDelegate::sizeHint(Option, Index);

	if (Index.data().toByteArray().length())
	{
		Size.setHeight(100);
	}

	return Size;
}
//-----------------------------------------------------------------------------
void ISDelegateImage::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyleOptionViewItem OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);

	const QWidget *Widget = OptionViewItem.widget;
	QStyle *Style = Widget ? Widget->style() : QApplication::style();
	Style->drawControl(QStyle::CE_ItemViewItem, &OptionViewItem, Painter, Widget);

	QByteArray ByteArray = Index.data().toByteArray();
	if (ByteArray.isNull())
	{
		dynamic_cast<QTableView*>(parent())->setRowHeight(Index.row(), 30);
		return;
	}
	
	QImage Image;
	Image.loadFromData(ByteArray, "PNG");
	if (Image.isNull())
	{
		return;
	}
	
	Image = Image.scaled(SIZE_100_100, Qt::KeepAspectRatio);

	const int x = Option.rect.center().x() - Image.rect().width() / 2;
	const int y = Option.rect.center().y() - Image.rect().height() / 2;

	Painter->drawImage(QRect(x, y, Image.rect().width(), Image.rect().height()), Image);
	dynamic_cast<QTableView*>(parent())->resizeRowToContents(Index.row());
}
//-----------------------------------------------------------------------------
void ISDelegateImage::initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const
{
	QVariant Value = Index.data(Qt::DisplayRole);
	if (Value.isValid() && !Value.isNull())
	{
		Option->features |= QStyleOptionViewItem::HasDisplay;
	}
}
//-----------------------------------------------------------------------------
