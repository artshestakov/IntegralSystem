#include "ISContextMenuBase.h"
#include "ISAssert.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISContextMenuBase::ISContextMenuBase(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty) : QMenu(ParentEdit)
{
	setToolTipsVisible(true);

	connect(this, &QMenu::aboutToShow, this, &ISContextMenuBase::BeforeToShow);

	QMetaObject::Connection ConnectedSlot;

	if (!ReadOnly)
	{
		//Отменить действие
		QAction *ActionUndo = ISControls::GetActionContextUndo(this);
		ActionUndo->setEnabled(UndoAvailable);
		ActionUndo->setShortcut(QKeySequence::Undo);
		ConnectedSlot = connect(ActionUndo, SIGNAL(triggered()), ParentEdit, SLOT(undo()));
		addAction(ActionUndo);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

		//Повторить действие
		QAction *ActionRedo = ISControls::GetActionContextRedo(this);
		ActionRedo->setEnabled(RedoAvailable);
		ActionRedo->setShortcut(QKeySequence::Redo);
		ConnectedSlot = connect(ActionRedo, SIGNAL(triggered()), ParentEdit, SLOT(redo()));
		addAction(ActionRedo);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

		addAction(ISControls::CreateSeparator(this));
	}

#ifndef QT_NO_CLIPBOARD
	if (!ReadOnly)
	{
		//Вырезать
		QAction *ActionCut = ISControls::GetActionContextCut(this);
		ActionCut->setEnabled(!ReadOnly && HasSelectedText && EchoMode == QLineEdit::Normal);
		ActionCut->setShortcut(QKeySequence::Cut);
		ConnectedSlot = connect(ActionCut, SIGNAL(triggered()), ParentEdit, SLOT(cut()));
		addAction(ActionCut);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");
	}

	//Копировать
	QAction *ActionCopy = ISControls::GetActionContextCopy(this);
	ActionCopy->setEnabled(HasSelectedText && EchoMode == QLineEdit::Normal);
	ActionCopy->setShortcut(QKeySequence::Copy);
	ConnectedSlot = connect(ActionCopy, SIGNAL(triggered()), ParentEdit, SLOT(copy()));
	addAction(ActionCopy);
	IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

	if (!ReadOnly)
	{
		//Вставить
		QAction *ActionPaste = ISControls::GetActionContextPaste(this);
		ActionPaste->setEnabled(!ReadOnly && !QApplication::clipboard()->text().isEmpty());
		ActionPaste->setShortcut(QKeySequence::Paste);
		ConnectedSlot = connect(ActionPaste, SIGNAL(triggered()), ParentEdit, SLOT(paste()));
		addAction(ActionPaste);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");
	}
#endif // !QT_NO_CLIPBOARD

	if (!ReadOnly)
	{
		//Удалить
		QAction *ActionDelete = ISControls::GetActionContextDelete(this);
		ActionDelete->setEnabled(!ReadOnly && !Empty && HasSelectedText);
		ConnectedSlot = connect(ActionDelete, &QAction::triggered, this, &ISContextMenuBase::Delete);
		addAction(ActionDelete);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");
	}

	if (!isEmpty())
	{
		addAction(ISControls::CreateSeparator(this));
	}

	//Выбрать все
	QAction *ActionSelectAll = ISControls::GetActionContextSelectAll(this);
	ActionSelectAll->setShortcut(QKeySequence::SelectAll);
	ActionSelectAll->setEnabled(!Empty);
	ConnectedSlot = connect(ActionSelectAll, SIGNAL(triggered()), ParentEdit, SLOT(selectAll()));
	addAction(ActionSelectAll);
	IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

	addAction(ISControls::CreateSeparator(this));

	//Виртуальная клавиатура
	QAction *ActionVirtualKeyboard = ISControls::GetActionContextVirtualKeyboard(this);
	ActionVirtualKeyboard->setEnabled(!ReadOnly);
	connect(ActionVirtualKeyboard, &QAction::triggered, this, &ISContextMenuBase::CallVirtualKeyboard);
	addAction(ActionVirtualKeyboard);
	IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

	addAction(ISControls::CreateSeparator(this));

	//Прочие действия
	ActionOtherMenu = new QAction(this);
	ActionOtherMenu->setText(LANG("OtherActions"));
	ActionOtherMenu->setMenu(new QMenu(this));
	addAction(ActionOtherMenu);
}
//-----------------------------------------------------------------------------
ISContextMenuBase::~ISContextMenuBase()
{
	
}
//-----------------------------------------------------------------------------
void ISContextMenuBase::AppendOtherAction(QAction *Action)
{
	ActionOtherMenu->menu()->addAction(Action);
}
//-----------------------------------------------------------------------------
void ISContextMenuBase::BeforeToShow()
{
	if (!ActionOtherMenu->menu()->actions().count())
	{
		ActionOtherMenu->setVisible(false);
	}
}
//-----------------------------------------------------------------------------
void ISContextMenuBase::CallVirtualKeyboard()
{
	if (!ISGui::OpenFile("C:/Windows/System32/osk.exe"))
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.CallVirtualKeyboard"));
	}
}
//-----------------------------------------------------------------------------
