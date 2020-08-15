#include "ISListWidget.h"
//-----------------------------------------------------------------------------
ISListWidget::ISListWidget(QWidget *parent)
	: QListWidget(parent),
	MaxVisibleItems(0)
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
		RemoveLastItem();
	}
	clear();
}
//-----------------------------------------------------------------------------
void ISListWidget::RemoveBeginItem()
{
	RemoveItem(item(0));
}
//-----------------------------------------------------------------------------
void ISListWidget::RemoveLastItem()
{
	RemoveItem(item(count() - 1));
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
void ISListWidget::SetMaxVisibleItems(int max_visible_items)
{
	MaxVisibleItems = max_visible_items;

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
void ISListWidget::paintEvent(QPaintEvent *PaintEvent)
{
	QListWidget::paintEvent(PaintEvent);
	if (MaxVisibleItems > 0 && count())
	{
		int Height = 0, LastHeight = 0;
		for (int i = 0; i < MaxVisibleItems; ++i) //Обходим все элементы
		{
			QListWidgetItem *ListWidgetItem = item(i); //Получаем текущий элемент
			if (ListWidgetItem) //Элемент получен - запоминаем его размер
			{
				LastHeight = ListWidgetItem->sizeHint().height();
			}
			Height += LastHeight;
		}
		setFixedHeight(Height);
	}
}
//-----------------------------------------------------------------------------
