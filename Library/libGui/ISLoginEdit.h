#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISLoginEdit : public ISLineEdit
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
