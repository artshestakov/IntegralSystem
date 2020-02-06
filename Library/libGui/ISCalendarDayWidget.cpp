#include "ISCalendarDayWidget.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
ISCalendarDayWidget::ISCalendarDayWidget(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_NULL);
	setLayout(MainLayout);

	LabelDayNumber = new QLabel(this);
	LabelDayNumber->setFont(DEFINES_GUI.FONT_TAHOMA_35);
	MainLayout->addWidget(LabelDayNumber, 0, Qt::AlignVCenter);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_NULL);
	MainLayout->addLayout(Layout);

	LabelDayName = new QLabel(this);
	LabelDayName->setFont(DEFINES_GUI.FONT_TAHOMA_18);
	Layout->addWidget(LabelDayName);

	LabelMonthYear = new QLabel(this);
	LabelMonthYear->setFont(DEFINES_GUI.FONT_TAHOMA_12);
	Layout->addWidget(LabelMonthYear);

	MainLayout->addStretch();
}
//-----------------------------------------------------------------------------
ISCalendarDayWidget::~ISCalendarDayWidget()
{

}
//-----------------------------------------------------------------------------
void ISCalendarDayWidget::SetSelectedDate(const QDate &Date)
{
	LabelDayNumber->setText(QString::number(Date.day()));

	QString DayName;
	if (Date == QDate::currentDate())
	{
		DayName = Date.longDayName(Date.dayOfWeek()) + " (" + LANG("Today") + ')';
	}
	else
	{
		DayName = Date.longDayName(Date.dayOfWeek());
	}

	LabelDayName->setText(DayName);
	LabelMonthYear->setText(Date.longMonthName(Date.month()) + SYMBOL_SPACE + QString::number(Date.year()));
}
//-----------------------------------------------------------------------------
