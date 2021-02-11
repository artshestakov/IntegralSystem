#include "ISTcp.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
unsigned int ISTcp::GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok)
{
    int Pos = ByteArray.indexOf(SYMBOL_POINT);
	Ok = Pos != -1;
	if (Ok) //Нашли точку
	{
		QByteArray ByteArraySize = ByteArray.mid(0, Pos);
		unsigned int Result = ByteArraySize.toUInt(&Ok);
		if (Ok)
		{
			ByteArray.remove(0, Pos + 1);
			return Result;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
void ISTcp::WaitForBytesWritten(QTcpSocket *TcpSocket)
{
	while (TcpSocket->bytesToWrite() > 0)
	{
		PROCESS_EVENTS();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
void ISTcp::WaitForDisconnected(QTcpSocket *TcpSocket)
{
	while (TcpSocket->state() == QAbstractSocket::ConnectedState)
	{
		PROCESS_EVENTS();
		ISSleep(1);
	}
}
//-----------------------------------------------------------------------------
ISNamespace::ApiMessageType ISTcp::GetMessageTypeByName(const QString &Name)
{
	if (Name == API_AUTH)
	{
		return ISNamespace::ApiMessageType::Auth;
	}
	else if (Name == API_SLEEP)
	{
		return ISNamespace::ApiMessageType::Sleep;
	}
	else if (Name == API_GET_META_DATA)
	{
		return ISNamespace::ApiMessageType::GetMetaData;
	}
	else if (Name == API_GET_LAST_CLIENT)
	{
		return ISNamespace::ApiMessageType::GetLastClient;
	}
	else if (Name == API_USER_PASSWORD_EXIST)
	{
		return ISNamespace::ApiMessageType::UserPasswordExist;
	}
	else if (Name == API_USER_PASSWORD_CREATE)
	{
		return ISNamespace::ApiMessageType::UserPasswordCreate;
	}
	else if (Name == API_USER_PASSWORD_EDIT)
	{
		return ISNamespace::ApiMessageType::UserPasswordEdit;
	}
	else if (Name == API_USER_PASSWORD_RESET)
	{
		return ISNamespace::ApiMessageType::UserPasswordReset;
	}
	else if (Name == API_USER_SETTINGS_RESET)
	{
		return ISNamespace::ApiMessageType::UserSettingsReset;
	}
	else if (Name == API_USER_DEVICE_ADD)
	{
		return ISNamespace::ApiMessageType::UserDeviceAdd;
	}
	else if (Name == API_USER_DEVICE_DELETE)
	{
		return ISNamespace::ApiMessageType::UserDeviceDelete;
	}
	else if (Name == API_GET_RECORD_CALL)
	{
		return ISNamespace::ApiMessageType::GetRecordCall;
	}
	else if (Name == API_GET_CLIENTS)
	{
		return ISNamespace::ApiMessageType::GetClients;
	}
	else if (Name == API_RECORD_ADD)
	{
		return ISNamespace::ApiMessageType::RecordAdd;
	}
	else if (Name == API_RECORD_EDIT)
	{
		return ISNamespace::ApiMessageType::RecordEdit;
	}
	else if (Name == API_RECORD_DELETE)
	{
		return ISNamespace::ApiMessageType::RecordDelete;
	}
	else if (Name == API_RECORD_GET)
	{
		return ISNamespace::ApiMessageType::RecordGet;
	}
	else if (Name == API_RECORD_GET_INFO)
	{
		return ISNamespace::ApiMessageType::RecordGetInfo;
	}
	else if (Name == API_DISCUSSION_ADD)
	{
		return ISNamespace::ApiMessageType::DiscussionAdd;
	}
	else if (Name == API_DISCUSSION_EDIT)
	{
		return ISNamespace::ApiMessageType::DiscussionEdit;
	}
	else if (Name == API_DISCUSSION_COPY)
	{
		return ISNamespace::ApiMessageType::DiscussionCopy;
	}
	else if (Name == API_GET_TABLE_DATA)
	{
		return ISNamespace::ApiMessageType::GetTableData;
	}
	else if (Name == API_GET_TABLE_QUERY)
	{
		return ISNamespace::ApiMessageType::GetTableQuery;
	}
	else if (Name == API_NOTE_RECORD_GET)
	{
		return ISNamespace::ApiMessageType::NoteRecordGet;
	}
	else if (Name == API_NOTE_RECORD_SET)
	{
		return ISNamespace::ApiMessageType::NoteRecordSet;
	}
	else if (Name == API_FILE_STORAGE_ADD)
	{
		return ISNamespace::ApiMessageType::FileStorageAdd;
	}
	else if (Name == API_FILE_STORAGE_COPY)
	{
		return ISNamespace::ApiMessageType::FileStorageCopy;
	}
	else if (Name == API_FILE_STORAGE_GET)
	{
		return ISNamespace::ApiMessageType::FileStorageGet;
	}
	else if (Name == API_SEARCH_TASK_TEXT)
	{
		return ISNamespace::ApiMessageType::SearchTaskText;
	}
	else if (Name == API_SEARCH_TASK_ID)
	{
		return ISNamespace::ApiMessageType::SearchTaskID;
	}
	else if (Name == API_SEARCH_FULL_TEXT)
	{
		return ISNamespace::ApiMessageType::SearchFullText;
	}
	else if (Name == API_GET_CALENDAR_EVENTS)
	{
		return ISNamespace::ApiMessageType::GetCalendarEvents;
	}
	else if (Name == API_CALENDAR_DELETE)
	{
		return ISNamespace::ApiMessageType::CalendarDelete;
	}
	else if (Name == API_GET_INTERNAL_LISTS)
	{
		return ISNamespace::ApiMessageType::GetInternalLists;
	}
	else if (Name == API_SAVE_META_DATA)
	{
		return ISNamespace::ApiMessageType::SaveMetaData;
	}
	else if (Name == API_GET_GROUP_RIGHTS)
	{
		return ISNamespace::ApiMessageType::GetGroupRights;
	}
	else if (Name == API_GROUP_RIGHT_SUBSYSTEM_ADD)
	{
		return ISNamespace::ApiMessageType::GroupRightSubSystemAdd;
	}
	else if (Name == API_GROUP_RIGHT_SUBSYSTEM_DELETE)
	{
		return ISNamespace::ApiMessageType::GroupRightSubSystemDelete;
	}
	else if (Name == API_GROUP_RIGHT_TABLE_ADD)
	{
		return ISNamespace::ApiMessageType::GroupRightTableAdd;
	}
	else if (Name == API_GROUP_RIGHT_TABLE_DELETE)
	{
		return ISNamespace::ApiMessageType::GroupRightTableDelete;
	}
	else if (Name == API_GROUP_RIGHT_SPECIAL_ADD)
	{
		return ISNamespace::ApiMessageType::GroupRightSpecialAdd;
	}
	else if (Name == API_GROUP_RIGHT_SPECIAL_DELETE)
	{
		return ISNamespace::ApiMessageType::GroupRightSpecialDelete;
	}
	else if (Name == API_GET_RECORD_VALUE)
	{
		return ISNamespace::ApiMessageType::GetRecordValue;
	}
	else if (Name == API_RECORD_FAVORITE_ADD)
	{
		return ISNamespace::ApiMessageType::RecordFavoriteAdd;
	}
	else if (Name == API_RECORD_FAVORITE_DELETE)
	{
		return ISNamespace::ApiMessageType::RecordFavoriteDelete;
	}
	else if (Name == API_GET_FAVORITE_NAMES)
	{
		return ISNamespace::ApiMessageType::GetFavoriteNames;
	}
	else if (Name == API_FAVORITES_DELETE)
	{
		return ISNamespace::ApiMessageType::FavoritesDelete;
	}
	else if (Name == API_LOG_GET_STRUCTURE)
	{
		return ISNamespace::ApiMessageType::LogGetStructure;
	}
	else if (Name == API_LOG_GET_CONTENT)
	{
		return ISNamespace::ApiMessageType::LogGetContent;
	}
	else if (Name == API_CALENDAR_CLOSE)
	{
		return ISNamespace::ApiMessageType::CalendarClose;
	}
	else if (Name == API_GET_HISTORY_LIST)
	{
		return ISNamespace::ApiMessageType::GetHistoryList;
	}
	else if (Name == API_TASK_COMMENT_ADD)
	{
		return ISNamespace::ApiMessageType::TaskCommentAdd;
	}
	else if (Name == API_GET_FOREIGN_LIST)
	{
		return ISNamespace::ApiMessageType::GetForeignList;
	}
	else if (Name == API_GET_SERVER_INFO)
	{
		return ISNamespace::ApiMessageType::GetServerInfo;
	}
	else if (Name == API_ORGANIZATION_FROM_INN)
	{
		return ISNamespace::ApiMessageType::OrganizationFromINN;
	}
	else if (Name == API_STATEMENTS_QUERY_GET)
	{
		return ISNamespace::ApiMessageType::StatementsQueryGet;
	}
	else if (Name == API_STATEMENTS_QUERY_RESET)
	{
		return ISNamespace::ApiMessageType::StatementsQueryReset;
	}
	else if (Name == API_PERIOD_CONTAINS)
	{
		return ISNamespace::ApiMessageType::PeriodContains;
	}
	else if (Name == API_GET_STOCK_LIST)
	{
		return ISNamespace::ApiMessageType::GetStockList;
	}
	else if (Name == API_STATEMENT_ADD)
	{
		return ISNamespace::ApiMessageType::StatementAdd;
	}
	else if (Name == API_GET_GAST_STATION)
	{
		return ISNamespace::ApiMessageType::GetGasStation;
	}
	else if (Name == API_GET_DEBT_IMPLEMENTATION)
	{
		return ISNamespace::ApiMessageType::GetDebtImplementation;
	}
	else if (Name == API_GET_DEBT_COUNTERPARTY)
	{
		return ISNamespace::ApiMessageType::GetDebtCounterparty;
	}
	else if (Name == API_GET_USERS_CONSUMPTION)
	{
		return ISNamespace::ApiMessageType::GetUserConsumption;
	}
	return ISNamespace::ApiMessageType::Unknown;
}
//-----------------------------------------------------------------------------
