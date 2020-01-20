#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISAddressBox : public QComboBox
{
	Q_OBJECT

signals:
	void ChangeValue(const QVariant &Value);

public:
	ISAddressBox(QWidget *parent = 0);
	virtual ~ISAddressBox();

	void Clear();

protected:
	void ClearClicked();
	void Activated(int Index);

private:
	QVariant CurrentValue;
};
//-----------------------------------------------------------------------------
