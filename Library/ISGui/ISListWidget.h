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

	void Clear(); //Очистка списка
	void RemoveBeginItem(); //Удалить первый элемент
	void RemoveLastItem(); //Удалить последний элемент
	void RemoveItem(int Row); //Удалить элемент по индексу
	void RemoveItem(QListWidgetItem *ListWidgetItem); //Удалить элемент
	void SetFontItems(const QFont &Font);
	void SetFontItem(int Row, const QFont &Font);
	void SetMaxVisibleItems(int max_visible_items);
	void SetVisibleItems(bool Visible); //Изменить видимость элементов
	void SetCheckedItems(bool Checked); //Изменить включение всех элементов
	QAction* AddAction(const QIcon &Icon, const QString &Text, bool InGroup);
	void AddAction(QAction *Action, bool InGroup); //Добавить действие
	QListWidgetItem* BeginItem(); //Получить первый элемент
	QListWidgetItem* LastItem(); //Получить последний элемент

protected:
	void paintEvent(QPaintEvent *PaintEvent);
	void mousePressEvent(QMouseEvent *MouseEvent);

private:
	int MaxVisibleItems;
};
//-----------------------------------------------------------------------------
#endif
