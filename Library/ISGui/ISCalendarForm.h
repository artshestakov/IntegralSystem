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
	void ReloadEvents(int Year, int Month); //�������� ������� �� �����
	void SelectedDateChanged(); //������� ����� ��������� ����

	void Create(); //�������� ������ ������� ���������
	void DateTo(); //������� �� ���� ���������
	void ToCurrentDate(); //������� �� ������� ����
	void ShowSettingsForm();
	void Edit(); //��������� ������� ���������
	void Delete(); //�������� ������� ���������
	void CloseEvent(); //���������� �������
	void DateEvent(); //������� �� ���� ���������� �������
	
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem); //������� ������� �� �������
	void ItemSelectionChanged();

	void EditEvent(int CalendarID);
	void DeleteEvent(int CalendarID);

	void SearchChanged(const QVariant &value);
	ISCalendarEventItem* AddEventItem(int CalendarID, const QString &Name, const QString &Text, const QTime &Time, bool Closed); //�������� ������� ������� � ������
	
private:
	bool FirstUpdate;
	std::map<unsigned int, QVariantList> EventsMap;

private:
	QHBoxLayout *MainLayout;
	
	ISCalendarPanel *CalendarPanel;
	QLabel *LabelDayNumber;
	QLabel *LabelDayName;
	QLabel *LabelMonthYear;

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
};
//-----------------------------------------------------------------------------
