#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
//Базовый класс контекстного меню для виджетов редактирования информации (преимущественно для полей редактирования информации IS*Edit)
class LIBGUI_EXPORT ISContextMenuBase : public QMenu
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
