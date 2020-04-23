#include "ISDeviceUserObjectForm.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_DEVICE_EXIST = PREPARE_QUERY("SELECT COUNT(*) "
											   "FROM _deviceuser "
											   "WHERE dvus_user = currentuserid() "
											   "AND dvus_type = :TypeID "
											   "AND dvus_device = :DeviceID");
//-----------------------------------------------------------------------------
ISDeviceUserObjectForm::ISDeviceUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	SetVisibleDelete(false);
	SetVisibleDeleteCascade(false);

	EditType = dynamic_cast<ISListEdit*>(GetFieldWidget("Type"));
	connect(EditType, &ISFieldEditBase::ValueChange, this, &ISDeviceUserObjectForm::TypeChanged);

	EditDevice = dynamic_cast<ISListEdit*>(GetFieldWidget("Device"));
	connect(EditDevice, &ISFieldEditBase::ValueChange, this, &ISDeviceUserObjectForm::DeviceChanged);

	EditName = dynamic_cast<ISLineEdit*>(GetFieldWidget("Name"));
}
//-----------------------------------------------------------------------------
ISDeviceUserObjectForm::~ISDeviceUserObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISDeviceUserObjectForm::Save()
{
	if (GetFormType() == ISNamespace::OFT_New)
	{
		ISQuery qSelectExist(QS_DEVICE_EXIST);
		qSelectExist.BindValue(":TypeID", GetFieldValue("Type"));
		qSelectExist.BindValue(":DeviceID", GetFieldValue("Device"));
		if (qSelectExist.ExecuteFirst())
		{
			if (qSelectExist.ReadColumn("count").toInt())
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.DeviceAlreadyExist"));
				return false;
			}
		}
	}

	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
void ISDeviceUserObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	if (GetFormType() == ISNamespace::OFT_New)
	{
		EditDevice->setEnabled(false);
		EditName->setEnabled(false);
	}
	else if (GetFormType() == ISNamespace::OFT_Edit)
	{
		EditType->setEnabled(false);
		EditDevice->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISDeviceUserObjectForm::TypeChanged(const QVariant &value)
{
	if (value.isValid())
	{
		EditDevice->SetSqlFilter(QString("dvce_type = (SELECT dvtp_uid FROM _devicetype WHERE dvtp_id = %1)").arg(value.toInt()));
		EditDevice->setEnabled(true);
	}
	else
	{
		EditDevice->ClearSqlFilter();
		EditDevice->setEnabled(false);
	}

	EditDevice->Clear();
}
//-----------------------------------------------------------------------------
void ISDeviceUserObjectForm::DeviceChanged(const QVariant &value)
{
	if (value.isValid())
	{
		EditName->setEnabled(true);
	}
	else
	{
		EditName->setEnabled(false);
		EditName->Clear();
	}
}
//-----------------------------------------------------------------------------
