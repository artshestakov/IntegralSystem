#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCalendarEventItem : public QWidget
{
    Q_OBJECT

signals:
    void SizeHintChanged();

public:
    ISCalendarEventItem(int calendar_id, const QString &Name, const QString &Text, const QTime &Time, bool closed, QWidget *parent = 0);
    virtual ~ISCalendarEventItem();

    int GetCalendarID() const;
    bool GetClosed() const;

    void SetVisibleDate(const QDate &Date);

private:
    int CalendarID;
    bool Closed;

    QLabel *LabelDate;
};
//-----------------------------------------------------------------------------
