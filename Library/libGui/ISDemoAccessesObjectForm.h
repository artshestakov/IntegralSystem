#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDemoAccessesObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemoAccessesObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISDemoAccessesObjectForm();

	void SetOrganizationID(int OrganizationID);

protected:
	void AfterShowEvent() override;
	bool Save() override;

private:
	QVariant Result;
};
//-----------------------------------------------------------------------------
