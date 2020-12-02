#include "ISCalendarPanel.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_CALENDAR = PREPARE_QUERY("SELECT COUNT(*) "
										   "FROM _calendar "
										   "WHERE cldr_creationuser = currentuserid() "
										   "AND cldr_date = :Date");
//-----------------------------------------------------------------------------
ISCalendarPanel::ISCalendarPanel(QWidget *parent) : ISCalendarWidget(parent)
{
	PixmapIndicator = BUFFER_ICONS("CalendarMain.Indicator").pixmap(ISDefines::Gui::SIZE_45_45);

	setHorizontalHeaderFormat(QCalendarWidget::LongDayNames);
	setFont(ISDefines::Gui::FONT_TAHOMA_14);
	GetNavigationBar()->setMinimumHeight(GetNavigationBar()->height() * 1.5);
	GetButtonPrevMonth()->setIconSize(GetButtonPrevMonth()->iconSize() * 1.5);
	GetButtonNextMonth()->setIconSize(GetButtonNextMonth()->iconSize() * 1.5);

	connect(this, &ISCalendarWidget::currentPageChanged, this, &ISCalendarPanel::CurrentPageChanged);
}
//-----------------------------------------------------------------------------
ISCalendarPanel::~ISCalendarPanel()
{

}
//-----------------------------------------------------------------------------
void ISCalendarPanel::UpdateCells()
{
	currentPageChanged(selectedDate().year(), selectedDate().month());
}
//-----------------------------------------------------------------------------
void ISCalendarPanel::paintCell(QPainter *Painter, const QRect &Rect, const QDate &Date) const
{
	Painter->save();

	if (Date == QDate::currentDate()) //���� �������� ������� ����
	{
		if (Date == selectedDate())
		{
			Painter->fillRect(Rect, ISDefines::Gui::COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //���������� ����
		}

		//��������� �����
		Painter->setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
		Painter->setPen(QPen(ISDefines::Gui::COLOR_MAIN_MENU_BAR, 3, Qt::SolidLine));
		Painter->drawRect(QRect(Rect.x() + 1, Rect.y() + 1, Rect.width() - 3, Rect.height() - 3));
	}
	else
	{
		if (Date == selectedDate()) //���� �������� ���������� ������������ ����
		{
			Painter->fillRect(Rect, ISDefines::Gui::COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //���������� ����
		}
		else
		{
			Painter->fillRect(Rect, Qt::white); //���������� ����
		}
	}

	if (Date.month() != monthShown()) //���� �������� ��� �� �������� ������
	{
		Painter->fillRect(Rect, ISDefines::Gui::COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH); //���������� ����
	}

	QRect RectText(Rect.x() + 3, Rect.y() + 3, Rect.width(), Rect.height()); //������������ ������
	QPoint PointIndicator(Rect.x(), Rect.y()); //������������ ���������� �������

	if (Date == QDate::currentDate())
	{
		RectText = QRect(Rect.x() + 3, Rect.y() + 1, Rect.width(), Rect.height());
		PointIndicator = QPoint(Rect.x() + 3, Rect.y() + 3);
	}

	if (DaysEvent.contains(Date))
	{
		Painter->drawPixmap(PointIndicator, PixmapIndicator);
	}

	char Buffer[3] = { 0 };
	_itoa(Date.day(), &Buffer[0], 10);

	//��������� ������ � �����
	Painter->drawText(RectText, Buffer);
	Painter->restore();
}
//-----------------------------------------------------------------------------
void ISCalendarPanel::CurrentPageChanged(int Year, int Month)
{
	ISGui::SetWaitGlobalCursor(true);
	DaysEvent.clear();

	QDate Date(Year, Month, 1);
	for (int i = 0; i < Date.daysInMonth(); ++i)
	{
		QDate DateEvent(Year, Month, i + 1);

		ISQuery qSelect(QS_CALENDAR);
		qSelect.BindValue(":Date", DateEvent);
		if (qSelect.ExecuteFirst())
		{
			int Count = qSelect.ReadColumn("count").toInt();
			if (Count)
			{
				DaysEvent.insert(DateEvent, Count);
			}
		}
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
