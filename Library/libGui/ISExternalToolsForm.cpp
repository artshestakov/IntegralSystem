#include "ISExternalToolsForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISMetaUser.h"
#include "ISFastAccessEntity.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_EXTERNAL_TOOLS = PREPARE_QUERY("SELECT extl_uid, extl_localname, extl_command "
												 "FROM _externaltools "
												 "WHERE extl_user = currentuserid() "
												 "ORDER BY extl_order");
//-----------------------------------------------------------------------------
static QString QS_EXTERNAL_TOOLS_COUNT = PREPARE_QUERY("SELECT COUNT(*) FROM _externaltools WHERE extl_uid = :UID");
//-----------------------------------------------------------------------------
static QString QU_EXTERNAL_TOOLS = PREPARE_QUERY("UPDATE _externaltools SET "
												 "extl_localname = :LocalName, "
												 "extl_command = :Command, "
												 "extl_icon = :Icon, "
												 "extl_order = :Order "
												 "WHERE extl_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_EXTERNAL_TOOLS = PREPARE_QUERY("INSERT INTO _externaltools(extl_localname, extl_command, extl_icon, extl_order) "
												 "VALUES(:LocalName, :Command, :Icon, :Order)");
//-----------------------------------------------------------------------------
static QString QD_EXTERNAL_TOOLS = PREPARE_QUERY("DELETE FROM _externaltools WHERE extl_uid = :UID");
//-----------------------------------------------------------------------------
ISExternalToolsForm::ISExternalToolsForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("ExternalTools"));

	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_5_PX);

	QLabel *Label = new QLabel(LANG("MenuStructure") + ':', this);
	GetMainLayout()->addWidget(Label);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ListWidget = new ISListWidget(this);
	connect(ListWidget, &QListWidget::currentItemChanged, this, &ISExternalToolsForm::CurrentItemChanged);
	Layout->addWidget(ListWidget);

	QVBoxLayout *LayoutSettings = new QVBoxLayout();
	Layout->addLayout(LayoutSettings);

	ButtonAdd = new ISPushButton(LANG("Add"), this);
	connect(ButtonAdd, &ISPushButton::clicked, this, &ISExternalToolsForm::Add);
	LayoutSettings->addWidget(ButtonAdd);

	ButtonDelete = new ISPushButton(LANG("Delete"), this);
	connect(ButtonDelete, &ISPushButton::clicked, this, &ISExternalToolsForm::Delete);
	LayoutSettings->addWidget(ButtonDelete);

	LayoutSettings->addStretch();

	ButtonUp = new ISPushButton(LANG("Up"), this);
	connect(ButtonUp, &ISPushButton::clicked, this, &ISExternalToolsForm::Up);
	LayoutSettings->addWidget(ButtonUp);

	ButtonDown = new ISPushButton(LANG("Down"), this);
	connect(ButtonDown, &ISPushButton::clicked, this, &ISExternalToolsForm::Down);
	LayoutSettings->addWidget(ButtonDown);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditName = new ISLineEdit(this);
	connect(EditName, &ISLineEdit::ValueChange, this, &ISExternalToolsForm::NameChanged);
	FormLayout->addRow(LANG("Named") + ':', EditName);

	EditCommand = new ISPathEditFile(this);
	connect(EditCommand, &ISPathEditFile::ValueChange, this, &ISExternalToolsForm::CommandChanged);
	FormLayout->addRow(LANG("Command") + ':', EditCommand);

	ButtonDialog = new ISButtonDialog(this, LANG("Save"));
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISExternalToolsForm::Save);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISExternalToolsForm::close);
	GetMainLayout()->addWidget(ButtonDialog);

	Load();
	CountItemsChanged();
}
//-----------------------------------------------------------------------------
ISExternalToolsForm::~ISExternalToolsForm()
{

}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::Load()
{
	ISQuery qSelect(QS_EXTERNAL_TOOLS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(qSelect.ReadColumn("extl_localname").toString());
			ListWidgetItem->setData(Qt::UserRole, qSelect.ReadColumn("extl_command").toString());
			ListWidgetItem->setData(Qt::UserRole * 2, qSelect.ReadColumn("extl_uid").toString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::Save()
{
	for (int i = 0; i < ListWidget->count(); ++i)
	{
		QListWidgetItem *ListWidgetItem = ListWidget->item(i);
		
		ISUuid UID = ListWidgetItem->data(Qt::UserRole * 2).toString();
		QString LocalName = ListWidgetItem->text();
		QString Command = ListWidgetItem->data(Qt::UserRole).toString();
		QIcon Icon = ISGui::GetIconFile(Command);
		int Order = i + 1;

		if (!LocalName.length())
		{
			ListWidget->setCurrentItem(ListWidgetItem);
			ListWidget->setFocus();
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SelectNamedTools"));
			return;
		}

		if (!Command.length())
		{
			ListWidget->setCurrentItem(ListWidgetItem);
			ListWidget->setFocus();
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SelectCommandTools"));
			return;
		}

		ISQuery qSelect(QS_EXTERNAL_TOOLS_COUNT);
		qSelect.BindValue(":UID", UID);
		if (qSelect.ExecuteFirst())
		{
			int Count = qSelect.ReadColumn("count").toInt();
			if (Count)
			{
				ISQuery qUpdate(QU_EXTERNAL_TOOLS);
				qUpdate.BindValue(":LocalName", LocalName);
				qUpdate.BindValue(":Command", Command);
				qUpdate.BindValue(":Icon", ISGui::PixmapToByteArray(Icon.pixmap(SIZE_32_32)));
				qUpdate.BindValue(":Order", Order);
				qUpdate.BindValue(":UID", UID);
				qUpdate.Execute();
			}
			else
			{
				ISQuery qInsert(QI_EXTERNAL_TOOLS);
				qInsert.BindValue(":LocalName", LocalName);
				qInsert.BindValue(":Command", Command);
				qInsert.BindValue(":Icon", ISGui::PixmapToByteArray(Icon.pixmap(SIZE_32_32)));
				qInsert.BindValue(":Order", Order);
				qInsert.Execute();
			}
		}
	}

	ISFastAccessEntity::GetInstance().ReloadExternalTools();
	close();
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::Add()
{
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
	ListWidgetItem->setText('[' + LANG("NewTools") + ']');
	ListWidget->setCurrentItem(ListWidgetItem);

	EditName->SetValue(ListWidgetItem->text());
	CountItemsChanged();
	EditName->SetFocus();
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::Delete()
{
	QListWidgetItem *ListWidgetItem = ListWidget->currentItem();
	if (ListWidgetItem)
	{
		ISQuery qDelete(QD_EXTERNAL_TOOLS);
		qDelete.BindValue(":UID", ListWidgetItem->data(Qt::UserRole * 2));
		if (qDelete.Execute())
		{
			ListWidget->removeItemWidget(ListWidgetItem);

			delete ListWidgetItem;
			ListWidgetItem = nullptr;

			CountItemsChanged();

			ISFastAccessEntity::GetInstance().ReloadExternalTools();
		}
	}
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::Up()
{
	int CurrentRow = ListWidget->currentRow();
	QListWidgetItem *ListWidgetItem = ListWidget->takeItem(CurrentRow);

	CurrentRow--;
	
	ListWidget->insertItem(CurrentRow, ListWidgetItem);
	ListWidget->setCurrentItem(ListWidgetItem);
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::Down()
{
	int CurrentRow = ListWidget->currentRow();
	QListWidgetItem *ListWidgetItem = ListWidget->takeItem(CurrentRow);
	
	CurrentRow++;
	
	ListWidget->insertItem(CurrentRow, ListWidgetItem);
	ListWidget->setCurrentItem(ListWidgetItem);
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::CountItemsChanged()
{
	int ItemCount = ListWidget->count();

	if (ItemCount)
	{
		ButtonDelete->setEnabled(true);
		EditName->setEnabled(true);
		EditCommand->setEnabled(true);

		if (ItemCount == 1)
		{
			ButtonUp->setEnabled(false);
			ButtonDown->setEnabled(false);
		}
		else
		{
			ButtonUp->setEnabled(true);
			ButtonDown->setEnabled(true);
		}
	}
	else
	{
		ButtonDelete->setEnabled(false);
		ButtonUp->setEnabled(false);
		ButtonDown->setEnabled(false);
		EditName->setEnabled(false);
		EditCommand->setEnabled(false);

		EditName->Clear();
		EditCommand->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::CurrentItemChanged(QListWidgetItem *CurrentItem, QListWidgetItem *PreviousItem)
{
	if (CurrentItem)
	{
		EditName->SetValue(CurrentItem->text());
		EditCommand->SetValue(CurrentItem->data(Qt::UserRole).toString());
	}
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::NameChanged(const QVariant &Name)
{
	if (ListWidget->currentItem())
	{
		ListWidget->currentItem()->setText(Name.toString());
	}
}
//-----------------------------------------------------------------------------
void ISExternalToolsForm::CommandChanged(const QVariant &Command)
{
	if (ListWidget->currentItem())
	{
		ListWidget->currentItem()->setData(Qt::UserRole, Command.toString());
	}
}
//-----------------------------------------------------------------------------
