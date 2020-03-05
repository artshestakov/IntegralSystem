#include "CLTimeEdit.h"
//#include "CLLabel.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CLTimeEdit::CLTimeEdit(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    setSizePolicy(QSizePolicy::Maximum, sizePolicy().verticalPolicy());

    QHBoxLayout *Layout = new QHBoxLayout();
    Layout->setContentsMargins(Layout->contentsMargins().left(), 0, Layout->contentsMargins().right(), 0);
    setLayout(Layout);

    ComboBoxHour = new ISQComboBox(this);
    ComboBoxHour->setCursor(Qt::PointingHandCursor);
    ComboBoxHour->addItem("00", 0);
    ComboBoxHour->addItem("01", 1);
    ComboBoxHour->addItem("02", 2);
    ComboBoxHour->addItem("03", 3);
    ComboBoxHour->addItem("04", 4);
    ComboBoxHour->addItem("05", 5);
    ComboBoxHour->addItem("06", 6);
    ComboBoxHour->addItem("07", 7);
    ComboBoxHour->addItem("08", 8);
    ComboBoxHour->addItem("09", 9);
    ComboBoxHour->addItem("10", 10);
    ComboBoxHour->addItem("11", 11);
    ComboBoxHour->addItem("12", 12);
    ComboBoxHour->addItem("13", 13);
    ComboBoxHour->addItem("14", 14);
    ComboBoxHour->addItem("15", 15);
    ComboBoxHour->addItem("16", 16);
    ComboBoxHour->addItem("17", 17);
    ComboBoxHour->addItem("18", 18);
    ComboBoxHour->addItem("19", 19);
    ComboBoxHour->addItem("20", 20);
    ComboBoxHour->addItem("21", 21);
    ComboBoxHour->addItem("22", 22);
    ComboBoxHour->addItem("23", 23);
	connect(ComboBoxHour, static_cast<void(ISQComboBox::*)(int)>(&ISQComboBox::currentIndexChanged), this, &CLTimeEdit::TimeChanged);
    Layout->addWidget(ComboBoxHour);

    Layout->addWidget(new QLabel(":", this));

    ComboBoxMinute = new ISQComboBox(this);
    ComboBoxMinute->setCursor(Qt::PointingHandCursor);
    ComboBoxMinute->addItem("00", 0);
    ComboBoxMinute->addItem("01", 1);
    ComboBoxMinute->addItem("02", 2);
    ComboBoxMinute->addItem("03", 3);
    ComboBoxMinute->addItem("04", 4);
    ComboBoxMinute->addItem("05", 5);
    ComboBoxMinute->addItem("06", 6);
    ComboBoxMinute->addItem("07", 7);
    ComboBoxMinute->addItem("08", 8);
    ComboBoxMinute->addItem("09", 9);
    ComboBoxMinute->addItem("10", 10);
    ComboBoxMinute->addItem("11", 11);
    ComboBoxMinute->addItem("12", 12);
    ComboBoxMinute->addItem("13", 13);
    ComboBoxMinute->addItem("14", 14);
    ComboBoxMinute->addItem("15", 15);
    ComboBoxMinute->addItem("16", 16);
    ComboBoxMinute->addItem("17", 17);
    ComboBoxMinute->addItem("18", 18);
    ComboBoxMinute->addItem("19", 19);
    ComboBoxMinute->addItem("20", 20);
    ComboBoxMinute->addItem("21", 21);
    ComboBoxMinute->addItem("22", 22);
    ComboBoxMinute->addItem("23", 23);
    ComboBoxMinute->addItem("24", 24);
    ComboBoxMinute->addItem("25", 25);
    ComboBoxMinute->addItem("26", 26);
    ComboBoxMinute->addItem("27", 27);
    ComboBoxMinute->addItem("28", 28);
    ComboBoxMinute->addItem("29", 29);
    ComboBoxMinute->addItem("30", 30);
    ComboBoxMinute->addItem("31", 31);
    ComboBoxMinute->addItem("32", 32);
    ComboBoxMinute->addItem("33", 33);
    ComboBoxMinute->addItem("34", 34);
    ComboBoxMinute->addItem("35", 35);
    ComboBoxMinute->addItem("36", 36);
    ComboBoxMinute->addItem("37", 37);
    ComboBoxMinute->addItem("38", 38);
    ComboBoxMinute->addItem("39", 39);
    ComboBoxMinute->addItem("40", 40);
    ComboBoxMinute->addItem("41", 41);
    ComboBoxMinute->addItem("42", 42);
    ComboBoxMinute->addItem("43", 43);
    ComboBoxMinute->addItem("44", 44);
    ComboBoxMinute->addItem("45", 45);
    ComboBoxMinute->addItem("46", 46);
    ComboBoxMinute->addItem("47", 47);
    ComboBoxMinute->addItem("48", 48);
    ComboBoxMinute->addItem("49", 49);
    ComboBoxMinute->addItem("50", 50);
    ComboBoxMinute->addItem("51", 51);
    ComboBoxMinute->addItem("52", 52);
    ComboBoxMinute->addItem("53", 53);
    ComboBoxMinute->addItem("54", 54);
    ComboBoxMinute->addItem("55", 55);
    ComboBoxMinute->addItem("56", 56);
    ComboBoxMinute->addItem("57", 57);
    ComboBoxMinute->addItem("58", 58);
    ComboBoxMinute->addItem("59", 59);
	connect(ComboBoxMinute, static_cast<void(ISQComboBox::*)(int)>(&ISQComboBox::currentIndexChanged), this, &CLTimeEdit::TimeChanged);
    Layout->addWidget(ComboBoxMinute);

    Layout->addWidget(new QLabel(":", this));

    ComboBoxSecond = new ISQComboBox(this);
    ComboBoxSecond->setCursor(Qt::PointingHandCursor);
    ComboBoxSecond->addItem("00", 0);
    ComboBoxSecond->addItem("01", 1);
    ComboBoxSecond->addItem("02", 2);
    ComboBoxSecond->addItem("03", 3);
    ComboBoxSecond->addItem("04", 4);
    ComboBoxSecond->addItem("05", 5);
    ComboBoxSecond->addItem("06", 6);
    ComboBoxSecond->addItem("07", 7);
    ComboBoxSecond->addItem("08", 8);
    ComboBoxSecond->addItem("09", 9);
    ComboBoxSecond->addItem("10", 10);
    ComboBoxSecond->addItem("11", 11);
    ComboBoxSecond->addItem("12", 12);
    ComboBoxSecond->addItem("13", 13);
    ComboBoxSecond->addItem("14", 14);
    ComboBoxSecond->addItem("15", 15);
    ComboBoxSecond->addItem("16", 16);
    ComboBoxSecond->addItem("17", 17);
    ComboBoxSecond->addItem("18", 18);
    ComboBoxSecond->addItem("19", 19);
    ComboBoxSecond->addItem("20", 20);
    ComboBoxSecond->addItem("21", 21);
    ComboBoxSecond->addItem("22", 22);
    ComboBoxSecond->addItem("23", 23);
    ComboBoxSecond->addItem("24", 24);
    ComboBoxSecond->addItem("25", 25);
    ComboBoxSecond->addItem("26", 26);
    ComboBoxSecond->addItem("27", 27);
    ComboBoxSecond->addItem("28", 28);
    ComboBoxSecond->addItem("29", 29);
    ComboBoxSecond->addItem("30", 30);
    ComboBoxSecond->addItem("31", 31);
    ComboBoxSecond->addItem("32", 32);
    ComboBoxSecond->addItem("33", 33);
    ComboBoxSecond->addItem("34", 34);
    ComboBoxSecond->addItem("35", 35);
    ComboBoxSecond->addItem("36", 36);
    ComboBoxSecond->addItem("37", 37);
    ComboBoxSecond->addItem("38", 38);
    ComboBoxSecond->addItem("39", 39);
    ComboBoxSecond->addItem("40", 40);
    ComboBoxSecond->addItem("41", 41);
    ComboBoxSecond->addItem("42", 42);
    ComboBoxSecond->addItem("43", 43);
    ComboBoxSecond->addItem("44", 44);
    ComboBoxSecond->addItem("45", 45);
    ComboBoxSecond->addItem("46", 46);
    ComboBoxSecond->addItem("47", 47);
    ComboBoxSecond->addItem("48", 48);
    ComboBoxSecond->addItem("49", 49);
    ComboBoxSecond->addItem("50", 50);
    ComboBoxSecond->addItem("51", 51);
    ComboBoxSecond->addItem("52", 52);
    ComboBoxSecond->addItem("53", 53);
    ComboBoxSecond->addItem("54", 54);
    ComboBoxSecond->addItem("55", 55);
    ComboBoxSecond->addItem("56", 56);
    ComboBoxSecond->addItem("57", 57);
    ComboBoxSecond->addItem("58", 58);
    ComboBoxSecond->addItem("59", 59);
	connect(ComboBoxSecond, static_cast<void(ISQComboBox::*)(int)>(&ISQComboBox::currentIndexChanged), this, &CLTimeEdit::TimeChanged);
    Layout->addWidget(ComboBoxSecond);

    Clear();
}
//-----------------------------------------------------------------------------
CLTimeEdit::~CLTimeEdit()
{

}
//-----------------------------------------------------------------------------
QTime CLTimeEdit::GetTime() const
{
    return QTime(ComboBoxHour->currentData().toInt(), ComboBoxMinute->currentData().toInt(), ComboBoxSecond->currentData().toInt());
}
//-----------------------------------------------------------------------------
void CLTimeEdit::SetTime(const QTime &Time)
{
    if (Time.isValid() && !Time.isNull())
    {
        ComboBoxHour->setCurrentIndex(Time.hour());
        ComboBoxMinute->setCurrentIndex(Time.minute());
        ComboBoxSecond->setCurrentIndex(Time.second());
    }
}
//-----------------------------------------------------------------------------
void CLTimeEdit::Clear()
{
    ComboBoxHour->setCurrentIndex(0);
    ComboBoxMinute->setCurrentIndex(0);
    ComboBoxSecond->setCurrentIndex(0);
}
//-----------------------------------------------------------------------------
