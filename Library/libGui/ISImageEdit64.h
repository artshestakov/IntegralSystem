#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISImageEdit.h"
//-----------------------------------------------------------------------------
class ISImageEdit64 : public ISImageEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISImageEdit64(QWidget *parent);
	virtual ~ISImageEdit64();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
};
//-----------------------------------------------------------------------------
