#include "StdAfx.h"
#include "ISCalendarPanel.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_CALENDAR = PREPARE_QUERY("SELECT COUNT(*) "
										   "FROM _calendar "
										   "WHERE NOT cldr_isdeleted "
										   "AND cldr_user = currentuserid() "
										   "AND cldr_date = :Date");
//-----------------------------------------------------------------------------
ISCalendarPanel::ISCalendarPanel(QWidget *parent) : ISCalendarWidget(parent)
{
	PixmapIndicator = BUFFER_ICONS("CalendarMain.Indicator").pixmap(SIZE_45_45);

	setHorizontalHeaderFormat(QCalendarWidget::LongDayNames);
	setFont(FONT_TAHOMA_14);
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

	if (Date == DATE_TODAY) //���� �������� ������� ����
	{
		if (Date == selectedDate())
		{
			Painter->fillRect(Rect, COLOR_CALENDAR_SELECTED_DATE_BACKGROUND); //���������� ����
		}

		//��������� �����
		Painter->setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
		Painter->setPen(QPen(COLOR_MAIN_MENU_BAR, 3, Qt::SolidLine));
		Painter->drawRect(QRect(Rect.x() + 1, Rect.y() + 1, Rect.width() - 3, Rect.height() - 3));
	}
	else
	{
		if (Date == selectedDate()) //���� �������� ���������� ������������ ����
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
		Painter->fillRect(Rect, COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH); //���������� ����
	}

	QRect RectText(Rect.x() + 3, Rect.y() + 3, Rect.width(), Rect.height()); //������������ ������
	QPoint PointIndicator(Rect.x(), Rect.y()); //������������ ���������� �������
	QString Text = QString::number(Date.day()); //�����

	if (Date == DATE_TODAY)
	{
		RectText = QRect(Rect.x() + 3, Rect.y() + 1, Rect.width(), Rect.height());
		PointIndicator = QPoint(Rect.x() + 3, Rect.y() + 3);
	}

	if (DaysEvent.contains(Date))
	{
		Painter->drawPixmap(PointIndicator, PixmapIndicator);
	}

	//��������� ������ � �����
	Painter->drawText(RectText, Text);

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
