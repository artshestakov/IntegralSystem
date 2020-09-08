#include "ISTreeWidget.h"
//-----------------------------------------------------------------------------
ISTreeWidget::ISTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

}
//-----------------------------------------------------------------------------
ISTreeWidget::~ISTreeWidget()
{

}
//-----------------------------------------------------------------------------
void ISTreeWidget::Clear()
{
	while (topLevelItemCount())
	{
		RemoveLastItem();
	}
}
//-----------------------------------------------------------------------------
void ISTreeWidget::RemoveBeginItem()
{

}
//-----------------------------------------------------------------------------
void ISTreeWidget::RemoveLastItem()
{
	ClearRecursive(takeTopLevelItem(topLevelItemCount() - 1));
}
//-----------------------------------------------------------------------------
void ISTreeWidget::RemoveItem(int Index, int Column)
{

}
//-----------------------------------------------------------------------------
void ISTreeWidget::RemoveItem(QTreeWidgetItem *TreeWidgetItem)
{
	
}
//-----------------------------------------------------------------------------
void ISTreeWidget::ClearRecursive(QTreeWidgetItem *TreeWidgetItem)
{
	int ChildCount = TreeWidgetItem->childCount();
	if (ChildCount) //Если у этого элемента есть дочерние элементы
	{
		ClearRecursive(TreeWidgetItem->child(ChildCount - 1)); //Получаем последний дочерний элемент и передаём его на рекурсивное удаление
	}
	else //У этого элемента нет дочерних элементов - удаляем
	{
		//Для начала, обходим его виджеты и удаляем каждый из них
		for (int i = 0, c = columnCount(); i < c; ++i)
		{
			QWidget *ItemWidget = itemWidget(TreeWidgetItem, i);
			if (ItemWidget)
			{
				removeItemWidget(TreeWidgetItem, i);
				delete ItemWidget; //???
			}
		}
		delete TreeWidgetItem;
	}
}
//-----------------------------------------------------------------------------
