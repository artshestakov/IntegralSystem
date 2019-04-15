#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISListWidget : public QListWidget
{
	Q_OBJECT

public:
	ISListWidget(QWidget *parent = 0);
	virtual ~ISListWidget();

	void Clear(); //������� �������

	void RemoveBeginItem(); //������� ������ �������
	void RemoveItem(int Row); //������� ������� �� �������
	void RemoveItem(QListWidgetItem *ListWidgetItem); //������� �������

	void SetFontItems(const QFont &Font);
	void SetFontItem(int Row, const QFont &Font);

	QListWidgetItem* BeginItem(); //�������� ������ �������
	QListWidgetItem* LastItem(); //�������� ��������� �������
};
//-----------------------------------------------------------------------------
