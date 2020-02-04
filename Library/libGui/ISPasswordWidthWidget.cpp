#include "ISPasswordWidthWidget.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISPassword.h"
//-----------------------------------------------------------------------------
ISPasswordWidthWidget::ISPasswordWidthWidget(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(MARGINS_LAYOUT_NULL);
	setLayout(Layout);

	LabelWidth = new QLabel(this);
	LabelWidth->setText(LANG("PasswordWidth") + ": -");
	Layout->addWidget(LabelWidth);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setMinimum(0);
	ProgressBar->setMaximum(100);
	ProgressBar->setValue(0);
	ProgressBar->setTextVisible(false);
	Layout->addWidget(ProgressBar);

	PropertyAnimation = new QPropertyAnimation(ProgressBar, "value", this);
	PropertyAnimation->setDuration(250);
}
//-----------------------------------------------------------------------------
ISPasswordWidthWidget::~ISPasswordWidthWidget()
{

}
//-----------------------------------------------------------------------------
void ISPasswordWidthWidget::SetPassword(const QVariant &Password)
{
	PropertyAnimation->setStartValue(ProgressBar->value());
	int Width = ISPassword::GetWidthPassword(Password.toString());
	if (Width > 100)
	{
		PropertyAnimation->setEndValue(100);
	}
	else if (Width < 0)
	{
		PropertyAnimation->setEndValue(0);
	}
	else
	{
		PropertyAnimation->setEndValue(Width);
	}

	PropertyAnimation->start();

	if (Width < 0) //[-бесконечность; 0]
	{
		LabelWidth->setText(LANG("PasswordWidth") + ": -");
	}
	else if (Width >= 0 && Width <= 25) //[0; 25]
	{
		LabelWidth->setText(LANG("PasswordWidth") + ": " + LANG("PasswordWidth.VeryLow"));
	}
	else if (Width > 25 && Width <= 50) //[26; 50]
	{
		LabelWidth->setText(LANG("PasswordWidth") + ": " + LANG("PasswordWidth.Low"));
	}
	else if (Width > 51 && Width <= 75) //[51; 75]
	{
		LabelWidth->setText(LANG("PasswordWidth") + ": " + LANG("PasswordWidth.Mean"));
	}
	else if (Width > 75 && Width <= 100) //[76; 100]
	{
		LabelWidth->setText(LANG("PasswordWidth") + ": " + LANG("PasswordWidth.High"));
	}
	else if (Width > 100) //[100; бесконечность]
	{
		LabelWidth->setText(LANG("PasswordWidth") + ": " + LANG("PasswordWidth.VeryHigh"));
	}
}
//-----------------------------------------------------------------------------
