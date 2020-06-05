#include "ISOilSphere.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISInputDialog.h"
//-----------------------------------------------------------------------------
static QString QU_RESULT_COUNT = PREPARE_QUERY2("UPDATE implementation SET "
												"impl_resultcount = (SELECT COUNT(*) FROM implementationdetail WHERE imdt_implementation = :ObjectID AND NOT imdt_isdeleted) "
												"WHERE impl_id = :ObjectID "
												"RETURNING impl_resultcount");
//-----------------------------------------------------------------------------
static QString QS_BEFORE_VALUES = PREPARE_QUERY2("SELECT "
												 "COALESCE(gsts_balanceendchange, 0) AS gsts_balanceendchange, "
												 "COALESCE(gsts_cashboxtotalpayment, 0) AS gsts_cashboxtotalpayment, "
												 "COALESCE(gsts_cashboxtotalactually, 0) AS gsts_cashboxtotalactually, "
												 "COALESCE(gsts_cashboxkkmtotal, 0) AS gsts_cashboxkkmtotal "
												 "FROM gasstationstatement "
												 "WHERE gsts_gasstation = :GasStation "
												 "AND gsts_date = :Date");
//-----------------------------------------------------------------------------
static QString QS_CASHBOX_TOTAL_PAYMENT = PREPARE_QUERY2("SELECT gsts_cashboxtotalpayment "
														 "FROM gasstationstatement "
														 "WHERE gsts_gasstation = :GasStation "
														 "AND gsts_date = :Date - INTERVAL '1 day'");
//-----------------------------------------------------------------------------
static QString QS_STATEMENT = PREPARE_QUERY2("SELECT COUNT(*) "
											 "FROM gasstationstatement "
											 "WHERE gsts_gasstation = :GasStation "
											 "AND gsts_date = :Date");
//-----------------------------------------------------------------------------
static QString QI_STATEMENT = PREPARE_QUERY2("INSERT INTO gasstationstatement(gsts_gasstation, gsts_date, gsts_change) "
											 "VALUES(:GasStation, :Date, (SELECT COALESCE(max(gsts_change) + 1, 1) FROM gasstationstatement WHERE gsts_gasstation = :GasStation))");
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
	qRegisterMetaType<ISOilSphere::GasStationStatementListForm*>("ISOilSphere::GasStationStatementListForm");
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
ISOilSphere::GasStationStatementListForm::GasStationStatementListForm(QWidget *parent) : ISListBaseForm("GasStationStatement", parent)
{
	GetQueryModel()->SetClassFilter("gsts_gasstation = 0");

	GetToolBar()->addWidget(new QLabel(LANG("OilSphere.SelectGasStation"), GetToolBar()));

	EditGasStation = new ISListEdit(GetToolBar());
	EditGasStation->InvokeList(GetMetaTable()->GetField("GasStation")->Foreign);
	connect(EditGasStation, &ISListEdit::DataChanged, this, &ISOilSphere::GasStationStatementListForm::GasStationChanged);
	GetToolBar()->addWidget(EditGasStation);
}
//-----------------------------------------------------------------------------
ISOilSphere::GasStationStatementListForm::~GasStationStatementListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementListForm::Create()
{
	QVariant GasStationID = EditGasStation->GetValue();
	if (GasStationID.isValid())
	{
		QDate Date = ISInputDialog::GetDate(QString(), LANG("SelectDate") + ':');
		if (Date.isValid())
		{
			ISQuery qSelect(QS_STATEMENT);
			qSelect.BindValue(":GasStation", GasStationID);
			qSelect.BindValue(":Date", Date);
			if (qSelect.ExecuteFirst())
			{
				if (qSelect.ReadColumn("count").toInt())
				{
					ISMessageBox::ShowWarning(this, LANG("OilSphere.ChangeAlreadyExist").arg(Date.toString(FORMAT_DATE_V2)));
				}
				else
				{
					ISQuery qInsert(QI_STATEMENT);
					qInsert.BindValue(":GasStation", GasStationID);
					qInsert.BindValue(":Date", Date);
					if (qInsert.Execute())
					{
						Update();
					}
				}
			}
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("OilSphere.NotSelectedGasStation"));
		EditGasStation->BlinkRed();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementListForm::GasStationChanged()
{
	QString GasStationID = EditGasStation->GetValue().toString();
	GetQueryModel()->SetClassFilter("gsts_gasstation = " + (GasStationID.isEmpty() ? "0" : GasStationID));
	Update();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::GasStationStatementObjectForm::GasStationStatementObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id),
	BeforeBalanceBeginChange(0),
	BeforeCashboxTotalPayment(0),
	BeforeCashboxTotalActually(0),
	BeforeCashboxKKMTotal(0)
{
	ISQuery qSelectBeforeValues(QS_BEFORE_VALUES); //Запрашиваем значения с предыдущей смены
	qSelectBeforeValues.BindValue(":GasStation", GetFieldValue("GasStation"));
	qSelectBeforeValues.BindValue(":Date", GetFieldValue("Date").toDate().addDays(-1));
	if (qSelectBeforeValues.ExecuteFirst())
	{
		BeforeBalanceBeginChange = qSelectBeforeValues.ReadColumn("gsts_balanceendchange").toDouble();
		BeforeCashboxTotalPayment = qSelectBeforeValues.ReadColumn("gsts_cashboxtotalpayment").toDouble();
		BeforeCashboxTotalActually = qSelectBeforeValues.ReadColumn("gsts_cashboxtotalactually").toDouble();
		BeforeCashboxKKMTotal = qSelectBeforeValues.ReadColumn("gsts_cashboxkkmtotal").toDouble();

		SetFieldValue("BalanceBeginChange", BeforeBalanceBeginChange);
		SetFieldValue("CashboxTotalPayment", BeforeCashboxTotalPayment);
		SetFieldValue("CashboxTotalActually", BeforeCashboxTotalActually);
		SetFieldValue("CashboxKKMTotal", BeforeCashboxKKMTotal);
	}

	//Остаток на конец смены
	connect(GetFieldWidget("BalanceBeginChange"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);
	connect(GetFieldWidget("VolumeIncome"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);
	connect(GetFieldWidget("VolumeHolidaysCounters"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);
	connect(GetFieldWidget("TechnicalStraitDeviation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);

	//Отаток в кассе, расчёт
	connect(GetFieldWidget("SalesAmount"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment);
	connect(GetFieldWidget("KKMCash"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment);
	connect(GetFieldWidget("PaySoldVolume"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment);

	//Остаток в кассе, фактический
	connect(GetFieldWidget("CashboxBalancePayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalanceActually);
	connect(GetFieldWidget("PresenceDebt"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalanceActually);

	//Расхождения по кассе
	connect(GetFieldWidget("CashboxBalancePayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxDiscrepancies);
	connect(GetFieldWidget("CashboxBalanceActually"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxDiscrepancies);

	//Наличные по ККМ (касса)
	connect(GetFieldWidget("KKMCash"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMCash);

	//Накопительный итог, расчёт
	connect(GetFieldWidget("CashboxBalancePayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment);
	connect(GetFieldWidget("CashboxCollectionAmount"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment);
	connect(GetFieldWidget("CashboxAdministrativeExpenses"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment);

	//Накопительный итог, фактический
	connect(GetFieldWidget("CashboxBalanceActually"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually);
	connect(GetFieldWidget("CashboxCollectionAmount"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually);
	connect(GetFieldWidget("CashboxAdministrativeExpenses"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually);

	//Расхождения итогов
	connect(GetFieldWidget("CashboxTotalPayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CaclulateCashboxDiscrepanciesTotals);
	connect(GetFieldWidget("CashboxTotalActually"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CaclulateCashboxDiscrepanciesTotals);

	//Накопительный итог по ККМ
	connect(GetFieldWidget("KKMCash"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMTotal);
	connect(GetFieldWidget("CashboxCollectionAmountKKM"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMTotal);
}
//-----------------------------------------------------------------------------
ISOilSphere::GasStationStatementObjectForm::~GasStationStatementObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange()
{
	double BalanceEndChange =
		GetFieldValue("BalanceBeginChange").toDouble() +
		GetFieldValue("VolumeIncome").toDouble() -
		GetFieldValue("VolumeHolidaysCounters").toDouble() -
		GetFieldValue("TechnicalStraitDeviation").toDouble();
	if (BalanceEndChange)
	{
		SetFieldValue("BalanceEndChange", BalanceEndChange);
	}
	else
	{
		GetFieldWidget("BalanceEndChange")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment()
{
	double CashboxBalancePayment =
		GetFieldValue("SalesAmount").toDouble() -
		GetFieldValue("KKMCashless").toDouble() -
		GetFieldValue("PaySoldVolume").toDouble();
	if (CashboxBalancePayment)
	{
		SetFieldValue("CashboxBalancePayment", CashboxBalancePayment);
	}
	else
	{
		GetFieldWidget("CashboxBalancePayment")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalanceActually()
{
	double CashboxBalanceActually = GetFieldValue("CashboxBalancePayment").toDouble() - GetFieldValue("PresenceDebt").toDouble();
	if (CashboxBalanceActually)
	{
		SetFieldValue("CashboxBalanceActually", CashboxBalanceActually);
	}
	else
	{
		GetFieldWidget("CashboxBalanceActually")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxDiscrepancies()
{
	double CashboxDiscrepancies = GetFieldValue("CashboxBalanceActually").toDouble() - GetFieldValue("CashboxBalancePayment").toDouble();
	if (CashboxDiscrepancies)
	{
		SetFieldValue("CashboxDiscrepancies", CashboxDiscrepancies);
	}
	else
	{
		GetFieldWidget("CashboxDiscrepancies")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMCash()
{
	SetFieldValue("CashboxKKMCash", GetFieldValue("KKMCash"));
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment()
{
	double CashboxTotalPayment = BeforeCashboxTotalPayment +
		GetFieldValue("CashboxBalancePayment").toDouble() -
		GetFieldValue("CashboxCollectionAmount").toDouble() -
		GetFieldValue("CashboxAdministrativeExpenses").toDouble();
	if (CashboxTotalPayment)
	{
		SetFieldValue("CashboxTotalPayment", CashboxTotalPayment);
	}
	else
	{
		GetFieldWidget("CashboxTotalPayment")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually()
{
	double CashboxTotalActually = GetFieldValue("CashboxBalanceActually").toDouble() +
		BeforeCashboxTotalActually -
		GetFieldValue("CashboxCollectionAmount").toDouble() -
		GetFieldValue("CashboxAdministrativeExpenses").toDouble();
	if (CashboxTotalActually)
	{
		SetFieldValue("CashboxTotalActually", CashboxTotalActually);
	}
	else
	{
		GetFieldWidget("CashboxTotalActually")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CaclulateCashboxDiscrepanciesTotals()
{
	double CashboxDiscrepanciesTotals = GetFieldValue("CashboxTotalActually").toDouble() - GetFieldValue("CashboxTotalPayment").toDouble();
	if (CashboxDiscrepanciesTotals)
	{
		SetFieldValue("CashboxDiscrepanciesTotals", CashboxDiscrepanciesTotals);
	}
	else
	{
		GetFieldWidget("CashboxDiscrepanciesTotals")->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMTotal()
{
	double CashboxKKMTotal = BeforeCashboxKKMTotal + GetFieldValue("KKMCash").toDouble() - GetFieldValue("CashboxCollectionAmountKKM").toDouble();
	if (CashboxKKMTotal)
	{
		SetFieldValue("CashboxKKMTotal", CashboxKKMTotal);
	}
	else
	{
		GetFieldWidget("CashboxKKMTotal")->Clear();
	}
}
//-----------------------------------------------------------------------------
