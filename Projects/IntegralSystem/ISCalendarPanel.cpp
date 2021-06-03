#include "ISCalendarPanel.h"
#include "ISBuffer.h"
#include "ISAlgorithm.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISCalendarPanel::ISCalendarPanel(QWidget *parent)
    : ISCalendarWidget(parent),
    PixmapIndicator(BUFFER_ICONS("CalendarMain.Indicator").pixmap(QSize(45, 45)))
{
    setHorizontalHeaderFormat(QCalendarWidget::LongDayNames);
    setFont(QFont("Tahoma", 14));
    GetNavigationBar()->setMinimumHeight(GetNavigationBar()->height() * 1.5);
    GetButtonPrevMonth()->setIconSize(GetButtonPrevMonth()->iconSize() * 1.5);
    GetButtonNextMonth()->setIconSize(GetButtonNextMonth()->iconSize() * 1.5);
}
//-----------------------------------------------------------------------------
ISCalendarPanel::~ISCalendarPanel()
{

}
//-----------------------------------------------------------------------------
void ISCalendarPanel::SetDays(const ISVectorUInt &days)
{
    Days = days;
    updateCells();
}
//-----------------------------------------------------------------------------
void ISCalendarPanel::paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const
{
    Painter->save();
    if (Date == QDate::currentDate()) //Если рисуется текущая дата
    {
        if (Date == selectedDate())
        {
            Painter->fillRect(Rect, COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //Заполнение фона
        }

        //Рисование рамки
        Painter->setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
        Painter->setPen(QPen(COLOR_MAIN_MENU_BAR, 3, Qt::SolidLine));
        Painter->drawRect(QRect(Rect.x() + 1, Rect.y() + 1, Rect.width() - 3, Rect.height() - 3));
    }
    else
    {
        //Если рисуется выделенная пользоваелем дата - заполняем фон одним цветом, иначе - другим
        Painter->fillRect(Rect, Date == selectedDate() ? COLOR_CALENDAR_SELECTED_DATE_BACKGROUND : Qt::white);
    }

    if (Date.month() != monthShown()) //Если рисуются дни НЕ ТЕКУЩЕГО месяца
    {
        Painter->fillRect(Rect, QColor(245, 245, 245)); //Заполнение фона
    }

    QRect RectText(Rect.x() + 3, Rect.y() + 3, Rect.width(), Rect.height()); //Расположение текста
    QPoint PointIndicator(Rect.x(), Rect.y()); //Расположение индикатора события

    if (Date == QDate::currentDate())
    {
        RectText = QRect(Rect.x() + 3, Rect.y() + 1, Rect.width(), Rect.height());
        PointIndicator = QPoint(Rect.x() + 3, Rect.y() + 3);
    }

    //Рисуем плажку события если по текущей дате есть события
    //и месяц текущей датысоответствует отображаемому месяц
    if (ISAlgorithm::VectorContains(Days, (unsigned int)Date.day()) && Date.month() == monthShown())
    {
        Painter->drawPixmap(PointIndicator, PixmapIndicator);
    }

    //Рисование текста с датой
    QString DayString = QString::number(Date.day());
    Painter->drawText(RectText, DayString);
    Painter->restore();
}
//-----------------------------------------------------------------------------
