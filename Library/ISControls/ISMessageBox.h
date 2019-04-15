#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISMessageBox : public QMessageBox
{
	Q_OBJECT

public:
	ISMessageBox(ISMessageBox::Icon Icon, const QString &Title, const QString &Message, ISMessageBox::StandardButtons Buttons = ISMessageBox::NoButton, QWidget *parent = 0);
	virtual ~ISMessageBox();

	static void ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static void ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static void ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static bool ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText = QString());

	void AddButton(const QString &Text, ISNamespace::MessageBoxButton ButtonType);
	ISNamespace::MessageBoxButton GetClickedButton();
	int Exec();

protected:
	void ButtonClicked();

private:
	ISNamespace::MessageBoxButton AdditionalButtonClicked;
	QMap<QPushButton*, ISNamespace::MessageBoxButton> AdditionalButtons;
};
//-----------------------------------------------------------------------------
