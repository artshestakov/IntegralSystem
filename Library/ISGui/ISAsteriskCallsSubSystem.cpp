#include "ISAsteriskCallsSubSystem.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISTcpQuery.h"
#include "ISAudioPlayerForm.h"
#include "ISDialogsCommon.h"
#include "ISProcessForm.h"
//-----------------------------------------------------------------------------
ISAsteriskCallsSubSystem::ISAsteriskCallsSubSystem(QWidget *parent) : ISListBaseForm("_AsteriskCalls", parent)
{
	QAction *ActionPlay = new QAction(BUFFER_ICONS("AudioPlayer"), LANG("PlayRecordCall"), GetToolBar());
	connect(ActionPlay, &QAction::triggered, this, &ISAsteriskCallsSubSystem::PlayRecordCall);
	AddAction(ActionPlay, true, true);
}
//-----------------------------------------------------------------------------
ISAsteriskCallsSubSystem::~ISAsteriskCallsSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISAsteriskCallsSubSystem::PlayRecordCall()
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
