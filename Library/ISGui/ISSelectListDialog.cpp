#include "ISSelectListDialog.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISSelectListDialog::ISSelectListDialog(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID) : ISInterfaceDialogForm()
{
	setWindowTitle(ISMetaData::Instance().GetMetaTable(TableName)->LocalListName);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelInfo = new QLabel(this);
	LabelInfo->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	ISGui::SetFontWidgetUnderline(LabelInfo, true);
	GetMainLayout()->addWidget(LabelInfo);

	SelectListForm = new ISSelectListForm(SelectMode, TableName, this);
	connect(SelectListForm, &ISSelectListForm::Select, this, &ISSelectListDialog::Selected);
	GetMainLayout()->addWidget(SelectListForm);

	if (SelectObjectID > 0)
	{
		SelectListForm->SetSelectObjectAfterUpdate(SelectObjectID);
	}

	LabelInfo->setText(SelectMode == ISNamespace::SLM_Single ? LANG("SelectDialog.Title.Single") : LANG("SelectDialog.Title.Multi") + ':');
}
//-----------------------------------------------------------------------------
ISSelectListDialog::~ISSelectListDialog()
{

}
//-----------------------------------------------------------------------------
bool ISSelectListDialog::Exec()
{
	SelectListForm->LoadData();
	return ISInterfaceDialogForm::Exec();
}
//-----------------------------------------------------------------------------
unsigned int ISSelectListDialog::GetSelectedObject() const
{
	return SelectListForm->GetObjectID();
}
//-----------------------------------------------------------------------------
ISVectorUInt ISSelectListDialog::GetSelectedObjects() const
{
	return SelectListForm->GetSelectedIDs();
}
//-----------------------------------------------------------------------------
void ISSelectListDialog::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	//Изменение размера из конструктора поочему-то не работает, пришлось сделать так
	resize(1000, 700);
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
}
//-----------------------------------------------------------------------------
void ISSelectListDialog::Selected()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
