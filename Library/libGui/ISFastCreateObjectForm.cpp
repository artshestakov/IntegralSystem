#include "ISFastCreateObjectForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISFastAccessEntity.h"
#include "ISButtonClose.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_FAST_CREATE_OBJECT = PREPARE_QUERY("SELECT fcob_table "
													 "FROM _fastcreateobject "
													 "WHERE fcob_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QI_FAST_CREATE_OBJECT = PREPARE_QUERY("INSERT INTO _fastcreateobject(fcob_table) "
													 "VALUES(:Table)");
//-----------------------------------------------------------------------------
static QString QD_FAST_CREATE_OBJECT = PREPARE_QUERY("DELETE FROM _fastcreateobject "
													 "WHERE fcob_user = currentuserid() "
													 "AND fcob_table = :Table");
//-----------------------------------------------------------------------------
ISFastCreateRecordsForm::ISFastCreateRecordsForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("CreateRecords"));

	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	Layout->addWidget(new QLabel(LANG("Table") + ':', this));

	ComboEdit = new ISComboEdit(this);
	ComboEdit->SetEditable(false);
	ComboEdit->AddItem(LANG("NotSelected"), QVariant());
	connect(ComboEdit, &ISComboEdit::ValueChange, this, &ISFastCreateRecordsForm::TableChanged);
	Layout->addWidget(ComboEdit);

	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().count(); ++i)
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		if (!MetaTable->GetIsSystem())
		{
			ComboEdit->AddItem(MetaTable->GetLocalListName(), MetaTable->GetName());
		}
	}

	ButtonAdd = new ISPushButton(LANG("Addition"), this);
	ButtonAdd->setEnabled(false);
	connect(ButtonAdd, &ISPushButton::clicked, this, &ISFastCreateRecordsForm::Add);
	Layout->addWidget(ButtonAdd);

	ButtonDelete = new ISPushButton(LANG("Delete"), this);
	ButtonDelete->setEnabled(false);
	connect(ButtonDelete, &ISPushButton::clicked, this, &ISFastCreateRecordsForm::Delete);
	GetMainLayout()->addWidget(ButtonDelete, 0, Qt::AlignRight);

	ListWidget = new ISListWidget(this);
	GetMainLayout()->addWidget(ListWidget);
	connect(ListWidget, &QListWidget::itemSelectionChanged, [=]
	{
		ButtonDelete->setEnabled(true);
	});

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISFastCreateRecordsForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);

	Load();
}
//-----------------------------------------------------------------------------
ISFastCreateRecordsForm::~ISFastCreateRecordsForm()
{

}
//-----------------------------------------------------------------------------
void ISFastCreateRecordsForm::Load()
{
	ISQuery qSelect(QS_FAST_CREATE_OBJECT);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString TableName = qSelect.ReadColumn("fcob_table").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(ISMetaData::GetInstanse().GetMetaTable(TableName)->GetLocalListName());
			ListWidgetItem->setData(Qt::UserRole, TableName);
		}
	}
}
//-----------------------------------------------------------------------------
void ISFastCreateRecordsForm::Add()
{
	QString TableName = ComboEdit->GetValue().toString();
	QString LocalListName = ISMetaData::GetInstanse().GetMetaTable(TableName)->GetLocalListName();

	for (int i = 0; i < ListWidget->count(); ++i)
	{
		if (ListWidget->item(i)->text() == LocalListName)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.TableAlreadyExistList").arg(LocalListName));
			return;
		}
	}

	ISQuery qInsert(QI_FAST_CREATE_OBJECT);
	qInsert.BindValue(":Table", TableName);
	if (qInsert.Execute())
	{
		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setText(ComboEdit->GetCurrentText());
		ListWidgetItem->setData(Qt::UserRole, TableName);
		ListWidget->setCurrentItem(ListWidgetItem);
	}

	ISFastAccessEntity::GetInstance().ReloadCreateRecords();
}
//-----------------------------------------------------------------------------
void ISFastCreateRecordsForm::Delete()
{
	ISQuery qDelete(QD_FAST_CREATE_OBJECT);
	qDelete.BindValue(":Table", ListWidget->currentItem()->data(Qt::UserRole));
	if (qDelete.Execute())
	{
		for (int i = 0; i < ListWidget->count(); ++i)
		{
			if (ListWidget->item(i)->data(Qt::UserRole).toString() == ListWidget->currentItem()->data(Qt::UserRole).toString())
			{
				QListWidgetItem *ListWidgetItem = ListWidget->takeItem(i);
				delete ListWidgetItem;
				ListWidgetItem = nullptr;
			}
		}
	}

	if (ListWidget->count())
	{
		ButtonDelete->setEnabled(true);
	}
	else
	{
		ButtonDelete->setEnabled(false);
	}

	ISFastAccessEntity::GetInstance().ReloadCreateRecords();
}
//-----------------------------------------------------------------------------
void ISFastCreateRecordsForm::TableChanged(const QVariant &value)
{
	if (value.isValid())
	{
		ButtonAdd->setEnabled(true);
	}
	else
	{
		ButtonAdd->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
