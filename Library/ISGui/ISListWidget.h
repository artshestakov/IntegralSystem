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

	void Clear(); //������� �������

	void RemoveBeginItem(); //������� ������ �������
	void RemoveLastItem(); //������� ��������� �������
	void RemoveItem(int Row); //������� ������� �� �������
	void RemoveItem(QListWidgetItem *ListWidgetItem); //������� �������

	void SetFontItems(const QFont &Font);
	void SetFontItem(int Row, const QFont &Font);

	QListWidgetItem* BeginItem(); //�������� ������ �������
	QListWidgetItem* LastItem(); //�������� ��������� �������
};
//-----------------------------------------------------------------------------
