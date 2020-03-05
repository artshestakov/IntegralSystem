#pragma once
#ifndef _CLTIMEEDIT_H_INCLUDED
#define _CLTIMEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class CLTimeEdit : public QWidget
{
    Q_OBJECT

signals:
	void TimeChanged();

public:
    CLTimeEdit(QWidget *parent = 0);
    virtual ~CLTimeEdit();

    QTime GetTime() const;
    void SetTime(const QTime &Time);
    void Clear();

private:
    ISQComboBox *ComboBoxHour;
    ISQComboBox *ComboBoxMinute;
    ISQComboBox *ComboBoxSecond;
};
//-----------------------------------------------------------------------------
#endif
