#include "ISOilSphere.h"
#include "ISQuery.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QU_RESULT_COUNT = PREPARE_QUERY2("UPDATE implementation SET "
											   "impl_resultcount = (SELECT COUNT(*) FROM implementationdetail WHERE imdt_implementation = :ObjectID AND NOT imdt_isdeleted) "
											   "WHERE impl_id = :ObjectID "
											   "RETURNING impl_resultcount");
//-----------------------------------------------------------------------------
static QString QU_LAST_CHANGE = PREPARE_QUERY2("UPDATE gasstation "
											   "SET gsst_lastchange = gsst_lastchange + 1 "
											   "WHERE gsst_id = :GasStation "
											   "RETURNING gsst_lastchange");
//-----------------------------------------------------------------------------
static QString QU_CHANGE = PREPARE_QUERY2("UPDATE gasstationstatement SET "
										  "gsts_change = (SELECT gsst_lastchange FROM gasstation WHERE gsst_id = :GasStation) "
										  "WHERE gsts_id = :ID");
//-----------------------------------------------------------------------------
ISOilSphere::Object::Object() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISOilSphere::Object::~Object()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Object::RegisterMetaTypes() const
{
	qRegisterMetaType<ISOilSphere::CounterpartyObjectForm*>("ISOilSphere::CounterpartyObjectForm");
	qRegisterMetaType<ISOilSphere::ImplementationObjectForm*>("ISOilSphere::ImplementationObjectForm");
	qRegisterMetaType<ISOilSphere::ImplementationDetailObjectForm*>("ISOilSphere::ImplementationDetailObjectForm");
	qRegisterMetaType<ISOilSphere::GasStationStatementObjectForm*>("ISOilSphere::GasStationStatementObjectForm");
}
//-----------------------------------------------------------------------------
void ISOilSphere::Object::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Object::InitializePlugin() const
{
	
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyObjectForm::CounterpartyObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	DaDataService = new ISDaDataService(this);
	connect(DaDataService, &ISDaDataService::FoundedOgranization, this, &ISOilSphere::CounterpartyObjectForm::SearchFinished);

	INNEdit = dynamic_cast<ISINNEdit*>(GetFieldWidget("Inn"));
	INNEdit->SetEnabledSearch(true);
	connect(INNEdit, &ISINNEdit::SearchFromINN, this, &ISOilSphere::CounterpartyObjectForm::SearchFromINN);
}
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyObjectForm::~CounterpartyObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyObjectForm::SearchFromINN()
{
	DaDataService->OrganizationFromINN(INNEdit->GetValue().toString());
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyObjectForm::SearchFinished(const ISDaDataOrganization &OrganizationStruct)
{
	GetFieldWidget("Name")->SetValue(OrganizationStruct.Name.ShortWithOPF);
	GetFieldWidget("Address")->SetValue(OrganizationStruct.Address);
	GetFieldWidget("Director")->SetValue(OrganizationStruct.Management.FIO);
	GetFieldWidget("Kpp")->SetValue(OrganizationStruct.Kpp);
	GetFieldWidget("Ogrn")->SetValue(OrganizationStruct.Ogrn);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationObjectForm::ImplementationObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	ImplementationDetailListForm = new ISListBaseForm("ImplementationDetail", this);
	ImplementationDetailListForm->setSizePolicy(ImplementationDetailListForm->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	ImplementationDetailListForm->GetQueryModel()->SetClassFilter("imdt_implementation = :ParentObjectID");
	connect(ImplementationDetailListForm, &ISListBaseForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
	connect(ImplementationDetailListForm, &ISListBaseForm::Updated, this, &ISOilSphere::ImplementationObjectForm::Updated);
	connect(this, &ISOilSphere::ImplementationObjectForm::SavedObject, this, &ISOilSphere::ImplementationObjectForm::Saved);
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
ISOilSphere::ImplementationObjectForm::~ImplementationObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationObjectForm::Saved(int ObjectID)
{
	ImplementationDetailListForm->setVisible(true);
	ImplementationDetailListForm->GetQueryModel()->SetParentObjectIDClassFilter(ObjectID);
	ImplementationDetailListForm->SetParentObjectID(ObjectID);
	ImplementationDetailListForm->SetParentTableName(GetMetaTable()->Name);
	ImplementationDetailListForm->LoadData();
}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationObjectForm::Updated()
{
	ISQuery qUpdate(QU_RESULT_COUNT);
	qUpdate.BindValue(":ObjectID", GetObjectID());
	if (qUpdate.ExecuteFirst())
	{
		GetFieldWidget("ResultCount")->SetValue(qUpdate.ReadColumn("impl_resultcount").toInt());
		SetModificationFlag(false);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationDetailObjectForm::ImplementationDetailObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	EditLoadContainer = GetFieldWidget("LoadContainer");
	EditLoadWeightGross = GetFieldWidget("LoadWeightGross");
	EditLoadWeightNet = GetFieldWidget("LoadWeightNet");
	EditLoadPriceUnit = GetFieldWidget("LoadPriceUnit");
	EditLoadCost = GetFieldWidget("LoadCost");
	connect(EditLoadContainer, &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationDetailObjectForm::CalculateLoad);
	connect(EditLoadWeightGross, &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationDetailObjectForm::CalculateLoad);
	connect(EditLoadPriceUnit, &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationDetailObjectForm::CalculateLoad);

	EditUnloadContainer = GetFieldWidget("UnloadContainer");
	EditUnloadWeightGross = GetFieldWidget("UnloadWeightGross");
	EditUnloadWeightNet = GetFieldWidget("UnloadWeightNet");
	EditUnloadPriceUnit = GetFieldWidget("UnloadPriceUnit");
	EditUnloadCost = GetFieldWidget("UnloadCost");
	connect(EditUnloadContainer, &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationDetailObjectForm::CalculateUnload);
	connect(EditUnloadWeightGross, &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationDetailObjectForm::CalculateUnload);
	connect(EditUnloadPriceUnit, &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationDetailObjectForm::CalculateUnload);

	EditWeightDifference = GetFieldWidget("WeightDifference");
}
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationDetailObjectForm::~ImplementationDetailObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationDetailObjectForm::CalculateLoad()
{
	//Тара (весы)
	double LoadContainer = EditLoadContainer->GetValue().toDouble();
	if (!LoadContainer)
	{
		EditLoadWeightNet->Clear();
		EditLoadCost->Clear();
		return;
	}

	//Вес (брутто)
	double LoadWeightGross = EditLoadWeightGross->GetValue().toDouble();
	if (!LoadWeightGross)
	{
		EditLoadWeightNet->Clear();
		EditLoadCost->Clear();
		return;
	}

	//Вес (нетто)
	double LoadWeightNet = LoadContainer * LoadWeightGross;
	EditLoadWeightNet->SetValue(LoadWeightNet);

	//Цена за кг./л
	double LoadPriceUnit = EditLoadPriceUnit->GetValue().toDouble();
	if (!LoadPriceUnit)
	{
		EditLoadCost->Clear();
		return;
	}

	//Стоимость загрузки
	double LoadCost = LoadWeightNet * LoadPriceUnit;
	EditLoadCost->SetValue(LoadCost);

	CalculateWeightDifference();
}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationDetailObjectForm::CalculateUnload()
{
	//Тара (весы)
	double UnloadContainer = EditUnloadContainer->GetValue().toDouble();
	if (!UnloadContainer)
	{
		EditUnloadWeightNet->Clear();
		EditUnloadCost->Clear();
		return;
	}

	//Вес (брутто)
	double UnloadWeightGross = EditUnloadWeightGross->GetValue().toDouble();
	if (!UnloadWeightGross)
	{
		EditUnloadWeightNet->Clear();
		EditUnloadCost->Clear();
		return;
	}

	//Вес (нетто)
	double UnloadWeightNet = UnloadContainer * UnloadWeightGross;
	EditUnloadWeightNet->SetValue(UnloadWeightNet);

	//Цена за кг./л
	double UnloadPriceUnit = EditUnloadPriceUnit->GetValue().toDouble();
	if (!UnloadPriceUnit)
	{
		EditUnloadCost->Clear();
		return;
	}

	//Стоимость загрузки
	double UnloadCost = UnloadWeightNet * UnloadPriceUnit;
	EditUnloadCost->SetValue(UnloadCost);

	CalculateWeightDifference();
}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationDetailObjectForm::CalculateWeightDifference()
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::GasStationStatementObjectForm::GasStationStatementObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::GasStationStatementObjectForm::~GasStationStatementObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::GasStationStatementObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		//Расчёт смены справедлив только для создания новой записи и создания копии существутющей
		if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
		{
			QVariant GasStation = GetFieldValue("GasStation");

			ISQuery qSelectChange(QU_LAST_CHANGE);
			qSelectChange.BindValue(":GasStation", GasStation);
			if (qSelectChange.ExecuteFirst()) //Последнюю смену получили - инкрементируем её
			{
				ISQuery q(QU_CHANGE);
				q.BindValue(":GasStation", GasStation);
				q.Execute();
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
