#include "ISOilSphereWorker.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
#include "ISTcpWorkerHelper.h"
//-----------------------------------------------------------------------------
static QString QS_PERIOD = PREPARE_QUERY("SELECT prod_constant "
										 "FROM period "
										 "WHERE CURRENT_DATE BETWEEN prod_datestart AND prod_dateend");
//-----------------------------------------------------------------------------
static QString QS_STOCK = PREPARE_QUERY("SELECT stck_id, stck_name "
										"FROM stock "
										"ORDER BY stck_name");
//-----------------------------------------------------------------------------
static QString QS_STATEMENT = PREPARE_QUERY("SELECT COUNT(*) "
											"FROM gasstationstatement "
											"WHERE gsts_implementationunload = :ImplementationUnload");
//-----------------------------------------------------------------------------
static QString QI_STATEMENT = PREPARE_QUERY("INSERT INTO gasstationstatement(gsts_implementationunload, gsts_stock, gsts_date, gsts_volumeincome) "
											"VALUES(:ImplementationUnload, :StockID, "
											"(SELECT impl_date FROM implementation WHERE impl_id = (SELECT iunl_implementation FROM implementationunload WHERE iunl_id = :ImplementationUnload)), "
											"(SELECT iunl_weightnet * :VolumeIncome FROM implementationunload WHERE iunl_id = :ImplementationUnload))");
//-----------------------------------------------------------------------------
static QString QS_FILL_IN_BASED = PREPARE_QUERY("SELECT "
												"COALESCE(gsts_balanceendchange, 0) AS gsts_balanceendchange, "
												"COALESCE(gsts_cashboxtotalpayment, 0) AS gsts_cashboxtotalpayment, "
												"COALESCE(gsts_cashboxtotalactually, 0) AS gsts_cashboxtotalactually, "
												"COALESCE(gsts_cashboxkkmtotal, 0) AS gsts_cashboxkkmtotal "
												"FROM gasstationstatement "
												"WHERE gsts_id = :StatementID");
//-----------------------------------------------------------------------------
static QString QS_IMPLEMENTATION_UNLOAD = PREPARE_QUERY("SELECT true AS is_load, ilod_implementation AS implementation_id, ilod_id AS id, impl_date AS date, ilod_cost AS cost "
														"FROM implementationload "
														"LEFT JOIN implementation ON ilod_implementation = impl_id "
														"WHERE ilod_counterparty = :CounterpartyID "
														""
														"UNION "
														""
														"SELECT false AS is_load, iunl_implementation AS implementation_id, iunl_id AS id, impl_date AS date, iunl_cost AS cost "
														"FROM implementationunload "
														"LEFT JOIN implementation ON iunl_implementation = impl_id "
														"WHERE iunl_counterparty = :CounterpartyID "
														"ORDER BY is_load, date DESC");
//-----------------------------------------------------------------------------
static QString QS_COUNTERPARTY_DEBT = PREPARE_QUERY("SELECT get_counterparty_unload(:CounterpartyID), get_counterparty_load(:CounterpartyID), get_counterparty_entrollment(:CounterpartyID), get_counterparty_move_wagon(:CounterpartyID)");
//-----------------------------------------------------------------------------
static QString QS_USERS_CONSUMPTION = PREPARE_QUERY("SELECT usrs_id, usrs_fio, get_user_balance(usrs_id) "
													"FROM _users "
													"ORDER BY usrs_fio");
//-----------------------------------------------------------------------------
static QString QS_USER_CONSUMPTION = PREPARE_QUERY("SELECT cpmp_date, cpmp_sum, cpmp_note "
												   "FROM consumption "
												   "WHERE cpmp_user = :UserID "
												   "ORDER BY cpmp_date DESC");
//-----------------------------------------------------------------------------
ISOilSphereWorker::ISOilSphereWorker() : ISTcpWorker()
{

}
//-----------------------------------------------------------------------------
ISOilSphereWorker::~ISOilSphereWorker()
{

}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	switch (TcpMessage->Type)
	{
	case ISNamespace::ApiMessageType::PeriodContains: return PeriodContains(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetStockList: return GetStockList(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::StatementAdd: return StatementAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetGasStation: return GetGasStation(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetDebtImplementation: return GetDebtImplementation(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetDebtCounterparty: return GetDebtCounterparty(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetUserConsumption: return GetUserConsumption(TcpMessage, TcpAnswer); break;
	default:
		return ISTcpWorker::Execute(TcpMessage, TcpAnswer);
	}
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::PeriodContains(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_PERIOD);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.PeriodContains.Select"), qSelect);
	}
	bool IsExist = qSelect.First();
	TcpAnswer->Parameters["Exist"] = IsExist;
	TcpAnswer->Parameters["Value"] = IsExist ? qSelect.ReadColumn("prod_constant") : QVariant();
	return true;
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::GetStockList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_STOCK);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetStockList.Select"), qSelect);
	}

	//Обходим результаты выборки
	QVariantList StockList;
	while (qSelect.Next())
	{
		StockList.append(QVariantMap
		{
			{ "ID", qSelect.ReadColumn("stck_id") },
			{ "Name", qSelect.ReadColumn("stck_name") },
		});
	}
	TcpAnswer->Parameters["List"] = StockList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::StatementAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant ImplementationUnload = CheckNullField("ImplementationUnload", TcpMessage),
		UnloadStock = CheckNullField("UnloadStock", TcpMessage),
		ValumeIncome = CheckNullField("ValumeIncome", TcpMessage);
	if (!ImplementationUnload.isValid() || !UnloadStock.isValid() || !ValumeIncome.isValid())
	{
		return false;
	}

	//Проверяем наличие такой записи
	ISQuery qSelectUnload(ISDatabase::Instance().GetDB(DBConnectionName), QS_STATEMENT);
	qSelectUnload.BindValue(":ImplementationUnload", ImplementationUnload);
	if (!qSelectUnload.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.StatementAdd.Select"), qSelectUnload);
	}

	if (!qSelectUnload.First()) //Не удалось перейти на первую запись
	{
		ErrorString = qSelectUnload.GetErrorString();
		return false;
	}

	//Если выгрузки в ведомостях АЗС нет - добавляем
	if (qSelectUnload.ReadColumn("count").toInt() == 0)
	{
		ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_STATEMENT);
		qInsert.BindValue(":ImplementationUnload", ImplementationUnload);
		qInsert.BindValue(":StockID", UnloadStock);
		qInsert.BindValue(":VolumeIncome", ValumeIncome);
		if (!qInsert.Execute()) //Не удалось добавить
		{
			return ErrorQuery(LANG("Carat.Error.Query.StatementAdd.Insert"), qInsert);
		}
	}

	//Выгрузка уже существует - все окей
	return true;
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::GetGasStation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant StatementID = CheckNullField("StatementID", TcpMessage);
	if (!StatementID.isValid())
	{
		return false;
	}

	//Вытаскиваем значения
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_FILL_IN_BASED);
	qSelect.BindValue(":StatementID", StatementID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetGasStation.Select"), qSelect);
	}

	if (!qSelect.First()) //Такая запись не существует
	{
		ErrorString = LANG("Carat.Error.Query.GetGasStation.NotExist");
		return false;
	}

	TcpAnswer->Parameters["BalanceEndChange"] = qSelect.ReadColumn("gsts_balanceendchange");
	TcpAnswer->Parameters["CashboxTotalPayment"] = qSelect.ReadColumn("gsts_cashboxtotalpayment");
	TcpAnswer->Parameters["CashboxTotalActually"] = qSelect.ReadColumn("gsts_cashboxtotalactually");
	TcpAnswer->Parameters["CashboxKKMTotal"] = qSelect.ReadColumn("gsts_cashboxkkmtotal");
	return true;
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::GetDebtImplementation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant CounterpartyID = CheckNullField("CounterpartyID", TcpMessage);
	if (!CounterpartyID.isValid())
	{
		return false;
	}

	//Вытаскиваем загрузки и выгрузки
	ISQuery qSelectLoadUnload(ISDatabase::Instance().GetDB(DBConnectionName), QS_IMPLEMENTATION_UNLOAD);
	qSelectLoadUnload.BindValue(":CounterpartyID", CounterpartyID);
	if (!qSelectLoadUnload.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetDebtImplementation.Select"), qSelectLoadUnload);
	}

	//Обходим результаты выборки
	QVariantList LoadUnloadList;
	while (qSelectLoadUnload.Next())
	{
		LoadUnloadList.append(QVariantMap
		{
			{ "IsLoad", qSelectLoadUnload.ReadColumn("is_load") },
			{ "ImplementationID", qSelectLoadUnload.ReadColumn("implementation_id") },
			{ "LoadUnloadID", qSelectLoadUnload.ReadColumn("id") },
			{ "Date", qSelectLoadUnload.ReadColumn("date").toDate().toString(FORMAT_DATE_V2) },
			{ "Cost", qSelectLoadUnload.ReadColumn("cost") }
		});
	}
	TcpAnswer->Parameters["LoadUnload"] = LoadUnloadList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::GetDebtCounterparty(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant CounterpartyID = CheckNullField("CounterpartyID", TcpMessage);
	if (!CounterpartyID.isValid())
	{
		return false;
	}

	//Запрашиваем итоговые цифры
	ISQuery qSelectTitle(ISDatabase::Instance().GetDB(DBConnectionName), QS_COUNTERPARTY_DEBT);
	qSelectTitle.BindValue(":CounterpartyID", CounterpartyID);
	if (!qSelectTitle.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetDebtCounterparty.Select"), qSelectTitle);
	}

	if (!qSelectTitle.First()) //Такая запись не существует
	{
		ErrorString = LANG("Carat.Error.Query.GetDebtCounterparty.NotExist");
		return false;
	}
	TcpAnswer->Parameters["TotalUnload"] = qSelectTitle.ReadColumn("get_counterparty_unload").toDouble();
	TcpAnswer->Parameters["TotalLoad"] = qSelectTitle.ReadColumn("get_counterparty_load").toDouble();
	TcpAnswer->Parameters["TotalEntrollment"] = qSelectTitle.ReadColumn("get_counterparty_entrollment").toDouble();
	TcpAnswer->Parameters["MoveWagonSum"] = qSelectTitle.ReadColumn("get_counterparty_move_wagon").toDouble();
	return true;
}
//-----------------------------------------------------------------------------
bool ISOilSphereWorker::GetUserConsumption(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Запрашиваем всех пользователей и их балансы
	ISQuery qSelectUsers(ISDatabase::Instance().GetDB(DBConnectionName), QS_USERS_CONSUMPTION);
	if (!qSelectUsers.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetUserConsumption.SelectUsers"), qSelectUsers);
	}

	//Обходим пользователей
	QVariantList UserList;
	while (qSelectUsers.Next())
	{
		unsigned int UserID = qSelectUsers.ReadColumn("usrs_id").toUInt();

		//Запрашиваем все расходы пользователя
		ISQuery qSelectUserConsumption(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_CONSUMPTION);
		qSelectUserConsumption.BindValue(":UserID", UserID);
		if (!qSelectUserConsumption.Execute()) //Ошибка запроса
		{
			return ErrorQuery(LANG("Carat.Error.Query.GetUserConsumption.SelectUser"), qSelectUserConsumption);
		}

		//Обходим расходы пользователя
		QVariantList ConsumptionList;
		while (qSelectUserConsumption.Next())
		{
			ConsumptionList.push_back(QVariantMap
			{
				{ "Date", ISTcpWorkerHelper::ConvertDateToString(qSelectUserConsumption.ReadColumn("cpmp_date").toDate()) },
				{ "Sum", DOUBLE_PREPARE(qSelectUserConsumption.ReadColumn("cpmp_sum").toDouble()) },
				{ "Note", qSelectUserConsumption.ReadColumn("cpmp_note").toString() }
			});
		}

		UserList.push_back(QVariantMap
		{
			{ "ID", UserID },
			{ "FIO", qSelectUsers.ReadColumn("usrs_fio") },
			{ "Balance", DOUBLE_PREPARE(qSelectUsers.ReadColumn("get_user_balance").toDouble()) },
			{ "ConsumptionList", ConsumptionList }
		});
	}
	TcpAnswer->Parameters["UserList"] = UserList;
	return true;
}
//-----------------------------------------------------------------------------
