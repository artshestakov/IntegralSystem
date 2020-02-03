#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISUuidEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUuidEdit(QWidget *parent = 0);
	virtual ~ISUuidEdit();

	void SetValue(const QVariant &value) override;
	bool IsValid() const override;

private:
	void Generate();
};
//-----------------------------------------------------------------------------
