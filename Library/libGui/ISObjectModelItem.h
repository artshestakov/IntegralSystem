#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISObjectModelItem : public QObject
{
	Q_OBJECT

public:
	ISObjectModelItem(QObject *parent = 0);
	virtual ~ISObjectModelItem();

	void SetOldValue(const QVariant &old_value);
	QVariant GetOldValue() const;

	void SetNewValue(const QVariant &new_value);
	QVariant GetNewValue() const;

private:
	QVariant OldValue;
	QVariant NewValue;
};
//-----------------------------------------------------------------------------
