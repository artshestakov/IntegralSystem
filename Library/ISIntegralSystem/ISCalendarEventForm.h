#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceForm.h"
#include "ISComboTimeEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISCalendarEventForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void ClosedEvent();

public:
	ISCalendarEventForm(int calendar_id, QWidget *parent = 0);
	virtual ~ISCalendarEventForm();

protected:
	void AfterShowEvent() override;
	void closeEvent(QCloseEvent *e);
	void Defer();
	void OpenCard();
	void EventClose(); //Завершение события

private:
	int CalendarID;
	QString TableName;
	int ObjectID;

	ISComboTimeEdit *ComboEdit;

	QTimer *TimerSound;

	bool CloseEvent;
	QSound *Sound;
};
//-----------------------------------------------------------------------------
