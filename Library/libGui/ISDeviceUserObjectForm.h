#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
#include "ISListEdit.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDeviceUserObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISDeviceUserObjectForm();

	bool Save() override;

protected:
	void AfterShowEvent();
	void TypeChanged(const QVariant &value);
	void DeviceChanged(const QVariant &value);

private:
	ISListEdit *EditType;
	ISListEdit *EditDevice;
	ISLineEdit *EditName;
};
//-----------------------------------------------------------------------------
