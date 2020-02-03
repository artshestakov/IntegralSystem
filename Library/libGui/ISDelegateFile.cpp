#include "ISDelegateFile.h"
#include "ISDefines.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISDelegateFile::ISDelegateFile(QWidget *parent) : QStyledItemDelegate(parent)
{
	ImageNotSelected = BUFFER_ICONS("Delegate.File.NotSelected").pixmap(SIZE_16_16).toImage();
	ImageSelected = BUFFER_ICONS("Delegate.File.Selected").pixmap(SIZE_16_16).toImage();
}
//-----------------------------------------------------------------------------
ISDelegateFile::~ISDelegateFile()
{

}
//-----------------------------------------------------------------------------
void ISDelegateFile::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyleOptionViewItem OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);

	const QWidget *Widget = OptionViewItem.widget;
	QStyle *Style = Widget ? Widget->style() : QApplication::style();
	Style->drawControl(QStyle::CE_ItemViewItem, &OptionViewItem, Painter, Widget);

	if (Index.data().isValid())
	{
		QImage Image;

		if (Option.state & QStyle::State_Selected)
		{
			Image = ImageSelected;
		}
		else
		{
			Image = ImageNotSelected;
		}

		const int x = Option.rect.center().x() - Image.rect().width() / 2;
		const int y = Option.rect.center().y() - Image.rect().height() / 2;
		Painter->drawImage(QRect(x, y, Image.rect().width(), Image.rect().height()), Image);
	}
}
//-----------------------------------------------------------------------------
void ISDelegateFile::initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const
{
	QVariant Value = Index.data(Qt::DisplayRole);
	if (Value.isValid() && !Value.isNull())
	{
		Option->features |= QStyleOptionViewItem::HasDisplay;
	}
}
//-----------------------------------------------------------------------------
