#include "ISContextMenu.h"
#include "ISAssert.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISMessageBox.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISContextMenuBase::ISContextMenuBase(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty) : QMenu(ParentEdit)
{
	setToolTipsVisible(true);

	connect(this, &QMenu::aboutToShow, this, &ISContextMenuBase::BeforeToShow);

	QMetaObject::Connection ConnectedSlot;

	if (!ReadOnly)
	{
		//�������� ��������
		QAction *ActionUndo = ISControls::GetActionContextUndo(this);
		ActionUndo->setEnabled(UndoAvailable);
		ActionUndo->setShortcut(QKeySequence::Undo);
		ConnectedSlot = connect(ActionUndo, SIGNAL(triggered()), ParentEdit, SLOT(undo()));
		addAction(ActionUndo);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

		//��������� ��������
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
		//��������
		QAction *ActionCut = ISControls::GetActionContextCut(this);
		ActionCut->setEnabled(!ReadOnly && HasSelectedText && EchoMode == QLineEdit::Normal);
		ActionCut->setShortcut(QKeySequence::Cut);
		ConnectedSlot = connect(ActionCut, SIGNAL(triggered()), ParentEdit, SLOT(cut()));
		addAction(ActionCut);
		IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");
	}

	//����������
	QAction *ActionCopy = ISControls::GetActionContextCopy(this);
	ActionCopy->setEnabled(HasSelectedText && EchoMode == QLineEdit::Normal);
	ActionCopy->setShortcut(QKeySequence::Copy);
	ConnectedSlot = connect(ActionCopy, SIGNAL(triggered()), ParentEdit, SLOT(copy()));
	addAction(ActionCopy);
	IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

	if (!ReadOnly)
	{
		//��������
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
		//�������
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

	//������� ���
	QAction *ActionSelectAll = ISControls::GetActionContextSelectAll(this);
	ActionSelectAll->setShortcut(QKeySequence::SelectAll);
	ActionSelectAll->setEnabled(!Empty);
	ConnectedSlot = connect(ActionSelectAll, SIGNAL(triggered()), ParentEdit, SLOT(selectAll()));
	addAction(ActionSelectAll);
	IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

	addAction(ISControls::CreateSeparator(this));

	//����������� ����������
	QAction *ActionVirtualKeyboard = ISControls::GetActionContextVirtualKeyboard(this);
	ActionVirtualKeyboard->setEnabled(!ReadOnly);
	connect(ActionVirtualKeyboard, &QAction::triggered, this, &ISContextMenuBase::CallVirtualKeyboard);
	addAction(ActionVirtualKeyboard);
	IS_ASSERT(ConnectedSlot, "Not connected slot for signal.");

	addAction(ISControls::CreateSeparator(this));

	//������ ��������
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISContextMenuDateTime::ISContextMenuDateTime(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionStepUp = ISControls::GetActionContextStepUp(this);
	connect(ActionStepUp, &QAction::triggered, this, &ISContextMenuDateTime::StepUp);
	addAction(ActionStepUp);

	QAction *ActionStepDown = ISControls::GetActionContextStepDown(this);
	connect(ActionStepDown, &QAction::triggered, this, &ISContextMenuDateTime::StepDown);
	addAction(ActionStepDown);
}
//-----------------------------------------------------------------------------
ISContextMenuDateTime::~ISContextMenuDateTime()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISContextMenuDouble::ISContextMenuDouble(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, double Value, double Minimum, double Maximum)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionStepUp = ISControls::GetActionContextStepUp(this);
	ActionStepUp->setEnabled(Value != Maximum);
	connect(ActionStepUp, &QAction::triggered, this, &ISContextMenuDouble::StepUp);
	addAction(ActionStepUp);

	QAction *ActionStepDown = ISControls::GetActionContextStepDown(this);
	ActionStepDown->setEnabled(Value != Minimum);
	connect(ActionStepDown, &QAction::triggered, this, &ISContextMenuDouble::StepDown);
	addAction(ActionStepDown);
}
//-----------------------------------------------------------------------------
ISContextMenuDouble::~ISContextMenuDouble()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISContextMenuImage::ISContextMenuImage(QWidget *parent, bool PixmapIsNull) : QMenu(parent)
{
	setToolTipsVisible(true);

	//�������
	QAction *ActionSelect = new QAction(this);
	ActionSelect->setText(LANG("ContextMenu.Image.SelectFile"));
	ActionSelect->setToolTip(LANG("ContextMenu.Image.SelectFile.ToolTip"));
	ActionSelect->setIcon(BUFFER_ICONS("Select"));
	connect(ActionSelect, &QAction::triggered, this, &ISContextMenuImage::Select);
	addAction(ActionSelect);

	addAction(ISControls::CreateSeparator(this));

	//��������
	QAction *ActionCut = new QAction(this);
	ActionCut->setEnabled(!PixmapIsNull);
	ActionCut->setText(LANG("ContextMenu.Image.Cut"));
	ActionCut->setToolTip(LANG("ContextMenu.Image.Cut.ToolTip"));
	ActionCut->setIcon(BUFFER_ICONS("Cut"));
	ActionCut->setShortcut(QKeySequence::Cut);
	connect(ActionCut, &QAction::triggered, this, &ISContextMenuImage::Cut);
	addAction(ActionCut);

	//����������
	QAction *ActionCopy = new QAction(this);
	ActionCopy->setEnabled(!PixmapIsNull);
	ActionCopy->setText(LANG("ContextMenu.Image.Copy"));
	ActionCopy->setToolTip(LANG("ContextMenu.Image.Copy.ToolTip"));
	ActionCopy->setIcon(BUFFER_ICONS("Copy"));
	ActionCopy->setShortcut(QKeySequence::Copy);
	connect(ActionCopy, &QAction::triggered, this, &ISContextMenuImage::Copy);
	addAction(ActionCopy);

	//��������
	QAction *ActionPaste = new QAction(this);
	ActionPaste->setEnabled(!QApplication::clipboard()->pixmap().isNull());
	ActionPaste->setText(LANG("ContextMenu.Image.Paste"));
	ActionPaste->setToolTip(LANG("ContextMenu.Image.Paste.ToolTip"));
	ActionPaste->setIcon(BUFFER_ICONS("Paste"));
	ActionPaste->setShortcut(QKeySequence::Paste);
	connect(ActionPaste, &QAction::triggered, this, &ISContextMenuImage::Paste);
	addAction(ActionPaste);

	//�������� �� ������
	QAction *ActionPasteFromLink = new QAction(this);
	ActionPasteFromLink->setText(LANG("ContextMenu.Image.PasteFromLink"));
	ActionPasteFromLink->setIcon(BUFFER_ICONS("PasteFromLink"));
	connect(ActionPasteFromLink, &QAction::triggered, this, &ISContextMenuImage::PasteFromLink);
	addAction(ActionPasteFromLink);

	addAction(ISControls::CreateSeparator(this));

	//���������
	QAction *ActionSave = ISControls::CreateActionSave(this);
	ActionSave->setEnabled(!PixmapIsNull);
	connect(ActionSave, &QAction::triggered, this, &ISContextMenuImage::Save);
	addAction(ActionSave);

	//��������
	QAction *ActionOpenView = new QAction(this);
	ActionOpenView->setEnabled(!PixmapIsNull);
	ActionOpenView->setText(LANG("ViewImage"));
	ActionOpenView->setToolTip(LANG("ViewImage"));
	ActionOpenView->setIcon(BUFFER_ICONS("Image"));
	connect(ActionOpenView, &QAction::triggered, this, &ISContextMenuImage::OpenView);
	addAction(ActionOpenView);
}
//-----------------------------------------------------------------------------
ISContextMenuImage::~ISContextMenuImage()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISContextMenuInteger::ISContextMenuInteger(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int Value, int Minimum, int Maximum)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionStepUp = ISControls::GetActionContextStepUp(this);
	ActionStepUp->setEnabled(Value != Maximum);
	connect(ActionStepUp, &QAction::triggered, this, &ISContextMenuInteger::StepUp);
	addAction(ActionStepUp);

	QAction *ActionStepDown = ISControls::GetActionContextStepDown(this);
	ActionStepDown->setEnabled(Value != Minimum);
	connect(ActionStepDown, &QAction::triggered, this, &ISContextMenuInteger::StepDown);
	addAction(ActionStepDown);
}
//-----------------------------------------------------------------------------
ISContextMenuInteger::~ISContextMenuInteger()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISContextMenuText::ISContextMenuText(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int SelectedTextCount)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionToUpper = ISControls::GetActionContextToUpper(this);
	ActionToUpper->setEnabled(!ReadOnly && SelectedTextCount);
	connect(ActionToUpper, &QAction::triggered, this, &ISContextMenuText::UppercaseText);
	addAction(ActionToUpper);

	QAction *ActionToLower = ISControls::GetActionContextToLower(this);
	ActionToLower->setEnabled(!ReadOnly && SelectedTextCount);
	connect(ActionToLower, &QAction::triggered, this, &ISContextMenuText::LowercaseText);
	addAction(ActionToLower);

	addSeparator();

	QAction *ActionSearch = new QAction(this);
	ActionSearch->setEnabled(!ReadOnly && !Empty && EchoMode == QLineEdit::Normal);
	ActionSearch->setText(LANG("SearchIn"));
	ActionSearch->setMenu(new QMenu(this));
	addAction(ActionSearch);

	QAction *ActionGoogle = ISControls::GetActionContextGoogle(this);
	connect(ActionGoogle, &QAction::triggered, this, &ISContextMenuText::SearchGoogle);
	ActionSearch->menu()->addAction(ActionGoogle);

	QAction *ActionYandex = ISControls::GetActionContextYandex(this);
	connect(ActionYandex, &QAction::triggered, this, &ISContextMenuText::SearchYandex);
	ActionSearch->menu()->addAction(ActionYandex);

	QAction *ActionWikipedia = ISControls::GetActionContextWikipedia(this);
	connect(ActionWikipedia, &QAction::triggered, this, &ISContextMenuText::SearchWikipedia);
	ActionSearch->menu()->addAction(ActionWikipedia);

	QAction *ActionMail = ISControls::GetActionContextMail(this);
	connect(ActionMail, &QAction::triggered, this, &ISContextMenuText::SearchMail);
	ActionSearch->menu()->addAction(ActionMail);

	QAction *ActionBing = ISControls::GetActionContextBing(this);
	connect(ActionBing, &QAction::triggered, this, &ISContextMenuText::SearchBing);
	ActionSearch->menu()->addAction(ActionBing);

	QAction *ActionYahoo = ISControls::GetActionContextYahoo(this);
	connect(ActionYahoo, &QAction::triggered, this, &ISContextMenuText::SearchYahoo);
	ActionSearch->menu()->addAction(ActionYahoo);
}
//-----------------------------------------------------------------------------
ISContextMenuText::~ISContextMenuText()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
