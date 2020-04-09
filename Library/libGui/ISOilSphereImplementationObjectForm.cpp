#include "ISOilSphereImplementationObjectForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISOilSphereImplementationObjectForm::ISOilSphereImplementationObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	ImplementationDetailListForm = new ISListBaseForm("ImplementationDetail", this);
	ImplementationDetailListForm->setSizePolicy(ImplementationDetailListForm->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	ImplementationDetailListForm->GetQueryModel()->SetClassFilter("imdt_implementation = :ParentObjectID");
	connect(ImplementationDetailListForm, &ISListBaseForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
	connect(this, &ISOilSphereImplementationObjectForm::SavedObject, this, &ISOilSphereImplementationObjectForm::Saved);
	AddWidgetToBottom(ImplementationDetailListForm);

	if (form_type == ISNamespace::OFT_New)
	{
		ImplementationDetailListForm->setVisible(false);
	}
	else
	{
		ImplementationDetailListForm->setVisible(true);
		ImplementationDetailListForm->GetQueryModel()->SetParentObjectIDClassFilter(object_id);
		ImplementationDetailListForm->SetParentObjectID(object_id);
		ImplementationDetailListForm->SetParentTableName(meta_table->Name);
		ImplementationDetailListForm->LoadData();
	}
}
//-----------------------------------------------------------------------------
ISOilSphereImplementationObjectForm::~ISOilSphereImplementationObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphereImplementationObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
}
//-----------------------------------------------------------------------------
void ISOilSphereImplementationObjectForm::Saved(int ObjectID)
{
	ImplementationDetailListForm->setVisible(true);
	ImplementationDetailListForm->GetQueryModel()->SetParentObjectIDClassFilter(ObjectID);
	ImplementationDetailListForm->SetParentObjectID(ObjectID);
	ImplementationDetailListForm->SetParentTableName(GetMetaTable()->Name);
	ImplementationDetailListForm->LoadData();
}
//-----------------------------------------------------------------------------
