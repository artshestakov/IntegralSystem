#include "ISSelectDialogForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISSelectDialogForm::ISSelectDialogForm(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID) : ISInterfaceDialogForm()
{
	resize(ISDefines::Gui::SIZE_640_480);
	setWindowTitle(ISMetaData::Instance().GetMetaTable(TableName)->LocalListName);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelInfo = new QLabel(this);
	LabelInfo->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	ISGui::SetFontWidgetUnderline(LabelInfo, true);
	GetMainLayout()->addWidget(LabelInfo);

	SelectListForm = new ISSelectListForm(SelectMode, TableName, this);
	connect(SelectListForm, &ISSelectListForm::Select, this, &ISSelectDialogForm::Selected);
	GetMainLayout()->addWidget(SelectListForm);

	if (SelectObjectID)
	{
		SelectListForm->SetSelectObjectAfterUpdate(SelectObjectID);
	}

	if (SelectMode == ISNamespace::SLM_Single)
	{
		LabelInfo->setText(LANG("SelectDialog.Title.Single") + ':');
	}
	else if (SelectMode == ISNamespace::SLM_Multi)
	{
		LabelInfo->setText(LANG("SelectDialog.Title.Multi") + ':');
	}
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
void ISSelectDialogForm::Selected()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
