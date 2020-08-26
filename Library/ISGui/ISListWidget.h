#pragma once
#ifndef _ISLISTWIDGET_H_INCLUDED
#define _ISLISTWIDGET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISListWidget : public QListWidget
{
	Q_OBJECT

signals:
	void ItemSelectionChanged(bool);

public:
	ISListWidget(QWidget *parent = 0);
	virtual ~ISListWidget();

	void Clear(); //������� ������
	void RemoveBeginItem(); //������� ������ �������
	void RemoveLastItem(); //������� ��������� �������
	void RemoveItem(int Row); //������� ������� �� �������
	void RemoveItem(QListWidgetItem *ListWidgetItem); //������� �������
	void SetFontItems(const QFont &Font);
	void SetFontItem(int Row, const QFont &Font);
	void SetMaxVisibleItems(int max_visible_items);
	void SetVisibleItems(bool Visible); //�������� ��������� ���������
	void SetCheckedItems(bool Checked); //�������� ��������� ���� ���������
	QAction* AddAction(const QIcon &Icon, const QString &Text, bool InGroup);
	void AddAction(QAction *Action, bool InGroup); //�������� ��������
	QListWidgetItem* BeginItem(); //�������� ������ �������
	QListWidgetItem* LastItem(); //�������� ��������� �������

protected:
	void paintEvent(QPaintEvent *PaintEvent);
	void mousePressEvent(QMouseEvent *MouseEvent);

private:
	int MaxVisibleItems;
};
//-----------------------------------------------------------------------------
#endif
