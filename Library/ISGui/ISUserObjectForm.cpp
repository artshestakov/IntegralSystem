#include "ISUserObjectForm.h"
#include "ISConstants.h"
#include "ISControls.h"
#include "ISUserListForm.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISSystem.h"
#include "ISUserPhotoCreator.h"
//-----------------------------------------------------------------------------
ISUserObjectForm::ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	EditAccountLifeTime = GetFieldWidget("AccountLifeTime");
	connect(EditAccountLifeTime, &ISFieldEditBase::DataChanged, this, &ISUserObjectForm::AccountLifeTimeChanged);

	EditAccountLifeTimeStart = GetFieldWidget("AccountLifeTimeStart");
	EditAccountLifeTimeStart->setEnabled(false);

	EditAccountLifeTimeEnd = GetFieldWidget("AccountLifeTimeEnd");
	EditAccountLifeTimeEnd->setEnabled(false);
}
//-----------------------------------------------------------------------------
ISUserObjectForm::~ISUserObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISUserObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	if (GetFormType() == ISNamespace::OFT_Edit && EditAccountLifeTime->GetValue().toBool())
	{
		EditAccountLifeTimeStart->setEnabled(true);
		EditAccountLifeTimeEnd->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
bool ISUserObjectForm::Save()
{
	//�������� �������� ������� ������ � ������
	bool Result = GetFieldValue("Group").isValid();
	if (!Result)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.UserNotLinkedToGroup"));
		GetFieldWidget("Group")->BlinkRed();
		return Result;
	}

	//�������� ������������ ����� ��������� ����� �������� ������� ������
	if (EditAccountLifeTime->GetValue().toBool())
	{
		QDate DateStart = EditAccountLifeTimeStart->GetValue().toDate();
		QDate DateEnd = EditAccountLifeTimeEnd->GetValue().toDate();
		if (DateStart > DateEnd || DateEnd < DateStart || !DateStart.isValid() || !DateEnd.isValid())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.InvalidAccountLifeTimeDatesUser"));
			EditAccountLifeTimeStart->BlinkRed();
			EditAccountLifeTimeEnd->BlinkRed();
		}
	}

	if (!GetFieldValue("Photo").isValid())
	{
		SetFieldValue("Photo", ISGui::PixmapToByteArray(ISUserPhotoCreator().Create(GetFieldValue("FIO").toString())));
	}

	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
	{
		Result = ISObjectFormBase::Save();
		if (Result)
		{
			bool PasswordCreated = false;
			ISGui::ShowUserPasswordForm(GetObjectID(), GetFieldValue("FIO").toString(), GetFieldValue("Login").toString(), PasswordCreated);
			if (!PasswordCreated)
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.UserPasswordNotCreated"));
			}
		}
	}
	else
	{
		Result = ISObjectFormBase::Save();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::AccountLifeTimeChanged()
{
	bool Enabled = EditAccountLifeTime->GetValue().toBool();
	EditAccountLifeTimeStart->setEnabled(Enabled);
	EditAccountLifeTimeEnd->setEnabled(Enabled);
	if (!Enabled)
	{
		EditAccountLifeTimeStart->Clear();
		EditAccountLifeTimeEnd->Clear();
	}
}
//-----------------------------------------------------------------------------
