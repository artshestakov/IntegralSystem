#include "ISAsteriskCallsListForm.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISTcpQuery.h"
#include "ISAudioPlayerForm.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISAsteriskCallsListForm::ISAsteriskCallsListForm(QWidget *parent) : ISListBaseForm("_AsteriskCalls", parent)
{
	QAction *ActionPlay = new QAction(BUFFER_ICONS("AudioPlayer"), LANG("PlayRecordCall"), GetToolBar());
	connect(ActionPlay, &QAction::triggered, this, &ISAsteriskCallsListForm::PlayRecordCall);
	AddAction(ActionPlay, true, true);
}
//-----------------------------------------------------------------------------
ISAsteriskCallsListForm::~ISAsteriskCallsListForm()
{

}
//-----------------------------------------------------------------------------
void ISAsteriskCallsListForm::PlayRecordCall()
{
	ISTcpQuery qGetRecordCall(API_GET_RECORD_CALL);
	qGetRecordCall.BindValue("RecordID", GetObjectID());
	if (qGetRecordCall.Execute())
	{
		ISAudioPlayerForm *AudioPlayerForm = new ISAudioPlayerForm();
		AudioPlayerForm->SetMedia(QByteArray::fromBase64(qGetRecordCall.GetAnswer()["Data"].toByteArray()));
		AudioPlayerForm->show();
	}
	else
	{
		ISMessageBox::ShowWarning(this, qGetRecordCall.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
