#include "ISCalendarWidget.h"
#include "ISButtons.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISDefinesGui.h"
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
	Palette.setColor(NavigationBar->backgroundRole(), DEFINES_GUI.COLOR_MAIN_MENU_BAR);
	NavigationBar->setPalette(Palette);
	
	QIcon IconPrevMouth = BUFFER_ICONS("CalendarWidget.Button.Left");
	IconPrevMouth.addPixmap(BUFFER_ICONS("CalendarWidget.Button.Left.Active").pixmap(DEFINES_GUI.SIZE_64_64), QIcon::Active);

	ButtonPrevMouth = findChild<QToolButton*>("qt_calendar_prevmonth");
	ButtonPrevMouth->setIcon(IconPrevMouth);
	ButtonPrevMouth->setToolTip(LANG("PrevMouth"));
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

		if (Action->data().toInt() == QDate::currentDate().month())
		{
			Action->setChecked(true);
			Action->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
		}

		MonthID++;
	}

	ButtonYear = findChild<QToolButton*>("qt_calendar_yearbutton");
	ButtonYear->setCursor(CURSOR_POINTING_HAND);
	ButtonYear->setStyleSheet(STYLE_SHEET("ISCalendarWidget.ButtonYear"));

	QIcon IconNextMouth = BUFFER_ICONS("CalendarWidget.Button.Right");
	IconNextMouth.addPixmap(BUFFER_ICONS("CalendarWidget.Button.Right.Active").pixmap(DEFINES_GUI.SIZE_64_64), QIcon::Active);
	
	ButtonNextMouth = findChild<QToolButton*>("qt_calendar_nextmonth");
	ButtonNextMouth->setIcon(IconNextMouth);
	ButtonNextMouth->setToolTip(LANG("NextMouth"));
	ButtonNextMouth->setCursor(CURSOR_POINTING_HAND);

	//Формат заголовка
	QTextCharFormat TextCharFormatHeader;
	TextCharFormatHeader.setBackground(QBrush(DEFINES_GUI.COLOR_BACKGROUND_INTERFACE));
	TextCharFormatHeader.setFontCapitalization(QFont::Capitalize);
	setWeekdayTextFormat(Qt::Monday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Tuesday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Wednesday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Thursday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Friday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Saturday, TextCharFormatHeader);
	setWeekdayTextFormat(Qt::Sunday, TextCharFormatHeader);

	//Формат заголовка
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
	setSelectedDate(QDate::currentDate());
	emit clicked(QDate::currentDate());
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

	if (Date == QDate::currentDate()) //Если рисуется текущая дата
	{
		Painter->fillRect(Rect, DEFINES_GUI.COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //Заполнение фона

		//Рисование рамки
		Painter->setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
		Painter->setPen(QPen(DEFINES_GUI.COLOR_MAIN_MENU_BAR, 2, Qt::SolidLine));
		Painter->drawRect(QRect(Rect.x(), Rect.y(), Rect.width(), Rect.height()));
	}
	else
	{
		if (Date == selectedDate()) //Есл рисуется выделенная пользоваелем дата
		{
			Painter->fillRect(Rect, DEFINES_GUI.COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //Заполнение фона
		}
		else
		{
			Painter->fillRect(Rect, Qt::white); //Заполнение фона
		}
	}

	if (Date.month() != monthShown()) //Если рисуются дни НЕ ТЕКУЩЕГО месяца
	{
		Painter->fillRect(Rect, DEFINES_GUI.COLOR_CALENDAR_CELL_BACKGROUP); //Заполнение фона
	}

	QString Text = QString::number(Date.day()); //Текст
	
	QRect RectText(Rect.x() + 3, Rect.y() + 2, Rect.width(), Rect.height()); //Расположение текста
	Painter->drawText(RectText, Text); //Рисование текста с датой

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
		Action->setFont(DEFINES_GUI.FONT_APPLICATION);
		Action->setChecked(false);
	}

	ActionClicked->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	ActionClicked->setChecked(true);
}
//-----------------------------------------------------------------------------
