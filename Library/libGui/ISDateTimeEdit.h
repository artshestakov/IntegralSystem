#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISCalendarPopup.h"
#include "ISQDateTimeEdit.h"
//-----------------------------------------------------------------------------
class ISDateTimeEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDateTimeEdit(QWidget *parent);
	virtual ~ISDateTimeEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;
	virtual void SetVisibleClear(bool Visible) override;

	void SetDisplayFormat(const QString &DisplayFormat);
	void SetVisibleCheck(bool Visible);
	void SetCheckEnable(Qt::CheckState Check);
	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);
	void SetMinimumDateTime(const QDateTime &DateTime);
	void SetMaximumDateTime(const QDateTime &DateTime);

public slots:
	void SetReadOnly(bool ReadOnly);

protected:
	QCheckBox* GetCheckEnable();
	QToolButton* GetButtonCalendar();

	void SetDate(const QDate &Date);
	void SetTime(const QTime &Time);

	virtual void DateEnableChanged(int State);
	void ShowCalendar();
	void HideCalendar();

	void SetVisibleCalendar(bool visible);
	void SetSelectSection(ISQDateTimeEdit::Section select_section);

private:
	QCheckBox *CheckEnable;
	ISQDateTimeEdit *DateTimeEdit;
	
	QToolButton *ButtonCalendar;
	ISCalendarPopup *CalendarWidget;
	QPropertyAnimation *AnimationCalendar;

	ISQDateTimeEdit::Section SelectSection;
	bool VisibleCalendar;
};
//-----------------------------------------------------------------------------
