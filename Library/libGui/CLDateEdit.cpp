#include "CLDateEdit.h"
//#include "CLConstants.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CLDateEdit::CLDateEdit(QWidget *parent) : ISQLineEdit(parent)
{
    setReadOnly(true);

    ButtonCalendar = new ISServiceButton(this);
    connect(ButtonCalendar, &ISServiceButton::clicked, this, &CLDateEdit::ShowCalendar);

    CalendarWidget = new QCalendarWidget();
    CalendarWidget->setWindowFlags(Qt::Popup);
    CalendarWidget->setVisible(false);
    connect(CalendarWidget, &QCalendarWidget::selectionChanged, this, &CLDateEdit::SelectionChanged);
}
//-----------------------------------------------------------------------------
CLDateEdit::~CLDateEdit()
{

}
//-----------------------------------------------------------------------------
QDate CLDateEdit::GetDate() const
{
    return text().isEmpty() ? QDate() : QDate::fromString(text(), DATE_FORMAT_V2);
}
//-----------------------------------------------------------------------------
void CLDateEdit::SetDate(const QDate &Date)
{
    setText(Date.toString(DATE_FORMAT_V2));
}
//-----------------------------------------------------------------------------
void CLDateEdit::Clear()
{
    clear();
}
//-----------------------------------------------------------------------------
void CLDateEdit::SetRange(const QDate &Minimum, const QDate &Maximum)
{
	CalendarWidget->setDateRange(Minimum, Maximum);
}
//-----------------------------------------------------------------------------
void CLDateEdit::resizeEvent(QResizeEvent *ResizeEvent)
{
    ISQLineEdit::resizeEvent(ResizeEvent);
    ButtonCalendar->move(QPoint(width() - ButtonCalendar->width(), 0));
}
//-----------------------------------------------------------------------------
void CLDateEdit::ShowCalendar()
{
    CalendarWidget->setSelectedDate(text().isEmpty() ? QDate::currentDate() : QDate::fromString(text(), DATE_FORMAT_V2));

    QPoint Point = mapToGlobal(QPoint());
    Point.setY(Point.y() + height());
    CalendarWidget->move(Point);
    CalendarWidget->setFocus();
    CalendarWidget->show();
}
//-----------------------------------------------------------------------------
void CLDateEdit::SelectionChanged()
{
    SetDate(CalendarWidget->selectedDate());
    CalendarWidget->hide();
	emit DateChanged();
}
//-----------------------------------------------------------------------------
