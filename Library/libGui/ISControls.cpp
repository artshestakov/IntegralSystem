#include "StdAfx.h"
#include "ISControls.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISControls::ISControls()
{

}
//-----------------------------------------------------------------------------
ISControls::~ISControls()
{

}
//-----------------------------------------------------------------------------
void ISControls::SetBackgroundColorWidget(QWidget *Widget, const QColor &Color)
{
	QPalette Palette = Widget->palette();
	Palette.setColor(QPalette::Window, Color);
	Widget->setPalette(Palette);
	ISGui::RepaintWidget(Widget);
}
//-----------------------------------------------------------------------------
QFrame* ISControls::CreateHorizontalLine(QWidget *ParentWidget)
{
	QFrame *Frame = new QFrame(ParentWidget);
	Frame->setFrameShadow(QFrame::Sunken);
	Frame->setFrameShape(QFrame::HLine);
	return Frame;
}
//-----------------------------------------------------------------------------
QFrame* ISControls::CreateHorizontalLinePlain(QWidget *ParentWidget)
{
	QFrame *Frame = new QFrame(ParentWidget);
	Frame->setFrameShadow(QFrame::Plain);
	Frame->setFrameShape(QFrame::HLine);
	return Frame;
}
//-----------------------------------------------------------------------------
QFrame* ISControls::CreateVerticalLine(QWidget *ParentWidget)
{
	QFrame *Frame = new QFrame(ParentWidget);
	Frame->setFrameShadow(QFrame::Sunken);
	Frame->setFrameShape(QFrame::VLine);
	return Frame;
}
//-----------------------------------------------------------------------------
QFrame* ISControls::CreateVerticalLinePlain(QWidget *ParentWidget)
{
	QFrame *Frame = new QFrame(ParentWidget);
	Frame->setFrameShadow(QFrame::Plain);
	Frame->setFrameShape(QFrame::VLine);
	return Frame;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionClose(QObject *ParentObject)
{
	QAction *ActionClose = new QAction(ParentObject);
	ActionClose->setText(LANG("Close"));
	ActionClose->setToolTip(LANG("Close"));
	ActionClose->setIcon(BUFFER_ICONS("Close"));
	return ActionClose;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSave(QObject *ParentObject)
{
	QAction *ActionSave = new QAction(ParentObject);
	ActionSave->setText(LANG("Save"));
	ActionSave->setToolTip(LANG("Save"));
	ActionSave->setIcon(BUFFER_ICONS("Save"));
	ActionSave->setShortcut(QKeySequence(Qt::Key_S));
	return ActionSave;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSaveAndClose(QObject *ParentObject)
{
	QAction *ActionSaveAndClose = new QAction(ParentObject);
	ActionSaveAndClose->setText(LANG("SaveClose"));
	ActionSaveAndClose->setToolTip(LANG("SaveClose"));
	ActionSaveAndClose->setIcon(BUFFER_ICONS("SaveClose"));
	ActionSaveAndClose->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Enter));
	return ActionSaveAndClose;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionExit(QObject *ParentObject)
{
	QAction *ActionExit = new QAction(ParentObject);
	ActionExit->setText(LANG("Exit"));
	ActionExit->setToolTip(LANG("Exit"));
	ActionExit->setIcon(BUFFER_ICONS("Exit"));
	return ActionExit;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionFavorites(QObject *ParentObject)
{
	QAction *ActionFavorites = new QAction(ParentObject);
	ActionFavorites->setText(LANG("Favorites"));
	ActionFavorites->setToolTip(LANG("Favorites"));
	ActionFavorites->setIcon(BUFFER_ICONS("Favorites"));
	return ActionFavorites;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionPasswordChange(QObject *ParentObject)
{
	QAction *ActionPasswordChange = new QAction(ParentObject);
	ActionPasswordChange->setText(LANG("ChangePassword"));
	ActionPasswordChange->setToolTip(LANG("ChangePassword"));
	ActionPasswordChange->setIcon(BUFFER_ICONS("User.ChangePassword"));
	return ActionPasswordChange;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateSeparator(QObject *ParentObject)
{
	QAction *ActionSeparator = new QAction(ParentObject);
	ActionSeparator->setSeparator(true);
	return ActionSeparator;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextUndo(QObject *ParentObject)
{
	QAction *ContextActionUndo = new QAction(ParentObject);
	ContextActionUndo->setText(LANG("ContextMenu.Undo"));
	ContextActionUndo->setToolTip(LANG("ContextMenu.Undo.ToolTip"));
	ContextActionUndo->setIcon(BUFFER_ICONS("Undo"));
	return ContextActionUndo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextRedo(QObject *ParentObject)
{
	QAction *ContextActionRedo = new QAction(ParentObject);
	ContextActionRedo->setText(LANG("ContextMenu.Redo"));
	ContextActionRedo->setToolTip(LANG("ContextMenu.Redo.ToolTip"));
	ContextActionRedo->setIcon(BUFFER_ICONS("Redo"));
	return ContextActionRedo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextCut(QObject *ParentObject)
{
	QAction *ContextActionCut = new QAction(ParentObject);
	ContextActionCut->setText(LANG("ContextMenu.Cut"));
	ContextActionCut->setToolTip(LANG("ContextMenu.Cut.ToolTip"));
	ContextActionCut->setIcon(BUFFER_ICONS("Cut"));
	return ContextActionCut;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextCopy(QObject *ParentObject)
{
	QAction *ContextActionCopy = new QAction(ParentObject);
	ContextActionCopy->setText(LANG("ContextMenu.Copy"));
	ContextActionCopy->setToolTip(LANG("ContextMenu.Copy.ToolTip"));
	ContextActionCopy->setIcon(BUFFER_ICONS("Copy"));
	return ContextActionCopy;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextPaste(QObject *ParentObject)
{
	QAction *ContextActionPaste = new QAction(ParentObject);
	ContextActionPaste->setText(LANG("ContextMenu.Paste"));
	ContextActionPaste->setToolTip(LANG("ContextMenu.Paste.ToolTip"));
	ContextActionPaste->setIcon(BUFFER_ICONS("Paste"));
	return ContextActionPaste;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextDelete(QObject *ParentObject)
{
	QAction *ContextActionDelete = new QAction(ParentObject);
	ContextActionDelete->setText(LANG("ContextMenu.Delete"));
	ContextActionDelete->setToolTip(LANG("ContextMenu.Delete.ToolTip"));
	ContextActionDelete->setIcon(BUFFER_ICONS("Delete"));
	return ContextActionDelete;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextSelectAll(QObject *ParentObject)
{
	QAction *ContextActionSelectAll = new QAction(ParentObject);
	ContextActionSelectAll->setText(LANG("ContextMenu.SelectAll"));
	ContextActionSelectAll->setToolTip(LANG("ContextMenu.SelectAll"));
	return ContextActionSelectAll;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextVirtualKeyboard(QObject *ParentObject)
{
	QAction *ContextActionVirtualKeyboard = new QAction(ParentObject);
	ContextActionVirtualKeyboard->setText(LANG("ContextMenu.ScreenKeyboard"));
	ContextActionVirtualKeyboard->setToolTip(LANG("ContextMenu.ScreenKeyboard.ToolTip"));
	ContextActionVirtualKeyboard->setIcon(BUFFER_ICONS("Keyboard"));
	return ContextActionVirtualKeyboard;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextToUpper(QObject *ParentObject)
{
	QAction *ContextActionUpper = new QAction(ParentObject);
	ContextActionUpper->setText(LANG("ContextMenu.ToUpper"));
	ContextActionUpper->setToolTip(LANG("ContextMenu.ToUpper.ToolTip"));
	ContextActionUpper->setIcon(BUFFER_ICONS("Register.Upper"));
	return ContextActionUpper;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextToLower(QObject *ParentObject)
{
	QAction *ContextActionLower = new QAction(ParentObject);
	ContextActionLower->setText(LANG("ContextMenu.ToLower"));
	ContextActionLower->setToolTip(LANG("ContextMenu.ToLower.ToolTip"));
	ContextActionLower->setIcon(BUFFER_ICONS("Register.Lower"));
	return ContextActionLower;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextGoogle(QObject *ParentObject)
{
	QAction *ContextActionGoogle = new QAction(ParentObject);
	ContextActionGoogle->setText(LANG("ContextMenu.Google"));
	ContextActionGoogle->setToolTip(LANG("ContextMenu.Google"));
	ContextActionGoogle->setIcon(BUFFER_ICONS("Google"));
	return ContextActionGoogle;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextYandex(QObject *ParentObject)
{
	QAction *ContextActionYandex = new QAction(ParentObject);
	ContextActionYandex->setText(LANG("ContextMenu.Yandex"));
	ContextActionYandex->setToolTip(LANG("ContextMenu.Yandex"));
	ContextActionYandex->setIcon(BUFFER_ICONS("Yandex"));
	return ContextActionYandex;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextWikipedia(QObject *ParentObject)
{
	QAction *ContextActionWikipedia = new QAction(ParentObject);
	ContextActionWikipedia->setText(LANG("ContextMenu.Wikipedia"));
	ContextActionWikipedia->setToolTip(LANG("ContextMenu.Wikipedia"));
	ContextActionWikipedia->setIcon(BUFFER_ICONS("Wikipedia"));
	return ContextActionWikipedia;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextMail(QObject *ParentObject)
{
	QAction *ContextActionMail = new QAction(ParentObject);
	ContextActionMail->setText(LANG("ContextMenu.Mail"));
	ContextActionMail->setToolTip(LANG("ContextMenu.Mail"));
	ContextActionMail->setIcon(BUFFER_ICONS("Mail.ru"));
	return ContextActionMail;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextBing(QObject *ParentObject)
{
	QAction *ContextActionBing = new QAction(ParentObject);
	ContextActionBing->setText(LANG("ContextMenu.Bing"));
	ContextActionBing->setToolTip(LANG("ContextMenu.Bing"));
	ContextActionBing->setIcon(BUFFER_ICONS("Bing"));
	return ContextActionBing;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextYahoo(QObject *ParentObject)
{
	QAction *ContextActionYahoo = new QAction(ParentObject);
	ContextActionYahoo->setText(LANG("ContextMenu.Yahoo"));
	ContextActionYahoo->setToolTip(LANG("ContextMenu.Yahoo"));
	ContextActionYahoo->setIcon(BUFFER_ICONS("Yahoo"));
	return ContextActionYahoo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextStepUp(QObject *ParentObject)
{
	QAction *ActionStepUp = new QAction(ParentObject);
	ActionStepUp->setText(LANG("ContextMenu.StepUp"));
	ActionStepUp->setIcon(BUFFER_ICONS("ContextMenu.StepUp"));
	return ActionStepUp;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextStepDown(QObject *ParentObject)
{
	QAction *ActionStepDown = new QAction(ParentObject);
	ActionStepDown->setText(LANG("ContextMenu.StepDown"));
	ActionStepDown->setIcon(BUFFER_ICONS("ContextMenu.StepDown"));
	return ActionStepDown;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionCreate(QObject *ParentObject)
{
	QAction *ActionCreate = new QAction(ParentObject);
	ActionCreate->setText(LANG("ListForm.Add"));
	ActionCreate->setToolTip(LANG("ListForm.Add.ToolTip"));
	ActionCreate->setIcon(BUFFER_ICONS("Add"));
	ActionCreate->setShortcut(QKeySequence(Qt::Key_Insert));
	return ActionCreate;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionCreateCopy(QObject *ParentObject)
{
	QAction *ActionCreateCopy = new QAction(ParentObject);
	ActionCreateCopy->setText(LANG("ListForm.AddCopy"));
	ActionCreateCopy->setToolTip(LANG("ListForm.AddCopy.ToolTip"));
	ActionCreateCopy->setIcon(BUFFER_ICONS("AddCopy"));
	ActionCreateCopy->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Insert));
	return ActionCreateCopy;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionEdit(QObject *ParentObject)
{
	QAction *ActionEdit = new QAction(ParentObject);
	ActionEdit->setText(LANG("ListForm.Edit"));
	ActionEdit->setToolTip(LANG("ListForm.Edit.ToolTip"));
	ActionEdit->setIcon(BUFFER_ICONS("Edit"));
	ActionEdit->setShortcut(QKeySequence(Qt::Key_F2));
	return ActionEdit;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDelete(QObject *ParentObject)
{
	QAction *ActionDelete = new QAction(ParentObject);
	ActionDelete->setText(LANG("ListForm.Delete"));
	ActionDelete->setToolTip(LANG("ListForm.Delete.ToolTip"));
	ActionDelete->setIcon(BUFFER_ICONS("Delete"));
	ActionDelete->setShortcut(QKeySequence(Qt::Key_Delete));
	return ActionDelete;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDeleteCascade(QObject *ParentObject)
{
	QAction *ActionDeleteCascade = new QAction(ParentObject);
	ActionDeleteCascade->setText(LANG("ListForm.DeleteCascade"));
	ActionDeleteCascade->setToolTip(LANG("ListForm.DeleteCascade.ToolTip"));
	ActionDeleteCascade->setIcon(BUFFER_ICONS("DeleteCascade"));
	ActionDeleteCascade->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
	return ActionDeleteCascade;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionUpdate(QObject *ParentObject)
{
	QAction *ActionUpdate = new QAction(ParentObject);
	ActionUpdate->setText(LANG("ListForm.Update"));
	ActionUpdate->setToolTip(LANG("ListForm.Update.ToolTip"));
	ActionUpdate->setIcon(BUFFER_ICONS("Update"));
	ActionUpdate->setShortcut(QKeySequence(Qt::Key_F5));
	return ActionUpdate;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionShowDeleted(QObject *ParentObject)
{
	QAction *ActionShowDeleted = new QAction(ParentObject);
	ActionShowDeleted->setText(LANG("ListForm.ShowDeleted"));
	ActionShowDeleted->setToolTip(LANG("ListForm.ShowDeleted.ToolTip"));
	ActionShowDeleted->setIcon(BUFFER_ICONS("ShowDeleted"));
	ActionShowDeleted->setCheckable(true);
	ActionShowDeleted->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F1));
	return ActionShowDeleted;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSearch(QObject *ParentObject)
{
	QAction *ActionSearch = new QAction(ParentObject);
	ActionSearch->setText(LANG("ListForm.Search"));
	ActionSearch->setToolTip(LANG("ListForm.Search.ToolTip"));
	ActionSearch->setIcon(BUFFER_ICONS("Search"));
	ActionSearch->setShortcut(QKeySequence(Qt::Key_F7));
	return ActionSearch;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSearchClearResults(QObject *ParentObject)
{
	QAction *ActionSearchFullTextClear = new QAction(ParentObject);
	ActionSearchFullTextClear->setText(LANG("Search.Cancel"));
	ActionSearchFullTextClear->setToolTip(LANG("Search.Cancel"));
	ActionSearchFullTextClear->setIcon(BUFFER_ICONS("SearchClearResult"));
	return ActionSearchFullTextClear;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionExport(QObject *ParentObject)
{
	QAction *ActionExport = new QAction(ParentObject);
	ActionExport->setText(LANG("ListForm.ExportTable"));
	ActionExport->setToolTip(LANG("ListForm.ExportTable"));
	ActionExport->setIcon(BUFFER_ICONS("ExportTable"));
	ActionExport->setShortcut(QKeySequence(Qt::Key_F12));
	return ActionExport;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionPrint(QObject *ParentObject)
{
	QAction *ActionPrint = new QAction(ParentObject);
	ActionPrint->setText(LANG("PrintForms"));
	ActionPrint->setToolTip(LANG("PrintForms"));
	ActionPrint->setIcon(BUFFER_ICONS("Print"));
	return ActionPrint;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionRecordInformartion(QObject *ParentObject)
{
	QAction *ActionSystemInformation = new QAction(ParentObject);
	ActionSystemInformation->setText(LANG("RecordInformation"));
	ActionSystemInformation->setToolTip(LANG("RecordInformation"));
	ActionSystemInformation->setIcon(BUFFER_ICONS("RecordInformation"));
	ActionSystemInformation->setShortcut(QKeySequence(Qt::Key_F9));
	return ActionSystemInformation;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionShare(QObject *ParentObject)
{
	QAction *ActionShare = new QAction(ParentObject);
	ActionShare->setText(LANG("Share"));
	ActionShare->setToolTip(LANG("Share"));
	ActionShare->setIcon(BUFFER_ICONS("Share"));
	return ActionShare;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionAttachTask(QObject *ParentObject)
{
	QAction *ActionAttachTask = new QAction(ParentObject);
	ActionAttachTask->setText(LANG("AttachTask"));
	ActionAttachTask->setToolTip(LANG("AttachTask"));
	ActionAttachTask->setIcon(BUFFER_ICONS("Task"));
	return ActionAttachTask;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDetachTask(QObject *ParentObject)
{
	QAction *ActionDetachTask = new QAction(ParentObject);
	ActionDetachTask->setText(LANG("Task.DetachFromTask"));
	ActionDetachTask->setToolTip(LANG("Task.DetachFromTask"));
	ActionDetachTask->setIcon(BUFFER_ICONS("Task.Detach"));
	ActionDetachTask->setShortcut(QKeySequence(Qt::Key_F10));
	return ActionDetachTask;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionReference(QObject *ParentObject)
{
	QAction *ActionReference = new QAction(ParentObject);
	ActionReference->setText(LANG("Reference"));
	ActionReference->setToolTip(LANG("Reference"));
	ActionReference->setIcon(BUFFER_ICONS("Reference"));
	return ActionReference;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationBegin(QObject *ParentObject)
{
	QAction *ActionTableNavigationBegin = new QAction(ParentObject);
	ActionTableNavigationBegin->setText(LANG("TableNavigationSelectBegin"));
	ActionTableNavigationBegin->setToolTip(LANG("TableNavigationSelectBegin"));
	ActionTableNavigationBegin->setIcon(BUFFER_ICONS("TableNavigationBegin"));
	ActionTableNavigationBegin->setShortcut(QKeySequence(Qt::Key_Home));
	return ActionTableNavigationBegin;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationPrevious(QObject *ParentObject)
{
	QAction *ActionTableNavigationPrevious = new QAction(ParentObject);
	ActionTableNavigationPrevious->setText(LANG("TableNavigationSelectPrevious"));
	ActionTableNavigationPrevious->setToolTip(LANG("TableNavigationSelectPrevious"));
	ActionTableNavigationPrevious->setIcon(BUFFER_ICONS("TableNavigationPrevious"));
	return ActionTableNavigationPrevious;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationNext(QObject *ParentObject)
{
	QAction *ActionTableNavigationNext = new QAction(ParentObject);
	ActionTableNavigationNext->setText(LANG("TableNavigationSelectNext"));
	ActionTableNavigationNext->setToolTip(LANG("TableNavigationSelectNext"));
	ActionTableNavigationNext->setIcon(BUFFER_ICONS("TableNavigationNext"));
	return ActionTableNavigationNext;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationLast(QObject *ParentObject)
{
	QAction *ActionTableNavigationLast = new QAction(ParentObject);
	ActionTableNavigationLast->setText(LANG("TableNavigationSelectLast"));
	ActionTableNavigationLast->setToolTip(LANG("TableNavigationSelectLast"));
	ActionTableNavigationLast->setIcon(BUFFER_ICONS("TableNavigationLast"));
	ActionTableNavigationLast->setShortcut(QKeySequence(Qt::Key_End));
	return ActionTableNavigationLast;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSortDefault(QObject *ParentObject)
{
	QAction *ActionSortDefault = new QAction(ParentObject);
	ActionSortDefault->setText(LANG("DefaultSorting"));
	ActionSortDefault->setToolTip(LANG("DefaultSorting"));
	ActionSortDefault->setIcon(BUFFER_ICONS("DefaultSorting"));
	return ActionSortDefault;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNoteObject(QObject *ParentObject)
{
	QAction *ActionTableNoteObject = new QAction(ParentObject);
	ActionTableNoteObject->setText(LANG("NoteToRecord"));
	ActionTableNoteObject->setToolTip(LANG("NoteToRecord"));
	ActionTableNoteObject->setIcon(BUFFER_ICONS("NoteObject"));
	return ActionTableNoteObject;
}
//-----------------------------------------------------------------------------
