#include "ISOilSphere.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDialogsCommon.h"
#include "ISDefinesGui.h"
#include "ISControls.h"
#include "ISObjects.h"
#include "ISLabels.h"
#include <ActiveQt/QAxObject>
#include <ActiveQt/QAxBase>
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
	qRegisterMetaType<ISOilSphere::CounterpartySubSystem*>("ISOilSphere::CounterpartyListForm");
	qRegisterMetaType<ISOilSphere::CounterpartyObjectForm*>("ISOilSphere::CounterpartyObjectForm");
	qRegisterMetaType<ISOilSphere::ImplementationSubSystem*>("ISOilSphere::ImplementationListForm");
	qRegisterMetaType<ISOilSphere::ImplementationLoadObjectForm*>("ISOilSphere::ImplementationLoadObjectForm");
	qRegisterMetaType<ISOilSphere::ImplementationUnloadObjectForm*>("ISOilSphere::ImplementationUnloadObjectForm");
	qRegisterMetaType<ISOilSphere::GasStationStatementSubSystem*>("ISOilSphere::GasStationStatementListForm");
	qRegisterMetaType<ISOilSphere::GasStationStatementObjectForm*>("ISOilSphere::GasStationStatementObjectForm");
	qRegisterMetaType<ISOilSphere::Debet1ObjectForm*>("ISOilSphere::Debet1ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet2ObjectForm*>("ISOilSphere::Debet2ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet3ObjectForm*>("ISOilSphere::Debet3ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet4ObjectForm*>("ISOilSphere::Debet4ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet5ObjectForm*>("ISOilSphere::Debet5ObjectForm");
	qRegisterMetaType<ISOilSphere::Debet1SubSystem*>("ISOilSphere::Debet1ListForm");
	qRegisterMetaType<ISOilSphere::Debet2SubSystem*>("ISOilSphere::Debet2ListForm");
	qRegisterMetaType<ISOilSphere::Debet3SubSystem*>("ISOilSphere::Debet3ListForm");
	qRegisterMetaType<ISOilSphere::Debet4SubSystem*>("ISOilSphere::Debet4ListForm");
	qRegisterMetaType<ISOilSphere::Debet5SubSystem*>("ISOilSphere::Debet5ListForm");
	qRegisterMetaType<ISOilSphere::DriverCostSubSystem*>("ISOilSphere::DriverCostListForm");
	qRegisterMetaType<ISOilSphere::StockAdmissionTrain*>("ISOilSphere::StockAdmissionTrain");
	qRegisterMetaType<ISOilSphere::StockAdmissionImplemetation*>("ISOilSphere::StockAdmissionImplemetation");
	qRegisterMetaType<ISOilSphere::StockWriteOff*>("ISOilSphere::StockWriteOff");
	qRegisterMetaType<ISOilSphere::MoveWagonSubSystem*>("ISOilSphere::MoveWagonListForm");
	qRegisterMetaType<ISOilSphere::ComingObjectForm*>("ISOilSphere::ComingObjectForm");
	qRegisterMetaType<ISOilSphere::DistributionObjectForm*>("ISOilSphere::DistributionObjectForm");
	qRegisterMetaType<ISOilSphere::ConsumptionAllSubSystem*>("ISOilSphere::ConsumptionAllSubSystem");
	qRegisterMetaType<ISOilSphere::ConsumptionMySubSystem*>("ISOilSphere::ConsumptionMySubSystem");
	qRegisterMetaType<ISOilSphere::ConsumptionObjectForm*>("ISOilSphere::ConsumptionObjectForm");
	qRegisterMetaType<ISOilSphere::ReturnMySubSystem*>("ISOilSphere::ReturnMySubSystem");
	qRegisterMetaType<ISOilSphere::ReturnObjectForm*>("ISOilSphere::ReturnObjectForm");
	qRegisterMetaType<ISOilSphere::BankListForm*>("ISOilSphere::BankListForm");
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
ISOilSphere::CounterpartySubSystem::CounterpartySubSystem(QWidget *parent) : ISListBaseForm("Counterparty", parent)
{
	QAction *ActionDebt = new QAction(ISObjects::Instance().GetInterface()->GetIcon("Debt"), LANG("OilSphere.Debts"), this);
	connect(ActionDebt, &QAction::triggered, this, &ISOilSphere::CounterpartySubSystem::ShowDebt);
	AddAction(ActionDebt, true, true);

	//LabelTotal = new QLabel(GetStatusBar());
	//ISGui::SetFontWidgetBold(LabelTotal, true);
	//GetStatusBar()->addWidget(LabelTotal);
}
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartySubSystem::~CounterpartySubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::CounterpartySubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	//LabelTotal->setText(LANG("OilSphere.Counterparty.Total").arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Debt", 0.0))));
	return Result;
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartySubSystem::ShowDebt()
{
	ISGui::SetWaitGlobalCursor(true);
	(new ISOilSphere::CounterpartyDebtForm(GetObjectID(), GetCurrentRecordValue("Name").toString()))->showMaximized();
	ISGui::SetWaitGlobalCursor(false);
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

	LabelTotal = new QLabel(this);
	ISGui::SetFontWidgetBold(LabelTotal, true);
	GetMainLayout()->addWidget(LabelTotal);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QVBoxLayout *LayoutLeft = new QVBoxLayout();
	Layout->addLayout(LayoutLeft);

	QGroupBox *GroupBoxLoad = new QGroupBox(LANG("OilSphere.Implementation.Loads"), this);
	GroupBoxLoad->setLayout(new QVBoxLayout());
	GroupBoxLoad->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	LayoutLeft->addWidget(GroupBoxLoad);

	QVBoxLayout *LayoutScrollLoad = new QVBoxLayout();
	LayoutScrollLoad->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	ISScrollArea *ScrollAreaLoad = new ISScrollArea(GroupBoxLoad);
	ScrollAreaLoad->widget()->setLayout(LayoutScrollLoad);
	GroupBoxLoad->layout()->addWidget(ScrollAreaLoad);

	QGroupBox *GroupBoxUnload = new QGroupBox(LANG("OilSphere.Implementation.Unloads"), this);
	GroupBoxUnload->setLayout(new QVBoxLayout());
	GroupBoxUnload->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	LayoutLeft->addWidget(GroupBoxUnload);

	QVBoxLayout *LayoutScrollUnload = new QVBoxLayout();
	LayoutScrollUnload->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	ISScrollArea *ScrollAreaUnload = new ISScrollArea(GroupBoxUnload);
	ScrollAreaUnload->widget()->setLayout(LayoutScrollUnload);
	GroupBoxUnload->layout()->addWidget(ScrollAreaUnload);

	ISTcpQuery qGetDebtImplementation("OilSphere_GetDebtImplementation");
	qGetDebtImplementation.BindValue("CounterpartyID", counterparty_id);
	if (qGetDebtImplementation.Execute())
	{
		QVariantList LoadUnloadList = qGetDebtImplementation.GetAnswer()["LoadUnload"].toList();
		for (const QVariant &Variant : LoadUnloadList)
		{
			QVariantMap LoadUnloadMap = Variant.toMap();

			bool IsLoad = LoadUnloadMap["IsLoad"].toBool();
			int ImplementationID = LoadUnloadMap["ImplementationID"].toInt();
			int LoadUnloadID = LoadUnloadMap["LoadUnloadID"].toInt();
			QString Date = LoadUnloadMap["Date"].toString();
			double Cost = LoadUnloadMap["Cost"].toDouble();

			QHBoxLayout *LayoutWidget = new QHBoxLayout();
			LayoutWidget->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);

			QWidget *WidgetLoadUnload = new QWidget(IsLoad ? ScrollAreaLoad : ScrollAreaUnload);
			WidgetLoadUnload->setLayout(LayoutWidget);

			QVBoxLayout *LayoutLabels = new QVBoxLayout();

			QLabel *LabelDateLoad = new QLabel(LANG("OilSphere.DateLoad").arg(Date), WidgetLoadUnload);
			ISGui::SetFontWidgetBold(LabelDateLoad, true);
			LayoutLabels->addWidget(LabelDateLoad);

			QLabel *LabelCost = new QLabel((IsLoad ? LANG("OilSphere.Cost.Load") : LANG("OilSphere.Cost.Unload")).arg(DOUBLE_PREPARE(Cost)), WidgetLoadUnload);
			ISGui::SetFontWidgetBold(LabelCost, true);
			LayoutLabels->addWidget(LabelCost);

			LayoutWidget->addLayout(LayoutLabels);

			LayoutWidget->addStretch();

			ISPushButton *ButtonShowImplementation = new ISPushButton(BUFFER_ICONS("Document"), LANG("OilSphere.ShowImplementation"), WidgetLoadUnload);
			connect(ButtonShowImplementation, &ISPushButton::clicked, this, &ISOilSphere::CounterpartyDebtForm::ShowImplementation);
			ButtonShowImplementation->setProperty("ImplementationID", ImplementationID);
			LayoutWidget->addWidget(ButtonShowImplementation);

			ISPushButton *ButtonShowLoadUnload = new ISPushButton(BUFFER_ICONS("Document"), IsLoad ? LANG("OilSphere.Show.ImplementationLoad") : LANG("OilSphere.Show.ImplementationUnload"), WidgetLoadUnload);
			connect(ButtonShowLoadUnload, &ISPushButton::clicked, this, &ISOilSphere::CounterpartyDebtForm::ShowLoadUnload);
			ButtonShowLoadUnload->setProperty("ID", LoadUnloadID);
			ButtonShowLoadUnload->setProperty("IsLoad", IsLoad);
			LayoutWidget->addWidget(ButtonShowLoadUnload);

			(IsLoad ? LayoutScrollLoad : LayoutScrollUnload)->addWidget(WidgetLoadUnload);
			(IsLoad ? LayoutScrollLoad : LayoutScrollUnload)->addWidget(ISControls::CreateHorizontalLine(ScrollAreaUnload));
		}
		LayoutScrollLoad->addStretch();
		LayoutScrollUnload->addStretch();
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetDebtImplementation.GetErrorString());
	}

	QVBoxLayout *LayoutRight = new QVBoxLayout();
	Layout->addLayout(LayoutRight);

	QGroupBox *GroupBoxEnrollment = new QGroupBox(LANG("OilSphere.Counterparty.Enrollments"), this);
	GroupBoxEnrollment->setLayout(new QVBoxLayout());
	LayoutRight->addWidget(GroupBoxEnrollment);

	EntrollmentListForm = new ISListBaseForm("CounterpartyEnrollment", GroupBoxEnrollment);
	EntrollmentListForm->SetParentFilterField("Counterparty");
	EntrollmentListForm->SetParentObjectID(counterparty_id);
	EntrollmentListForm->GetTcpQuery()->AddFilter("Counterparty", counterparty_id);
	EntrollmentListForm->GetTcpQuery()->AddFilter("Counterparty", counterparty_id);
	connect(EntrollmentListForm, &ISListBaseForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
	connect(EntrollmentListForm, &ISListBaseForm::Updated, this, &ISOilSphere::CounterpartyDebtForm::UpdatedLists);
	GroupBoxEnrollment->layout()->addWidget(EntrollmentListForm);
	EntrollmentListForm->LoadData();

	QGroupBox *GroupBoxMoveWagon = new QGroupBox(LANG("OilSphere.Counterparty.MoveWagon"), this);
	GroupBoxMoveWagon->setLayout(new QVBoxLayout());
	LayoutRight->addWidget(GroupBoxMoveWagon);

	MoveWagonViewForm = new ISListViewForm("SelectCounterpartyMoveWagon", GroupBoxMoveWagon);
	MoveWagonViewForm->BindValue("CounterpartyID", counterparty_id);
	GroupBoxMoveWagon->layout()->addWidget(MoveWagonViewForm);
	MoveWagonViewForm->LoadData();
}
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyDebtForm::~CounterpartyDebtForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::UpdatedLists()
{
	ISTcpQuery qGetDebtCounterparty("OilSphere_GetDebtCounterparty");
	qGetDebtCounterparty.BindValue("CounterpartyID", CounterpartyID);
	if (qGetDebtCounterparty.Execute())
	{
		QVariantMap AnswerMap = qGetDebtCounterparty.GetAnswer();
		double TotalUnload = AnswerMap["TotalUnload"].toDouble();
		double TotalLoad = AnswerMap["TotalLoad"].toDouble();
		double TotalEntrollment = AnswerMap["TotalEntrollment"].toDouble();
		double MoveWagonSum = AnswerMap["MoveWagonSum"].toDouble();
		LabelTotal->setText(LANG("OilSphere.Debts.Label")
			.arg(DOUBLE_PREPARE(TotalLoad))
			.arg(DOUBLE_PREPARE(TotalUnload))
			.arg(DOUBLE_PREPARE(TotalEntrollment))
			.arg(DOUBLE_PREPARE(MoveWagonSum)).arg(DOUBLE_PREPARE(TotalUnload - (TotalLoad + TotalEntrollment + MoveWagonSum))));
	}
	else
	{
		LabelTotal->setText(qGetDebtCounterparty.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::ShowImplementation()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, "Implementation", sender()->property("ImplementationID").toInt()));
}
//-----------------------------------------------------------------------------
void ISOilSphere::CounterpartyDebtForm::ShowLoadUnload()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, sender()->property("IsLoad").toBool() ? "ImplementationLoad" : "ImplementationUnload", sender()->property("ID").toInt()));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::CounterpartyObjectForm::CounterpartyObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
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
	ISTcpQuery qOrganizationFromINN(API_ORGANIZATION_FROM_INN);
	qOrganizationFromINN.BindValue("INN", INNEdit->GetValue());

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qOrganizationFromINN.Execute();
	ISGui::SetWaitGlobalCursor(false);
	if (!Result)
	{
		ISMessageBox::ShowCritical(this, qOrganizationFromINN.GetErrorString());
		return;
	}

	QVariantMap AnswerMap = qOrganizationFromINN.TakeAnswer()["Reply"].toMap();
	GetFieldWidget("Name")->SetValue(AnswerMap["data"].toMap()["name"].toMap()["short_with_opf"].toString());
	GetFieldWidget("Address")->SetValue(AnswerMap["data"].toMap()["address"].toMap()["value"].toString());
	GetFieldWidget("Director")->SetValue(AnswerMap["data"].toMap()["management"].toMap()["name"].toString());
	GetFieldWidget("Kpp")->SetValue(AnswerMap["data"].toMap()["kpp"].toString());
	GetFieldWidget("Ogrn")->SetValue(AnswerMap["data"].toMap()["ogrn"].toString());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationSubSystem::ImplementationSubSystem(QWidget *parent) : ISListBaseForm("Implementation", parent)
{
	LabelTotal = new QLabel(GetStatusBar());
	ISGui::SetFontWidgetBold(LabelTotal, true);
	GetStatusBar()->addWidget(LabelTotal);
}
//-----------------------------------------------------------------------------
ISOilSphere::ImplementationSubSystem::~ImplementationSubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ImplementationSubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	/*LabelTotal->setText(LANG("OilSphere.Implementation.Total")
		.arg(GetSqlModel()->GetFieldSum<int>("LoadCount", 0))
		.arg(GetSqlModel()->GetFieldSum<int>("UnloadCount", 0))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("LoadWeightNet", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("UnloadWeightNet", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("LoadCost", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("UnloadCost", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("WeightDifference", 0.0))));*/
	return Result;
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
		ISTcpQuery qPeriodConstant("OilSphere_PeriodContains");
		if (qPeriodConstant.Execute())
		{
			QVariantMap AnswerMap = qPeriodConstant.GetAnswer();
			if (!AnswerMap["Exist"].toBool())
			{
				ISMessageBox::ShowCritical(this, LANG("OilSphere.Message.Warning.NotFoundCurrentConstant"));
				return false;
			}
			ValumeIncome = AnswerMap["Value"].toInt();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qPeriodConstant.GetErrorString());
			return false;
		}
	}

	bool Result = ISObjectFormBase::Save();
	if (Result && UnloadStock.isValid()) //Если сохранение прошло успешно и поле "Склад" заполнено - производим добавление в ведомомсть АЗС
	{
		ISTcpQuery qStatementAdd("OilSphere_StatementAdd");
		qStatementAdd.BindValue("ImplementationUnload", GetObjectID());
		qStatementAdd.BindValue("UnloadStock", UnloadStock);
		qStatementAdd.BindValue("ValumeIncome", ValumeIncome);
		Result = qStatementAdd.Execute();
		if (!Result)
		{
			ISMessageBox::ShowCritical(this, qStatementAdd.GetErrorString());
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
ISOilSphere::GasStationStatementSubSystem::GasStationStatementSubSystem(QWidget *parent) : ISListBaseForm("GasStationStatement", parent)
{
	GetToolBar()->addWidget(new QLabel(LANG("OilSphere.GasStationLabel"), GetToolBar()));

	EditStock = new ISComboEdit(this);
	EditStock->SetEditable(false);
	EditStock->AddItem(LANG("All"));
	connect(EditStock, &ISFieldEditBase::ValueChange, this, &ISOilSphere::GasStationStatementSubSystem::StockChanged);
	GetToolBar()->addWidget(EditStock);

	ISTcpQuery qGetStockList("OilSphere_GetStockList");
	if (qGetStockList.Execute())
	{
		QVariantList StockList = qGetStockList.GetAnswer()["List"].toList();
		for (const QVariant &Variant : StockList)
		{
			QVariantMap StockMap = Variant.toMap();
			EditStock->AddItem(StockMap["Name"].toString(), StockMap["ID"]);
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetStockList.GetErrorString());
	}

	LabelTotal = new QLabel(GetStatusBar());
	ISGui::SetFontWidgetBold(LabelTotal, true);
	GetStatusBar()->addWidget(LabelTotal);
}
//-----------------------------------------------------------------------------
ISOilSphere::GasStationStatementSubSystem::~GasStationStatementSubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::GasStationStatementSubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	/*LabelTotal->setText(LANG("OilSphere.GasStationStatement.Total")
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("BalanceBeginChange", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("VolumeIncome", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("VolumeHolidaysCounters", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("TechnicalStraitDeviation", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("BalanceEndChange", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("SalesAmount", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("KKMCash", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("KKMCashless", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("Statements", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("PaySoldVolume", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("PresenceDebt", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxBalancePayment", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxBalanceActually", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxDiscrepancies", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxKKMCash", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxTotalPayment", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxTotalActually", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxDiscrepanciesTotals", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxKKMTotal", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxCollectionAmount", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxCollectionAmountKKM", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("CashboxAdministrativeExpenses", 0.0))));*/
	return Result;
}
//-----------------------------------------------------------------------------
void ISOilSphere::GasStationStatementSubSystem::StockChanged(const QVariant &Value)
{
	//???
	Q_UNUSED(Value);
	/*Value.isValid()
		? GetQueryModel()->SetClassFilter("gsts_stock = " + Value.toString()) : 
		GetQueryModel()->ClearClassFilter();*/
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
	QAction *ActionFillInBasedOn = new QAction(BUFFER_ICONS("Arrow.Down"), LANG("OilSphere.FillInBased"), GetToolBar());
	connect(ActionFillInBasedOn, &QAction::triggered, this, &ISOilSphere::GasStationStatementObjectForm::FillInBased);
	AddActionToolBar(ActionFillInBasedOn);	

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
void ISOilSphere::GasStationStatementObjectForm::FillInBased()
{
	ISObjectPair SelectedObject = ISGui::SelectObject("GasStationStatement");
	if (SelectedObject.first) //Если запись была выбрана - заполняем
	{
		if (SelectedObject.first == GetObjectID())
		{
			ISMessageBox::ShowWarning(this, LANG("OilSphere.Message.Warning.FillInBased"));
		}
		else
		{
			ISTcpQuery qGetGasStation("OilSphere_GetGasStation");
			qGetGasStation.BindValue("StatementID", SelectedObject.first);
			if (qGetGasStation.Execute())
			{
				QVariantMap AnswerMap = qGetGasStation.GetAnswer();
				SetFieldValue("BalanceBeginChange", AnswerMap["BalanceEndChange"].toDouble());
				SetFieldValue("CashboxTotalPayment", AnswerMap["CashboxTotalPayment"].toDouble());
				SetFieldValue("CashboxTotalActually", AnswerMap["CashboxTotalActually"].toDouble());
				SetFieldValue("CashboxKKMTotal", AnswerMap["CashboxKKMTotal"].toDouble());
				SetModificationFlag(false);
			}
			else
			{
				ISMessageBox::ShowWarning(this, qGetGasStation.GetErrorString());
			}
		}
	}
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
ISOilSphere::Debet3ObjectForm::Debet3ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet3ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Percent"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet3ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Calculation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet3ObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet3ObjectForm::~Debet3ObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet3ObjectForm::CalculateTotal()
{
	double Coming = GetFieldValue("Coming").toDouble(), Percent = GetFieldValue("Percent").toDouble();
	Coming > 0 && Percent > 0 ? SetFieldValue("Total", Coming - ((Coming * Percent) / 100)) : GetFieldWidget("Total")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet3ObjectForm::CalculateRemainder()
{
	double Total = GetFieldValue("Total").toDouble(), Calculation = GetFieldValue("Calculation").toDouble();
	Total > 0 && Calculation > 0 ? SetFieldValue("Remainder", Total - Calculation) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet4ObjectForm::Debet4ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet4ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Percent"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet4ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Calculation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet4ObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet4ObjectForm::~Debet4ObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet4ObjectForm::CalculateTotal()
{
	double Coming = GetFieldValue("Coming").toDouble(), Percent = GetFieldValue("Percent").toDouble();
	Coming > 0 && Percent > 0 ? SetFieldValue("Total", Coming - ((Coming * Percent) / 100)) : GetFieldWidget("Total")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet4ObjectForm::CalculateRemainder()
{
	double Total = GetFieldValue("Total").toDouble(), Calculation = GetFieldValue("Calculation").toDouble();
	Total > 0 && Calculation > 0 ? SetFieldValue("Remainder", Total - Calculation) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet5ObjectForm::Debet5ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Coming"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet5ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Percent"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet5ObjectForm::CalculateTotal);
	connect(GetFieldWidget("Calculation"), &ISFieldEditBase::DataChanged, this, &ISOilSphere::Debet5ObjectForm::CalculateRemainder);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet5ObjectForm::~Debet5ObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet5ObjectForm::CalculateTotal()
{
	double Coming = GetFieldValue("Coming").toDouble(), Percent = GetFieldValue("Percent").toDouble();
	Coming > 0 && Percent > 0 ? SetFieldValue("Total", Coming - ((Coming * Percent) / 100)) : GetFieldWidget("Total")->Clear();
}
//-----------------------------------------------------------------------------
void ISOilSphere::Debet5ObjectForm::CalculateRemainder()
{
	double Total = GetFieldValue("Total").toDouble(), Calculation = GetFieldValue("Calculation").toDouble();
	Total > 0 && Calculation > 0 ? SetFieldValue("Remainder", Total - Calculation) : GetFieldWidget("Remainder")->Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet1SubSystem::Debet1SubSystem(QWidget *parent) : ISListBaseForm("Debet1", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	Label = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(Label, true);
	GetToolBar()->addWidget(Label);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet1SubSystem::~Debet1SubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::Debet1SubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	double Total = 0, Calculation = 0;
	for (int Row = 0; Row < GetTcpModel()->rowCount(); ++Row)
	{
		QString Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Total"))).toString();
		Total += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
		Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Calculation"))).toString();
		Calculation += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
	}
	Label->setText(LANG("OilSphere.Debet.Label").arg(DOUBLE_PREPARE(Total)).arg(DOUBLE_PREPARE(Calculation)).arg(DOUBLE_PREPARE(Total - Calculation)));
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet2SubSystem::Debet2SubSystem(QWidget *parent) : ISListBaseForm("Debet2", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	Label = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(Label, true);
	GetToolBar()->addWidget(Label);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet2SubSystem::~Debet2SubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::Debet2SubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	double Total = 0, Calculation = 0;
	for (int Row = 0; Row < GetTcpModel()->rowCount(); ++Row)
	{
		QString Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Total"))).toString();
		Total += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
		Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Calculation"))).toString();
		Calculation += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
	}
	Label->setText(LANG("OilSphere.Debet.Label").arg(DOUBLE_PREPARE(Total)).arg(DOUBLE_PREPARE(Calculation)).arg(DOUBLE_PREPARE(Total - Calculation)));
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet3SubSystem::Debet3SubSystem(QWidget *parent) : ISListBaseForm("Debet3", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	Label = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(Label, true);
	GetToolBar()->addWidget(Label);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet3SubSystem::~Debet3SubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::Debet3SubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	double Total = 0, Calculation = 0;
	for (int Row = 0; Row < GetTcpModel()->rowCount(); ++Row)
	{
		QString Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Total"))).toString();
		Total += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
		Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Calculation"))).toString();
		Calculation += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
	}
	Label->setText(LANG("OilSphere.Debet.Label").arg(DOUBLE_PREPARE(Total)).arg(DOUBLE_PREPARE(Calculation)).arg(DOUBLE_PREPARE(Total - Calculation)));
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet4SubSystem::Debet4SubSystem(QWidget *parent) : ISListBaseForm("Debet4", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	Label = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(Label, true);
	GetToolBar()->addWidget(Label);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet4SubSystem::~Debet4SubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::Debet4SubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	double Total = 0, Calculation = 0;
	for (int Row = 0; Row < GetTcpModel()->rowCount(); ++Row)
	{
		QString Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Total"))).toString();
		Total += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
		Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Calculation"))).toString();
		Calculation += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
	}
	Label->setText(LANG("OilSphere.Debet.Label").arg(DOUBLE_PREPARE(Total)).arg(DOUBLE_PREPARE(Calculation)).arg(DOUBLE_PREPARE(Total - Calculation)));
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::Debet5SubSystem::Debet5SubSystem(QWidget *parent) : ISListBaseForm("Debet5", parent)
{
	GetToolBar()->addWidget(ISControls::CreateVerticalLine(GetToolBar()));

	Label = new QLabel(GetToolBar());
	ISGui::SetFontWidgetBold(Label, true);
	GetToolBar()->addWidget(Label);
}
//-----------------------------------------------------------------------------
ISOilSphere::Debet5SubSystem::~Debet5SubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::Debet5SubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	double Total = 0, Calculation = 0;
	for (int Row = 0; Row < GetTcpModel()->rowCount(); ++Row)
	{
		QString Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Total"))).toString();
		Total += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
		Temp = GetTcpModel()->data(GetTcpModel()->index(Row, GetTcpModel()->GetFieldIndex("Calculation"))).toString();
		Calculation += Temp.replace(SYMBOL_COMMA, SYMBOL_POINT).remove(' ').toDouble();
	}
	Label->setText(LANG("OilSphere.Debet.Label").arg(DOUBLE_PREPARE(Total)).arg(DOUBLE_PREPARE(Calculation)).arg(DOUBLE_PREPARE(Total - Calculation)));
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::DriverCostSubSystem::DriverCostSubSystem(QWidget *parent) : ISListBaseForm("DriverCost", parent)
{
	QAction *ActionCreateInBased = new QAction(BUFFER_ICONS("Add.Green"), LANG("OilSphere.CreateInBased"), GetToolBar());
	connect(ActionCreateInBased, &QAction::triggered, this, &ISOilSphere::DriverCostSubSystem::CreateOnBased);
	InsertAction(ActionCreateInBased, GetAction(ISNamespace::ActionType::Create), true, true);

	LabelTotal = new QLabel(GetStatusBar());
	ISGui::SetFontWidgetBold(LabelTotal, true);
	GetStatusBar()->addWidget(LabelTotal);
}
//-----------------------------------------------------------------------------
ISOilSphere::DriverCostSubSystem::~DriverCostSubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::DriverCostSubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	/*LabelTotal->setText(LANG("OilSphere.DriverCost.Total")
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Coming", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Consumption", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Remainder", 0.0))));*/
	return Result;
}
//-----------------------------------------------------------------------------
void ISOilSphere::DriverCostSubSystem::CreateOnBased()
{
	///???
	/*ISQuery qSelectRemainder("SELECT get_driver_cost_remainder(:DriverCostID, :DriverCostComing, :PreviousRemainder)");
	qSelectRemainder.BindValue(":DriverCostID", GetSelectedIDs()[0]);
	qSelectRemainder.BindValue(":DriverCostComing", GetCurrentRecordValue("Coming"));
	qSelectRemainder.BindValue(":PreviousRemainder", GetCurrentRecordValue("PreviousRemainder"));
	if (qSelectRemainder.ExecuteFirst())
	{
		ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::New, "DriverCost");
		ObjectFormBase->SetFieldValue("PreviousRemainder", qSelectRemainder.ReadColumn("get_driver_cost_remainder"));
		connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListBaseForm::Updated);
		connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISListBaseForm::Update);
		ISGui::ShowObjectForm(ObjectFormBase);
	}
	else
	{
		ISMessageBox::ShowWarning(this, qSelectRemainder.GetErrorString());
	}*/
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::StockAdmissionTrain::StockAdmissionTrain(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ListViewForm = new ISListViewForm("SelectStockAdmissionTrain", this);
	GetMainLayout()->addWidget(ListViewForm);
}
//-----------------------------------------------------------------------------
ISOilSphere::StockAdmissionTrain::~StockAdmissionTrain()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::StockAdmissionTrain::LoadData()
{
	ListViewForm->BindValue(":StockID", GetParentObjectID());
	ListViewForm->LoadData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::StockAdmissionImplemetation::StockAdmissionImplemetation(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ListViewForm = new ISListViewForm("SelectStockAdmissionImplementation", this);
	GetMainLayout()->addWidget(ListViewForm);
}
//-----------------------------------------------------------------------------
ISOilSphere::StockAdmissionImplemetation::~StockAdmissionImplemetation()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::StockAdmissionImplemetation::LoadData()
{
	ListViewForm->BindValue(":StockID", GetParentObjectID());
	ListViewForm->LoadData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::StockWriteOff::StockWriteOff(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	ListViewForm = new ISListViewForm("SelectStockWriteOff", this);
	GetMainLayout()->addWidget(ListViewForm);
}
//-----------------------------------------------------------------------------
ISOilSphere::StockWriteOff::~StockWriteOff()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::StockWriteOff::LoadData()
{
	ListViewForm->BindValue(":StockID", GetParentObjectID());
	ListViewForm->LoadData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::MoveWagonSubSystem::MoveWagonSubSystem(QWidget *parent) : ISListBaseForm("MoveWagon", parent)
{
	LabelTotal = new QLabel(GetStatusBar());
	ISGui::SetFontWidgetBold(LabelTotal, true);
	GetStatusBar()->addWidget(LabelTotal);
}
//-----------------------------------------------------------------------------
ISOilSphere::MoveWagonSubSystem::~MoveWagonSubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::MoveWagonSubSystem::Update()
{
	bool Result = ISListBaseForm::Update();
	/*LabelTotal->setText(LANG("OilSphere.MoveWagon.Total")
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("Kilogram", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Price", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Sum", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("TechnicalLosses", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("StorageServices", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("Transshipment", 0.0)))
		.arg(DOUBLE_PREPARE(GetSqlModel()->GetFieldSum<double>("TechnicalLossesWeight", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("PriceFull", 0.0)))
		.arg(DOUBLE_PREPAREM(GetSqlModel()->GetFieldSum<double>("TotalCost", 0.0))));*/
	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ComingObjectForm::ComingObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::ComingObjectForm::~ComingObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ComingObjectForm::Save()
{
	AddVirtualField("User", CURRENT_USER_ID);
	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::DistributionObjectForm::DistributionObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::DistributionObjectForm::~DistributionObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::DistributionObjectForm::Save()
{
	AddVirtualField("UserFrom", CURRENT_USER_ID);
	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionAllSubSystem::ConsumptionAllSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	LabelTitle = new QLabel(LANG("OilSphere.TotalBalance").arg(0).arg(0).arg(0), this);
	ISGui::SetFontWidgetBold(LabelTitle, true);
	GetMainLayout()->addWidget(LabelTitle, 0, Qt::AlignCenter);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	GroupBoxUsers = new QGroupBox(LANG("OilSphere.Users"), this);
	GroupBoxUsers->setSizePolicy(QSizePolicy::Maximum, GroupBoxUsers->sizePolicy().verticalPolicy());
	GroupBoxUsers->setLayout(new QFormLayout());
	Layout->addWidget(GroupBoxUsers);

	QVBoxLayout *LayoutRight = new QVBoxLayout();
	Layout->addLayout(LayoutRight);

	QGroupBox *GroupBoxConsumption = new QGroupBox(LANG("OilSphere.Consumption"), this);
	GroupBoxConsumption->setLayout(new QVBoxLayout());
	GroupBoxConsumption->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_1_PX);
	LayoutRight->addWidget(GroupBoxConsumption);

	ListConsumption = new ISListWidget(GroupBoxConsumption);
	ListConsumption->setAlternatingRowColors(true);
	ListConsumption->setFrameShape(QFrame::NoFrame);
	GroupBoxConsumption->layout()->addWidget(ListConsumption);
	
	QGroupBox *GroupBoxReturn = new QGroupBox(LANG("OilSphere.Return"), this);
	GroupBoxReturn->setLayout(new QVBoxLayout());
	GroupBoxReturn->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_1_PX);
	LayoutRight->addWidget(GroupBoxReturn);

	ListReturn = new ISListWidget(GroupBoxReturn);
	ListReturn->setAlternatingRowColors(true);
	ListReturn->setFrameShape(QFrame::NoFrame);
	GroupBoxReturn->layout()->addWidget(ListReturn);
}
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionAllSubSystem::~ConsumptionAllSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::ConsumptionAllSubSystem::LoadData()
{
	ISTcpQuery qGetUsersConsumption("OilSphere_GetUserConsumption");

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qGetUsersConsumption.Execute();
	ISGui::SetWaitGlobalCursor(false);

	if (!Result)
	{
		ISMessageBox::ShowCritical(this, qGetUsersConsumption.GetErrorString());
		return;
	}

	QVariantMap AnswerMap = qGetUsersConsumption.TakeAnswer();
	UserList = AnswerMap["UserList"].toList();
	for (const QVariant &Variant : UserList)
	{
		QVariantMap UserMap = Variant.toMap();
		double Balance = UserMap["Balance"].toDouble();

		ISLabelPixmapText *Label = new ISLabelPixmapText(UserMap["FIO"].toString(), GroupBoxUsers);
		if (Balance > 0)
		{
			Label->SetPixmap(ISObjects::Instance().GetInterface()->GetIcon("Balance.Positive").pixmap(16, 16));
		}
		else if (Balance < 0)
		{
			Label->SetPixmap(ISObjects::Instance().GetInterface()->GetIcon("Balance.Negative").pixmap(16, 16));
		}
		else
		{
			Label->SetPixmap(ISObjects::Instance().GetInterface()->GetIcon("Balance.Null").pixmap(16, 16));
		}

		ISPushButton *ButtonBalance = new ISPushButton(LANG("OilSphere.Balance").arg(Balance), GroupBoxUsers);
		ButtonBalance->setProperty("UserID", UserMap["ID"]);
		connect(ButtonBalance, &ISPushButton::clicked, this, &ISOilSphere::ConsumptionAllSubSystem::BalanceClicked);
		dynamic_cast<QFormLayout*>(GroupBoxUsers->layout())->addRow(Label, ButtonBalance);
	}
	LabelTitle->setText(LANG("OilSphere.TotalBalance").arg(AnswerMap["TotalComing"].toString()).arg(AnswerMap["TotalConsumption"].toString()).arg(AnswerMap["Balance"].toString()));
}
//-----------------------------------------------------------------------------
void ISOilSphere::ConsumptionAllSubSystem::BalanceClicked()
{
	ListConsumption->Clear();
	ListReturn->Clear();

	QVariant UserID = sender()->property("UserID");

	QVariantList ConsumptionList, ReturnList;
	for (const QVariant &Variant : UserList)
	{
		QVariantMap UserMap = Variant.toMap();
		if (UserMap["ID"] == UserID)
		{
			ConsumptionList = UserMap["ConsumptionList"].toList();
			ReturnList = UserMap["ReturnList"].toList();
			break;
		}
	}

	for (const QVariant &Variant : ConsumptionList)
	{
		QVariantMap ConsumptionMap = Variant.toMap();

		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListConsumption);
		ListWidgetItem->setText(ConsumptionMap["Date"].toString() + ": " + ConsumptionMap["Sum"].toString());
		ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

		QString Note = ConsumptionMap["Note"].toString();
		if (!Note.isEmpty())
		{
			ListWidgetItem->setText(ListWidgetItem->text() + '\n' + Note);
		}
	}

	for (const QVariant &Variant : ReturnList)
	{
		QVariantMap ReturnMap = Variant.toMap();

		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListReturn);
		ListWidgetItem->setText(ReturnMap["Date"].toString() + ": " + ReturnMap["Sum"].toString());
		ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

		QString Note = ReturnMap["Note"].toString();
		if (!Note.isEmpty())
		{
			ListWidgetItem->setText(ListWidgetItem->text() + '\n' + Note);
		}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionMySubSystem::ConsumptionMySubSystem(QWidget *parent) : ISListBaseForm("Consumption", parent)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionMySubSystem::~ConsumptionMySubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ConsumptionMySubSystem::Update()
{
	GetTcpQuery()->AddFilter("User", CURRENT_USER_ID);
	return ISListBaseForm::Update();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionObjectForm::ConsumptionObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::ConsumptionObjectForm::~ConsumptionObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ConsumptionObjectForm::Save()
{
	AddVirtualField("User", CURRENT_USER_ID);
	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ReturnMySubSystem::ReturnMySubSystem(QWidget *parent) : ISListBaseForm("Return", parent)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::ReturnMySubSystem::~ReturnMySubSystem()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ReturnMySubSystem::Update()
{
	GetTcpQuery()->AddFilter("User", CURRENT_USER_ID);
	return ISListBaseForm::Update();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::ReturnObjectForm::ReturnObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISOilSphere::ReturnObjectForm::~ReturnObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphere::ReturnObjectForm::Save()
{
	AddVirtualField("User", CURRENT_USER_ID);
	return ISObjectFormBase::Save();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOilSphere::BankListForm::BankListForm(QWidget *parent) : ISListBaseForm("Bank", parent)
{
	QAction *ActionLoad = new QAction(BUFFER_ICONS("Arrow.Up"), LANG("OilSphere.LoadData"), GetToolBar());
	connect(ActionLoad, &QAction::triggered, this, &ISOilSphere::BankListForm::Load);
	AddAction(ActionLoad, false, true);
}
//-----------------------------------------------------------------------------
ISOilSphere::BankListForm::~BankListForm()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::BankListForm::Load()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this);
	if (FilePath.isEmpty())
	{
		return;
	}

	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly))
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), File.errorString());
		return;
	}

	QByteArray ByteArray = File.readAll().toBase64();
	File.close();

	ISTcpQuery qLoadBanks("OilSphere_LoadBanks");
	qLoadBanks.BindValue("ByteArray", ByteArray);
	if (qLoadBanks.Execute())
	{
		QVariantMap AnswerMap = qLoadBanks.GetAnswer();
		int Total = AnswerMap["Total"].toInt(),
			Loaded = AnswerMap["Loaded"].toInt(),
			Invalid = AnswerMap["Invalid"].toInt();
		ISMessageBox::ShowInformation(this, LANG("OilSphere.Message.Information.LoadBanks").arg(Total).arg(Loaded).arg(Invalid),
			LANG("OilSphere.Message.Information.LoadBanks.DetailText"));
		Update();
	}
	else
	{
		ISMessageBox::ShowCritical(this, qLoadBanks.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
