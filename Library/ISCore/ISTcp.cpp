#include "ISTcp.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
unsigned int ISTcp::GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok)
{
    int Pos = ByteArray.indexOf(SYMBOL_POINT);
	Ok = Pos != -1;
	if (Ok)
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
		return ISNamespace::AMT_Auth;
	}
	else if (Name == API_SLEEP)
	{
		return ISNamespace::AMT_Sleep;
	}
	else if (Name == API_GET_META_DATA)
	{
		return ISNamespace::AMT_GetMetaData;
	}
	else if (Name == API_GET_LAST_CLIENT)
	{
		return ISNamespace::AMT_GetLastClient;
	}
	else if (Name == API_USER_PASSWORD_EXIST)
	{
		return ISNamespace::AMT_UserPasswordExist;
	}
	else if (Name == API_USER_PASSWORD_CREATE)
	{
		return ISNamespace::AMT_UserPasswordCreate;
	}
	else if (Name == API_USER_PASSWORD_EDIT)
	{
		return ISNamespace::AMT_UserPasswordEdit;
	}
	else if (Name == API_USER_PASSWORD_RESET)
	{
		return ISNamespace::AMT_UserPasswordReset;
	}
	else if (Name == API_USER_SETTINGS_RESET)
	{
		return ISNamespace::AMT_UserSettingsReset;
	}
	else if (Name == API_GET_RECORD_CALL)
	{
		return ISNamespace::AMT_GetRecordCall;
	}
	else if (Name == API_GET_CLIENTS)
	{
		return ISNamespace::AMT_GetClients;
	}
	else if (Name == API_RECORD_DELETE)
	{
		return ISNamespace::AMT_RecordDelete;
	}
	else if (Name == API_DISCUSSION_ADD)
	{
		return ISNamespace::AMT_DiscussionAdd;
	}
	else if (Name == API_DISCUSSION_EDIT)
	{
		return ISNamespace::AMT_DiscussionEdit;
	}
	else if (Name == API_DISCUSSION_COPY)
	{
		return ISNamespace::AMT_DiscussionCopy;
	}
	else if (Name == API_GET_TABLE_DATA)
	{
		return ISNamespace::AMT_GetTableData;
	}
	else if (Name == API_NOTE_RECORD_GET)
	{
		return ISNamespace::AMT_NoteRecordGet;
	}
	else if (Name == API_NOTE_RECORD_SET)
	{
		return ISNamespace::AMT_NoteRecordSet;
	}
	else if (Name == API_FILE_STORAGE_ADD)
	{
		return ISNamespace::AMT_FileStorageAdd;
	}
	else if (Name == API_FILE_STORAGE_COPY)
	{
		return ISNamespace::AMT_FileStorageCopy;
	}
	else if (Name == API_FILE_STORAGE_GET)
	{
		return ISNamespace::AMT_FileStorageGet;
	}
	else if (Name == API_SEARCH_TASK_TEXT)
	{
		return ISNamespace::AMT_SearchTaskText;
	}
	else if (Name == API_SEARCH_TASK_ID)
	{
		return ISNamespace::AMT_SearchTaskID;
	}
	else if (Name == API_SEARCH_FULL_TEXT)
	{
		return ISNamespace::AMT_SearchFullText;
	}
	else if (Name == API_GET_CALENDAR_EVENTS)
	{
		return ISNamespace::AMT_GetCalendarEvents;
	}
	else if (Name == API_CALENDAR_DELETE)
	{
		return ISNamespace::AMT_CalendarDelete;
	}
	else if (Name == API_GET_INTERNAL_LISTS)
	{
		return ISNamespace::AMT_GetInternalLists;
	}
	else if (Name == API_SAVE_META_DATA)
	{
		return ISNamespace::AMT_SaveMetaData;
	}
	else if (Name == API_GET_GROUP_RIGHTS)
	{
		return ISNamespace::AMT_GetGroupRights;
	}
	else if (Name == API_GROUP_RIGHT_SUBSYSTEM_ADD)
	{
		return ISNamespace::AMT_GroupRightSubSystemAdd;
	}
	else if (Name == API_GROUP_RIGHT_SUBSYSTEM_DELETE)
	{
		return ISNamespace::AMT_GroupRightSubSystemDelete;
	}
	else if (Name == API_GROUP_RIGHT_TABLE_ADD)
	{
		return ISNamespace::AMT_GroupRightTableAdd;
	}
	else if (Name == API_GROUP_RIGHT_TABLE_DELETE)
	{
		return ISNamespace::AMT_GroupRightTableDelete;
	}
	else if (Name == API_GROUP_RIGHT_SPECIAL_ADD)
	{
		return ISNamespace::AMT_GroupRightSpecialAdd;
	}
	else if (Name == API_GROUP_RIGHT_SPECIAL_DELETE)
	{
		return ISNamespace::AMT_GroupRightSpecialDelete;
	}
	else if (Name == API_GET_RECORD_VALUE)
	{
		return ISNamespace::AMT_GetRecordValue;
	}
	else if (Name == API_RECORD_FAVORITE_ADD)
	{
		return ISNamespace::AMT_RecordFavoriteAdd;
	}
	return ISNamespace::AMT_Unknown;
}
//-----------------------------------------------------------------------------
