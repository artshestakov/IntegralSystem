#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "PMetaClass.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISFavoritesForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void OpenObject(const QString &TableName, int ObjectID);

public:
	ISFavoritesForm(QWidget *parent = 0, PMetaTable *meta_table = nullptr);
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
	QToolBar *ToolBar;
	QAction *ActionOpen;
	QAction *ActionDelete;
	QAction *ActionClearFavorites;
	ISListWidget *ListWidget;
	PMetaTable *MetaTable;
};
//-----------------------------------------------------------------------------
