#include "ISSelectDialogForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISSelectDialogForm::ISSelectDialogForm(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID) : ISInterfaceDialogForm()
{
	setWindowTitle(ISMetaData::Instance().GetMetaTable(TableName)->LocalListName);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelInfo = new QLabel(this);
	LabelInfo->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	ISGui::SetFontWidgetUnderline(LabelInfo, true);
	GetMainLayout()->addWidget(LabelInfo);

	SelectListForm = new ISSelectListForm(SelectMode, TableName, this);
	connect(SelectListForm, &ISSelectListForm::Select, this, &ISSelectDialogForm::Selected);
	GetMainLayout()->addWidget(SelectListForm);

	if (SelectObjectID > 0)
	{
		SelectListForm->SetSelectObjectAfterUpdate(SelectObjectID);
	}

	LabelInfo->setText(LANG(SelectMode == ISNamespace::SLM_Single ? "SelectDialog.Title.Single" : "SelectDialog.Title.Multi") + ':');
}
//-----------------------------------------------------------------------------
ISSelectDialogForm::~ISSelectDialogForm()
{

}
//-----------------------------------------------------------------------------
bool ISSelectDialogForm::Exec()
{
	SelectListForm->LoadData();
	return ISInterfaceDialogForm::Exec();
}
//-----------------------------------------------------------------------------
int ISSelectDialogForm::GetSelectedObject() const
{
	return SelectListForm->GetObjectID();
}
//-----------------------------------------------------------------------------
ISVectorInt ISSelectDialogForm::GetSelectedObjects() const
{
	return SelectListForm->GetSelectedIDs();
}
//-----------------------------------------------------------------------------
void ISSelectDialogForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	//Изменение размера из конструктора поочему-то не работает, пришлось сделать так
	resize(1000, 700);
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
}
//-----------------------------------------------------------------------------
void ISSelectDialogForm::Selected()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
