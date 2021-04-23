#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
//������� �� Msec �����������
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

//���������� �������������� ��������������� ���������
#define IS_UNUSED(x) (void)(x)

#ifdef WIN32
#define __CLASS__ ISAlgorithm::GetClassName(__FUNCTION__)
#else
#define __CLASS__ ISAlgorithm::GetClassName(__PRETTY_FUNCTION__)
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
#define CRITICAL_SECTION_INIT(CRITICAL_SECTION) InitializeCriticalSection(CRITICAL_SECTION)
#define CRITICAL_SECTION_LOCK(CRITICAL_SECTION) EnterCriticalSection(CRITICAL_SECTION)
#define CRITICAL_SECTION_UNLOCK(CRITICAL_SECTION) LeaveCriticalSection(CRITICAL_SECTION)
#define CRITICAL_SECTION_DESTROY(CRITICAL_SECTION) DeleteCriticalSection(CRITICAL_SECTION)
#define CURRENT_THREAD_ID GetCurrentThreadId
#define CURRENT_PID GetCurrentProcessId
#else
#define CRITICAL_SECTION_INIT(CRITICAL_SECTION) pthread_mutex_init(CRITICAL_SECTION, NULL)
#define CRITICAL_SECTION_LOCK(CRITICAL_SECTION) pthread_mutex_lock(CRITICAL_SECTION)
#define CRITICAL_SECTION_UNLOCK(CRITICAL_SECTION) pthread_mutex_unlock(CRITICAL_SECTION)
#define CRITICAL_SECTION_DESTROY(CRITICAL_SECTION) pthread_mutex_destroy(CRITICAL_SECTION)
#define CURRENT_THREAD_ID pthread_self
#define CURRENT_PID getpid
#endif
//-----------------------------------------------------------------------------
namespace ISAlgorithm
{
    //! �������� ��� ������
    //! \return ���������� ��� ������
    std::string GetClassName(const char *FunctionName);

    //! �������� ��������� �����
    //! \return ���������� ��������� �����
    ISTimePoint GetTick();

    //! �������� ������� ��������� �����
    //! \param TickA ��������� �����
    //! \param TickB ��������� �����
    //! \return ���������� ������� ����� ����� ���������� �������
    ISUInt64 GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

    //! �������� ������� ����� ������� � ������� Unixtime
    //! \return ���������� ������� ����� ������� � ������� Unixtime
    ISUInt64 GetCurrentUnixtime();

    //! �������� ����� ��������� ������
    ISErrorNumber GetLastErrorN();

    //! �������� �������� ��������� ������
    std::string GetLastErrorS();

    //! �������� ������������� �����
    //! \param DirPath ���� � �����
    //! \return ���������� true � ������ ������������� �����, ����� - false
    bool DirExist(const std::string &DirPath);

    //! �������� ����� ���������
    //! \param DirPath ���� � �����
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    bool DirCreate(const std::string &DirPath);

    //! �������� ����� ���������
    //! \param DirPath ���� � �����
    //! \param ErrorString ������ �� ������ � �������
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    bool DirCreate(const std::string &DirPath, std::string &ErrorString);

    //! �������� ������ ������ � �����
    //! \param DirPath ���� � �����
    //! \return ���������� ������ ��� ������. ���� ������ ������ - ��������� ������
    std::vector<ISFileInfo> DirFiles(const std::string &DirPath, ISNamespace::DirFileSorting SortType = ISNamespace::DirFileSorting::DoNotSort, ISNamespace::SortingOrder SortOrder = ISNamespace::SortingOrder::Ascending);

    //! �������� ������ ������ � �����
    //! \param DirPath ���� � �����
    //! \param ErrorString ������ �� ������ � �������
    //! \return ���������� ������ ��� ������. ���� ������ ������ - ��������� ������
    std::vector<ISFileInfo> DirFiles(const std::string &DirPath, std::string &ErrorString, ISNamespace::DirFileSorting SortType = ISNamespace::DirFileSorting::DoNotSort, ISNamespace::SortingOrder SortOrder = ISNamespace::SortingOrder::Ascending);

    //! �������� ������������� �����
    //! \param FilePath ���� � �����
    //! \return ���������� true � ������ ������������� �����, ����� - false
    bool FileExist(const std::string &FilePath);

    //! �������� �����
    //! \param FilePath ���� � �����
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    bool FileDelete(const std::string &FilePath);

    //! �������� �����
    //! \param FilePath ���� � �����
    //! \param ErrorString ������ �� ������ � �������
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    bool FileDelete(const std::string &FilePath, std::string &ErrorString);

    //! �������� ���� � ������������ �����
    //! \return ���������� ���� � ������������� �����
    std::string GetApplicationPath();

    //! �������� ���� � ����� ����������
    //! \return ���������� ���� � ����� ����������
    std::string GetApplicationDir();

    //! �������� ��� ����������
    //! \return ���������� ��� ����������
    std::string GetApplicationName();

    //! �������� ��� ����������
    //! \return ���������� ��� ����������
    std::string GetHostName();

    //! �������� ��� ������������
    //! \return ���������� ��� �������� ������������ � ��
    std::string GetUserName();

    //! ������� ���������� ��������� ������
    //! \param argc ���������� ����������
    //! \param argv ������ ����������
    //! \return ���������� ������ ����������
    ISVectorString ParseArgs(int argc, char **argv);

    //! ��������� ������
    //! \param Separator �����������
    //! \return ���������� ������ ���������� �����
    ISVectorString StringSplit(const std::string &String, char Separator);

    //! �������� ������ �� �����
    //! \param String ������
    //! \return ���������� true ���� ������ �������� �����, ����� - false
    bool StringIsNumber(const std::string &String);

    //! ���������� ������ � ������� ��������
    //! \param String ������
    void StringToLower(std::string &String);

    //! ���������� ������ � �������� ��������
    //! \param String ������
    void StringToUpper(std::string &String);

    //! �������� N �������� ������ ������
    //! \param String ������
    //! \param N ���������� ��������
    //! \return ���������� ������
    std::string StringRight(const std::string &String, size_t N);

    //! ������� N �������� � ����� ������
    //! \param String ������
    //! \param N ���������� ��������
    void StringChop(std::string &String, size_t N);

    //! �������������� ������
    //! \param Format ������ ���������
    //! \param ... ���������
    std::string StringF(const char *Format, ...);

    //! ������������ MD5-���� �� ������
    //! \param String ������, �� ������� ����� �������� MD5-���
    //! \return ���������� MD5-���. ���� ��������� ������ ������ - ������ ������
    std::string StringToMD5(const std::string &String);

    //! ������� ��������� ������ � �����
    //! \param String ������ � ������� ����� ����������� ��������
    //! \param Symbol ������ ������� ����� ���������
    void RemoveLastSymbolLoop(std::string &String, char Symbol);

    //! �������������� ������ �����. ��������, ����� 6985473 ������ ��� 6 985 473
    //! \Number ����� �����
    //! \return ���������� ��������������� �����
    std::string FormatNumber(long long Number, char Separator = ' ');

    //! �������������� �������� �����. ��������, ����� 1024,965 ������ ��� 1 024,965
    //! \Number ������� �����
    //! \return ���������� ��������������� �����
    std::string FormatNumber(double Number, char Separator = ' ', unsigned int Precision = 0);

    //! ��������� ������������ ����������� �������������� � ������� b75ed238-411a-4f06-85ea-a2ecca37cfa8
    //! \return ���������� ����������� ���������� �������������
    std::string GenerateUuidStandart();

    //! ��������� ����������� �������������� � ������� {B75ED238-411A-4F06-85EA-A2ECCA37CFA8}
    //! \return ���������� ���������� �������������
    std::string GenerateUuid();

    //! ��������� ����������� ����������� �������������� � ������� b75ed238411a4f0685eaa2ecca37cfa8
    //! \return ���������� ���������� ���������� �������������
    std::string GenerateUuidLite();

    //! ������������� ����
    //! \param Salt ������, � ������� ����� �������� ��������������� ����
    //! \param ErrorString ������, � ������� ����� �������� �������� ������
    //! \return ���������� true � ������ �������� ���������, � ��������� ������ - false
    bool GenerateSalt(std::string &Salt, std::string &ErrorString);

    //! ������� ������
    //! \param HashPassword ������, ������� ������ ��������� ��� ������ � ������ (sha256(Login + Password))
    //! \param Salt ������, ������� ������ ��������� ���� (sha256(Login + Password))
    //! \return ���������� ������ ������
    std::string SaltPassword(const std::string &HashPassword, const std::string &Salt); //�������� ������

    //! ������������ ������ � base64
    //! \param String ������, ������� ����� ������������
    //! \return ���������� �������������� ������. � ������ ������ ������������ ������ ������
    std::string Base64Encode(const std::string &String, std::string &ErrorString);

    //! ������������ ������ � base64
    //! \param Data ������ ������
    //! \param Size ������ ������
    //! \return ���������� �������������� ������. � ������ ������ ������������ ������ ������
    std::string Base64Encode(unsigned char *Data, size_t Size, std::string &ErrorString);

    //! ��������� ����
    //! \param FilePath ���� � �����
    //! \param Mode ����� ������
    //! \param FileSize ������ �����
    //! \param ErrorString ������ �� ������ � ��������� ������
    //! \return ���������� ������ ������ ����� � ������ ���������� ������, ����� ������� nullptr
    unsigned char* ReadFile(const char *FilePath, const char *Mode, long &FileSize);

    //! ��������� ����
    //! \param FilePath ���� � �����
    //! \param Mode ����� ������
    //! \param FileSize ������ �����
    //! \param ErrorString ������ �� ������ � ��������� ������
    //! \return ���������� ������ ������ ����� � ������ ���������� ������, ����� ������� nullptr
    unsigned char* ReadFile(const char *FilePath, const char *Mode, long &FileSize, std::string &ErrorString);

    //������� ������� �� ������� �� ��������� �������
    template <typename T> T VectorTakeAt(std::vector<T> &Vector, size_t Index)
    {
        T Value = Vector[Index];
        Vector.erase(Vector.begin() + Index);
        return Value;
    }

    //������� ������ ������� �� �������
    template <typename T> T VectorTakeFront(std::vector<T> &Vector)
    {
        return VectorTakeAt(Vector, 0);
    }

    //������� ��������� ������� �� �������
    template <typename T> T VectorTakeBack(std::vector<T> &Vector)
    {
        return VectorTakeAt(Vector, Vector.size() - 1);
    }

    //��������� ������� �������� � �������
    template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
    {
        return std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
    }

    //������� ��� �������� ��������������� ��������
    template <typename T> void VectorRemoveAll(std::vector<T> &Vector, T Value)
    {
        for (size_t i = 0, c = Vector.size(); i < c; ++i)
        {
            if (Vector[i] == Value) //���� �������� �������� - ������� ���
            {
                Vector.erase(Vector.begin() + i);
                --i, --c; //������� �������������� ������� ������ � ������ (�.�. ������� ���� �������)
            }
        }
    }

    //�������� ��� ����� �������
    template <typename TKey, typename TValue> std::vector<TKey> ConvertMapToKeys(const std::map<TKey, TValue> &Map)
    {
        std::vector<TKey> Vector(Map.size());
        size_t Index = 0;
        for (const auto &MapItem : Map)
        {
            Vector[Index] = MapItem.first;
            ++Index;
        }
        return Vector;
    }

    //�������� ��� ����� �������
    template <typename TKey, typename TValue> std::vector<TKey> ConvertUnorderedMapToKeys(const std::unordered_map<TKey, TValue> &UnorderedMap)
    {
        std::vector<TKey> Vector(UnorderedMap.size());
        size_t Index = 0;
        for (const auto &MapItem : UnorderedMap)
        {
            Vector[Index] = MapItem.first;
            ++Index;
        }
        return Vector;
    }
}
//-----------------------------------------------------------------------------
#endif
