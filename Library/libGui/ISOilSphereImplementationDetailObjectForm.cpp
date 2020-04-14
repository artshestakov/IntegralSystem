#include "ISOilSphereImplementationDetailObjectForm.h"
//-----------------------------------------------------------------------------
ISOilSphereImplementationDetailObjectForm::ISOilSphereImplementationDetailObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	EditLoadContainer = GetFieldWidget("LoadContainer");
	EditLoadWeightGross = GetFieldWidget("LoadWeightGross");
	EditLoadWeightNet = GetFieldWidget("LoadWeightNet");
	EditLoadPriceUnit = GetFieldWidget("LoadPriceUnit");
	EditLoadCost = GetFieldWidget("LoadCost");
	connect(EditLoadContainer, &ISFieldEditBase::DataChanged, this, &ISOilSphereImplementationDetailObjectForm::CalculateLoad);
	connect(EditLoadWeightGross, &ISFieldEditBase::DataChanged, this, &ISOilSphereImplementationDetailObjectForm::CalculateLoad);
	connect(EditLoadPriceUnit, &ISFieldEditBase::DataChanged, this, &ISOilSphereImplementationDetailObjectForm::CalculateLoad);

	EditUnloadContainer = GetFieldWidget("UnloadContainer");
	EditUnloadWeightGross = GetFieldWidget("UnloadWeightGross");
	EditUnloadWeightNet = GetFieldWidget("UnloadWeightNet");
	EditUnloadPriceUnit = GetFieldWidget("UnloadPriceUnit");
	EditUnloadCost = GetFieldWidget("UnloadCost");
	connect(EditUnloadContainer, &ISFieldEditBase::DataChanged, this, &ISOilSphereImplementationDetailObjectForm::CalculateUnload);
	connect(EditUnloadWeightGross, &ISFieldEditBase::DataChanged, this, &ISOilSphereImplementationDetailObjectForm::CalculateUnload);
	connect(EditUnloadPriceUnit, &ISFieldEditBase::DataChanged, this, &ISOilSphereImplementationDetailObjectForm::CalculateUnload);

	EditWeightDifference = GetFieldWidget("WeightDifference");
}
//-----------------------------------------------------------------------------
ISOilSphereImplementationDetailObjectForm::~ISOilSphereImplementationDetailObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphereImplementationDetailObjectForm::CalculateLoad()
{
	//���� (����)
	double LoadContainer = EditLoadContainer->GetValue().toDouble();
	if (!LoadContainer)
	{
		EditLoadWeightNet->Clear();
		EditLoadCost->Clear();
		return;
	}

	//��� (������)
	double LoadWeightGross = EditLoadWeightGross->GetValue().toDouble();
	if (!LoadWeightGross)
	{
		EditLoadWeightNet->Clear();
		EditLoadCost->Clear();
		return;
	}

	//��� (�����)
	double LoadWeightNet = LoadContainer * LoadWeightGross;
	EditLoadWeightNet->SetValue(LoadWeightNet);

	//���� �� ��./�
	double LoadPriceUnit = EditLoadPriceUnit->GetValue().toDouble();
	if (!LoadPriceUnit)
	{
		EditLoadCost->Clear();
		return;
	}

	//��������� ��������
	double LoadCost = LoadWeightNet * LoadPriceUnit;
	EditLoadCost->SetValue(LoadCost);

	CalculateWeightDifference();
}
//-----------------------------------------------------------------------------
void ISOilSphereImplementationDetailObjectForm::CalculateUnload()
{
	//���� (����)
	double UnloadContainer = EditUnloadContainer->GetValue().toDouble();
	if (!UnloadContainer)
	{
		EditUnloadWeightNet->Clear();
		EditUnloadCost->Clear();
		return;
	}

	//��� (������)
	double UnloadWeightGross = EditUnloadWeightGross->GetValue().toDouble();
	if (!UnloadWeightGross)
	{
		EditUnloadWeightNet->Clear();
		EditUnloadCost->Clear();
		return;
	}

	//��� (�����)
	double UnloadWeightNet = UnloadContainer * UnloadWeightGross;
	EditUnloadWeightNet->SetValue(UnloadWeightNet);

	//���� �� ��./�
	double UnloadPriceUnit = EditUnloadPriceUnit->GetValue().toDouble();
	if (!UnloadPriceUnit)
	{
		EditUnloadCost->Clear();
		return;
	}

	//��������� ��������
	double UnloadCost = UnloadWeightNet * UnloadPriceUnit;
	EditUnloadCost->SetValue(UnloadCost);

	CalculateWeightDifference();
}
//-----------------------------------------------------------------------------
void ISOilSphereImplementationDetailObjectForm::CalculateWeightDifference()
{
	double LoadWeightNet = EditLoadWeightNet->GetValue().toDouble();
	double UnloadWeightNet = EditUnloadWeightNet->GetValue().toDouble();
	double Result = 0;
	if (LoadWeightNet > UnloadWeightNet)
	{
		Result = LoadWeightNet - UnloadWeightNet;
	}
	else if (LoadWeightNet < UnloadWeightNet)
	{
		Result = UnloadWeightNet - LoadWeightNet;
	}
	EditWeightDifference->SetValue(Result);
}
//-----------------------------------------------------------------------------
