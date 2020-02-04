#include "ISBirthdayEdit.h"
#include "EXDefines.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISBirthdayEdit::ISBirthdayEdit(QWidget *parent) : ISDateEdit(parent)
{
	SetMinimumDate(QDate(1900, 1, 1));
	SetMaximumDate(DATE_TODAY);

	Label = new QLabel(this);
	Label->setFrameShape(QFrame::Shape::Panel);
	Label->setFrameShadow(QFrame::Plain);
	Label->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	Label->setSizePolicy(QSizePolicy::Maximum, Label->sizePolicy().verticalPolicy());
	AddWidgetToRight(Label);

	UpdateLabel(QDate());

	connect(this, &ISBirthdayEdit::DataChanged, [=]
	{
		UpdateLabel(GetValue().toDate());
	});
}
//-----------------------------------------------------------------------------
ISBirthdayEdit::~ISBirthdayEdit()
{

}
//-----------------------------------------------------------------------------
void ISBirthdayEdit::SetValue(const QVariant &value)
{
	ISDateEdit::SetValue(value);
	UpdateLabel(value.toDate());
}
//-----------------------------------------------------------------------------
void ISBirthdayEdit::UpdateLabel(const QDate &Date)
{
	if (Date.isValid())
	{
		int Age = DATE_TODAY.year() - Date.year();
		if (Date.month() > DATE_TODAY.month() || (Date.month() == DATE_TODAY.month() && Date.day() > DATE_TODAY.day()))
		{
			Age--;
		}

		Label->setText(LANG("Age") + ": " + QString::number(Age));
	}
	else
	{
		Label->setText(LANG("Age") + ": " + LANG("AgeNotSelected"));
	}
}
//-----------------------------------------------------------------------------
void ISBirthdayEdit::DateEnableChanged(int State)
{
	ISDateEdit::DateEnableChanged(State);

	if (State == Qt::Checked)
	{
		UpdateLabel(GetValue().toDate());
	}
	else
	{
		UpdateLabel(QDate());
	}
}
//-----------------------------------------------------------------------------
