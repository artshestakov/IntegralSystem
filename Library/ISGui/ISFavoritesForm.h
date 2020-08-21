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

protected:
	void LoadFavorites(); //�������� ����������
	void ReloadFavorites(); //������������ ���������
	void OpenFavorite(); //������� ��������� ������
	void DeleteFavorite(); //������� ��������� ������
	void ClearFavorites(); //�������� ��������� �������
	void EscapeClicked() override;

protected slots:
	void ListWidgetDoubleClicked(QListWidgetItem *Item);
	void ItemClicked(QListWidgetItem *Item);

private:
	QString TableName;

	QToolBar *ToolBar;
	QAction *ActionOpen;
	QAction *ActionDelete;
	QAction *ActionClearFavorites;
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
