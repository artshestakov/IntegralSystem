#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISObjectModelItem
{
public:
	ISObjectModelItem();
	~ISObjectModelItem();

	void SetOldValue(const QVariant &old_value);
	QVariant GetOldValue() const;

	void SetNewValue(const QVariant &new_value);
	QVariant GetNewValue() const;

private:
	QVariant OldValue;
	QVariant NewValue;
};
//-----------------------------------------------------------------------------
