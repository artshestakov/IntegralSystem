#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceMetaForm.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISKnowledgeBaseSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISKnowledgeBaseSubSystem(QWidget *parent = 0);
	virtual ~ISKnowledgeBaseSubSystem();

	void LoadData() override;

protected:
	void CreateGroup();
	void EditGroup();
	void DeleteGroup();

	void ItemSelectionChanged();
	void ClearSubGroups(QTreeWidgetItem *TreeWidgetTop);
	void LoadSubGroup(QTreeWidgetItem *TreeItemParent);
	QTreeWidgetItem* CreateItem(int GroupID, const QString &GroupName);
	void CreateObjectForm(QWidget *ObjectForm);

private:
	QAction *ActionCreate;
	QAction *ActionEdit;
	QAction *ActionDelete;
	QTreeWidget *TreeWidget;
	ISListBaseForm *ListBaseForm;
};
//-----------------------------------------------------------------------------
