#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
#include "PMetaClassTable.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISFavoritesForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void OpenObject(const QString &TableName, int ObjectID);

public:
	ISFavoritesForm(QWidget *parent = 0, PMetaClassTable *meta_table = nullptr);
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
	PMetaClassTable *MetaTable;
};
//-----------------------------------------------------------------------------
