#pragma once
#ifndef _ISMESSAGEBOX_H_INCLUDED
#define _ISMESSAGEBOX_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISNamespace.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISMessageBox : public QMessageBox
{
	Q_OBJECT

public:
	ISMessageBox(ISMessageBox::Icon Icon, const QString &Title, const QString &Message, const QString &DetailedText, const std::vector<ISMessageBoxButton> &Buttons, QWidget *parent = 0);
	virtual ~ISMessageBox();

	static void ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static void ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static void ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static bool ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
	static int ShowQuestion(QWidget *parent, const QString &Message, const std::vector<ISMessageBoxButton> &Buttons, const QString &DetailedText = QString());

public:
	int Exec();
	void AddButtons(const std::vector<ISMessageBoxButton> &Buttons); //Добавить кнопку на диалог
	void SetDefaultButton(int ID); //Изменить кнопку по умолчанию

private:
	void ButtonClicked(); //Событие нажатия на кнопку

private:
	int ClickedID;
};
//-----------------------------------------------------------------------------
#endif
