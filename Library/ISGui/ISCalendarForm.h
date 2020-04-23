#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISCalendarPanel.h"
#include "ISCalendarDayWidget.h"
#include "ISCalendarEventItem.h"
#include "ISLineEdit.h"
#include "ISCalendarEventsWidget.h"
//-----------------------------------------------------------------------------
class ISCalendarForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCalendarForm(QWidget *parent = 0);
	virtual ~ISCalendarForm();

	void Invoke() override;

protected:
	void ShowOverdueEvents(); //Показ просроченных событий
	void SelectedDateChanged(); //Событие смены выбранной даты

	void Create(); //Создание нового события календаря
	void DateTo(); //Переход на дату календаря
	void ToCurrentDate(); //Переход на текущую дату
	void ShowSettingsForm();
	void Edit(); //Изменение события календаря
	void Delete(); //Удаление события календаря
	void CloseEvent(); //Завершение события
	void DateEvent(); //Переход на дату выбранного события
	
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem); //Двойное нажатие на событие
	void ItemSelectionChanged();

	void EditEvent(int CalendarID);
	void DeleteEvent(int CalendarID);

	void SearchChanged(const QVariant &value);
	ISCalendarEventItem* AddEventItem(int CalendarID, const QString &Name, const QString &Text, const QTime &Time, bool Closed); //Добавить элемент события в список
	
	void CalendarEvent(const QVariantMap &VariantMap); //Показать форму события
	void ShowEventForm(int CalendarID);

private:
	QHBoxLayout *MainLayout;
	
	ISCalendarPanel *CalendarPanel;
	ISCalendarDayWidget *SelectedDayWidget;

	QAction *ActionCreate;
	QAction *ActionDateTo;
	QAction *ActionToday;
	QAction *ActionSettings;
	QAction *ActionEdit;
	QAction *ActionDelete;
	QAction *ActionCloseEvent;
	QAction *ActionDateEvent;

	ISLineEdit *EditSearch;
	QGroupBox *GroupBox;
	ISCalendarEventsWidget *ListWidget;
	bool FirstUpdate;
};
//-----------------------------------------------------------------------------
