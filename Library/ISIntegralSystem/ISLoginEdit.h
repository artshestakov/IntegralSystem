#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISLoginEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISLoginEdit(QWidget *parent);
	virtual ~ISLoginEdit();

	bool IsValid() const override;

protected:
	void LoginChanged();

private:
	bool Valid;
};
//-----------------------------------------------------------------------------
