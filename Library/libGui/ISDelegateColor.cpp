#include "ISDelegateColor.h"
#include "ISGui.h"
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
	if (!ColorString.length())
	{
		return;
	}

	QColor Color = ISGui::StringToColor(ColorString);
	
	Painter->setPen(QPen(Color, 1, Qt::SolidLine, Qt::FlatCap));
	Painter->setBrush(QBrush(Color, Qt::SolidPattern));
	Painter->drawEllipse(OptionViewItem.rect.center(), 12, 12);
}
//-----------------------------------------------------------------------------
