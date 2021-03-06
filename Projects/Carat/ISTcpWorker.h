#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISQuery.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
class ISTcpWorker
{
public:
    ISTcpWorker();
    ~ISTcpWorker();

    bool GetBusy(); //�������� "���������" �������
    void SetMessage(ISTcpMessage *TcpMessage); //���������� ��������� �� ���������
    void Start(); //��������� ������
    void Shutdown(); //���������� ������

private:
    void SetRunning(bool is_running);
    bool GetRunning();
    void Process();
    void GetLogParameters(ISTcpMessage *TcpMessage, std::string &String);
    bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool CheckIsNull(ISTcpMessage *TcpMessage, const char *ParameterName, rapidjson::Value &JsonValue);
    bool CheckIsNullString(ISTcpMessage *TcpMessage, const char *ParameterName, std::string &String);
    bool CheckIsNullBool(ISTcpMessage *TcpMessage, const char *ParameterName, bool &Bool);
    bool CheckIsNullUInt(ISTcpMessage *TcpMessage, const char *ParameterName, unsigned int &UInt);
    bool CheckIsNullDouble(ISTcpMessage *TcpMessage, const char *ParameterName, double &Double);
    bool ErrorQuery(const std::string &LocalError, ISQuery &SqlQuery);
    void Protocol(unsigned int UserID, const char *ActionUID, const std::string &TableName = std::string(), const std::string &TableLocalName = std::string(), unsigned int ObjectID = 0, const std::string &Information = std::string());
    unsigned int ExtractVersionFile(const std::string &FileName);
    bool GetObjectName(PMetaTable *MetaTable, unsigned int ObjectID, std::string &ObjectName); //�������� ������������ �������
    PMetaTable* GetMetaTable(const std::string &TableName); //�������� ��������� �� ����-�������
    bool UserPasswordExist(unsigned int UserID, bool &Exist); //�������� ������� ������ � ������������
    bool UserIsSystem(unsigned int UserID, bool &IsSystem); //�������� ������������ �� �����������

private:
    bool Ping(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordGetInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetTableQuery(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetNoteRecord(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool SetNoteRecord(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetCalendarEvents(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool CalendarDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetInternalLists(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetGroupRights(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GroupRightSubSystemAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GroupRightSubSystemDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GroupRightTableAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GroupRightTableDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GroupRightSpecialAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GroupRightSpecialDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetRecordValue(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordFavoriteAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool RecordFavoriteDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetFavoriteNames(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool FavoritesDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool CalendarClose(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetHistoryList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetForeignList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetServerInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OrganizationFromINN(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool BlockedIPAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool BlockedIPDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool BlockedIPGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

    //����������
    bool OilSphere_PeriodContains(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_GetStockList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_StatementAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_GetGasStation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_GetDebtImplementation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_GetDebtCounterparty(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_GetUserConsumption(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_LoadBanks(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool OilSphere_GetUserBalance(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
    std::string ErrorString;
    bool IsBusy;
    bool IsRunning;
    bool IsFinished;
    ISTcpMessage *CurrentMessage;
    PGconn *DBConnection; //��������� �� ����������
    ISQuery *qProtocol;
    ISCriticalSection CriticalSection;
    ISCriticalSection CSRunning;

    //������ TCP-�������
    typedef bool (ISTcpWorker::*TCPFunction)(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    std::unordered_map<std::string, TCPFunction> MapFunction;
};
//-----------------------------------------------------------------------------
#endif
