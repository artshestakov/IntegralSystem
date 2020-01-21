#include "StdAfx.h"
#include "ISInternalDirectoriesForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_INTERNAL_DIRECTORIES = PREPARE_QUERY("SELECT intd_tablename "
													   "FROM _internaldirectories "
													   "WHERE NOT intd_isdeleted "
													   "ORDER BY intd_order");
//-----------------------------------------------------------------------------
ISInternalDirectoriesForm::ISInternalDirectoriesForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ListBaseForm = nullptr;

	Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);
	
	ListWidget = new ISListWidget(this);
	ListWidget->setAlternatingRowColors(true);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	ListWidget->setSizePolicy(QSizePolicy::Maximum, ListWidget->sizePolicy().verticalPolicy());
	connect(ListWidget, &ISListWidget::itemSelectionChanged, this, &ISInternalDirectoriesForm::ItemSelectionChanged);
	Layout->addWidget(ListWidget);

	ISQuery qSelect(QS_INTERNAL_DIRECTORIES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString TableName = qSelect.ReadColumn("intd_tablename").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(ISMetaData::GetInstanse().GetMetaTable(TableName)->GetLocalListName());
			ListWidgetItem->setData(Qt::UserRole, TableName);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setFont(FONT_APPLICATION);
		}
	}

	Label = new QLabel(this);
	Label->setWordWrap(true);
	Label->setText(LOCALIZATION("ClickFromViewList"));
	Label->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	Label->setFont(FONT_TAHOMA_14_BOLD);
	Layout->addWidget(Label, 0, Qt::AlignCenter);
}
//-----------------------------------------------------------------------------
ISInternalDirectoriesForm::~ISInternalDirectoriesForm()
{

}
//-----------------------------------------------------------------------------
void ISInternalDirectoriesForm::LoadData()
{

}
//-----------------------------------------------------------------------------
void ISInternalDirectoriesForm::ItemSelectionChanged()
{
	ISGui::SetWaitGlobalCursor(true);

	if (Label)
	{
		delete Label;
		Label = nullptr;
	}

	if (ListBaseForm)
	{
		delete	ListBaseForm;
		ListBaseForm = nullptr;
	}

	for (int i = 0; i < ListWidget->count(); i++)
	{
		ListWidget->item(i)->setFont(FONT_APPLICATION);
	}

	QListWidgetItem *CurrentItem = ListWidget->currentItem();
	CurrentItem->setFont(FONT_APPLICATION_BOLD);

	ListBaseForm = new ISListBaseForm(CurrentItem->data(Qt::UserRole).toString(), this);
	ListBaseForm->LoadData();
	Layout->addWidget(ListBaseForm);

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
