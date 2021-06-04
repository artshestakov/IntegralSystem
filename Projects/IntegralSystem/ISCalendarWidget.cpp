#include "ISCalendarWidget.h"
#include "ISButtons.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstantsOld.h"
#include "ISConstantsGui.h"
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
    IconPrevMouth.addPixmap(BUFFER_ICONS("CalendarWidget.Button.Left.Active").pixmap(QSize(64, 64)), QIcon::Active);

    ButtonPrevMouth = findChild<QToolButton*>("qt_calendar_prevmonth");
    ButtonPrevMouth->setIcon(IconPrevMouth);
    ButtonPrevMouth->setToolTip(LANG("PrevMouth"));
    ButtonPrevMouth->setCursor(CURSOR_POINTING_HAND);

    ButtonMonth = findChild<QToolButton*>("qt_calendar_monthbutton");
    ButtonMonth->setStyleSheet(BUFFER_STYLE_SHEET("ISCalendarWidget.ButtonMonth"));
    ButtonMonth->setCursor(CURSOR_POINTING_HAND);

    int MonthID = 1;
    for (QAction *Action : ButtonMonth->menu()->actions())
    {
        QString Text = Action->text();
        Text[0] = Text[0].toUpper();
        Action->setText(QString::number(MonthID) + " - " + Text);
        Action->setCheckable(true);

        if (Action->data().toInt() == QDate::currentDate().month())
        {
            Action->setChecked(true);
            Action->setFont(FONT_APPLICATION_BOLD);
        }
        ++MonthID;
    }

    ButtonYear = findChild<QToolButton*>("qt_calendar_yearbutton");
    ButtonYear->setCursor(CURSOR_POINTING_HAND);
    ButtonYear->setStyleSheet(BUFFER_STYLE_SHEET("ISCalendarWidget.ButtonYear"));

    QIcon IconNextMouth = BUFFER_ICONS("CalendarWidget.Button.Right");
    IconNextMouth.addPixmap(BUFFER_ICONS("CalendarWidget.Button.Right.Active").pixmap(QSize(64, 64)), QIcon::Active);

    ButtonNextMouth = findChild<QToolButton*>("qt_calendar_nextmonth");
    ButtonNextMouth->setIcon(IconNextMouth);
    ButtonNextMouth->setToolTip(LANG("NextMouth"));
    ButtonNextMouth->setCursor(CURSOR_POINTING_HAND);

    //Формат заголовка
    QTextCharFormat TextCharFormatHeader;
    TextCharFormatHeader.setBackground(QBrush(QColor(230, 230, 230)));
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
    Q_UNUSED(Year);
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
    if (Date < minimumDate() || Date > maximumDate()) //Если рисуемая дата не попадает в допустимый диапазон - выходим
    {
        return;
    }

    Painter->save();
    if (Date == QDate::currentDate()) //Если рисуется текущая дата
    {
        Painter->fillRect(Rect, COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //Заполнение фона

        //Рисование рамки
        Painter->setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
        Painter->setPen(QPen(COLOR_MAIN_MENU_BAR, 2, Qt::SolidLine));
        Painter->drawRect(QRect(Rect.x(), Rect.y(), Rect.width(), Rect.height()));
    }
    else
    {
        Painter->fillRect(Rect, Date == selectedDate() ? COLOR_CALENDAR_SELECTED_DATE_BACKGROUND : Qt::white); //Заполнение фона
    }

    if (Date.month() != monthShown()) //Если рисуются дни НЕ ТЕКУЩЕГО месяца
    {
        Painter->fillRect(Rect, QColor(245, 245, 245)); //Заполнение фона
    }
    Painter->drawText(QRect(Rect.x() + 3, Rect.y() + 2, Rect.width(), Rect.height()), QString::number(Date.day())); //Рисование текста с датой
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
