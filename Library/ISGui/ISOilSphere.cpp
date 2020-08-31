#include "ISOilSphere.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISInputDialog.h"
#include "ISDefinesGui.h"
#include "ISControls.h"
#include "ISStyleSheet.h"
#include "ISDatabase.h"
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
											 "VALUES(:GasStation, :Date, (SELECT COALESCE(max(gsts_change) + 1, 1) FROM gasstationstatement WHERE gsts_gasstation = :GasStation)) "
											 "RETURNING gsts_id");
//-----------------------------------------------------------------------------
static QString QS_DEBT = PREPARE_QUERY2("SELECT "
										"imdt_implementation, "
										"imdt_id, "
										"cnpr_id, "
										"cnpr_name, "
										"(SELECT COALESCE(SUM(cpen_sum), 0) FROM counterpartyenrollment WHERE NOT cpen_isdeleted AND cpen_counterparty = cnpr_id) - "
										"(SELECT COALESCE(SUM(cpwo_sum), 0) FROM counterpartywriteoff WHERE NOT cpwo_isdeleted AND cpwo_counterparty = cnpr_id) AS balance, "
										"impl_dateload, "
										"pdtp_name, "
										"imdt_unloadcost, "
										"(SELECT sum(cpwo_sum) FROM counterpartywriteoff WHERE NOT cpwo_isdeleted AND cpwo_implementationdetail = imdt_id) AS accrued "
										"FROM implementationdetail "
										"LEFT JOIN counterparty ON imdt_counterparty = cnpr_id "
										"LEFT JOIN implementation ON imdt_implementation = impl_id "
										"LEFT JOIN producttype ON impl_producttype = pdtp_id "
										"WHERE NOT imdt_isdeleted "
										"AND imdt_counterparty IS NOT NULL "
										"AND NOT (SELECT impl_isdeleted FROM implementation WHERE imdt_implementation = impl_id)");
//-----------------------------------------------------------------------------
static QString QS_ACCRUED = PREPARE_QUERY2("SELECT cpwo_creationdate, cpwo_sum, cpwo_note "
										   "FROM counterpartywriteoff "
										   "WHERE NOT cpwo_isdeleted "
										   "AND cpwo_implementationdetail = :ImplementationDetailID "
										   "ORDER BY cpwo_id");
//-----------------------------------------------------------------------------
static QString QS_ARRIVAL_STOCK = PREPARE_QUERY2("SELECT m.mwag_dateshipping, c.cnpr_name, m.mwag_datearrival, sum(d.mwdt_kilogram), COUNT(*) "
												 "FROM movewagon m "
												 "LEFT JOIN counterparty c ON c.cnpr_id = m.mwag_provider "
												 "LEFT JOIN movewagondetail d ON d.mwdt_movewagon = m.mwag_id AND NOT d.mwdt_isdeleted "
												 "WHERE NOT m.mwag_isdeleted "
												 "AND m.mwag_datearrival IS NOT NULL "
												 "AND m.mwag_stock = :StockID "
												 "GROUP BY m.mwag_dateshipping, c.cnpr_name, m.mwag_datearrival");
//-----------------------------------------------------------------------------
static QString QS_STOCK_WRITE_OFF = PREPARE_QUERY2("SELECT i.impl_dateload, p.pdtp_name, d.imdt_loadweightnet "
												   "FROM implementationdetail d "
												   "LEFT JOIN implementation i ON i.impl_id = d.imdt_implementation "
												   "LEFT JOIN producttype p ON p.pdtp_id = i.impl_producttype "
												   "WHERE NOT d.imdt_isdeleted "
												   "AND NOT i.impl_isdeleted "
												   "AND d.imdt_stock = :StockID");
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
	qRegisterMetaType<ISOilSphere::ImplementationDetailObjectForm*>("ISOilSphere::ImplementationDetailObjectForm");
	qRegisterMetaType<ISOilSphere::GasStationStatementListForm*>("ISOilSphere::GasStationStatementListForm");
	qRegisterMetaType<ISOilSphere::GasStationStatementObjectForm*>("ISOilSphere::GasStationStatementObjectForm");
	qRegisterMetaType<ISOilSphere::DebtSubSystemForm*>("ISOilSphere::DebtSubSystemForm");
	qRegisterMetaType<ISOilSphere::DebetObjectForm*>("ISOilSphere::DebetObjectForm");
	qRegisterMetaType<ISOilSphere::DebetListForm*>("ISOilSphere::DebetListForm");
	qRegisterMetaType<ISOilSphere::ConsumptionSUGObjectForm*>("ISOilSphere::ConsumptionSUGObjectForm");
	qRegisterMetaType<ISOilSphere::ArrivalStock*>("ISOilSphere::ArrivalStock");
	qRegisterMetaType<ISOilSphere::StockWriteOff*>("ISOilSphere::StockWriteOff");
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
	double LoadWeightNet = LoadWeightGross - LoadContainer;
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
	double UnloadWeightNet = UnloadWeightGross - UnloadContainer;
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
					if (qInsert.ExecuteFirst())
					{
						SetSelectObjectAfterUpdate(qInsert.ReadColumn("gsts_id").toInt());
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
void ISOilSphere::GasStationStatementObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	CalculateCashboxDiscrepancies();
	CaclulateCashboxDiscrepanciesTotals();
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange()
{
	double BalanceEndChange =
		GetFieldValue("BalanceBeginChange").toDouble() +
		GetFieldValue("VolumeIncome").toDouble() -
		GetFieldValue("VolumeHolidaysCounters").toDouble() -
		GetFieldValue("TechnicalStraitDeviation").toDouble();
	BalanceEndChange ? SetFieldValue("BalanceEndChange", BalanceEndChange) : GetFieldWidget("BalanceEndChange")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment()
{
	double CashboxBalancePayment = GetFieldValue("SalesAmount").toDouble() - (GetFieldValue("KKMCashless").toDouble() + GetFieldValue("Statements").toDouble());
	CashboxBalancePayment ? SetFieldValue("CashboxBalancePayment", CashboxBalancePayment) : GetFieldWidget("CashboxBalancePayment")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalanceActually()
{
	double CashboxBalanceActually = GetFieldValue("CashboxBalancePayment").toDouble() - GetFieldValue("PresenceDebt").toDouble();
	CashboxBalanceActually ? SetFieldValue("CashboxBalanceActually", CashboxBalanceActually) : GetFieldWidget("CashboxBalanceActually")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxDiscrepancies()
{
	double CashboxDiscrepancies = GetFieldValue("CashboxBalanceActually").toDouble() - GetFieldValue("CashboxBalancePayment").toDouble();
	if (CashboxDiscrepancies)
	{
		SetFieldValue("CashboxDiscrepancies", CashboxDiscrepancies);
		GetFieldWidget("CashboxDiscrepancies")->SetColorText(CashboxDiscrepancies > 0 ? ISDefines::Gui::COLOR_BLACK : ISDefines::Gui::COLOR_RED);
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
	CashboxTotalPayment ? SetFieldValue("CashboxTotalPayment", CashboxTotalPayment) : GetFieldWidget("CashboxTotalPayment")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually()
{
	double CashboxTotalActually = GetFieldValue("CashboxBalanceActually").toDouble() +
		BeforeCashboxTotalActually -
		GetFieldValue("CashboxCollectionAmount").toDouble() -
		GetFieldValue("CashboxAdministrativeExpenses").toDouble();
	CashboxTotalActually ? SetFieldValue("CashboxTotalActually", CashboxTotalActually) : GetFieldWidget("CashboxTotalActually")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementObjectForm::CaclulateCashboxDiscrepanciesTotals()
{
	double CashboxDiscrepanciesTotals = GetFieldValue("CashboxTotalActually").toDouble() - GetFieldValue("CashboxTotalPayment").toDouble();
	if (CashboxDiscrepanciesTotals)
	{
		SetFieldValue("CashboxDiscrepanciesTotals", CashboxDiscrepanciesTotals);
		GetFieldWidget("CashboxDiscrepanciesTotals")->SetColorText(CashboxDiscrepanciesTotals > 0 ? ISDefines::Gui::COLOR_BLACK : ISDefines::Gui::COLOR_RED);
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
	CashboxKKMTotal ? SetFieldValue("CashboxKKMTotal", CashboxKKMTotal) : GetFieldWidget("CashboxKKMTotal")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::DebtSubSystemForm::DebtSubSystemForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	TreeWidget = new QTreeWidget(this);
	TreeWidget->setHeaderHidden(true);
	TreeWidget->setAnimated(true);
	TreeWidget->setAlternatingRowColors(true);
	GetMainLayout()->addWidget(TreeWidget);
}
//-----------------------------------------------------------------------------
ISOilSphere::DebtSubSystemForm::~DebtSubSystemForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::DebtSubSystemForm::LoadData()
{
	ISGui::SetWaitGlobalCursor(true);
	for (int i = 0; i < TreeWidget->topLevelItemCount(); ++i)
	{
		while (TreeWidget->topLevelItem(i)->childCount())
		{
			delete TreeWidget->topLevelItem(i)->takeChild(0);
		}
	}

	while (TreeWidget->topLevelItemCount())
	{
		delete TreeWidget->takeTopLevelItem(0);
	}

	ISQuery qSelectDebt(QS_DEBT);
	if (qSelectDebt.Execute())
	{
		while (qSelectDebt.Next())
		{
			int ImplementationID = qSelectDebt.ReadColumn("imdt_implementation").toInt();
			int ImplementationDetailID = qSelectDebt.ReadColumn("imdt_id").toInt();
			int CounterpartyID = qSelectDebt.ReadColumn("cnpr_id").toInt();
			QString CounterpartyName = qSelectDebt.ReadColumn("cnpr_name").toString();
			double Balance = qSelectDebt.ReadColumn("balance").toDouble();
			QDate DateLoad = qSelectDebt.ReadColumn("impl_dateload").toDate();
			QString ProductTypeName = qSelectDebt.ReadColumn("pdtp_name").toString();
			double UnloadCost = qSelectDebt.ReadColumn("imdt_unloadcost").toDouble();
			double Accrued = qSelectDebt.ReadColumn("accrued").toDouble();

			QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(TreeWidget);
			TreeWidget->setItemWidget(TreeWidgetItem, 0,
				CreateItemWidget(ImplementationID, ImplementationDetailID, CounterpartyID, CounterpartyName, Balance, DateLoad, ProductTypeName, UnloadCost, Accrued));

			ISQuery qSelectAccrueds(QS_ACCRUED);
			qSelectAccrueds.BindValue(":ImplementationDetailID", ImplementationDetailID);
			if (qSelectAccrueds.Execute())
			{
				while (qSelectAccrueds.Next())
				{
					QString Date = qSelectAccrueds.ReadColumn("cpwo_creationdate").toDate().toString(FORMAT_DATE_V2);
					double Sum = qSelectAccrueds.ReadColumn("cpwo_sum").toDouble();
					QString Note = qSelectAccrueds.ReadColumn("cpwo_note").toString();

					QTreeWidgetItem *TreeItemAccrued = new QTreeWidgetItem(TreeWidgetItem);
					TreeItemAccrued->setSizeHint(0, QSize(TreeItemAccrued->sizeHint(0).width(), 30));
					TreeItemAccrued->setText(0, Note.isEmpty() ?
						LANG("OilSphere.AccruedWithDate").arg(Sum).arg(Date) :
						LANG("OilSphere.AccruedWithDateAndNote").arg(Sum).arg(Date).arg(Note));
				}
			}
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, qSelectDebt.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISOilSphere::DebtSubSystemForm::AddAccrued()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "CounterpartyWriteOff");
	ObjectFormBase->SetFieldValue("Counterparty", sender()->property("CounterpartyID"));
	ObjectFormBase->SetFieldValue("ImplementationDetail", sender()->property("ImplementationDetailID"));
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISOilSphere::DebtSubSystemForm::LoadData);
	ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISOilSphere::DebtSubSystemForm::ShowImplementation()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Implementation", sender()->property("ImplementationID").toInt()));
}
//-----------------------------------------------------------------------------
void ISOilSphere::DebtSubSystemForm::ShowImplementationDetails()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "ImplementationDetail", sender()->property("ImplementationDetailID").toInt()));
}
//-----------------------------------------------------------------------------
void ISOilSphere::DebtSubSystemForm::ShowCounterparty()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Counterparty", sender()->property("CounterpartyID").toInt()));
}
//-----------------------------------------------------------------------------
QWidget* ISOilSphere::DebtSubSystemForm::CreateItemWidget(int ImplementationID, int ImplementationDetailID, int CounterpartyID, const QString &CounterpartyName, double Balance, const QDate &DateLoad, const QString &ProductTypeName, double UnloadCost, int Accrued)
{
	QHBoxLayout *LayoutWidget = new QHBoxLayout();

	QWidget *Widget = new QWidget(TreeWidget);
	Widget->setLayout(LayoutWidget);

	QVBoxLayout *LayoutLabels = new QVBoxLayout();

	QLabel *LabelCounterpartyName = new QLabel(LANG("OilSphere.Counterpatry").arg(CounterpartyName), Widget);
	ISGui::SetFontWidgetBold(LabelCounterpartyName, true);
	LayoutLabels->addWidget(LabelCounterpartyName);

	QLabel *LabelBalance = new QLabel(LANG("OilSphere.CurrentBalance").arg(Balance), Widget);
	ISGui::SetFontWidgetBold(LabelBalance, true);
	LayoutLabels->addWidget(LabelBalance);
	
	if (Balance == 0)
	{
		LabelBalance->setStyleSheet(STYLE_SHEET("QLabel.Color.Blue"));
	}
	else if (Balance > 0)
	{
		LabelBalance->setStyleSheet(STYLE_SHEET("QLabel.Color.Green"));
	}
	else if (Balance < 0)
	{
		LabelBalance->setStyleSheet(STYLE_SHEET("QLabel.Color.Red"));
	}

	QLabel *LabelDateLoad = new QLabel(LANG("OilSphere.DateLoad").arg(DateLoad.toString(FORMAT_DATE_V2)), Widget);
	ISGui::SetFontWidgetBold(LabelDateLoad, true);
	LayoutLabels->addWidget(LabelDateLoad);

	QLabel *LabelProductType = new QLabel(LANG("OilSphere.ProductType").arg(ProductTypeName), Widget);
	ISGui::SetFontWidgetBold(LabelProductType, true);
	LayoutLabels->addWidget(LabelProductType);

	QLabel *LabelUnloadCost = new QLabel(LANG("OilSphere.UnloadCost").arg(UnloadCost), Widget);
	ISGui::SetFontWidgetBold(LabelUnloadCost , true);
	LayoutLabels->addWidget(LabelUnloadCost);

	double Debt = UnloadCost - Accrued;
	QLabel *LabelDebt = new QLabel(LANG("OilSphere.Debt").arg(Debt), Widget);
	ISGui::SetFontWidgetBold(LabelDebt, true);
	LayoutLabels->addWidget(LabelDebt);

	QLabel *LabelAccrued = new QLabel(LANG("OilSphere.Accrued").arg(Accrued), Widget);
	ISGui::SetFontWidgetBold(LabelAccrued, true);
	LayoutLabels->addWidget(LabelAccrued);

	LayoutWidget->addLayout(LayoutLabels);

	LayoutWidget->addWidget(ISControls::CreateVerticalLine(Widget));

	QToolButton *ButtonMenu = new QToolButton(Widget);
	ButtonMenu->setText(LANG("Menu"));
	ButtonMenu->setToolTip(LANG("Menu"));
	ButtonMenu->setIcon(BUFFER_ICONS("Document"));
	ButtonMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ButtonMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonMenu->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
	ButtonMenu->setPopupMode(QToolButton::InstantPopup);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	LayoutWidget->addWidget(ButtonMenu);

	QAction *ActionAddAccrued = ButtonMenu->menu()->addAction(LANG("OilSphere.AddAccrued"), this, &ISOilSphere::DebtSubSystemForm::AddAccrued);
	ActionAddAccrued->setProperty("ImplementationDetailID", ImplementationDetailID);
	ActionAddAccrued->setProperty("CounterpartyID", CounterpartyID);
	ActionAddAccrued->setEnabled(Debt > 0);

	QAction *ActionShowImplementation = ButtonMenu->menu()->addAction(LANG("OilSphere.ShowImplementation"), this, &ISOilSphere::DebtSubSystemForm::ShowImplementation);
	ActionShowImplementation->setProperty("ImplementationID", ImplementationID);

	QAction *ActionShowImplementationDetails = ButtonMenu->menu()->addAction(LANG("OilSphere.ShowImplementationDetails"), this, &ISOilSphere::DebtSubSystemForm::ShowImplementationDetails);
	ActionShowImplementationDetails->setProperty("ImplementationDetailID", ImplementationDetailID);

	QAction *ActionShowCounterparty = ButtonMenu->menu()->addAction(LANG("OilSphere.ShowCounterparty"), this, &ISOilSphere::DebtSubSystemForm::ShowCounterparty);
	ActionShowCounterparty->setProperty("CounterpartyID", CounterpartyID);

	LayoutWidget->addStretch();
	return Widget;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::DebetObjectForm::DebetObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::DebetObjectForm::CalculateTotal);
	connect(GetFieldWidget("Percent"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::DebetObjectForm::CalculateTotal);
	connect(GetFieldWidget("Calculation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::DebetObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::DebetObjectForm::~DebetObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::DebetObjectForm::CalculateTotal()
{
	double Coming = GetFieldValue("Coming").toDouble(), Percent = GetFieldValue("Percent").toDouble();
	Coming > 0 && Percent > 0 ? SetFieldValue("Total", Coming - ((Coming * Percent) / 100)) : GetFieldWidget("Total")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::DebetObjectForm::CalculateRemainder()
{
	double Total = GetFieldValue("Total").toDouble(), Calculation = GetFieldValue("Calculation").toDouble();
	Total > 0 && Calculation > 0 ? SetFieldValue("Remainder", Total - Calculation) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::DebetListForm::DebetListForm(QWidget *parent) : ISListBaseForm("Debet1", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	LabelTotalSum = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(LabelTotalSum, true);
	GetToolBar()->addWidget(LabelTotalSum);

	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	LabelRemainderSum = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(LabelRemainderSum, true);
	GetToolBar()->addWidget(LabelRemainderSum);
}
//-----------------------------------------------------------------------------
ISOilSphere::DebetListForm::~DebetListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::DebetListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();

	double Total = 0, Remainder = 0;
	for (int Row = 0; Row < GetSqlModel()->rowCount(); ++Row)
	{
		Total += GetSqlModel()->data(GetSqlModel()->index(Row, GetSqlModel()->GetFieldIndex("Total"))).toDouble();
		Remainder += GetSqlModel()->data(GetSqlModel()->index(Row, GetSqlModel()->GetFieldIndex("Remainder"))).toDouble();
	}
	LabelTotalSum->setText(LANG("OilSphere.Debet.Total").arg(Total));
	LabelRemainderSum->setText(LANG("OilSphere.Debet.Remainder").arg(Remainder));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionSUGObjectForm::ConsumptionSUGObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ConsumptionSUGObjectForm::CalculateRemainder);
	connect(GetFieldWidget("Consumption"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ConsumptionSUGObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionSUGObjectForm::~ConsumptionSUGObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::ConsumptionSUGObjectForm::CalculateRemainder()
{
	int Coming = GetFieldValue("Coming").toInt(), Consumption = GetFieldValue("Consumption").toInt();
	Coming > 0 && Consumption > 0 ? SetFieldValue("Remainder", Coming - Consumption) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ArrivalStock::ArrivalStock(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	TableView = new ISBaseTableView(this);
	GetMainLayout()->addWidget(TableView);
}
//-----------------------------------------------------------------------------
ISOilSphere::ArrivalStock::~ArrivalStock()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::ArrivalStock::LoadData()
{
	QSqlQuery qSelect(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT));
	qSelect.prepare(QS_ARRIVAL_STOCK);
	qSelect.bindValue(":StockID", GetParentObjectID());
	if (qSelect.exec())
	{
		QSqlQueryModel *SqlQueryModel = new QSqlQueryModel();
		SqlQueryModel->setQuery(qSelect);
		SqlQueryModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Дата отгрузки"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Поставщик"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Прибыло"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Масса"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Количество вагонов"), Qt::DisplayRole);
		TableView->setModel(SqlQueryModel);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::StockWriteOff::StockWriteOff(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	TableView = new ISBaseTableView(this);
	GetMainLayout()->addWidget(TableView);
}
//-----------------------------------------------------------------------------
ISOilSphere::StockWriteOff::~StockWriteOff()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::StockWriteOff::LoadData()
{
	QSqlQuery qSelect(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT));
	qSelect.prepare(QS_STOCK_WRITE_OFF);
	qSelect.bindValue(":StockID", GetParentObjectID());
	if (qSelect.exec())
	{
		QSqlQueryModel *SqlQueryModel = new QSqlQueryModel();
		SqlQueryModel->setQuery(qSelect);
		SqlQueryModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Дата загрузки"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Тип продукта"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Списано"), Qt::DisplayRole);
		TableView->setModel(SqlQueryModel);
	}
}
//-----------------------------------------------------------------------------
