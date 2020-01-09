#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISOrganizationScoreObjectForm : public ISObjectFormBase
{
	Q_OBJECT
		
public:
	Q_INVOKABLE ISOrganizationScoreObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOrganizationScoreObjectForm();

	void SetOrganization(int OrganizationID);

protected:
	bool Save() override;
};
//-----------------------------------------------------------------------------
