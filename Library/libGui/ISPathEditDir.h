#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISPathEditDir : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditDir(QWidget *parent);
	virtual ~ISPathEditDir();

	void SetValue(const QVariant &value) override;
	void Clear() override;

protected:
	void SelectDir();
	void OpenDir();

private:
	ISServiceButton *ButtonOpenDir;
};
//-----------------------------------------------------------------------------
