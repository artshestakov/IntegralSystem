#include "ISInternalDirectoriesSubSystem.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISTcpQuery.h"
#include "ISMetaData.h"
#include "ISGui.h"
#include "ISBuffer.h"
#include "ISDialogsCommon.h"
//-----------------------------------------------------------------------------
ISInternalDirectoriesSubSystem::ISInternalDirectoriesSubSystem(QWidget *parent)
	: ISInterfaceMetaForm(parent),
	ListBaseForm(nullptr)
{
	Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);
	
	ListWidget = new ISListWidget(this);
	ListWidget->setAlternatingRowColors(true);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	ListWidget->setSizePolicy(QSizePolicy::Maximum, ListWidget->sizePolicy().verticalPolicy());
	connect(ListWidget, &ISListWidget::itemSelectionChanged, this, &ISInternalDirectoriesSubSystem::ItemSelectionChanged);
	Layout->addWidget(ListWidget);

	ISTcpQuery qGetInternalLists(API_GET_INTERNAL_LISTS);
	if (qGetInternalLists.Execute())
	{
		QVariantList Lists = qGetInternalLists.GetAnswer()["Lists"].toList();
		for (const QVariant &ListName : Lists)
		{
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(ISMetaData::Instance().GetMetaTable(ListName.toString())->LocalListName);
			ListWidgetItem->setData(Qt::UserRole, ListName);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setFont(ISDefines::Gui::FONT_APPLICATION);
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetInternalLists.GetErrorString());
	}

	Label = new QLabel(this);
	Label->setWordWrap(true);
	Label->setText(LANG("ClickFromViewList"));
	Label->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
	Label->setFont(ISDefines::Gui::FONT_TAHOMA_14_BOLD);
	Layout->addWidget(Label, 0, Qt::AlignCenter);
}
//-----------------------------------------------------------------------------
ISInternalDirectoriesSubSystem::~ISInternalDirectoriesSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISInternalDirectoriesSubSystem::LoadData()
{

}
//-----------------------------------------------------------------------------
void ISInternalDirectoriesSubSystem::ItemSelectionChanged()
{
	ISGui::SetWaitGlobalCursor(true);
	POINTER_DELETE(Label);
	POINTER_DELETE(ListBaseForm);

	ListWidget->SetFontItems(ISDefines::Gui::FONT_APPLICATION);
	QListWidgetItem *CurrentItem = ListWidget->currentItem();
	CurrentItem->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	ListBaseForm = new ISListBaseForm(CurrentItem->data(Qt::UserRole).toString(), this);
	connect(ListBaseForm, &ISListBaseForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
	ListBaseForm->LoadData();
	Layout->addWidget(ListBaseForm);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
