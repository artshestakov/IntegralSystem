#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISFavoritesForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISFavoritesForm(QWidget *parent = 0, const QString &table_name = QString());
	virtual ~ISFavoritesForm();

private:
	void LoadFavorites(); //�������� ����������
	void ReloadFavorites(); //������������ ���������
	void OpenFavorite(); //������� ��������� ������
	void ClearFavorites(); //�������� ��������� �������
	void EscapeClicked() override;

private:
	void ListWidgetDoubleClicked(QListWidgetItem *Item);

private:
	QString TableName;
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
