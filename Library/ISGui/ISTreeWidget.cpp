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
	if (ChildCount) //���� � ����� �������� ���� �������� ��������
	{
		ClearRecursive(TreeWidgetItem->child(ChildCount - 1)); //�������� ��������� �������� ������� � ������� ��� �� ����������� ��������
	}
	else //� ����� �������� ��� �������� ��������� - �������
	{
		//��� ������, ������� ��� ������� � ������� ������ �� ���
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
