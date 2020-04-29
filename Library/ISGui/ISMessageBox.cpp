#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISMessageBox::ISMessageBox(QMessageBox::Icon Icon, const QString &Title, const QString &Message, QMessageBox::StandardButtons Buttons, QWidget *parent)
	: QMessageBox(Icon, Title, Message, Buttons, parent),
	AdditionalButtonClicked(ISNamespace::MBB_Unknown)
{
	
}
//-----------------------------------------------------------------------------
ISMessageBox::~ISMessageBox()
{
	
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Information, LANG("Information"), Message, QMessageBox::Ok, parent);
	if (!DetailedText.isEmpty())
	{
		MessageBox.setDetailedText(DetailedText);
	}
	MessageBox.Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Warning, LANG("Warning"), Message, QMessageBox::Ok, parent);
	if (!DetailedText.isEmpty())
	{
		MessageBox.setDetailedText(DetailedText);
	}
	MessageBox.Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Critical, LANG("Error"), Message, QMessageBox::Ok, parent);
	if (!DetailedText.isEmpty())
	{
		MessageBox.setDetailedText(DetailedText);
	}
	MessageBox.Exec();
}
//-----------------------------------------------------------------------------
bool ISMessageBox::ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox MessageBox(QMessageBox::Question, LANG("Question"), Message, QMessageBox::Yes | QMessageBox::No, parent);
	MessageBox.setDefaultButton(QMessageBox::No);
	if (!DetailedText.isEmpty())
	{
		MessageBox.setDetailedText(DetailedText);
	}
	return MessageBox.Exec() == QMessageBox::Yes ? true : false;
}
//-----------------------------------------------------------------------------
void ISMessageBox::AddButton(const QString &Text, ISNamespace::MessageBoxButton ButtonType)
{
	QPushButton *Button = new QPushButton(this);
	Button->setText(Text);
	connect(Button, &QPushButton::clicked, this, &ISMessageBox::ButtonClicked);
	addButton(Button, ButtonRole::NoRole);
	AdditionalButtons.emplace(Button, ButtonType);
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
	AdditionalButtonClicked = Button ? AdditionalButtons[Button] : ISNamespace::MBB_Unknown;
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
	QApplication::beep();
	ISGui::ProcessEvents();
	return exec();
}
//-----------------------------------------------------------------------------
