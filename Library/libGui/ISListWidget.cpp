#include "ISListWidget.h"
//-----------------------------------------------------------------------------
ISListWidget::ISListWidget(QWidget *parent) : QListWidget(parent)
{

}
//-----------------------------------------------------------------------------
ISListWidget::~ISListWidget()
{

}
//-----------------------------------------------------------------------------
void ISListWidget::Clear()
{
	while (count())
	{
		RemoveBeginItem();
	}

	clear();
}
//-----------------------------------------------------------------------------
void ISListWidget::RemoveBeginItem()
{
	RemoveItem(item(0));
}
//-----------------------------------------------------------------------------
void ISListWidget::RemoveItem(int Row)
{
	RemoveItem(item(Row));
}
//-----------------------------------------------------------------------------
void ISListWidget::RemoveItem(QListWidgetItem *ListWidgetItem)
{
	if (itemWidget(ListWidgetItem))
	{
		removeItemWidget(ListWidgetItem);
	}

	takeItem(indexFromItem(ListWidgetItem).row());
	delete ListWidgetItem;
}
//-----------------------------------------------------------------------------
void ISListWidget::SetFontItems(const QFont &Font)
{
	for (int i = 0; i < count(); ++i)
	{
		item(i)->setFont(Font);
	}
}
//-----------------------------------------------------------------------------
void ISListWidget::SetFontItem(int Row, const QFont &Font)
{
	item(Row)->setFont(Font);
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISListWidget::BeginItem()
{
	if (item(0))
	{
		return item(0);
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISListWidget::LastItem()
{
	if (item(count() - 1))
	{
		return item(count() - 1);
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
