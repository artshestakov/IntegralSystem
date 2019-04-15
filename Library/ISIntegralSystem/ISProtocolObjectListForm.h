#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISProtocolListForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISProtocolObjectListForm : public ISProtocolListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISProtocolObjectListForm(QWidget *parent = 0);
	virtual ~ISProtocolObjectListForm();

protected:
	virtual void LoadDataAfterEvent() override;
	void LoadData() override;
};
//-----------------------------------------------------------------------------
