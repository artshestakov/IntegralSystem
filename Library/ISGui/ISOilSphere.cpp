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
#include "ISObjects.h"
//-----------------------------------------------------------------------------
static QString QS_PERIOD = PREPARE_QUERY2("SELECT COUNT(*) "
										  "FROM period "
										  "WHERE CURRENT_DATE BETWEEN prod_datestart AND prod_dateend");
//-----------------------------------------------------------------------------
static QString QS_STATEMENT = PREPARE_QUERY2("SELECT COUNT(*) "
											 "FROM gasstationstatement "
											 "WHERE gsts_implementationunload = :ImplementationUnload");
//-----------------------------------------------------------------------------
static QString QI_STATEMENT = PREPARE_QUERY2("INSERT INTO gasstationstatement(gsts_implementationunload, gsts_stock, gsts_date, gsts_volumeincome) "
											 "VALUES(:ImplementationUnload, :StockID, CURRENT_DATE, :VolumeIncome)");
//-----------------------------------------------------------------------------
static QString QS_CONSTANT = PREPARE_QUERY2("SELECT prod_constant "
											"FROM period "
											"WHERE NOT prod_isdeleted "
											"AND CURRENT_DATE BETWEEN prod_datestart AND prod_dateend");
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
static QString QS_DEBT = PREPARE_QUERY2("SELECT "
										"iunl_implementation, "
										"iunl_id, "
										"cnpr_id, "
										"impl_date, "
										"iunl_cost, "
										"(SELECT sum(cpwo_sum) FROM counterpartywriteoff WHERE NOT cpwo_isdeleted AND cpwo_unload = iunl_id) AS accrued "
										"FROM implementationunload "
										"LEFT JOIN counterparty ON iunl_counterparty = cnpr_id "
										"LEFT JOIN implementation ON iunl_implementation = impl_id "
										"WHERE NOT iunl_isdeleted "
										"AND iunl_counterparty = :CounterpartyID "
										"AND NOT (SELECT impl_isdeleted FROM implementation WHERE iunl_implementation = impl_id)");
//-----------------------------------------------------------------------------
static QString QS_ACCRUED = PREPARE_QUERY2("SELECT cpwo_creationdate, cpwo_sum, cpwo_note "
										   "FROM counterpartywriteoff "
										   "WHERE NOT cpwo_isdeleted "
										   "AND cpwo_unload = :ImplementationUnloadID "
										   "ORDER BY cpwo_id");
//-----------------------------------------------------------------------------
static QString QS_ARRIVAL_STOCK = PREPARE_QUERY2("SELECT mwag_dateshipping, cnpr_name, mwag_datearrival, sum(mwdt_kilogram), COUNT(*) "
												 "FROM movewagon "
												 "LEFT JOIN counterparty ON cnpr_id = mwag_provider "
												 "LEFT JOIN movewagondetail ON mwdt_movewagon = mwag_id AND NOT mwdt_isdeleted "
												 "WHERE NOT mwag_isdeleted "
												 "AND mwag_datearrival IS NOT NULL "
												 "AND mwag_stock = :StockID "
												 "GROUP BY mwag_dateshipping, cnpr_name, mwag_datearrival");
//-----------------------------------------------------------------------------
static QString QS_STOCK_WRITE_OFF = PREPARE_QUERY2("SELECT impl_date, pdtp_name, iunl_weightnet "
												   "FROM implementationunload "
												   "LEFT JOIN implementation ON impl_id = iunl_implementation "
												   "LEFT JOIN producttype ON pdtp_id = impl_producttype "
												   "WHERE NOT iunl_isdeleted "
												   "AND NOT impl_isdeleted "
												   "AND iunl_stock = :StockID");
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
	qRegisterMetaType<ISOilSphere::CounterpartyListForm*>("ISOilSphere::CounterpartyListForm");
	qRegisterMetaType<ISOilSphere::CounterpartyObjectForm*>("ISOilSphere::CounterpartyObjectForm");
	qRegisterMetaType<ISOilSphere::ImplementationLoadObjectForm*>("ISOilSphere::ImplementationLoadObjectForm");
	qRegisterMetaType<ISOilSphere::ImplementationUnloadObjectForm*>("ISOilSphere::ImplementationUnloadObjectForm");
	qRegisterMetaType<ISOilSphere::GasStationStatementObjectForm*>("ISOilSphere::GasStationStatementObjectForm");
	qRegisterMetaType<ISOilSphere::Debet1ObjectForm*>("ISOilSphere::Debet1ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet2ObjectForm*>("ISOilSphere::Debet2ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet1ListForm*>("ISOilSphere::Debet1ListForm");
	qRegisterMetaType<ISOilSphere::Debet2ListForm*>("ISOilSphere::Debet2ListForm");
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
	//Проверяем наличие константы за текущий период
	ISQuery qSelectPeriod(QS_PERIOD);
	if (qSelectPeriod.ExecuteFirst())
	{
		if (qSelectPeriod.ReadColumn("count") == 0) //Константа отсутствует - предлагаем создать её
		{
			if (ISMessageBox::ShowQuestion(nullptr, LANG("OilSphere.Message.Question.CreateCurrentConstant")))
			{
				ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "Period");
				ObjectFormBase->SetFieldValue("DateStart", QDate::currentDate());
				ObjectFormBase->SetFieldValue("DateEnd", QDate::currentDate());
				ISGui::ShowObjectForm(ObjectFormBase);
			}
		}
	}
	else
	{
		ISMessageBox::ShowCritical(nullptr, LANG("OilSphere.Message.Critical.SelectCurrentConstant"), qSelectPeriod.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyListForm::CounterpartyListForm(QWidget *parent) : ISListBaseForm("Counterparty", parent)
{
	QAction *ActionDebt = new QAction(ISObjects::Instance().GetInterface()->GetIcon("Debt"), LANG("OilSphere.Debts"), this);
	connect(ActionDebt, &QAction::triggered, this, &ISOilSphere::CounterpartyListForm::ShowDebt);
	AddAction(ActionDebt, true, true);
}
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyListForm::~CounterpartyListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyListForm::ShowDebt()
{
	(new ISOilSphere::CounterpartyDebtForm(GetObjectID(), GetCurrentRecordValue("Name").toString()))->showMaximized();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyDebtForm::CounterpartyDebtForm(int counterparty_id, const QString &counterparty_name, QWidget *parent)
	: ISInterfaceForm(parent),
	CounterpartyID(counterparty_id)
{
	setWindowTitle(LANG("OilSphere.Debts.Title").arg(counterparty_name));
	setWindowIcon(ISObjects::Instance().GetInterface()->GetIcon("Debt"));
	resize(800, 600);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);

	ISPushButton *ButtonUpdate = new ISPushButton(BUFFER_ICONS("Update"), LANG("Update"), this);
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISOilSphere::CounterpartyDebtForm::LoadData);
	GetMainLayout()->addWidget(ButtonUpdate, 0, Qt::AlignLeft);

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setHeaderHidden(true);
	TreeWidget->setAnimated(true);
	TreeWidget->setAlternatingRowColors(true);
	GetMainLayout()->addWidget(TreeWidget);

	LoadData();
}
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyDebtForm::~CounterpartyDebtForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::LoadData()
{
	ISGui::SetWaitGlobalCursor(true);
	TreeWidget->clear();

	ISQuery qSelectDebt(QS_DEBT);
	qSelectDebt.BindValue(":CounterpartyID", CounterpartyID);
	if (qSelectDebt.Execute())
	{
		while (qSelectDebt.Next())
		{
			int ImplementationID = qSelectDebt.ReadColumn("iunl_implementation").toInt();
			int ImplementationUnloadID = qSelectDebt.ReadColumn("iunl_id").toInt();
			QDate DateLoad = qSelectDebt.ReadColumn("impl_date").toDate();
			double Cost = qSelectDebt.ReadColumn("iunl_cost").toDouble();
			double Accrued = qSelectDebt.ReadColumn("accrued").toDouble();

			QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(TreeWidget);
			TreeWidget->setItemWidget(TreeWidgetItem, 0, CreateItemWidget(ImplementationID, ImplementationUnloadID, DateLoad, Cost, Accrued));

			ISQuery qSelectAccrueds(QS_ACCRUED);
			qSelectAccrueds.BindValue(":ImplementationUnloadID", ImplementationUnloadID);
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
						LANG("OilSphere.AccruedWithDate").arg(DOUBLE_PREPARE(Sum)).arg(Date) :
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
void ISOilSphere::CounterpartyDebtForm::AddAccrued()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "CounterpartyWriteOff");
	ObjectFormBase->AddVirtualField("Counterparty", sender()->property("CounterpartyID"));
	ObjectFormBase->AddVirtualField("Unload", sender()->property("ImplementationUnloadID"));
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISOilSphere::CounterpartyDebtForm::LoadData);
	ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::ShowImplementation()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Implementation", sender()->property("ImplementationID").toInt()));
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::ShowImplementationUnload()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "ImplementationUnload", sender()->property("ImplementationUnloadID").toInt()));
}
//-----------------------------------------------------------------------------
QWidget* ISOilSphere::CounterpartyDebtForm::CreateItemWidget(int ImplementationID, int ImplementationUnloadID, const QDate &DateLoad, double Cost, int Accrued)
{
	QHBoxLayout *LayoutWidget = new QHBoxLayout();

	QWidget *Widget = new QWidget(TreeWidget);
	Widget->setLayout(LayoutWidget);

	QVBoxLayout *LayoutLabels = new QVBoxLayout();

	QLabel *LabelDateLoad = new QLabel(LANG("OilSphere.DateLoad").arg(DateLoad.toString(FORMAT_DATE_V2)), Widget);
	ISGui::SetFontWidgetBold(LabelDateLoad, true);
	LayoutLabels->addWidget(LabelDateLoad);

	QLabel *LabelUnloadCost = new QLabel(LANG("OilSphere.UnloadCost").arg(DOUBLE_PREPARE(Cost)), Widget);
	ISGui::SetFontWidgetBold(LabelUnloadCost, true);
	LayoutLabels->addWidget(LabelUnloadCost);

	double Debt = Cost - Accrued;
	QLabel *LabelDebt = new QLabel(LANG("OilSphere.Debt").arg(DOUBLE_PREPARE(Debt)), Widget);
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

	QAction *ActionAddAccrued = ButtonMenu->menu()->addAction(LANG("OilSphere.AddAccrued"), this, &ISOilSphere::CounterpartyDebtForm::AddAccrued);
	ActionAddAccrued->setProperty("ImplementationUnloadID", ImplementationUnloadID);
	ActionAddAccrued->setProperty("CounterpartyID", CounterpartyID);

	QAction *ActionShowImplementation = ButtonMenu->menu()->addAction(LANG("OilSphere.ShowImplementation"), this, &ISOilSphere::CounterpartyDebtForm::ShowImplementation);
	ActionShowImplementation->setProperty("ImplementationID", ImplementationID);

	QAction *ActionShowImplementationUnload = ButtonMenu->menu()->addAction(LANG("OilSphere.ShowImplementationUnload"), this, &ISOilSphere::CounterpartyDebtForm::ShowImplementationUnload);
	ActionShowImplementationUnload->setProperty("ImplementationUnloadID", ImplementationUnloadID);

	LayoutWidget->addStretch();
	return Widget;
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
ISOilSphere::ImplementationLoadObjectForm::ImplementationLoadObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Container"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationLoadObjectForm::Calculate);
	connect(GetFieldWidget("WeightGross"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationLoadObjectForm::Calculate);
	connect(GetFieldWidget("PriceUnit"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationLoadObjectForm::Calculate);
}
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationLoadObjectForm::~ImplementationLoadObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ImplementationLoadObjectForm::Save()
{
	QVariant Counterparty = GetFieldValue("Counterparty"), Stock = GetFieldValue("Stock");
	if (!Counterparty.isValid() && !Stock.isValid()) //Контрагент и склад не выбран
	{
		ISMessageBox::ShowWarning(this, LANG("OilSphere.Message.Warning.LoadCounterpartyAndStockIsEmpty"));
		return false;
	}
	if (Counterparty.isValid() && Stock.isValid()) //Выбран и контрагент и склад
	{
		ISMessageBox::ShowWarning(this, LANG("OilSphere.Message.Warning.LoadCounterpartyAndStockNotIsEmpty"));
		return false;
	}
	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationLoadObjectForm::Calculate()
{
	//Тара (весы)
	double Container = GetFieldValue("Container").toDouble();
	if (!Container)
	{
		GetFieldWidget("WeightNet")->Clear();
		GetFieldWidget("Cost")->Clear();
		return;
	}

	//Вес (брутто)
	double WeightGross = GetFieldValue("WeightGross").toDouble();
	if (!WeightGross)
	{
		GetFieldWidget("WeightNet")->Clear();
		GetFieldWidget("Cost")->Clear();
		return;
	}

	//Вес (нетто)
	double WeightNet = WeightGross - Container;
	SetFieldValue("WeightNet", WeightNet);

	//Цена за кг./л
	double PriceUnit = GetFieldValue("PriceUnit").toDouble();
	if (!PriceUnit)
	{
		GetFieldWidget("Cost")->Clear();
		return;
	}

	//Стоимость загрузки
	SetFieldValue("Cost", WeightNet * PriceUnit);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationUnloadObjectForm::ImplementationUnloadObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Container"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationUnloadObjectForm::Calculate);
	connect(GetFieldWidget("WeightGross"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationUnloadObjectForm::Calculate);
	connect(GetFieldWidget("PriceUnit"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::ImplementationUnloadObjectForm::Calculate);
}
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationUnloadObjectForm::~ImplementationUnloadObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ImplementationUnloadObjectForm::Save()
{
	QVariant UnloadCounterparty = GetFieldValue("Counterparty"), UnloadStock = GetFieldValue("Stock");
	if (!UnloadCounterparty.isValid() && !UnloadStock.isValid()) //Контрагент и склад не выбран
	{
		ISMessageBox::ShowWarning(this, LANG("OilSphere.Message.Warning.UnloadCounterpartyAndStockIsEmpty"));
		return false;
	}
	if (UnloadCounterparty.isValid() && UnloadStock.isValid()) //Выбран и контрагент и склад
	{
		ISMessageBox::ShowWarning(this, LANG("OilSphere.Message.Warning.UnloadCounterpartyAndStockNotIsEmpty"));
		return false;
	}

	double ValumeIncome = 0;
	if (UnloadStock.isValid()) //Если заполнено поле склад - проверяем наличие константы
	{
		ISQuery qSelectConstant(QS_CONSTANT);
		if (qSelectConstant.ExecuteFirst())
		{
			ValumeIncome = qSelectConstant.ReadColumn("prod_constant").toDouble() * GetFieldValue("WeightNet").toDouble();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("OilSphere.Message.Warning.NotFoundCurrentConstant"));
			return false;
		}
	}

	bool Result = ISObjectFormBase::Save();
	if (Result && UnloadStock.isValid()) //Если сохранение прошло успешно и поле "Склад" валидное - производим добавление в ведомомсть АЗС
	{
		ISQuery qSelect(QS_STATEMENT);
		qSelect.BindValue(":ImplementationUnload", GetObjectID());
		if (qSelect.ExecuteFirst())
		{
			if (qSelect.ReadColumn("count").toInt() == 0)
			{
				ISQuery qInsert(QI_STATEMENT);
				qInsert.BindValue(":ImplementationUnload", GetObjectID());
				qInsert.BindValue(":StockID", UnloadStock);
				qInsert.BindValue(":VolumeIncome", ValumeIncome);
				Result = qInsert.Execute();
				if (!Result)
				{
					ISMessageBox::ShowCritical(this, LANG("OilSphere.Message.Critical.InsertGasStatment"), qInsert.GetErrorString());
				}
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, qSelect.GetErrorString());
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISOilSphere::ImplementationUnloadObjectForm::Calculate()
{
	//Тара (весы)
	double Container = GetFieldValue("Container").toDouble();
	if (!Container)
	{
		GetFieldWidget("WeightNet")->Clear();
		GetFieldWidget("Cost")->Clear();
		return;
	}

	//Вес (брутто)
	double WeightGross = GetFieldValue("WeightGross").toDouble();
	if (!WeightGross)
	{
		GetFieldWidget("WeightNet")->Clear();
		GetFieldWidget("Cost")->Clear();
		return;
	}

	//Вес (нетто)
	double WeightNet = WeightGross - Container;
	SetFieldValue("WeightNet", WeightNet);

	//Цена за кг./л
	double PriceUnit = GetFieldValue("PriceUnit").toDouble();
	if (!PriceUnit)
	{
		GetFieldWidget("Cost")->Clear();
		return;
	}

	//Стоимость загрузки
	SetFieldValue("Cost", WeightNet * PriceUnit);
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
	/*ISQuery qSelectBeforeValues(QS_BEFORE_VALUES); //Запрашиваем значения с предыдущей смены
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
	}*/

	//Остаток на конец смены
	//connect(GetFieldWidget("BalanceBeginChange"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);
	//connect(GetFieldWidget("VolumeIncome"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);
	//connect(GetFieldWidget("VolumeHolidaysCounters"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);
	//connect(GetFieldWidget("TechnicalStraitDeviation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateBalanceEndChange);

	//Отаток в кассе, расчёт
	//connect(GetFieldWidget("SalesAmount"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment);
	//connect(GetFieldWidget("KKMCash"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment);
	//connect(GetFieldWidget("PaySoldVolume"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalancePayment);

	//Остаток в кассе, фактический
	//connect(GetFieldWidget("CashboxBalancePayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalanceActually);
	//connect(GetFieldWidget("PresenceDebt"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxBalanceActually);

	//Расхождения по кассе
	//connect(GetFieldWidget("CashboxBalancePayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxDiscrepancies);
	//connect(GetFieldWidget("CashboxBalanceActually"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxDiscrepancies);

	//Наличные по ККМ (касса)
	//connect(GetFieldWidget("KKMCash"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMCash);

	//Накопительный итог, расчёт
	//connect(GetFieldWidget("CashboxBalancePayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment);
	//connect(GetFieldWidget("CashboxCollectionAmount"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment);
	//connect(GetFieldWidget("CashboxAdministrativeExpenses"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalPayment);

	//Накопительный итог, фактический
	//connect(GetFieldWidget("CashboxBalanceActually"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually);
	//connect(GetFieldWidget("CashboxCollectionAmount"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually);
	//connect(GetFieldWidget("CashboxAdministrativeExpenses"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxTotalActually);

	//Расхождения итогов
	//connect(GetFieldWidget("CashboxTotalPayment"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CaclulateCashboxDiscrepanciesTotals);
	//connect(GetFieldWidget("CashboxTotalActually"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CaclulateCashboxDiscrepanciesTotals);

	//Накопительный итог по ККМ
	//connect(GetFieldWidget("KKMCash"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMTotal);
	//connect(GetFieldWidget("CashboxCollectionAmountKKM"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::GasStationStatementObjectForm::CalculateCashboxKKMTotal);
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
ISOilSphere::Debet1ObjectForm::Debet1ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet1ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Percent"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet1ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Calculation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet1ObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet1ObjectForm::~Debet1ObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet1ObjectForm::CalculateTotal()
{
	double Coming = GetFieldValue("Coming").toDouble(), Percent = GetFieldValue("Percent").toDouble();
	Coming > 0 && Percent > 0 ? SetFieldValue("Total", Coming - ((Coming * Percent) / 100)) : GetFieldWidget("Total")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet1ObjectForm::CalculateRemainder()
{
	double Total = GetFieldValue("Total").toDouble(), Calculation = GetFieldValue("Calculation").toDouble();
	Total > 0 && Calculation > 0 ? SetFieldValue("Remainder", Total - Calculation) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet2ObjectForm::Debet2ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet2ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Percent"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet2ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Calculation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet2ObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet2ObjectForm::~Debet2ObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet2ObjectForm::CalculateTotal()
{
	double Coming = GetFieldValue("Coming").toDouble(), Percent = GetFieldValue("Percent").toDouble();
	Coming > 0 && Percent > 0 ? SetFieldValue("Total", Coming - ((Coming * Percent) / 100)) : GetFieldWidget("Total")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet2ObjectForm::CalculateRemainder()
{
	double Total = GetFieldValue("Total").toDouble(), Calculation = GetFieldValue("Calculation").toDouble();
	Total > 0 && Calculation > 0 ? SetFieldValue("Remainder", Total - Calculation) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet1ListForm::Debet1ListForm(QWidget *parent) : ISListBaseForm("Debet1", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	Label = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(Label, true);
	GetToolBar()->addWidget(Label);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet1ListForm::~Debet1ListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet1ListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();
	double Total = 0, Calculation = 0;
	for (int Row = 0; Row < GetSqlModel()->rowCount(); ++Row)
	{
		QString Temp = GetSqlModel()->data(GetSqlModel()->index(Row, GetSqlModel()->GetFieldIndex("Total"))).toString();
		Total += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).toDouble();
		Temp = GetSqlModel()->data(GetSqlModel()->index(Row, GetSqlModel()->GetFieldIndex("Calculation"))).toString();
		Calculation += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).toDouble();
	}
	Label->setText(LANG("OilSphere.Debet.Label").arg(DOUBLE_PREPARE(Total)).arg(DOUBLE_PREPARE(Calculation)).arg(DOUBLE_PREPARE(Total - Calculation)));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet2ListForm::Debet2ListForm(QWidget *parent) : ISListBaseForm("Debet2", parent)
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
ISOilSphere::Debet2ListForm::~Debet2ListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet2ListForm::LoadDataAfterEvent()
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
		QSqlQueryModel *SqlQueryModel = new QSqlQueryModel(TableView);
		SqlQueryModel->setQuery(qSelect);
		SqlQueryModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Дата загрузки"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Тип продукта"), Qt::DisplayRole);
		SqlQueryModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Списано"), Qt::DisplayRole);
		TableView->setModel(SqlQueryModel);
	}
}
//-----------------------------------------------------------------------------
