#include "StdAfx.h"
#include "ISKnowledgeBaseSubSystem.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_GROUP_TOP = PREPARE_QUERY("SELECT qagr_id, qagr_name "
											"FROM _questionanswergroup "
											"WHERE NOT qagr_isdeleted "
											"AND qagr_parent IS NULL "
											"ORDER BY qagr_name");
//-----------------------------------------------------------------------------
static QString QS_SUB_GROUP = PREPARE_QUERY("SELECT qagr_id, qagr_name "
											"FROM _questionanswergroup "
											"WHERE NOT qagr_isdeleted "
											"AND qagr_parent = :ParentID "
											"ORDER BY qagr_name");
//-----------------------------------------------------------------------------
static QString QD_GROUP = PREPARE_QUERY("DELETE FROM _questionanswergroup "
										"WHERE qagr_id = :GroupID");
//-----------------------------------------------------------------------------
ISKnowledgeBaseSubSystem::ISKnowledgeBaseSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);
	
	QVBoxLayout *LayoutTreeWidget = new QVBoxLayout();
	Layout->addLayout(LayoutTreeWidget);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	LayoutTreeWidget->addWidget(ToolBar);

	ActionCreate = ToolBar->addAction(BUFFER_ICONS("Add"), LOCALIZATION("Add"), this, &ISKnowledgeBaseSubSystem::CreateGroup);

	ActionEdit = ToolBar->addAction(BUFFER_ICONS("Edit"), LOCALIZATION("Edit"), this, &ISKnowledgeBaseSubSystem::EditGroup);
	ActionEdit->setEnabled(false);

	ActionDelete = ToolBar->addAction(BUFFER_ICONS("Delete"), LOCALIZATION("Delete"), this, &ISKnowledgeBaseSubSystem::DeleteGroup);
	ActionDelete->setEnabled(false);

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setAlternatingRowColors(true);
	TreeWidget->setAnimated(true);
	TreeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidget->setDropIndicatorShown(false);
	TreeWidget->setHeaderHidden(true);
	TreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	TreeWidget->setSizePolicy(QSizePolicy::Maximum, TreeWidget->sizePolicy().verticalPolicy());
	TreeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
	TreeWidget->addAction(ActionCreate);
	TreeWidget->addAction(ActionEdit);
	TreeWidget->addAction(ActionDelete);
	connect(TreeWidget, &QTreeWidget::itemSelectionChanged, this, &ISKnowledgeBaseSubSystem::ItemSelectionChanged);
	LayoutTreeWidget->addWidget(TreeWidget);

	ListBaseForm = new ISListBaseForm("_QuestionAnswer", this);
	ListBaseForm->setEnabled(false);
	ListBaseForm->setSizePolicy(QSizePolicy::Minimum, ListBaseForm->sizePolicy().verticalPolicy());
	ListBaseForm->GetQueryModel()->SetClassFilter("quan_id = 0");
	connect(ListBaseForm, &ISListBaseForm::AddFormFromTab, this, &ISKnowledgeBaseSubSystem::CreateObjectForm);
	Layout->addWidget(ListBaseForm);
}
//-----------------------------------------------------------------------------
ISKnowledgeBaseSubSystem::~ISKnowledgeBaseSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::LoadData()
{
	ISSystem::SetWaitGlobalCursor(true);
	while (TreeWidget->topLevelItemCount())
	{
		ClearSubGroups(TreeWidget->topLevelItem(0));
		QTreeWidgetItem *TreeWidgetItem = TreeWidget->takeTopLevelItem(0);
		delete TreeWidgetItem;
	}

	ISQuery qSelectTop(QS_GROUP_TOP);
	if (qSelectTop.Execute())
	{
		while (qSelectTop.Next())
		{
			int GroupID = qSelectTop.ReadColumn("qagr_id").toInt();
			QString GroupName = qSelectTop.ReadColumn("qagr_name").toString();

			QTreeWidgetItem *TreeWidgetItemTop = CreateItem(GroupID, GroupName);
			TreeWidget->addTopLevelItem(TreeWidgetItemTop);
			LoadSubGroup(TreeWidgetItemTop);
		}
	}

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::CreateGroup()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "_QuestionAnswerGroup");
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISKnowledgeBaseSubSystem::LoadData);
	QList<QTreeWidgetItem*> SelectedItems = TreeWidget->selectedItems();
	if (SelectedItems.count() == 1)
	{
		ObjectFormBase->SetFieldValue("Parent", SelectedItems.first()->data(0, Qt::UserRole));
	}

	ObjectFormBase->show();
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::EditGroup()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_QuestionAnswerGroup", TreeWidget->currentItem()->data(0, Qt::UserRole).toInt());
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISKnowledgeBaseSubSystem::LoadData);
	ObjectFormBase->show();
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::DeleteGroup()
{
	QTreeWidgetItem *TreeItemGroup = TreeWidget->selectedItems().first();
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.DeleteGroup").arg(TreeItemGroup->text(0))))
	{
		ISQuery qDeleteGroup(QD_GROUP);
		qDeleteGroup.BindValue(":GroupID", TreeItemGroup->data(0, Qt::UserRole));
		if (qDeleteGroup.Execute())
		{
			LoadData();
		}
	}
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::ItemSelectionChanged()
{
	if (TreeWidget->selectedItems().count() == 1)
	{
		ActionEdit->setEnabled(true);
		ActionDelete->setEnabled(true);
		
		ListBaseForm->GetQueryModel()->SetClassFilter("quan_group = " + QString::number(TreeWidget->selectedItems().first()->data(0, Qt::UserRole).toInt()));
		ListBaseForm->setEnabled(true);
	}
	else if (TreeWidget->selectedItems().count() > 1)
	{
		ActionCreate->setEnabled(false);
		ActionEdit->setEnabled(false);
		ActionDelete->setEnabled(false);
		
		ListBaseForm->GetQueryModel()->SetClassFilter("quan_id = 0");
		ListBaseForm->setEnabled(false);
	}
	else
	{
		ActionCreate->setEnabled(true);
		ActionEdit->setEnabled(false);
		ActionDelete->setEnabled(false);
		
		ListBaseForm->GetQueryModel()->SetClassFilter("quan_id = 0");
		ListBaseForm->setEnabled(false);
	}
	
	ListBaseForm->Update();
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::ClearSubGroups(QTreeWidgetItem *TreeWidgetTop)
{
	while (TreeWidgetTop->childCount())
	{
		QTreeWidgetItem *TreeWidgetItem = TreeWidgetTop->takeChild(0);
		ClearSubGroups(TreeWidgetItem);
		delete TreeWidgetItem;
	}
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::LoadSubGroup(QTreeWidgetItem *TreeItemParent)
{
	ISQuery qSelectSubGroup(QS_SUB_GROUP);
	qSelectSubGroup.BindValue(":ParentID", TreeItemParent->data(0, Qt::UserRole));
	if (qSelectSubGroup.Execute())
	{
		while (qSelectSubGroup.Next())
		{
			int GroupID = qSelectSubGroup.ReadColumn("qagr_id").toInt();
			QString GroupName = qSelectSubGroup.ReadColumn("qagr_name").toString();

			QTreeWidgetItem *TreeWidgetItem = CreateItem(GroupID, GroupName);
			TreeItemParent->addChild(TreeWidgetItem);
			LoadSubGroup(TreeWidgetItem);
		}
	}

	if (TreeItemParent->childCount())
	{
		TreeItemParent->setText(0, TreeItemParent->text(0) + " (" + QString::number(TreeItemParent->childCount()) + ")");
	}
}
//-----------------------------------------------------------------------------
QTreeWidgetItem* ISKnowledgeBaseSubSystem::CreateItem(int GroupID, const QString &GroupName)
{
	QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem();
	TreeWidgetItem->setData(0, Qt::UserRole, GroupID);
	TreeWidgetItem->setText(0, GroupName);
	TreeWidgetItem->setToolTip(0, GroupName);
	TreeWidgetItem->setSizeHint(0, QSize(TreeWidgetItem->sizeHint(0).width(), 30));
	return TreeWidgetItem;
}
//-----------------------------------------------------------------------------
void ISKnowledgeBaseSubSystem::CreateObjectForm(QWidget *ObjectForm)
{
	ISObjectFormBase *ObjectFormBase = dynamic_cast<ISObjectFormBase*>(ObjectForm);
	ObjectFormBase->SetFieldValue("Group", TreeWidget->selectedItems().first()->data(0, Qt::UserRole));
	ObjectFormBase->setParent(nullptr);
	ObjectFormBase->show();
}
//-----------------------------------------------------------------------------
