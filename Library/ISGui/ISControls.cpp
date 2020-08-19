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
	return new QAction(BUFFER_ICONS("Close"), LANG("Close"), ParentObject);
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSave(QObject *ParentObject)
{
	QAction *ActionSave = new QAction(BUFFER_ICONS("Save"), LANG("Save"), ParentObject);
	ActionSave->setShortcut(QKeySequence(Qt::Key_S));
	return ActionSave;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSaveAndClose(QObject *ParentObject)
{
	QAction *ActionSaveAndClose = new QAction(BUFFER_ICONS("SaveClose"), LANG("SaveClose"), ParentObject);
	ActionSaveAndClose->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Enter));
	return ActionSaveAndClose;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionExit(QObject *ParentObject)
{
	return new QAction(BUFFER_ICONS("Exit"), LANG("Exit"), ParentObject);
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionPasswordChange(QObject *ParentObject)
{
	return new QAction(BUFFER_ICONS("User.ChangePassword"), LANG("ChangePassword"), ParentObject);
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
	QAction *ContextActionUndo = new QAction(BUFFER_ICONS("Undo"), LANG("ContextMenu.Undo"), ParentObject);
	ContextActionUndo->setToolTip(LANG("ContextMenu.Undo.ToolTip"));
	return ContextActionUndo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextRedo(QObject *ParentObject)
{
	QAction *ContextActionRedo = new QAction(BUFFER_ICONS("Redo"), LANG("ContextMenu.Redo"), ParentObject);
	ContextActionRedo->setToolTip(LANG("ContextMenu.Redo.ToolTip"));
	return ContextActionRedo;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextCut(QObject *ParentObject)
{
	QAction *ContextActionCut = new QAction(BUFFER_ICONS("Cut"), LANG("ContextMenu.Cut"), ParentObject);
	ContextActionCut->setToolTip(LANG("ContextMenu.Cut.ToolTip"));
	return ContextActionCut;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextCopy(QObject *ParentObject)
{
	QAction *ContextActionCopy = new QAction(BUFFER_ICONS("Copy"), LANG("ContextMenu.Copy"), ParentObject);
	ContextActionCopy->setToolTip(LANG("ContextMenu.Copy.ToolTip"));
	return ContextActionCopy;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextPaste(QObject *ParentObject)
{
	QAction *ContextActionPaste = new QAction(BUFFER_ICONS("Paste"), LANG("ContextMenu.Paste"), ParentObject);
	ContextActionPaste->setToolTip(LANG("ContextMenu.Paste.ToolTip"));
	return ContextActionPaste;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextDelete(QObject *ParentObject)
{
	QAction *ContextActionDelete = new QAction(BUFFER_ICONS("Delete"), LANG("ContextMenu.Delete"), ParentObject);
	ContextActionDelete->setToolTip(LANG("ContextMenu.Delete.ToolTip"));
	return ContextActionDelete;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextSelectAll(QObject *ParentObject)
{
	return new QAction(LANG("ContextMenu.SelectAll"), ParentObject);
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextToUpper(QObject *ParentObject)
{
	QAction *ContextActionUpper = new QAction(BUFFER_ICONS("Register.Upper"), LANG("ContextMenu.ToUpper"), ParentObject);
	ContextActionUpper->setToolTip(LANG("ContextMenu.ToUpper.ToolTip"));
	return ContextActionUpper;
}
//-----------------------------------------------------------------------------
QAction* ISControls::GetActionContextToLower(QObject *ParentObject)
{
	QAction *ContextActionLower = new QAction(BUFFER_ICONS("Register.Lower"), LANG("ContextMenu.ToLower"), ParentObject);
	ContextActionLower->setToolTip(LANG("ContextMenu.ToLower.ToolTip"));
	return ContextActionLower;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionCreate(QObject *ParentObject)
{
	QAction *ActionCreate = new QAction(BUFFER_ICONS("Add"), LANG("ListForm.Add"), ParentObject);
	ActionCreate->setToolTip(LANG("ListForm.Add.ToolTip"));
	ActionCreate->setShortcut(QKeySequence(Qt::Key_Insert));
	return ActionCreate;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionCreateCopy(QObject *ParentObject)
{
	QAction *ActionCreateCopy = new QAction(BUFFER_ICONS("AddCopy"), LANG("ListForm.AddCopy"), ParentObject);
	ActionCreateCopy->setToolTip(LANG("ListForm.AddCopy.ToolTip"));
	ActionCreateCopy->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Insert));
	return ActionCreateCopy;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionEdit(QObject *ParentObject)
{
	QAction *ActionEdit = new QAction(BUFFER_ICONS("Edit"), LANG("ListForm.Edit"), ParentObject);
	ActionEdit->setToolTip(LANG("ListForm.Edit.ToolTip"));
	ActionEdit->setShortcut(QKeySequence(Qt::Key_F2));
	return ActionEdit;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDelete(QObject *ParentObject)
{
	QAction *ActionDelete = new QAction(BUFFER_ICONS("Delete"), LANG("ListForm.Delete"), ParentObject);
	ActionDelete->setToolTip(LANG("ListForm.Delete.ToolTip"));
	ActionDelete->setShortcut(QKeySequence(Qt::Key_Delete));
	return ActionDelete;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionDeleteCascade(QObject *ParentObject)
{
	QAction *ActionDeleteCascade = new QAction(BUFFER_ICONS("DeleteCascade"), LANG("ListForm.DeleteCascade"), ParentObject);
	ActionDeleteCascade->setToolTip(LANG("ListForm.DeleteCascade.ToolTip"));
	ActionDeleteCascade->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
	return ActionDeleteCascade;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionUpdate(QObject *ParentObject)
{
	QAction *ActionUpdate = new QAction(BUFFER_ICONS("Update"), LANG("ListForm.Update"), ParentObject);
	ActionUpdate->setToolTip(LANG("ListForm.Update.ToolTip"));
	ActionUpdate->setShortcut(QKeySequence(Qt::Key_F5));
	return ActionUpdate;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSearch(QObject *ParentObject)
{
	QAction *ActionSearch = new QAction(BUFFER_ICONS("Search"), LANG("ListForm.Search"), ParentObject);
	ActionSearch->setToolTip(LANG("ListForm.Search.ToolTip"));
	ActionSearch->setShortcut(QKeySequence(Qt::Key_F7));
	return ActionSearch;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionSearchClearResults(QObject *ParentObject)
{
	QAction *ActionSearchFullTextClear = new QAction(BUFFER_ICONS("SearchClearResult"), LANG("Search.Cancel"), ParentObject);
	ActionSearchFullTextClear->setToolTip(LANG("Search.Cancel"));
	return ActionSearchFullTextClear;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionPrint(QObject *ParentObject)
{
	return new QAction(BUFFER_ICONS("Print"), LANG("PrintForms"), ParentObject);
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionRecordInformartion(QObject *ParentObject)
{
	QAction *ActionSystemInformation = new QAction(BUFFER_ICONS("RecordInformation"), LANG("RecordInformation"), ParentObject);
	ActionSystemInformation->setShortcut(QKeySequence(Qt::Key_F9));
	return ActionSystemInformation;
}
//-----------------------------------------------------------------------------
QAction* ISControls::CreateActionNoteObject(QObject *ParentObject)
{
	return new QAction(BUFFER_ICONS("NoteObject"), LANG("NoteObject"), ParentObject);
}
//-----------------------------------------------------------------------------
