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

	QWidget* GetNavigationBar(); //Получить указатель на панель навигации
	QToolButton* GetButtonPrevMonth(); //Получить указатель на кнопку "Предыдущий месяц"
	QToolButton* GetButtonNextMonth(); //Получить указатель на кнопку "Следующий месяц"
	QToolButton* GetButtonMonth(); //Получить указатель на кнопку со списком месяцев
	QVBoxLayout* GetLayout(); //Получить указатель на компоновщик

	void Today(); //Перейти к сегоднящней дате
	void SelectionChanged();
	void CurrentPageChanged(int Year, int Month);

protected:
	void paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const;
	void hideEvent(QHideEvent *e);
	void MonthClicked(QAction *ActionClicked);

private:
	QVBoxLayout *Layout;
	QWidget *NavigationBar; //Указатель на панель навигации
	QToolButton *ButtonPrevMouth; //Указатель на кнопку "Предыдущий месяц"
	QToolButton *ButtonMonth; //Указатель на кнопку со списком месяцев
	QToolButton *ButtonYear; //Указатель на кнопку года
	QToolButton *ButtonNextMouth; //Указатель на кнопку "Следующий месяц"
};
//-----------------------------------------------------------------------------
