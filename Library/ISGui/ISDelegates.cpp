#include "ISDelegates.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISDelegateBoolean::ISDelegateBoolean(QObject *parent)
	: QStyledItemDelegate(parent),
	ImageNotSelected(BUFFER_ICONS("Delegate.Boolean.NotSelected").pixmap(ISDefines::Gui::SIZE_16_16).toImage()),
	ImageSelected(BUFFER_ICONS("Delegate.Boolean.Selected").pixmap(ISDefines::Gui::SIZE_16_16).toImage())
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
		QImage Image = Option.state & QStyle::State_Selected ? ImageSelected : ImageNotSelected;
		const int x = Option.rect.center().x() - Image.rect().width() / 2;
		const int y = Option.rect.center().y() - Image.rect().height() / 2;
		Painter->drawImage(QRect(x, y, Image.rect().width(), Image.rect().height()), Image);
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
ISDelegateCallDetails::ISDelegateCallDetails(QObject *parent)
	: QStyledItemDelegate(parent),
	IconDetailsNotNull(BUFFER_ICONS("CallDetails.NotNull")),
	IconDetailsNull(BUFFER_ICONS("CallDetails.Null"))
{

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

	QIcon IconDetails = Index.data().toBool() ? IconDetailsNotNull : IconDetailsNull;
	QImage Image = IconDetails.pixmap(ISDefines::Gui::SIZE_18_18).toImage();
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
	ImageNotSelected(BUFFER_ICONS("Delegate.File.NotSelected").pixmap(ISDefines::Gui::SIZE_16_16).toImage()),
	ImageSelected(BUFFER_ICONS("Delegate.File.Selected").pixmap(ISDefines::Gui::SIZE_16_16).toImage())
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

	if (Index.data().isValid())
	{
		QImage Image = Option.state & QStyle::State_Selected ? ImageSelected : ImageNotSelected;
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
	Size.setHeight(30);
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
