#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCalendarWidget : public QCalendarWidget
{
	Q_OBJECT

signals:
	void Hide();
	void DateChanged(const QDate &Date);

public:
	ISCalendarWidget(QWidget *parent = 0);
	virtual ~ISCalendarWidget();

	QWidget* GetNavigationBar(); //�������� ��������� �� ������ ���������
	QToolButton* GetButtonPrevMonth(); //�������� ��������� �� ������ "���������� �����"
	QToolButton* GetButtonNextMonth(); //�������� ��������� �� ������ "��������� �����"
	QToolButton* GetButtonMonth(); //�������� ��������� �� ������ �� ������� �������
	QVBoxLayout* GetLayout(); //�������� ��������� �� �����������

	void Today(); //������� � ����������� ����
	void SelectionChanged();
	void CurrentPageChanged(int Year, int Month);

protected:
	void paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const;
	void hideEvent(QHideEvent *e);
	void MonthClicked(QAction *ActionClicked);

private:
	QVBoxLayout *Layout;
	QWidget *NavigationBar; //��������� �� ������ ���������
	QToolButton *ButtonPrevMouth; //��������� �� ������ "���������� �����"
	QToolButton *ButtonMonth; //��������� �� ������ �� ������� �������
	QToolButton *ButtonYear; //��������� �� ������ ����
	QToolButton *ButtonNextMouth; //��������� �� ������ "��������� �����"
};
//-----------------------------------------------------------------------------
