#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISGui.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISMessageBox::ISMessageBox(ISMessageBox::Icon Icon, const QString &Title, const QString &Message, const QString &DetailedText, const std::vector<ISMessageBoxButton> &Buttons, QWidget *parent)
	: QMessageBox(Icon, Title, Message, ISMessageBox::NoButton, parent),
	ClickedID(-1)
{
	setWindowIcon(BUFFER_ICONS("Logo"));
	AddButtons(Buttons);
	if (!DetailedText.isEmpty())
	{
		setInformativeText(DetailedText);
	}
}
//-----------------------------------------------------------------------------
ISMessageBox::~ISMessageBox()
{
	
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox(ISMessageBox::Information, LANG("Information"), Message, DetailedText, { { 1, "OK", true } },parent).Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox(ISMessageBox::Warning, LANG("Warning"), Message, DetailedText, { { 1, "OK", true } }, parent).Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox(ISMessageBox::Critical, LANG("Error"), Message, DetailedText, { { 1, "OK", true } }, parent).Exec();
}
//-----------------------------------------------------------------------------
bool ISMessageBox::ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	return ISMessageBox(ISMessageBox::Question, LANG("Question"), Message, DetailedText,
	{
		{ 1, LANG("Yes") },
		{ 2, LANG("No"), true }
	}, parent).Exec() == 1;
}
//-----------------------------------------------------------------------------
int ISMessageBox::ShowQuestion(QWidget *parent, const QString &Message, const std::vector<ISMessageBoxButton> &Buttons, const QString &DetailedText)
{
	return ISMessageBox(ISMessageBox::Question, LANG("Question"), Message, DetailedText, Buttons, parent).Exec();
}
//-----------------------------------------------------------------------------
int ISMessageBox::Exec()
{
	QApplication::beep();
	PROCESS_EVENTS();
	for (QAbstractButton *AbstractButton : buttons())
	{
		AbstractButton->setStyleSheet(BUFFER_STYLE_SHEET("ISPushButton"));
		AbstractButton->setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
	}
	exec();
	return ClickedID;
}
//-----------------------------------------------------------------------------
void ISMessageBox::AddButtons(const std::vector<ISMessageBoxButton> &Buttons)
{
	for (const ISMessageBoxButton &Button : Buttons)
	{
		ISPushButton *PushButton = new ISPushButton(Button.Icon, Button.Text, this);
		PushButton->setProperty("ID", Button.ID);
		connect(PushButton, &ISPushButton::clicked, this, &ISMessageBox::ButtonClicked);
		addButton(PushButton, ISMessageBox::ActionRole);
		if (Button.Default)
		{
			PushButton->setFocus();
		}
	}
}
//-----------------------------------------------------------------------------
void ISMessageBox::SetDefaultButton(int ID)
{
	for (QAbstractButton *AbstractButton : buttons())
	{
		if (AbstractButton->property("ID").toInt() == ID)
		{
			AbstractButton->setFocus();
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISMessageBox::ButtonClicked()
{
	ClickedID = sender()->property("ID").toInt();
}
//-----------------------------------------------------------------------------
