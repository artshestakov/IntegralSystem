#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISCalendarEventsWidget : public ISListWidget
{
    Q_OBJECT

public:
    ISCalendarEventsWidget(QWidget *parent = 0);
    virtual ~ISCalendarEventsWidget();

protected:
    void paintEvent(QPaintEvent *e) override;
    void RowsInserted(const QModelIndex &ModelIndex, int First, int Last);
    void RowsRemoved(const QModelIndex &ModelIndex, int First, int Last);

private:
    QLabel *LabelEmpty;
};
//-----------------------------------------------------------------------------
