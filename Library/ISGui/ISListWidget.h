#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISListWidget : public QListWidget
{
	Q_OBJECT

public:
	ISListWidget(QWidget *parent = 0);
	virtual ~ISListWidget();

	void Clear(); //Очистка списока

	void RemoveBeginItem(); //Удалить первый элемент
	void RemoveLastItem(); //Удалить последний элемент
	void RemoveItem(int Row); //Удалить элемент по индексу
	void RemoveItem(QListWidgetItem *ListWidgetItem); //Удалить элемент

	void SetFontItems(const QFont &Font);
	void SetFontItem(int Row, const QFont &Font);

	QListWidgetItem* BeginItem(); //Получить первый элемент
	QListWidgetItem* LastItem(); //Получить последний элемент
};
//-----------------------------------------------------------------------------
