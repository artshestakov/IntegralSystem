#include "ISAsteriskCallsListForm.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISTcpQuery.h"
#include "ISAudioPlayerForm.h"
#include "ISMessageBox.h"
#include "ISProcessForm.h"
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

	ISProcessForm ProcessForm(LANG("RecordCallLoading"));
	ProcessForm.show();
	if (qGetRecordCall.Execute())
	{
		ISAudioPlayerForm::Instance().SetMedia(QByteArray::fromBase64(qGetRecordCall.GetAnswer()["Data"].toByteArray()));
		ISAudioPlayerForm::Instance().show();
		ProcessForm.hide();
	}
	else
	{
		ProcessForm.hide();
		ISMessageBox::ShowWarning(this, qGetRecordCall.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
