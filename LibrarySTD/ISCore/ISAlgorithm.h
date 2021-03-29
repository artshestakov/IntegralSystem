#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
//������� �� Msec �����������
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

//���������� �������������� ��������������� ���������
#define IS_UNUSED(x) (void)x

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
    ISCORE_EXPORT std::string GetClassName(char *FunctionName);

    //! �������� ��������� �����
    //! \return ���������� ��������� �����
    ISCORE_EXPORT ISTimePoint GetTick();

    //! �������� ������� ��������� �����
    //! \param TickA ��������� �����
    //! \param TickB ��������� �����
    //! \return ���������� ������� ����� ����� ���������� �������
    ISCORE_EXPORT unsigned long long GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

    //! �������� �������� ��������� ������
    ISCORE_EXPORT std::string GetLastErrorS();

    //! ���������� ������� ��������
    //! \param CodePage ����� ������� ��������
    //! \param ErrorString ������ �� ������ � �������
    //! \return ���������� true � ������ �������� ���������, ����� - false
    ISCORE_EXPORT bool ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString);

    //! �������� ������������� �����
    //! \param DirPath ���� � �����
    //! \return ���������� true � ������ ������������� �����, ����� - false
    ISCORE_EXPORT bool DirExist(const std::string &DirPath);

    //! �������� ����� ���������
    //! \param DirPath ���� � �����
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    ISCORE_EXPORT bool DirCreate(const std::string &DirPath);

    //! �������� ����� ���������
    //! \param DirPath ���� � �����
    //! \param ErrorString ������ �� ������ � �������
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    ISCORE_EXPORT bool DirCreate(const std::string &DirPath, std::string &ErrorString);

    //! �������� ������������� �����
    //! \param FilePath ���� � �����
    //! \return ���������� true � ������ ������������� �����, ����� - false
    ISCORE_EXPORT bool FileExist(const std::string &FilePath);

    //! �������� �����
    //! \param FilePath ���� � �����
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    ISCORE_EXPORT bool FileDelete(const std::string &FilePath);

    //! �������� �����
    //! \param FilePath ���� � �����
    //! \param ErrorString ������ �� ������ � �������
    //! \return ���������� true � ������ ��������� �������� �����, ����� - false
    ISCORE_EXPORT bool FileDelete(const std::string &FilePath, std::string &ErrorString);

    //! �������� ���� � ������������ �����
    //! \return ���������� ���� � ������������� �����
    ISCORE_EXPORT std::string GetApplicationPath();

    //! �������� ���� � ����� ����������
    //! \return ���������� ���� � ����� ����������
    ISCORE_EXPORT std::string GetApplicationDir();

    //! �������� ��� ����������
    //! \return ���������� ��� ����������
    ISCORE_EXPORT std::string GetApplicationName();

    //! �������� ��� ����������
    //! \return ���������� ��� ����������
    ISCORE_EXPORT std::string GetHostName();

    //! �������� ��� ������������
    //! \return ���������� ��� �������� ������������ � ��
    ISCORE_EXPORT std::string GetUserName();

    //! ������� ���������� ��������� ������
    //! \param argc ���������� ����������
    //! \param argv ������ ����������
    //! \return ���������� ������ ����������
    ISCORE_EXPORT ISVectorString ParseArgs(int argc, char **argv);

    //! �������� ������� ���� � �����
    //! \return ���������� ��������� � ������� ����� � ��������
    ISCORE_EXPORT ISDateTime GetCurrentDate();

    //! ��������� ������
    //! \param Separator �����������
    //! \return ���������� ������ ���������� �����
    ISCORE_EXPORT ISVectorString StringSplit(const std::string &String, char Separator);

    //! �������� ������ �� �����
    //! \param String ������
    //! \return ���������� true ���� ������ �������� �����, ����� - false
    ISCORE_EXPORT bool StringIsNumber(const std::string &String);

    //! ���������� ������ � ������� ��������
    //! \param String ������
    ISCORE_EXPORT void StringToLower(std::string &String);

    //! ���������� ������ � �������� ��������
    //! \param String ������
    ISCORE_EXPORT void StringToUpper(std::string &String);

    //! �������������� ������
    //! \param
    ISCORE_EXPORT std::string StringF(const char *Format, ...);

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
}
//-----------------------------------------------------------------------------
#endif
