#include "ISNoteForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISDefinesCore.h"
#include "ISCore.h"
#include "ISQuery.h"
#include "ISInputDialog.h"
#include "ISControls.h"
#include "ISButtonClose.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_NOTE = PREPARE_QUERY("SELECT note_id, note_name, note_text FROM _note WHERE NOT note_isdeleted AND note_user = currentuserid() ORDER BY note_id");
//-----------------------------------------------------------------------------
static QString QI_NOTE = PREPARE_QUERY("INSERT INTO _note(note_name) VALUES(:Name) RETURNING note_id");
//-----------------------------------------------------------------------------
static QString QU_NOTE_NAME = PREPARE_QUERY("UPDATE _note SET note_name = :Name WHERE note_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QU_NOTE_TEXT = PREPARE_QUERY("UPDATE _note SET note_text = :Text WHERE note_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QD_NOTE = PREPARE_QUERY("DELETE FROM _note WHERE note_id = :ObjectID");
//-----------------------------------------------------------------------------
ISNoteForm::ISNoteForm(QWidget *parent) : ISInterfaceForm(parent)
{
	ModificationFlag = false;
	PreviousItem = nullptr;

	setWindowTitle(LANG("Notebook"));
	setWindowIcon(BUFFER_ICONS("NoteObject"));

	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	ActionCreate = ISControls::CreateActionCreate(ToolBar);
	connect(ActionCreate, &QAction::triggered, this, &ISNoteForm::Create);
	ToolBar->addAction(ActionCreate);

	ActionEdit = ISControls::CreateActionEdit(ToolBar);
	ActionEdit->setEnabled(false);
	connect(ActionEdit, &QAction::triggered, this, &ISNoteForm::Edit);
	ToolBar->addAction(ActionEdit);

	ActionDelete = ISControls::CreateActionDelete(ToolBar);
	ActionDelete->setEnabled(false);
	connect(ActionDelete, &QAction::triggered, this, &ISNoteForm::Delete);
	ToolBar->addAction(ActionDelete);
	
	ActionSave = ISControls::CreateActionSave(ToolBar);
	ActionSave->setEnabled(false);
	connect(ActionSave, &QAction::triggered, this, &ISNoteForm::SaveClicked);
	ToolBar->addAction(ActionSave);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	ListWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISNoteForm::ItemSelectionChanged);
	Layout->addWidget(ListWidget);

	Layout->addWidget(ISControls::CreateVerticalLine(this));

	TextEdit = new ISTextEdit(this);
	TextEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	connect(TextEdit, &ISTextEdit::DataChanged, this, &ISNoteForm::TextChanged);
	Layout->addWidget(TextEdit);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISNoteForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);

	Load();
}
//-----------------------------------------------------------------------------
ISNoteForm::~ISNoteForm()
{

}
//-----------------------------------------------------------------------------
void ISNoteForm::ItemSelectionChanged()
{
	QListWidgetItem *CurrentItem = ListWidget->currentItem();
	if (ModificationFlag)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SaveNote").arg(PreviousItem->text())))
		{
			QString Text = TextEdit->GetValue().toString();

			if (SaveText(Items.value(PreviousItem), Text))
			{
				PreviousItem->setToolTip(Text);
				SetFontBoldItem(PreviousItem, false);
				SetModificationFlag(false);
			}
		}
		else
		{
			SetFontBoldItem(CurrentItem, false);
			SetModificationFlag(false);
		}
	}

	SetNoteText(CurrentItem->toolTip());
	TextEdit->SetFocus();
	TextEdit->setEnabled(true);

	ActionEdit->setEnabled(true);
	ActionDelete->setEnabled(true);

	PreviousItem = CurrentItem;
}
//-----------------------------------------------------------------------------
void ISNoteForm::TextChanged()
{
	SetModificationFlag(true);
}
//-----------------------------------------------------------------------------
void ISNoteForm::Load()
{
	ISQuery qSelect(QS_NOTE);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int NoteID = qSelect.ReadColumn("note_id").toInt();
			QString Name = qSelect.ReadColumn("note_name").toString();
			QString Text = qSelect.ReadColumn("note_text").toString();

			QListWidgetItem *ListWidgetItem = AddItem(Name, Text);
			Items.insert(ListWidgetItem, NoteID);
		}
	}

	TextEdit->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISNoteForm::Reload()
{
	ListWidget->Clear();
	Items.clear();
	Load();
}
//-----------------------------------------------------------------------------
void ISNoteForm::Create()
{
	QString Name = ISInputDialog::GetString(this, LANG("NewRecord"), LANG("Named")).toString();
	if (Name.length())
	{
		ISQuery qInsert(QI_NOTE);
		qInsert.BindValue(":Name", Name);
		if (qInsert.ExecuteFirst())
		{
			int NoteID = qInsert.ReadColumn("note_id").toInt();

			Reload();
			SelectItem(NoteID);
			TextEdit->SetFocus();
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("Named")));
	}
}
//-----------------------------------------------------------------------------
void ISNoteForm::Edit()
{
	QString Name = ISInputDialog::GetString(this, LANG("EditRecord"), LANG("Named") + ':', ListWidget->currentItem()->text()).toString();

	if (Name == ListWidget->currentItem()->text())
	{
		return;
	}

	if (Name.length())
	{
		int NoteID = Items.value(ListWidget->currentItem());

		ISQuery qUpdate(QU_NOTE_NAME);
		qUpdate.BindValue(":Name", Name);
		qUpdate.BindValue(":ObjectID", NoteID);
		if (qUpdate.Execute())
		{
			Reload();
			SelectItem(NoteID);
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("Named")));
	}
}
//-----------------------------------------------------------------------------
void ISNoteForm::Delete()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteNote")))
	{
		ISQuery qDelete(QD_NOTE);
		qDelete.BindValue(":ObjectID", Items.value(ListWidget->currentItem()));
		if (qDelete.Execute())
		{
			Reload();
		}
	}
}
//-----------------------------------------------------------------------------
void ISNoteForm::SaveClicked()
{
	QString Text = TextEdit->GetValue().toString();
	if (SaveText(Items.value(ListWidget->currentItem()), Text))
	{
		SetModificationFlag(false);
		ListWidget->currentItem()->setToolTip(Text);
	}
}
//-----------------------------------------------------------------------------
bool ISNoteForm::SaveText(int NoteID, const QString &Text)
{
	ISQuery qUpdate(QU_NOTE_TEXT);
	qUpdate.BindValue(":Text", Text);
	qUpdate.BindValue(":ObjectID", NoteID);
	return qUpdate.Execute();
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISNoteForm::AddItem(const QString &Name, const QString &Text)
{
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
	ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

	if (Name.length())
	{
		ListWidgetItem->setText(Name);
	}

	if (Text.length())
	{
		ListWidgetItem->setToolTip(Text);
	}

	return ListWidgetItem;
}
//-----------------------------------------------------------------------------
void ISNoteForm::SelectItem(int NoteID)
{
	if (NoteID)
	{
		for (const auto &MapItem : Items.toStdMap())
		{
			QListWidgetItem *ListWidgetItem = MapItem.first;
			int ID = MapItem.second;

			if (ID == NoteID)
			{
				ListWidget->setCurrentItem(ListWidgetItem);
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISNoteForm::SetFontBoldItem(QListWidgetItem *Item, bool Bold)
{
	QFont Font = Item->font();
	Font.setBold(Bold);
	Item->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISNoteForm::SetNoteText(const QString &Text)
{
	disconnect(TextEdit, &ISTextEdit::DataChanged, this, &ISNoteForm::TextChanged);
	TextEdit->SetValue(Text);
	connect(TextEdit, &ISTextEdit::DataChanged, this, &ISNoteForm::TextChanged);
}
//-----------------------------------------------------------------------------
void ISNoteForm::SetModificationFlag(bool modification_flag)
{
	ModificationFlag = modification_flag;
	ActionSave->setEnabled(modification_flag);
	SetFontBoldItem(ListWidget->currentItem(), modification_flag);
}
//-----------------------------------------------------------------------------
void ISNoteForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
