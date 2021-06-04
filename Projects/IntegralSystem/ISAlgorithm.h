#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISSettingsDatabase.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
#define POINTER_DELETE(POINTER) \
if (POINTER) \
{ \
	delete(POINTER); \
	POINTER = nullptr; \
}

//��������� �������� ��������
#define DOUBLE_PREPAREN(d, n) QString::number(d, 'f', n)
#define DOUBLE_PREPARE(d) DOUBLE_PREPAREN(d, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//��������� ��������� �������� (���-�� ������ ����� ������� ������ ���� 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//������� �� Msec �����������
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

//���������� ������� �� ������� �������
#define PROCESS_EVENTS qApp->processEvents

//������������� ����������� ���������� �������������
#define GENERATE_UUID_STANDART ISAlgorithm::GenerateUuidStandart

//������������� ���������� �������������
#define GENERATE_UUID ISAlgorithm::GenerateUuid

//������������� ���������� ���������� �������������
#define GENERATE_UUID_LITE ISAlgorithm::GenerateUuidLite

//�������� ��� SQL-��������
#define PREPARE_QUERY(x) x

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
	//! ������������ ����� ����� �� ���� � ����
	//! \param FilePath ���� � �����
	//! \return ���������� ��� �����
	std::string GetFileNameFromPath(const std::string &FilePath);

	//! �������������� ������ ����� � ������
	//! \param FileSize ������ ����� � ������
	//! \return ������ ����� �������. � ������ ������ ������� -1
	QString StringFromSize(qint64 FileSize);

	//! �������� ������ ����������
	//! \param PathDir ���� � ����������
	//! \return ������ ����������
	quint64 DirSize(const QString &PathDir, const QStringList &Filter = QStringList());

	//! �������� ��������� �����
	//! \return ���������� ��������� �����
	ISTimePoint GetTick();

	//! �������� ������� ��������� �����
	//! \param TickA ��������� �����
	//! \param TickB ��������� �����
	//! \return ���������� ������� ����� ����� ���������� �������
	unsigned long long GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

	//! �������� ��� ������
	//! \param FunctionName � �������� ����� ��������� ���������� ���������� ������ __FUNCTION__
	//! \return ���������� ��� ������ � ������ ������, ����� ������ ������
    std::string GetClassName(const std::string &FunctionName);

	//! ������� ������ ������ �� ������
	//! \param String ������, �� ������� ����� ����������� �������� �������
	//! \param Char ������, ������� ����� ��������� �� ������
	//! \return ������� ������ �� ����������
	void RemoveBeginSymbolLoop(QString &String, char Char);

	//! ������� ��������� ������ �� ������
	//! \param String ������, �� ������� ����� ����������� �������� �������
	//! \param Char ������, ������� ����� ��������� �� ������
	//! \return ������� ������ �� ����������
	void RemoveLastSymbolLoop(QString &String, char Char);

	//! �������� ��������� ������ �� ������� �������
	//! \return ���������� ������ � �������
	QString GetLastErrorString();

	//! ������������� ����
	//! \param Salt ������, � ������� ����� �������� ��������������� ����
	//! \param ErrorString ������, � ������� ����� �������� �������� ������
	//! \return ���������� true � ������ �������� ���������, � ��������� ������ - false
	bool GenerateSalt(QString &Salt, QString &ErrorString);

	//! ������� ������
	//! \param HashPassword ������, ������� ������ ��������� ��� ������ � ������ (sha256(Login + Password))
	//! \param Salt ������, ������� ������ ��������� ���� (sha256(Login + Password))
	//! \return ���������� ������ ������
	QString SaltPassword(const QString &HashPassword, const QString &Salt); //�������� ������

	//! �������� ������ �� ������������ ����������� ���������
	//! \param Password ������, ���������� ������
	//! \return ���������� true � ������ ���� ������ ������ ��������, � ��������� ������ - false
	bool PasswordVerification(const QString &Password);

	//! �������������� ������� � ���, ������ � �������
	//! \param Seconds �������, ������� ����� ��������������
	//! \param Day ���� ���������� ���-�� ����
	//! \param Hour ���� ���������� ���-�� �����
	//! \param Minute ���� ���������� ���-�� �����
	//! \param Second ���� ���������� ���-�� ������
	//! \return ������� ������ �� ����������
	void ConvertSecondToTime(unsigned int Seconds, unsigned int &Day, unsigned int &Hour, unsigned int &Minute, unsigned int &Second);

	//! ������� ������ ����� ������������
	//! \param FilePath ���� � ����� ������������
	//! \return ���������� ������. ���� �������� ���� - ������ �� ���� ���������
	unsigned int ExtractVersionFile(const QString &FilePath);

	//! ����������� ����� �������� (������� ��� ����� ����)
	//! \param PhoneNumber ������, ������� �������� ����� ��������
	void FormatPhoneNumber(QString &PhoneNumber);

	//! �������������� ������ �����. ��������, ����� 6985473 ������ ��� 6 985 473
	//! \Number ����� �����
	//! \return ���������� ��������������� �����
	QString FormatNumber(long long Number, char Separator = ' ');

	//! �������������� �������� �����. ��������, ����� 1024,965 ������ ��� 1 024,965
	//! \Number ������� �����
	//! \return ���������� ��������������� �����
	QString FormatNumber(double Number, char Separator = ' ', unsigned int Precision = 0);

	//! ��������� ������������ ����������� �������������� � ������� b75ed238-411a-4f06-85ea-a2ecca37cfa8
	//! \return ���������� ����������� ���������� �������������
	std::string GenerateUuidStandart();

	//! ��������� ����������� �������������� � ������� {B75ED238-411A-4F06-85EA-A2ECCA37CFA8}
	//! \return ���������� ���������� �������������
	std::string GenerateUuid();

	//! ��������� ����������� ����������� �������������� � ������� B75ED238411A4F0685EAA2ECCA37CFA8
	//! \return ���������� ���������� ���������� �������������
    std::string GenerateUuidLite();

	//! ��������� ���� SHA256
	//! \param String ������, �� ������� ����� �������� ���
	//! \return ���������� ������ � �����
	std::string StringToSha256(const std::string &String);

	//! ��������� ���� MD5
	//! \param String ������, �� ������� ����� �������� ���
	//! \return ���������� ������ � �����
	std::string StringToMD5(const std::string &String);

	//! ������� ��������� �� ������
	//! \param String ������, �� ������� ����� ����������� ����������
	//! \param Pos �������, � ������� �������� ����������
	//! \param Size ���������� ����������� ��������
	//! \return ���������� ����������� ������
	QString StringTake(QString &String, int Pos, int N);

	//! ������� ���������� ��������� ������
	//! \param argc ���������� ����������
	//! \param argv ������ ����������
	//! \return ���������� ������ ����������
	ISVectorString ParseCommandArgs(int argc, char **argv);

	//����� �������� � �������
    template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
    {
		return Vector.empty() ? false : std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
    }

	//������� �������� �� �������
	template <typename T> void VectorErase(std::vector<T> &Vector, T t)
	{
        auto It = std::find(Vector.begin(), Vector.end(), t);
		if (It != Vector.end())
		{
			Vector.erase(It);
		}
	}

	//������� �������� �� �������
    template <typename T> bool VectorTake(std::vector<T> &Vector, T t)
	{
		auto It = std::find(Vector.begin(), Vector.end(), t);
		bool Result = It != Vector.end();
		if (Result)
		{
			Vector.erase(It);
		}
		return Result;
	}

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

    //�������� ������ �������� � �������
    template <typename T> size_t VectorIndexOf(const std::vector<T> &Vector, T Value)
    {
		auto It = std::find(Vector.begin(), Vector.end(), Value);
        if (It == Vector.end())
        {
            return size_t();
        }
        return std::distance(Vector.begin(), It);
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

	//������� ������ ������� �� �������
	template <typename T> void VectorRemoveFirst(std::vector<T> &Vector)
	{
		Vector.erase(Vector.begin());
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

    //�������� ��� �������� �������
    template <typename TKey, typename TValue> std::vector<TValue> ConvertMapToValues(const std::map<TKey, TValue> &Map)
    {
        std::vector<TValue> Vector(Map.size());
        size_t Index = 0;
        for (const auto &MapItem : Map)
        {
            Vector[Index] = MapItem.second;
            ++Index;
        }
        return Vector;
    }

    //������� ������ �������� �������
    template <typename TKey, typename TValue> TValue MapTakeFront(std::map<TKey, TValue> &Map)
    {
		auto It = Map.begin();
        if (It != Map.end())
        {
            TValue Value = It->second;
            Map.erase(It);
            return Value;
        }
        return TValue();
    }

	//�������� ��������� �� ����� �� ��� �����
	template <typename Class> Class CreatePointer(const QString &ClassName,
		QGenericArgument val0 = QGenericArgument(nullptr),
		QGenericArgument val1 = QGenericArgument(),
		QGenericArgument val2 = QGenericArgument(),
		QGenericArgument val3 = QGenericArgument(),
		QGenericArgument val4 = QGenericArgument(),
		QGenericArgument val5 = QGenericArgument(),
		QGenericArgument val6 = QGenericArgument(),
		QGenericArgument val7 = QGenericArgument(),
		QGenericArgument val8 = QGenericArgument(),
		QGenericArgument val9 = QGenericArgument())
	{
		Class Pointer = nullptr;
        int ObjectType = QMetaType::type((ClassName + '*').toLocal8Bit().constData());
		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		if (ObjectType && MetaObject)
		{
			Pointer = dynamic_cast<Class>(MetaObject->newInstance(val0, val1, val2, val3, val4, val5, val6, val7, val8, val9));
		}

		if (!Pointer)
		{
			IS_ASSERT(Pointer, "Not created pointer with class name \"" + ClassName + "\"");
		}
		return Pointer;
	}
}
//-----------------------------------------------------------------------------
#endif
