#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISDispatchOrganizationListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDispatchOrganizationListForm(QWidget *parent = 0);
	virtual ~ISDispatchOrganizationListForm();

	void LoadData() override;

protected:
	void Picking();
	void Matched(const QVectorInt &Vector);
};
//-----------------------------------------------------------------------------
