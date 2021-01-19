#include "ISDelegates.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISDelegateBoolean::ISDelegateBoolean(QObject *parent)
	: QStyledItemDelegate(parent),
	Image(BUFFER_ICONS("Delegate.Boolean").pixmap(ISDefines::Gui::SIZE_16_16).toImage()),
	ImageWidth(Image.width()),
	ImageHeight(Image.height())
{
	
}
//-----------------------------------------------------------------------------
ISDelegateBoolean::~ISDelegateBoolean()
{

}
//-----------------------------------------------------------------------------
void ISDelegateBoolean::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyleOptionViewItem OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);

	const QWidget *Widget = OptionViewItem.widget;
	QStyle *Style = Widget ? Widget->style() : QApplication::style();
	Style->drawControl(QStyle::CE_ItemViewItem, &OptionViewItem, Painter, Widget);

	if (Index.data().toBool())
	{
		Painter->drawImage(QRect(Option.rect.center().x() - ImageWidth / 2, Option.rect.center().y() - ImageHeight / 2, ImageWidth, ImageHeight), Image);
	}
}
//-----------------------------------------------------------------------------
void ISDelegateBoolean::initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const
{
	QVariant Value = Index.data(Qt::DisplayRole);
	if (Value.isValid() && !Value.isNull())
	{
		Option->features |= QStyleOptionViewItem::HasDisplay;
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDelegateColor::ISDelegateColor(QObject *parent) : QStyledItemDelegate(parent)
{

}
//-----------------------------------------------------------------------------
ISDelegateColor::~ISDelegateColor()
{

}
//-----------------------------------------------------------------------------
void ISDelegateColor::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyleOptionViewItem OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);
	OptionViewItem.text.clear();

	const QWidget *Widget = OptionViewItem.widget;
	QStyle *Style = Widget ? Widget->style() : QApplication::style();
	Style->drawControl(QStyle::CE_ItemViewItem, &OptionViewItem, Painter, Widget);

	QString ColorString = Index.data().toString();
	if (ColorString.isEmpty())
	{
		return;
	}
	else
	{
		QColor Color = ISGui::StringToColor(ColorString);
		Painter->setPen(QPen(Color, 1, Qt::SolidLine, Qt::FlatCap));
		Painter->setBrush(QBrush(Color, Qt::SolidPattern));
		Painter->drawEllipse(OptionViewItem.rect.center(), 12, 12);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDelegateFile::ISDelegateFile(QWidget *parent)
	: QStyledItemDelegate(parent),
	Image(BUFFER_ICONS("Delegate.File").pixmap(ISDefines::Gui::SIZE_16_16).toImage()),
	ImageWidth(Image.width()),
	ImageHeight(Image.height())
{

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
	if (!Index.data().toBool())
	{
		Painter->drawImage(QRect(Option.rect.center().x() - ImageWidth / 2, Option.rect.center().y() - ImageHeight / 2, ImageWidth, ImageHeight), Image);
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDelegateTaskImportant::ISDelegateTaskImportant(QWidget *parent)
	: QStyledItemDelegate(parent),
	Image(BUFFER_ICONS("Task.Important.Checked").pixmap(ISDefines::Gui::SIZE_22_22).toImage()),
	ImageWidth(Image.width()),
	ImageHeight(Image.height())
{

}
//-----------------------------------------------------------------------------
ISDelegateTaskImportant::~ISDelegateTaskImportant()
{

}
//-----------------------------------------------------------------------------
void ISDelegateTaskImportant::paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QStyleOptionViewItem OptionViewItem = Option;
	initStyleOption(&OptionViewItem, Index);

	const QWidget *Widget = OptionViewItem.widget;
	QStyle *Style = Widget ? Widget->style() : QApplication::style();
	Style->drawControl(QStyle::CE_ItemViewItem, &OptionViewItem, Painter, Widget);
	if (Index.data().toBool())
	{
		Painter->drawImage(QRect(Option.rect.center().x() - ImageWidth / 2, Option.rect.center().y() - ImageHeight / 2, ImageWidth, ImageHeight), Image);
	}
}
//-----------------------------------------------------------------------------
void ISDelegateTaskImportant::initStyleOption(QStyleOptionViewItem *Option, const QModelIndex &Index) const
{
	QVariant Value = Index.data(Qt::DisplayRole);
	if (Value.isValid() && !Value.isNull())
	{
		Option->features |= QStyleOptionViewItem::HasDisplay;
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
	if (Image.loadFromData(ByteArray, "PNG"))
	{
		if (Image.isNull())
		{
			return;
		}
	}

	Image = Image.scaled(ISDefines::Gui::SIZE_100_100, Qt::KeepAspectRatio);
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
//-----------------------------------------------------------------------------
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
	Size.setHeight(50);
	return Size;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
	QStyleOptionViewItem OptionViewItem = Option;
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
