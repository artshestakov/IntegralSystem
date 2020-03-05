#pragma once
#ifndef _CLDATETIMEEDIT_H_INCLUDED
#define _CLDATETIMEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "CLDateEdit.h"
#include "CLTimeEdit.h"
//-----------------------------------------------------------------------------
class CLDateTimeEdit : public QWidget
{
    Q_OBJECT

signals:
	void DateTimeChanged();

public:
    CLDateTimeEdit(QWidget *parent = 0);
    virtual ~CLDateTimeEdit();

    QDateTime GetDateTime() const;
    void SetDateTime(const QDateTime &DateTime);
    void SetDate(const QDate &Date);
    void SetTime(const QTime &Time);
    void Clear();

    void SetVisibleDateEdit(bool Visible);
    void SetVisibleTimeEdit(bool Visible);

	void SetRangeDate(const QDate &Minimum, const QDate &Maximum);

private:
    CLDateEdit *DateEdit;
    CLTimeEdit *TimeEdit;
};
//-----------------------------------------------------------------------------
#endif
