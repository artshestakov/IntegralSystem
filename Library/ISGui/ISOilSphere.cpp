#include "ISOilSphere.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISInputDialog.h"
#include "ISDefinesGui.h"
#include "ISControls.h"
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
										"imdt_id, "
										"cnpr_name, "
										"impl_dateload, "
										"pdtp_name, "
										"imdt_unloadcost, "
										"(SELECT COALESCE(SUM(debt_accrued), 0) FROM debt WHERE debt_ImplementationDetail = imdt_id) AS debt_accrued, "
										"imdt_implementation "
										"FROM implementationdetail "
										"LEFT JOIN implementation ON imdt_implementation = impl_id "
										"LEFT JOIN producttype ON impl_producttype = pdtp_id "
										"LEFT JOIN counterparty ON imdt_counterparty = cnpr_id "
										"WHERE imdt_unloadcost > (SELECT COALESCE(SUM(debt_accrued), 0) FROM debt WHERE debt_implementationdetail = imdt_id)");
//-----------------------------------------------------------------------------
static QString QS_ACCRUED = PREPARE_QUERY2("SELECT debt_accrued, debt_date, debt_note "
										   "FROM debt "
										   "WHERE NOT debt_isdeleted "
										   "AND debt_implementationdetail = :ImplementationDetailID "
										   "ORDER BY debt_id");
//-----------------------------------------------------------------------------
static QString QS_PAYMENT = PREPARE_QUERY2("SELECT imdt_unloadcost, "
										   "imdt_unloadcost - (SELECT sum(idpt_sum) FROM implementationdetailpayment WHERE idpt_implementationdetail = :ImplementationDetail) AS left "
										   "FROM implementationdetail "
										   "WHERE imdt_id = :ImplementationDetail");
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
	qRegisterMetaType<ISOilSphere::PaymentListForm*>("ISOilSphere::PaymentListForm");
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
			int ImplementationDetailID = qSelectDebt.ReadColumn("imdt_id").toInt();
			QString CounterpartyName = qSelectDebt.ReadColumn("cnpr_name").toString();
			QDate DateLoad = qSelectDebt.ReadColumn("impl_dateload").toDate();
			QString ProductTypeName = qSelectDebt.ReadColumn("pdtp_name").toString();
			double UnloadCost = qSelectDebt.ReadColumn("imdt_unloadcost").toDouble();
			int Accrued = qSelectDebt.ReadColumn("debt_accrued").toInt();
			int ImplementationID = qSelectDebt.ReadColumn("imdt_implementation").toInt();

			QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(TreeWidget);
			TreeWidget->setItemWidget(TreeWidgetItem, 0, CreateItemWidget(ImplementationID, ImplementationDetailID, CounterpartyName, DateLoad, ProductTypeName, UnloadCost, Accrued));

			ISQuery qSelectAccrueds(QS_ACCRUED);
			qSelectAccrueds.BindValue(":ImplementationDetailID", ImplementationDetailID);
			if (qSelectAccrueds.Execute())
			{
				while (qSelectAccrueds.Next())
				{
					QString Accrued = qSelectAccrueds.ReadColumn("debt_accrued").toString();
					QString Date = qSelectAccrueds.ReadColumn("debt_date").toDate().toString(FORMAT_DATE_V2);
					QString Note = qSelectAccrueds.ReadColumn("debt_note").toString();

					QTreeWidgetItem *TreeItemAccrued = new QTreeWidgetItem(TreeWidgetItem);
					TreeItemAccrued->setSizeHint(0, QSize(TreeItemAccrued->sizeHint(0).width(), 30));
					TreeItemAccrued->setText(0, Note.isEmpty() ?
						LANG("OilSphere.AccruedWithDate").arg(Accrued).arg(Date) :
						LANG("OilSphere.AccruedWithDateAndNote").arg(Accrued).arg(Date).arg(Note));
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
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "Debt");
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
QWidget* ISOilSphere::DebtSubSystemForm::CreateItemWidget(int ImplementationID, int ImplementationDetailID, const QString &CounterpartyName, const QDate &DateLoad, const QString &ProductTypeName, double UnloadCost, int Accrued)
{
	QHBoxLayout *LayoutWidget = new QHBoxLayout();

	QWidget *Widget = new QWidget(TreeWidget);
	Widget->setLayout(LayoutWidget);

	QVBoxLayout *LayoutLabels = new QVBoxLayout();
	LayoutLabels->addWidget(new QLabel(LANG("OilSphere.Counterpatry") + ": " + CounterpartyName, Widget));
	LayoutLabels->addWidget(new QLabel(LANG("OilSphere.DateLoad") + ": " + DateLoad.toString(FORMAT_DATE_V2), Widget));
	LayoutLabels->addWidget(new QLabel(LANG("OilSphere.ProductType") + ": " + ProductTypeName, Widget));
	LayoutLabels->addWidget(new QLabel(LANG("OilSphere.UnloadCost") + ": " + QString::number(UnloadCost), Widget));
	LayoutLabels->addWidget(new QLabel(LANG("OilSphere.Accrued") + ": " + QString::number(Accrued), Widget));
	LayoutLabels->addWidget(new QLabel(LANG("OilSphere.Debt") + ": " + QString::number(UnloadCost - Accrued), Widget));
	LayoutWidget->addLayout(LayoutLabels);

	LayoutWidget->addWidget(ISControls::CreateVerticalLine(Widget));

	ISPushButton *ButtonAddAccrued = new ISPushButton(BUFFER_ICONS("Add"), LANG("OilSphere.AddAccrued"), Widget);
	ButtonAddAccrued->setProperty("ImplementationDetailID", ImplementationDetailID);
	ButtonAddAccrued->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonAddAccrued, &ISPushButton::clicked, this, &ISOilSphere::DebtSubSystemForm::AddAccrued);
	LayoutWidget->addWidget(ButtonAddAccrued);

	ISPushButton *ButtonShowImplementation = new ISPushButton(BUFFER_ICONS("Document"), LANG("OilSphere.ShowImplementation"), Widget);
	ButtonShowImplementation->setProperty("ImplementationID", ImplementationID);
	ButtonShowImplementation->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonShowImplementation, &ISPushButton::clicked, this, &ISOilSphere::DebtSubSystemForm::ShowImplementation);
	LayoutWidget->addWidget(ButtonShowImplementation);

	ISPushButton *ButtonShowImplementationDetails = new ISPushButton(BUFFER_ICONS("Document"), LANG("OilSphere.ShowImplementationDetails"), Widget);
	ButtonShowImplementationDetails->setProperty("ImplementationDetailID", ImplementationDetailID);
	ButtonShowImplementationDetails->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonShowImplementationDetails, &ISPushButton::clicked, this, &ISOilSphere::DebtSubSystemForm::ShowImplementationDetails);
	LayoutWidget->addWidget(ButtonShowImplementationDetails);

	LayoutWidget->addStretch();

	return Widget;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::PaymentListForm::PaymentListForm(QWidget *parent) : ISListBaseForm("ImplementationDetailPayment", parent)
{
	GetToolBar()->addSeparator();

	LabelDebtTotal = new QLabel(GetToolBar());
	LabelDebtTotal->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	GetToolBar()->addWidget(LabelDebtTotal);

	GetToolBar()->addSeparator();

	LabelDebtLeft = new QLabel(GetToolBar());
	LabelDebtLeft->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	GetToolBar()->addWidget(LabelDebtLeft);
}
//-----------------------------------------------------------------------------
ISOilSphere::PaymentListForm::~PaymentListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::PaymentListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();

	ISQuery qSelect(QS_PAYMENT);
	qSelect.BindValue(":ImplementationDetail", GetParentObjectID());
	if (qSelect.ExecuteFirst())
	{
		LabelDebtTotal->setText(LANG("OilSphere.PaymentTotal").arg(qSelect.ReadColumn("imdt_unloadcost").toDouble()));
		LabelDebtLeft->setText(LANG("OilSphere.PaymentLeft").arg(qSelect.ReadColumn("left").toDouble()));
	}
}
//-----------------------------------------------------------------------------
