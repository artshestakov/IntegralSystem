#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISUuidEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUuidEdit(QWidget *parent = 0);
	virtual ~ISUuidEdit();

	void SetValue(const QVariant &value) override;
	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
