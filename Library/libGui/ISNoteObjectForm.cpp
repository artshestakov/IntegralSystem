#include "ISNoteObjectForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISPushButton.h"
#include "ISButtonClose.h"
#include "ISQuery.h"
#include "ISProtocol.h"
#include "ISMetaData.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_NOTE_OBJECT = PREPARE_QUERY("SELECT COUNT(*) FROM _noteobject WHERE nobj_tablename = :TableName AND nobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_NOTE = PREPARE_QUERY("SELECT nobj_note FROM _noteobject WHERE nobj_tablename = :TableName AND nobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QU_NOTE = PREPARE_QUERY("UPDATE _noteobject SET nobj_note = :Note WHERE nobj_tablename = :TableName AND nobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QI_NOTE = PREPARE_QUERY("INSERT INTO _noteobject(nobj_tablename, nobj_objectid, nobj_note) VALUES(:TableName, :ObjectID, :Note)");
//-----------------------------------------------------------------------------
ISNoteObjectForm::ISNoteObjectForm(const QString &table_name, int object_id, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	TableName = table_name;
	ObjectID = object_id;

	setWindowTitle(LANG("Note"));
	setWindowIcon(BUFFER_ICONS("NoteObject"));

	ForbidResize();

	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);

	TextEdit = new ISTextEdit(this);
	GetMainLayout()->addWidget(TextEdit);

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->addStretch();
	GetMainLayout()->addLayout(Layout);

	ISPushButton *ButtonSave = new ISPushButton(this);
	ButtonSave->setText(LANG("Save"));
	ButtonSave->setIcon(BUFFER_ICONS("Save"));
	connect(ButtonSave, &ISPushButton::clicked, this, &ISNoteObjectForm::Save);
	Layout->addWidget(ButtonSave);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISNoteObjectForm::close);
	Layout->addWidget(ButtonClose);

	LoadNote();
}
//-----------------------------------------------------------------------------
ISNoteObjectForm::~ISNoteObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISNoteObjectForm::Save()
{
	ISQuery qSelectCount(QS_NOTE_OBJECT);
	qSelectCount.BindValue(":TableName", TableName.toLower());
	qSelectCount.BindValue(":ObjectID", ObjectID);
	if (qSelectCount.ExecuteFirst())
	{
		int Count = qSelectCount.ReadColumn("count").toInt();
		if (Count)
		{
			ISQuery qUpdate(QU_NOTE);
			qUpdate.BindValue(":Note", TextEdit->GetValue());
			qUpdate.BindValue(":TableName", TableName.toLower());
			qUpdate.BindValue(":ObjectID", ObjectID);
			if (qUpdate.Execute())
			{
				ISProtocol::Insert(true, CONST_UID_PROTOCOL_EDIT_NOTE_RECORD, TableName, ISMetaData::GetInstanse().GetMetaTable(TableName)->GetLocalListName(), ObjectID);
				close();
			}
		}
		else
		{
			ISQuery qInsert(QI_NOTE);
			qInsert.BindValue(":Note", TextEdit->GetValue());
			qInsert.BindValue(":TableName", TableName.toLower());
			qInsert.BindValue(":ObjectID", ObjectID);
			if (qInsert.Execute())
			{
				ISProtocol::Insert(true, CONST_UID_PROTOCOL_ADD_NOTE_RECORD, TableName, ISMetaData::GetInstanse().GetMetaTable(TableName)->GetLocalListName(), ObjectID);
				close();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISNoteObjectForm::LoadNote()
{
	ISQuery qSelectCount(QS_NOTE_OBJECT);
	qSelectCount.BindValue(":TableName", TableName.toLower());
	qSelectCount.BindValue(":ObjectID", ObjectID);
	if (qSelectCount.ExecuteFirst())
	{
		int Count = qSelectCount.ReadColumn("count").toInt();
		if (Count)
		{
			ISQuery qSelectNote(QS_NOTE);
			qSelectNote.BindValue(":TableName", TableName.toLower());
			qSelectNote.BindValue(":ObjectID", ObjectID);
			if (qSelectNote.ExecuteFirst())
			{
				QString Note = qSelectNote.ReadColumn("nobj_note").toString();
				TextEdit->SetValue(Note);
			}
		}
	}
}
//-----------------------------------------------------------------------------
