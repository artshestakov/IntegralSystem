#include "ISContextMenu.h"
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

	if (!ReadOnly)
	{
		//Отменить действие
		QAction *ActionUndo = ISControls::GetActionContextUndo(this);
		ActionUndo->setEnabled(UndoAvailable);
		ActionUndo->setShortcut(QKeySequence::Undo);
		connect(ActionUndo, SIGNAL(triggered()), ParentEdit, SLOT(undo()));
		addAction(ActionUndo);

		//Повторить действие
		QAction *ActionRedo = ISControls::GetActionContextRedo(this);
		ActionRedo->setEnabled(RedoAvailable);
		ActionRedo->setShortcut(QKeySequence::Redo);
		connect(ActionRedo, SIGNAL(triggered()), ParentEdit, SLOT(redo()));
		addAction(ActionRedo);

		addAction(ISControls::CreateSeparator(this));
	}

	if (!ReadOnly)
	{
		//Вырезать
		QAction *ActionCut = ISControls::GetActionContextCut(this);
		ActionCut->setEnabled(!ReadOnly && HasSelectedText && EchoMode == QLineEdit::Normal);
		ActionCut->setShortcut(QKeySequence::Cut);
		connect(ActionCut, SIGNAL(triggered()), ParentEdit, SLOT(cut()));
		addAction(ActionCut);
	}

	//Копировать
	QAction *ActionCopy = ISControls::GetActionContextCopy(this);
	ActionCopy->setEnabled(HasSelectedText && EchoMode == QLineEdit::Normal);
	ActionCopy->setShortcut(QKeySequence::Copy);
	connect(ActionCopy, SIGNAL(triggered()), ParentEdit, SLOT(copy()));
	addAction(ActionCopy);

	if (!ReadOnly)
	{
		//Вставить
		QAction *ActionPaste = ISControls::GetActionContextPaste(this);
		ActionPaste->setEnabled(!ReadOnly && !QApplication::clipboard()->text().isEmpty());
		ActionPaste->setShortcut(QKeySequence::Paste);
		connect(ActionPaste, SIGNAL(triggered()), ParentEdit, SLOT(paste()));
		addAction(ActionPaste);
	}

	if (!ReadOnly)
	{
		//Удалить
		QAction *ActionDelete = ISControls::GetActionContextDelete(this);
		ActionDelete->setEnabled(!ReadOnly && !Empty && HasSelectedText);
		connect(ActionDelete, &QAction::triggered, this, &ISContextMenuBase::Delete);
		addAction(ActionDelete);
	}

	if (!isEmpty())
	{
		addAction(ISControls::CreateSeparator(this));
	}

	//Выбрать все
	QAction *ActionSelectAll = ISControls::GetActionContextSelectAll(this);
	ActionSelectAll->setShortcut(QKeySequence::SelectAll);
	ActionSelectAll->setEnabled(!Empty);
	connect(ActionSelectAll, SIGNAL(triggered()), ParentEdit, SLOT(selectAll()));
	addAction(ActionSelectAll);

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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISContextMenuImage::ISContextMenuImage(QWidget *parent, bool PixmapIsNull) : QMenu(parent)
{
	setToolTipsVisible(true);

	//Выбрать
	QAction *ActionSelect = new QAction(this);
	ActionSelect->setText(LANG("ContextMenu.Image.SelectFile"));
	ActionSelect->setToolTip(LANG("ContextMenu.Image.SelectFile.ToolTip"));
	ActionSelect->setIcon(BUFFER_ICONS("Select"));
	connect(ActionSelect, &QAction::triggered, this, &ISContextMenuImage::Select);
	addAction(ActionSelect);

	addAction(ISControls::CreateSeparator(this));

	//Вырезать
	QAction *ActionCut = new QAction(this);
	ActionCut->setEnabled(!PixmapIsNull);
	ActionCut->setText(LANG("ContextMenu.Image.Cut"));
	ActionCut->setToolTip(LANG("ContextMenu.Image.Cut.ToolTip"));
	ActionCut->setIcon(BUFFER_ICONS("Cut"));
	ActionCut->setShortcut(QKeySequence::Cut);
	connect(ActionCut, &QAction::triggered, this, &ISContextMenuImage::Cut);
	addAction(ActionCut);

	//Копировать
	QAction *ActionCopy = new QAction(this);
	ActionCopy->setEnabled(!PixmapIsNull);
	ActionCopy->setText(LANG("ContextMenu.Image.Copy"));
	ActionCopy->setToolTip(LANG("ContextMenu.Image.Copy.ToolTip"));
	ActionCopy->setIcon(BUFFER_ICONS("Copy"));
	ActionCopy->setShortcut(QKeySequence::Copy);
	connect(ActionCopy, &QAction::triggered, this, &ISContextMenuImage::Copy);
	addAction(ActionCopy);

	//Вставить
	QAction *ActionPaste = new QAction(this);
	ActionPaste->setEnabled(!QApplication::clipboard()->pixmap().isNull());
	ActionPaste->setText(LANG("ContextMenu.Image.Paste"));
	ActionPaste->setToolTip(LANG("ContextMenu.Image.Paste.ToolTip"));
	ActionPaste->setIcon(BUFFER_ICONS("Paste"));
	ActionPaste->setShortcut(QKeySequence::Paste);
	connect(ActionPaste, &QAction::triggered, this, &ISContextMenuImage::Paste);
	addAction(ActionPaste);

	//Вставить по ссылке
	QAction *ActionPasteFromLink = new QAction(this);
	ActionPasteFromLink->setText(LANG("ContextMenu.Image.PasteFromLink"));
	ActionPasteFromLink->setIcon(BUFFER_ICONS("PasteFromLink"));
	connect(ActionPasteFromLink, &QAction::triggered, this, &ISContextMenuImage::PasteFromLink);
	addAction(ActionPasteFromLink);

	addAction(ISControls::CreateSeparator(this));

	//Сохранить
	QAction *ActionSave = ISControls::CreateActionSave(this);
	ActionSave->setEnabled(!PixmapIsNull);
	connect(ActionSave, &QAction::triggered, this, &ISContextMenuImage::Save);
	addAction(ActionSave);

	//Просмотр
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
ISContextMenuText::ISContextMenuText(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int SelectedTextCount)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionToUpper = ISControls::GetActionContextToUpper(this);
	ActionToUpper->setEnabled(SelectedTextCount);
	ActionToUpper->setVisible(!ReadOnly);
	connect(ActionToUpper, &QAction::triggered, this, &ISContextMenuText::UppercaseText);
	addAction(ActionToUpper);

	QAction *ActionToLower = ISControls::GetActionContextToLower(this);
	ActionToLower->setEnabled(SelectedTextCount);
	ActionToLower->setVisible(!ReadOnly);
	connect(ActionToLower, &QAction::triggered, this, &ISContextMenuText::LowercaseText);
	addAction(ActionToLower);
}
//-----------------------------------------------------------------------------
ISContextMenuText::~ISContextMenuText()
{

}
//-----------------------------------------------------------------------------
