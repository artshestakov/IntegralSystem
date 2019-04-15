#include "StdAfx.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISMessageBox::ISMessageBox(QMessageBox::Icon Icon, const QString &Title, const QString &Message, QMessageBox::StandardButtons Buttons, QWidget *parent) : QMessageBox(Icon, Title, Message, Buttons, parent)
{
	AdditionalButtonClicked = ISNamespace::MBB_Unknown;
	
	setWindowIcon(QApplication::windowIcon());
}
//-----------------------------------------------------------------------------
ISMessageBox::~ISMessageBox()
{
	
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Information, LOCALIZATION("Information"), Message, QMessageBox::Ok, parent);
	MessageBox.setDetailedText(DetailedText);
	MessageBox.Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Warning, LOCALIZATION("Warning"), Message, QMessageBox::Ok, parent);
	MessageBox.setDetailedText(DetailedText);
	MessageBox.Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Critical, LOCALIZATION("Error"), Message, QMessageBox::Ok, parent);
	MessageBox.setDetailedText(DetailedText);
	MessageBox.Exec();
}
//-----------------------------------------------------------------------------
bool ISMessageBox::ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Question, LOCALIZATION("Question"), Message, QMessageBox::Yes | QMessageBox::No, parent);
	MessageBox.setDetailedText(DetailedText);
	MessageBox.setDefaultButton(QMessageBox::No);

	QApplication::beep();
	if (MessageBox.Exec() == QMessageBox::Yes)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
void ISMessageBox::AddButton(const QString &Text, ISNamespace::MessageBoxButton ButtonType)
{
	QPushButton *Button = new QPushButton(this);
	Button->setText(Text);
	connect(Button, &QPushButton::clicked, this, &ISMessageBox::ButtonClicked);
	addButton(Button, ButtonRole::NoRole);
	AdditionalButtons.insert(Button, ButtonType);
}
//-----------------------------------------------------------------------------
ISNamespace::MessageBoxButton ISMessageBox::GetClickedButton()
{
	return AdditionalButtonClicked;
}
//-----------------------------------------------------------------------------
void ISMessageBox::ButtonClicked()
{
	QPushButton *Button = dynamic_cast<QPushButton*>(sender());
	if (Button)
	{
		AdditionalButtonClicked = AdditionalButtons.value(Button);
	}
	else
	{
		AdditionalButtonClicked = ISNamespace::MBB_Unknown;
	}
}
//-----------------------------------------------------------------------------
int ISMessageBox::Exec()
{
	for (QAbstractButton *Button : buttons())
	{
		Button->setStyleSheet(STYLE_SHEET("ISPushButton"));
		Button->setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
		Button->setFixedHeight(23);
	}

	return exec();
}
//-----------------------------------------------------------------------------
