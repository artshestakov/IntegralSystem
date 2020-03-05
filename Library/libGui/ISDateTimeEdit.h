#pragma once
#ifndef _CLDATETIMEEDIT_H_INCLUDED
#define _CLDATETIMEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISFieldEditBase.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDateTimeEdit : public ISFieldEditBase
{
    Q_OBJECT

signals:
	void DateTimeChanged();

public:
    Q_INVOKABLE ISDateTimeEdit(QWidget *parent = 0);
    virtual ~ISDateTimeEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;
	virtual void SetVisibleClear(bool Visible) override;

    void SetDate(const QDate &Date);
    void SetTime(const QTime &Time);
	void SetRangeDate(const QDate &Minimum, const QDate &Maximum);
	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);

protected:
	void SetVisibleDateEdit(bool Visible);
	void SetVisibleTimeEdit(bool Visible);

private:
    ISQDateEdit *DateEdit;
    ISQTimeEdit *TimeEdit;
};
//-----------------------------------------------------------------------------
#endif
