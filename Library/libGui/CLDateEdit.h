#pragma once
#ifndef _CLDATEEDIT_H_INCLUDED
#define _CLDATEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
//#include "StdAfx.h"
#include "libGui_global.h"
//#include "CLLineEdit.h"
#include "ISEdits.h"
//#include "CLServiceButton.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT CLDateEdit : public ISQLineEdit
{
    Q_OBJECT

signals:
	void DateChanged();

public:
    CLDateEdit(QWidget *parent = 0);
    virtual ~CLDateEdit();

    QDate GetDate() const;
    void SetDate(const QDate &Date);
    void Clear();

	void SetRange(const QDate &Minimum, const QDate &Maximum);

protected:
    void resizeEvent(QResizeEvent *ResizeEvent);

    void ShowCalendar();
    void SelectionChanged();

private:
    ISServiceButton *ButtonCalendar;
    QCalendarWidget *CalendarWidget;
};
//-----------------------------------------------------------------------------
#endif
