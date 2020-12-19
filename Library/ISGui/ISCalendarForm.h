#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISCalendarPanel.h"
#include "ISCalendarEventItem.h"
#include "ISCalendarEventsWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISCalendarForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCalendarForm(QWidget *parent = 0);
	virtual ~ISCalendarForm();

	void Invoke() override;

protected:
	void ReloadEvents(int Year, int Month); //Загрузка событий на месяц
	void SelectedDateChanged(); //Событие смены выбранной даты

	void Create(); //Создание нового события календаря
	void DateTo(); //Переход на дату календаря
	void ToCurrentDate(); //Переход на текущую дату
	void ShowSettingsForm();
	void Edit(); //Изменение события календаря
	void Delete(); //Удаление события календаря
	void CloseEvent(); //Завершение события
	
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem); //Двойное нажатие на событие
	void ItemSelectionChanged();

	void EditEvent(int CalendarID);
	void DeleteEvent(int CalendarID);

	ISCalendarEventItem* AddEventItem(int CalendarID, const QString &Name, const QString &Text, const QTime &Time, bool Closed); //Добавить элемент события в список
	
private:
	bool FirstUpdate;
	std::map<unsigned int, QVariantList> EventsMap;

private:
	QHBoxLayout *MainLayout;
	
	ISCalendarPanel *CalendarPanel;
	QLabel *LabelDayNumber;
	QLabel *LabelDayName;
	QLabel *LabelMonthYear;

	QAction *ActionEdit;
	QAction *ActionDelete;
	QAction *ActionCloseEvent;

	ISCalendarEventsWidget *ListWidget;
};
//-----------------------------------------------------------------------------
