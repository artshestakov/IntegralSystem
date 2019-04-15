#include "StdAfx.h"
#include "ISCalendarWidget.h"
#include "ISPushButton.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "EXDefines.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISCalendarWidget::ISCalendarWidget(QWidget * parent) : QCalendarWidget(parent)
{
	setGridVisible(true);
	setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
	setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
	setCursor(CURSOR_POINTING_HAND);
	setDateEditEnabled(true);
	setFirstDayOfWeek(Qt::Monday);

	connect(this, &ISCalendarWidget::selectionChanged, this, &ISCalendarWidget::SelectionChanged);
	connect(this, &ISCalendarWidget::currentPageChanged, this, &ISCalendarWidget::CurrentPageChanged);

	Layout = dynamic_cast<QVBoxLayout*>(layout());
	NavigationBar = findChild<QWidget*>("qt_calendar_navigationbar");
	NavigationBar->setCursor(CURSOR_ARROW);

	QPalette Palette = NavigationBar->palette();
	Palette.setColor(NavigationBar->backgroundRole(), COLOR_MAIN_MENU_BAR);
	NavigationBar->setPalette(Palette);
	
	QIcon IconPrevMouth = BUFFER_ICONS("CalendarWidget.Button.Left");
	IconPrevMouth.addPixmap(BUFFER_ICONS("CalendarWidget.Button.Left.Active").pixmap(SIZE_64_64), QIcon::Active);

	ButtonPrevMouth = findChild<QToolButton*>("qt_calendar_prevmonth");
	ButtonPrevMouth->setIcon(IconPrevMouth);
	ButtonPrevMouth->setToolTip(LOCALIZATION("PrevMouth"));
	ButtonPrevMouth->setCursor(CURSOR_POINTING_HAND);

	ButtonMonth = findChild<QToolButton*>("qt_calendar_monthbutton");
	ButtonMonth->setStyleSheet(STYLE_SHEET("ISCalendarWidget.ButtonMoth"));
	ButtonMonth->setCursor(CURSOR_POINTING_HAND);

	int MonthID = 1;
	for (QAction *Action : ButtonMonth->menu()->actions())
	{
        QString Text = Action->text();
        Text.replace(0, 1, Text.at(0).toUpper());
        Action->setText(QString::number(MonthID) + " - " + Text);

		Action->setCheckable(true);

		if (Action->data().toInt() == DATE_TODAY.month())
		{
			Action->setChecked(true);
			Action->setFont(FONT_APPLICATION_BOLD);
		}

		MonthID++;
	}

	ButtonYear = findChild<QToolButton*>("qt_calendar_yearbutton");
	ButtonYear->setCursor(CURSOR_POINTING_HAND);
	ButtonYear->setStyleSheet(STYLE_SHEET("ISCalendarWidget.ButtonYear"));

	QIcon IconNextMouth = BUFFER_ICONS("CalendarWidget.Button.Right");
	IconNextMouth.addPixmap(BUFFER_ICONS("CalendarWidget.Button.Right.Active").pixmap(SIZE_64_64), QIcon::Active);
	
	ButtonNextMouth = findChild<QToolButton*>("qt_calendar_nextmonth");
	ButtonNextMouth->setIcon(IconNextMouth);
	ButtonNextMouth->setToolTip(LOCALIZATION("NextMouth"));
	ButtonNextMouth->setCursor(CURSOR_POINTING_HAND);

	//������ ���������
	QTextCharFormat TextCharFormatHeader;
	TextCharFormatHeader.setBackground(QBrush(COLOR_BACKGROUND_INTERFACE));
	TextCharFormatHeader.setFontCapitalization(QFont::Capitalize);
	setWeekdayTextFormat(Qt::Monday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Tuesday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Wednesday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Thursday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Friday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Saturday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Sunday, TextCharFormatHeader);

	//������ ���������
	QTextCharFormat HeaderFormat;
	HeaderFormat.setFontWeight(QFont::Bold);
	setHeaderTextFormat(HeaderFormat);
}
//-----------------------------------------------------------------------------
ISCalendarWidget::~ISCalendarWidget()
{

}
//-----------------------------------------------------------------------------
QWidget* ISCalendarWidget::GetNavigationBar()
{
	return NavigationBar;
}
//-----------------------------------------------------------------------------
QToolButton* ISCalendarWidget::GetButtonPrevMonth()
{
	return ButtonPrevMouth;
}
//-----------------------------------------------------------------------------
QToolButton* ISCalendarWidget::GetButtonNextMonth()
{
	return ButtonNextMouth;
}
//-----------------------------------------------------------------------------
QToolButton* ISCalendarWidget::GetButtonMonth()
{
	return ButtonMonth;
}
//-----------------------------------------------------------------------------
QVBoxLayout* ISCalendarWidget::GetLayout()
{
	return Layout;
}
//-----------------------------------------------------------------------------
void ISCalendarWidget::Today()
{
	setSelectedDate(DATE_TODAY);
	emit clicked(DATE_TODAY);
}
//-----------------------------------------------------------------------------
void ISCalendarWidget::SelectionChanged()
{
	emit DateChanged(selectedDate());
}
//-----------------------------------------------------------------------------
void ISCalendarWidget::CurrentPageChanged(int Year, int Month)
{
	for (QAction *ActionMonth : ButtonMonth->menu()->actions())
	{
		if (ActionMonth->data().toInt() == Month)
		{
			MonthClicked(ActionMonth);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISCalendarWidget::paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const
{
	Painter->save();

	if (Date == DATE_TODAY) //���� �������� ������� ����
	{
		Painter->fillRect(Rect, COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //���������� ����

		//��������� �����
		Painter->setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
		Painter->setPen(QPen(COLOR_MAIN_MENU_BAR, 2, Qt::SolidLine));
		Painter->drawRect(QRect(Rect.x(), Rect.y(), Rect.width(), Rect.height()));
	}
	else
	{
		if (Date == selectedDate()) //��� �������� ���������� ������������ ����
		{
			Painter->fillRect(Rect, COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //���������� ����
		}
		else
		{
			Painter->fillRect(Rect, Qt::white); //���������� ����
		}
	}

	if (Date.month() != monthShown()) //���� �������� ��� �� �������� ������
	{
		Painter->fillRect(Rect, COLOR_CALENDAR_CELL_BACKGROUP); //���������� ����
	}

	QString Text = QString::number(Date.day()); //�����
	
	QRect RectText(Rect.x() + 3, Rect.y() + 2, Rect.width(), Rect.height()); //������������ ������
	Painter->drawText(RectText, Text); //��������� ������ � �����

	Painter->restore();
}
//-----------------------------------------------------------------------------
void ISCalendarWidget::hideEvent(QHideEvent *e)
{
	QCalendarWidget::hideEvent(e);
	emit Hide();
}
//-----------------------------------------------------------------------------
void ISCalendarWidget::MonthClicked(QAction *ActionClicked)
{
	for (QAction *Action : ButtonMonth->menu()->actions())
	{
		Action->setFont(FONT_APPLICATION);
		Action->setChecked(false);
	}

	ActionClicked->setFont(FONT_APPLICATION_BOLD);
	ActionClicked->setChecked(true);
}
//-----------------------------------------------------------------------------
