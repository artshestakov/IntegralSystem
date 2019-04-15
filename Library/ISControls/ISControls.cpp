#include "StdAfx.h"
#include "ISControls.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
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
	ISSystem::RepaintWidget(Widget);
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
	ActionClose->setText(LOCALIZATION("Close"));
	ActionClose->setToolTip(LOCALIZATION("Close"));
	ActionClose->setIcon(BUFFER_ICONS("Close"));
	return ActionClose;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSave(QObject *ParentObject)
{
	QAction *ActionSave = new QAction(ParentObject);
	ActionSave->setText(LOCALIZATION("Save"));
	ActionSave->setToolTip(LOCALIZATION("Save"));
	ActionSave->setIcon(BUFFER_ICONS("Save"));
	ActionSave->setShortcut(QKeySequence(Qt::Key_S));
	return ActionSave;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSaveAndClose(QObject *ParentObject)
{
	QAction *ActionSaveAndClose = new QAction(ParentObject);
	ActionSaveAndClose->setText(LOCALIZATION("SaveClose"));
	ActionSaveAndClose->setToolTip(LOCALIZATION("SaveClose"));
	ActionSaveAndClose->setIcon(BUFFER_ICONS("SaveClose"));
	ActionSaveAndClose->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Enter));
	return ActionSaveAndClose;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionExit(QObject *ParentObject)
{
	QAction *ActionExit = new QAction(ParentObject);
	ActionExit->setText(LOCALIZATION("Exit"));
	ActionExit->setToolTip(LOCALIZATION("Exit"));
	ActionExit->setIcon(BUFFER_ICONS("Exit"));
	return ActionExit;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionFavorites(QObject *ParentObject)
{
	QAction *ActionFavorites = new QAction(ParentObject);
	ActionFavorites->setText(LOCALIZATION("Favorites"));
	ActionFavorites->setToolTip(LOCALIZATION("Favorites"));
	ActionFavorites->setIcon(BUFFER_ICONS("Favorites"));
	return ActionFavorites;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionPasswordChange(QObject *ParentObject)
{
	QAction *ActionPasswordChange = new QAction(ParentObject);
	ActionPasswordChange->setText(LOCALIZATION("ChangePassword"));
	ActionPasswordChange->setToolTip(LOCALIZATION("ChangePassword"));
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
	ContextActionUndo->setText(LOCALIZATION("ContextMenu.Undo"));
	ContextActionUndo->setToolTip(LOCALIZATION("ContextMenu.Undo.ToolTip"));
	ContextActionUndo->setIcon(BUFFER_ICONS("Undo"));
	return ContextActionUndo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextRedo(QObject *ParentObject)
{
	QAction *ContextActionRedo = new QAction(ParentObject);
	ContextActionRedo->setText(LOCALIZATION("ContextMenu.Redo"));
	ContextActionRedo->setToolTip(LOCALIZATION("ContextMenu.Redo.ToolTip"));
	ContextActionRedo->setIcon(BUFFER_ICONS("Redo"));
	return ContextActionRedo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextCut(QObject *ParentObject)
{
	QAction *ContextActionCut = new QAction(ParentObject);
	ContextActionCut->setText(LOCALIZATION("ContextMenu.Cut"));
	ContextActionCut->setToolTip(LOCALIZATION("ContextMenu.Cut.ToolTip"));
	ContextActionCut->setIcon(BUFFER_ICONS("Cut"));
	return ContextActionCut;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextCopy(QObject *ParentObject)
{
	QAction *ContextActionCopy = new QAction(ParentObject);
	ContextActionCopy->setText(LOCALIZATION("ContextMenu.Copy"));
	ContextActionCopy->setToolTip(LOCALIZATION("ContextMenu.Copy.ToolTip"));
	ContextActionCopy->setIcon(BUFFER_ICONS("Copy"));
	return ContextActionCopy;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextPaste(QObject *ParentObject)
{
	QAction *ContextActionPaste = new QAction(ParentObject);
	ContextActionPaste->setText(LOCALIZATION("ContextMenu.Paste"));
	ContextActionPaste->setToolTip(LOCALIZATION("ContextMenu.Paste.ToolTip"));
	ContextActionPaste->setIcon(BUFFER_ICONS("Paste"));
	return ContextActionPaste;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextDelete(QObject *ParentObject)
{
	QAction *ContextActionDelete = new QAction(ParentObject);
	ContextActionDelete->setText(LOCALIZATION("ContextMenu.Delete"));
	ContextActionDelete->setToolTip(LOCALIZATION("ContextMenu.Delete.ToolTip"));
	ContextActionDelete->setIcon(BUFFER_ICONS("Delete"));
	return ContextActionDelete;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextSelectAll(QObject *ParentObject)
{
	QAction *ContextActionSelectAll = new QAction(ParentObject);
	ContextActionSelectAll->setText(LOCALIZATION("ContextMenu.SelectAll"));
	ContextActionSelectAll->setToolTip(LOCALIZATION("ContextMenu.SelectAll"));
	return ContextActionSelectAll;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextVirtualKeyboard(QObject *ParentObject)
{
	QAction *ContextActionVirtualKeyboard = new QAction(ParentObject);
	ContextActionVirtualKeyboard->setText(LOCALIZATION("ContextMenu.ScreenKeyboard"));
	ContextActionVirtualKeyboard->setToolTip(LOCALIZATION("ContextMenu.ScreenKeyboard.ToolTip"));
	ContextActionVirtualKeyboard->setIcon(BUFFER_ICONS("Keyboard"));
	return ContextActionVirtualKeyboard;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextToUpper(QObject *ParentObject)
{
	QAction *ContextActionUpper = new QAction(ParentObject);
	ContextActionUpper->setText(LOCALIZATION("ContextMenu.ToUpper"));
	ContextActionUpper->setToolTip(LOCALIZATION("ContextMenu.ToUpper.ToolTip"));
	ContextActionUpper->setIcon(BUFFER_ICONS("Register.Upper"));
	return ContextActionUpper;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextToLower(QObject *ParentObject)
{
	QAction *ContextActionLower = new QAction(ParentObject);
	ContextActionLower->setText(LOCALIZATION("ContextMenu.ToLower"));
	ContextActionLower->setToolTip(LOCALIZATION("ContextMenu.ToLower.ToolTip"));
	ContextActionLower->setIcon(BUFFER_ICONS("Register.Lower"));
	return ContextActionLower;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextGoogle(QObject *ParentObject)
{
	QAction *ContextActionGoogle = new QAction(ParentObject);
	ContextActionGoogle->setText(LOCALIZATION("ContextMenu.Google"));
	ContextActionGoogle->setToolTip(LOCALIZATION("ContextMenu.Google"));
	ContextActionGoogle->setIcon(BUFFER_ICONS("Google"));
	return ContextActionGoogle;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextYandex(QObject *ParentObject)
{
	QAction *ContextActionYandex = new QAction(ParentObject);
	ContextActionYandex->setText(LOCALIZATION("ContextMenu.Yandex"));
	ContextActionYandex->setToolTip(LOCALIZATION("ContextMenu.Yandex"));
	ContextActionYandex->setIcon(BUFFER_ICONS("Yandex"));
	return ContextActionYandex;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextWikipedia(QObject *ParentObject)
{
	QAction *ContextActionWikipedia = new QAction(ParentObject);
	ContextActionWikipedia->setText(LOCALIZATION("ContextMenu.Wikipedia"));
	ContextActionWikipedia->setToolTip(LOCALIZATION("ContextMenu.Wikipedia"));
	ContextActionWikipedia->setIcon(BUFFER_ICONS("Wikipedia"));
	return ContextActionWikipedia;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextMail(QObject *ParentObject)
{
	QAction *ContextActionMail = new QAction(ParentObject);
	ContextActionMail->setText(LOCALIZATION("ContextMenu.Mail"));
	ContextActionMail->setToolTip(LOCALIZATION("ContextMenu.Mail"));
	ContextActionMail->setIcon(BUFFER_ICONS("Mail.ru"));
	return ContextActionMail;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextBing(QObject *ParentObject)
{
	QAction *ContextActionBing = new QAction(ParentObject);
	ContextActionBing->setText(LOCALIZATION("ContextMenu.Bing"));
	ContextActionBing->setToolTip(LOCALIZATION("ContextMenu.Bing"));
	ContextActionBing->setIcon(BUFFER_ICONS("Bing"));
	return ContextActionBing;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextYahoo(QObject *ParentObject)
{
	QAction *ContextActionYahoo = new QAction(ParentObject);
	ContextActionYahoo->setText(LOCALIZATION("ContextMenu.Yahoo"));
	ContextActionYahoo->setToolTip(LOCALIZATION("ContextMenu.Yahoo"));
	ContextActionYahoo->setIcon(BUFFER_ICONS("Yahoo"));
	return ContextActionYahoo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextStepUp(QObject *ParentObject)
{
	QAction *ActionStepUp = new QAction(ParentObject);
	ActionStepUp->setText(LOCALIZATION("ContextMenu.StepUp"));
	ActionStepUp->setIcon(BUFFER_ICONS("ContextMenu.StepUp"));
	return ActionStepUp;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextStepDown(QObject *ParentObject)
{
	QAction *ActionStepDown = new QAction(ParentObject);
	ActionStepDown->setText(LOCALIZATION("ContextMenu.StepDown"));
	ActionStepDown->setIcon(BUFFER_ICONS("ContextMenu.StepDown"));
	return ActionStepDown;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionCreate(QObject *ParentObject)
{
	QAction *ActionCreate = new QAction(ParentObject);
	ActionCreate->setText(LOCALIZATION("ListForm.Add"));
	ActionCreate->setToolTip(LOCALIZATION("ListForm.Add.ToolTip"));
	ActionCreate->setIcon(BUFFER_ICONS("Add"));
	ActionCreate->setShortcut(QKeySequence(Qt::Key_Insert));
	return ActionCreate;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionCreateCopy(QObject *ParentObject)
{
	QAction *ActionCreateCopy = new QAction(ParentObject);
	ActionCreateCopy->setText(LOCALIZATION("ListForm.AddCopy"));
	ActionCreateCopy->setToolTip(LOCALIZATION("ListForm.AddCopy.ToolTip"));
	ActionCreateCopy->setIcon(BUFFER_ICONS("AddCopy"));
	ActionCreateCopy->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Insert));
	return ActionCreateCopy;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionEdit(QObject *ParentObject)
{
	QAction *ActionEdit = new QAction(ParentObject);
	ActionEdit->setText(LOCALIZATION("ListForm.Edit"));
	ActionEdit->setToolTip(LOCALIZATION("ListForm.Edit.ToolTip"));
	ActionEdit->setIcon(BUFFER_ICONS("Edit"));
	ActionEdit->setShortcut(QKeySequence(Qt::Key_F2));
	return ActionEdit;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDelete(QObject *ParentObject)
{
	QAction *ActionDelete = new QAction(ParentObject);
	ActionDelete->setText(LOCALIZATION("ListForm.Delete"));
	ActionDelete->setToolTip(LOCALIZATION("ListForm.Delete.ToolTip"));
	ActionDelete->setIcon(BUFFER_ICONS("Delete"));
	ActionDelete->setShortcut(QKeySequence(Qt::Key_Delete));
	return ActionDelete;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDeleteCascade(QObject *ParentObject)
{
	QAction *ActionDeleteCascade = new QAction(ParentObject);
	ActionDeleteCascade->setText(LOCALIZATION("ListForm.DeleteCascade"));
	ActionDeleteCascade->setToolTip(LOCALIZATION("ListForm.DeleteCascade.ToolTip"));
	ActionDeleteCascade->setIcon(BUFFER_ICONS("DeleteCascade"));
	ActionDeleteCascade->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
	return ActionDeleteCascade;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionUpdate(QObject *ParentObject)
{
	QAction *ActionUpdate = new QAction(ParentObject);
	ActionUpdate->setText(LOCALIZATION("ListForm.Update"));
	ActionUpdate->setToolTip(LOCALIZATION("ListForm.Update.ToolTip"));
	ActionUpdate->setIcon(BUFFER_ICONS("Update"));
	ActionUpdate->setShortcut(QKeySequence(Qt::Key_F5));
	return ActionUpdate;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionShowDeleted(QObject *ParentObject)
{
	QAction *ActionShowDeleted = new QAction(ParentObject);
	ActionShowDeleted->setText(LOCALIZATION("ListForm.ShowDeleted"));
	ActionShowDeleted->setToolTip(LOCALIZATION("ListForm.ShowDeleted.ToolTip"));
	ActionShowDeleted->setIcon(BUFFER_ICONS("ShowDeleted"));
	ActionShowDeleted->setCheckable(true);
	ActionShowDeleted->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F1));
	return ActionShowDeleted;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSearch(QObject *ParentObject)
{
	QAction *ActionSearch = new QAction(ParentObject);
	ActionSearch->setText(LOCALIZATION("ListForm.Search"));
	ActionSearch->setToolTip(LOCALIZATION("ListForm.Search.ToolTip"));
	ActionSearch->setIcon(BUFFER_ICONS("Search"));
	ActionSearch->setShortcut(QKeySequence(Qt::Key_F7));
	return ActionSearch;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSearchClearResults(QObject *ParentObject)
{
	QAction *ActionSearchFullTextClear = new QAction(ParentObject);
	ActionSearchFullTextClear->setText(LOCALIZATION("Search.Cancel"));
	ActionSearchFullTextClear->setToolTip(LOCALIZATION("Search.Cancel"));
	ActionSearchFullTextClear->setIcon(BUFFER_ICONS("SearchClearResult"));
	return ActionSearchFullTextClear;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionExport(QObject *ParentObject)
{
	QAction *ActionExport = new QAction(ParentObject);
	ActionExport->setText(LOCALIZATION("ListForm.ExportTable"));
	ActionExport->setToolTip(LOCALIZATION("ListForm.ExportTable"));
	ActionExport->setIcon(BUFFER_ICONS("ExportTable"));
	ActionExport->setShortcut(QKeySequence(Qt::Key_F12));
	return ActionExport;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionPrint(QObject *ParentObject)
{
	QAction *ActionPrint = new QAction(ParentObject);
	ActionPrint->setText(LOCALIZATION("PrintForms"));
	ActionPrint->setToolTip(LOCALIZATION("PrintForms"));
	ActionPrint->setIcon(BUFFER_ICONS("Print"));
	return ActionPrint;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionRecordInformartion(QObject *ParentObject)
{
	QAction *ActionSystemInformation = new QAction(ParentObject);
	ActionSystemInformation->setText(LOCALIZATION("RecordInformation"));
	ActionSystemInformation->setToolTip(LOCALIZATION("RecordInformation"));
	ActionSystemInformation->setIcon(BUFFER_ICONS("RecordInformation"));
	ActionSystemInformation->setShortcut(QKeySequence(Qt::Key_F9));
	return ActionSystemInformation;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionShare(QObject *ParentObject)
{
	QAction *ActionShare = new QAction(ParentObject);
	ActionShare->setText(LOCALIZATION("Share"));
	ActionShare->setToolTip(LOCALIZATION("Share"));
	ActionShare->setIcon(BUFFER_ICONS("Share"));
	return ActionShare;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionAttachTask(QObject *ParentObject)
{
	QAction *ActionAttachTask = new QAction(ParentObject);
	ActionAttachTask->setText(LOCALIZATION("AttachTask"));
	ActionAttachTask->setToolTip(LOCALIZATION("AttachTask"));
	ActionAttachTask->setIcon(BUFFER_ICONS("Task"));
	return ActionAttachTask;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDetachTask(QObject *ParentObject)
{
	QAction *ActionDetachTask = new QAction(ParentObject);
	ActionDetachTask->setText(LOCALIZATION("Task.DetachFromTask"));
	ActionDetachTask->setToolTip(LOCALIZATION("Task.DetachFromTask"));
	ActionDetachTask->setIcon(BUFFER_ICONS("Task.Detach"));
	ActionDetachTask->setShortcut(QKeySequence(Qt::Key_F10));
	return ActionDetachTask;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionReference(QObject *ParentObject)
{
	QAction *ActionReference = new QAction(ParentObject);
	ActionReference->setText(LOCALIZATION("Reference"));
	ActionReference->setToolTip(LOCALIZATION("Reference"));
	ActionReference->setIcon(BUFFER_ICONS("Reference"));
	return ActionReference;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationBegin(QObject *ParentObject)
{
	QAction *ActionTableNavigationBegin = new QAction(ParentObject);
	ActionTableNavigationBegin->setText(LOCALIZATION("TableNavigationSelectBegin"));
	ActionTableNavigationBegin->setToolTip(LOCALIZATION("TableNavigationSelectBegin"));
	ActionTableNavigationBegin->setIcon(BUFFER_ICONS("TableNavigationBegin"));
	ActionTableNavigationBegin->setShortcut(QKeySequence(Qt::Key_Home));
	return ActionTableNavigationBegin;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationPrevious(QObject *ParentObject)
{
	QAction *ActionTableNavigationPrevious = new QAction(ParentObject);
	ActionTableNavigationPrevious->setText(LOCALIZATION("TableNavigationSelectPrevious"));
	ActionTableNavigationPrevious->setToolTip(LOCALIZATION("TableNavigationSelectPrevious"));
	ActionTableNavigationPrevious->setIcon(BUFFER_ICONS("TableNavigationPrevious"));
	return ActionTableNavigationPrevious;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationNext(QObject *ParentObject)
{
	QAction *ActionTableNavigationNext = new QAction(ParentObject);
	ActionTableNavigationNext->setText(LOCALIZATION("TableNavigationSelectNext"));
	ActionTableNavigationNext->setToolTip(LOCALIZATION("TableNavigationSelectNext"));
	ActionTableNavigationNext->setIcon(BUFFER_ICONS("TableNavigationNext"));
	return ActionTableNavigationNext;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNavigationLast(QObject *ParentObject)
{
	QAction *ActionTableNavigationLast = new QAction(ParentObject);
	ActionTableNavigationLast->setText(LOCALIZATION("TableNavigationSelectLast"));
	ActionTableNavigationLast->setToolTip(LOCALIZATION("TableNavigationSelectLast"));
	ActionTableNavigationLast->setIcon(BUFFER_ICONS("TableNavigationLast"));
	ActionTableNavigationLast->setShortcut(QKeySequence(Qt::Key_End));
	return ActionTableNavigationLast;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSortDefault(QObject *ParentObject)
{
	QAction *ActionSortDefault = new QAction(ParentObject);
	ActionSortDefault->setText(LOCALIZATION("DefaultSorting"));
	ActionSortDefault->setToolTip(LOCALIZATION("DefaultSorting"));
	ActionSortDefault->setIcon(BUFFER_ICONS("DefaultSorting"));
	return ActionSortDefault;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNoteObject(QObject *ParentObject)
{
	QAction *ActionTableNoteObject = new QAction(ParentObject);
	ActionTableNoteObject->setText(LOCALIZATION("NoteToRecord"));
	ActionTableNoteObject->setToolTip(LOCALIZATION("NoteToRecord"));
	ActionTableNoteObject->setIcon(BUFFER_ICONS("NoteObject"));
	return ActionTableNoteObject;
}
//-----------------------------------------------------------------------------
