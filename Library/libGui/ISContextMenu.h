#pragma once
#ifndef _ISCONTEXTMENU_H_INCLUDED
#define _ISCONTEXTMENU_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
//Базовый класс контекстного меню для виджетов редактирования информации (преимущественно для полей редактирования информации IS*Edit)
class ISContextMenuBase : public QMenu
{
	Q_OBJECT

signals:
	void Delete();

public:
	ISContextMenuBase(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty);
	virtual ~ISContextMenuBase();

protected:
	void AppendOtherAction(QAction *Action);
	void BeforeToShow(); //Действие перед отображением меню
	void CallVirtualKeyboard(); //Вызов виртуальной клавиатуры

private:
	QAction *ActionOtherMenu;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuDateTime : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuDateTime(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty);
	virtual ~ISContextMenuDateTime();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuDouble : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuDouble(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, double Value, double Minimum, double Maximum);
	virtual ~ISContextMenuDouble();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuImage : public QMenu
{
	Q_OBJECT

signals:
	void Select();
	void Cut();
	void Copy();
	void Paste();
	void PasteFromLink();
	void Save();
	void OpenView();

public:
	ISContextMenuImage(QWidget *parent, bool PixmapIsNull);
	virtual ~ISContextMenuImage();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuInteger : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuInteger(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int Value, int Minimum, int Maximum);
	virtual ~ISContextMenuInteger();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuText : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void UppercaseText();
	void LowercaseText();
	void SearchGoogle();
	void SearchYandex();
	void SearchWikipedia();
	void SearchMail();
	void SearchBing();
	void SearchYahoo();

public:
	ISContextMenuText(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int SelectedTextCount);
	virtual ~ISContextMenuText();
};
//-----------------------------------------------------------------------------
#endif
