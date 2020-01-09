#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCenterSevenPatientsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenPatientsListForm(QWidget *parent = 0);
	virtual ~ISCenterSevenPatientsListForm();

protected:
	void Send();
	void AddWaitAdmission();
	QString GetSMSTemplate(int TemplateID) const;
};
//-----------------------------------------------------------------------------
