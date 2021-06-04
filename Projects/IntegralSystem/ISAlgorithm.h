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

//Обработка дробного значения
#define DOUBLE_PREPAREN(d, n) QString::number(d, 'f', n)
#define DOUBLE_PREPARE(d) DOUBLE_PREPAREN(d, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//Обработка денежного значения (кол-во знаков после запятой должно быть 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//Заснуть на Msec миллисекунд
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

//Обработать события из очереди событий
#define PROCESS_EVENTS qApp->processEvents

//Сгенерировать стандартный уникальный идентификатор
#define GENERATE_UUID_STANDART ISAlgorithm::GenerateUuidStandart

//Сгенерировать уникальный идентификатор
#define GENERATE_UUID ISAlgorithm::GenerateUuid

//Сгенерировать упрощенный уникальный идентификатор
#define GENERATE_UUID_LITE ISAlgorithm::GenerateUuidLite

//Заглушка для SQL-запросов
#define PREPARE_QUERY(x) x

//Подавление предупреждения неиспользуемого параметра
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
	//! Вытаскивание имени файла из пути к нему
	//! \param FilePath путь к файлу
	//! \return возвращаем имя файла
	std::string GetFileNameFromPath(const std::string &FilePath);

	//! Конвертировать размер файла в строку
	//! \param FileSize размер файла в байтах
	//! \return размер файла строкой. В случае ошибки вернётся -1
	QString StringFromSize(qint64 FileSize);

	//! Получить размер директории
	//! \param PathDir путь к директории
	//! \return размер директории
	quint64 DirSize(const QString &PathDir, const QStringList &Filter = QStringList());

	//! Получить временную метку
	//! \return возвращает временную метку
	ISTimePoint GetTick();

	//! Получить разницу временных меток
	//! \param TickA временная метка
	//! \param TickB временная метка
	//! \return возвращает разницу между двумя временными метками
	unsigned long long GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2);

	//! Получить имя класса
	//! \param FunctionName в качестве этого параметра необходимо передавать макрос __FUNCTION__
	//! \return возвращает имя класса в случае успеха, иначе пустую строку
    std::string GetClassName(const std::string &FunctionName);

	//! Удалить первый символ из строки
	//! \param String строка, из которой будет происходить удаление символа
	//! \param Char символ, который будет удаляться из строки
	//! \return функция ничего не возвращает
	void RemoveBeginSymbolLoop(QString &String, char Char);

	//! Удалить последний символ из строки
	//! \param String строка, из которой будет происходить удаление символа
	//! \param Char символ, который будет удаляться из строки
	//! \return функция ничего не возвращает
	void RemoveLastSymbolLoop(QString &String, char Char);

	//! Получить последнюю ошибку на текущей системе
	//! \return возвращает строку с ошибкой
	QString GetLastErrorString();

	//! Сгенерировать соль
	//! \param Salt строка, в которую будет помещена сгенерированная соль
	//! \param ErrorString строка, в которую будет помещено описание ошибки
	//! \return возвращает true в случае успешное генерации, в противном случае - false
	bool GenerateSalt(QString &Salt, QString &ErrorString);

	//! Соление пароля
	//! \param HashPassword строка, которая должна содержать хэш логина и пароля (sha256(Login + Password))
	//! \param Salt строка, которая должна содержать соль (sha256(Login + Password))
	//! \return возвращает солёный пароль
	QString SaltPassword(const QString &HashPassword, const QString &Salt); //Посолить пароль

	//! Проверка пароля на соответствие требованиям сложности
	//! \param Password строка, содержащая пароль
	//! \return возвращает true в случае если пароль прошёл проверку, в противном случае - false
	bool PasswordVerification(const QString &Password);

	//! Конвертировать секунды в дни, минуты и секунды
	//! \param Seconds секунды, которые нужно конвертировать
	//! \param Day сюда помещается кол-во дней
	//! \param Hour сюда помещается кол-во часов
	//! \param Minute сюда помещается кол-во минут
	//! \param Second сюда помещается кол-во секунд
	//! \return функция ничего не возвращает
	void ConvertSecondToTime(unsigned int Seconds, unsigned int &Day, unsigned int &Hour, unsigned int &Minute, unsigned int &Second);

	//! Извлечь версию файла дистрибутива
	//! \param FilePath путь к файлу дистрибутива
	//! \return возвращает версию. Если вернулся ноль - версия не была извлечена
	unsigned int ExtractVersionFile(const QString &FilePath);

	//! Подготовить номер телефона (удалить все кроме цифр)
	//! \param PhoneNumber строка, которая содержит номер телефона
	void FormatPhoneNumber(QString &PhoneNumber);

	//! Форматирование целого числа. Например, число 6985473 примет вид 6 985 473
	//! \Number целое число
	//! \return возвращает форматированное число
	QString FormatNumber(long long Number, char Separator = ' ');

	//! Форматирование дробного числа. Например, число 1024,965 примет вид 1 024,965
	//! \Number дробное число
	//! \return возвращает форматированное число
	QString FormatNumber(double Number, char Separator = ' ', unsigned int Precision = 0);

	//! Генерация стандартного уникального идентификатора в формате b75ed238-411a-4f06-85ea-a2ecca37cfa8
	//! \return возвращает стандартный уникальный идентификатор
	std::string GenerateUuidStandart();

	//! Генерация уникального идентификатора в формате {B75ED238-411A-4F06-85EA-A2ECCA37CFA8}
	//! \return возвращает уникальный идентификатор
	std::string GenerateUuid();

	//! Генерация упрощённого уникального идентификатора в формате B75ED238411A4F0685EAA2ECCA37CFA8
	//! \return возвращает упрощённый уникальный идентификатор
    std::string GenerateUuidLite();

	//! Генерация хеша SHA256
	//! \param String строка, из которой будет извлечен хеш
	//! \return возвращает строку с хешем
	std::string StringToSha256(const std::string &String);

	//! Генерация хеша MD5
	//! \param String строка, из которой будет извлечен хеш
	//! \return возвращает строку с хешем
	std::string StringToMD5(const std::string &String);

	//! Извлечь подстроку из строки
	//! \param String строка, из которой будет происходить извлечение
	//! \param Pos позиция, с которой начнется извлечение
	//! \param Size количество извлекаемых символов
	//! \return возвращает извлеченную строку
	QString StringTake(QString &String, int Pos, int N);

	//! Парсинг аргументов командной строки
	//! \param argc количество аргументов
	//! \param argv массив аргументов
	//! \return возвращает вектор аргументов
	ISVectorString ParseCommandArgs(int argc, char **argv);

	//Поиск значения в векторе
    template <typename T> bool VectorContains(const std::vector<T> &Vector, T Value)
    {
		return Vector.empty() ? false : std::find(Vector.begin(), Vector.end(), Value) != Vector.end();
    }

	//Удалить значение из вектора
	template <typename T> void VectorErase(std::vector<T> &Vector, T t)
	{
        auto It = std::find(Vector.begin(), Vector.end(), t);
		if (It != Vector.end())
		{
			Vector.erase(It);
		}
	}

	//Извлечь значение из вектора
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

    //Извлечь элемент из вектора по заданному индексу
    template <typename T> T VectorTakeAt(std::vector<T> &Vector, size_t Index)
    {
        T Value = Vector[Index];
        Vector.erase(Vector.begin() + Index);
        return Value;
    }

    //Извлечь первый элемент из вектора
    template <typename T> T VectorTakeFront(std::vector<T> &Vector)
    {
        return VectorTakeAt(Vector, 0);
    }

    //Извлечь последний элемент из вектора
    template <typename T> T VectorTakeBack(std::vector<T> &Vector)
    {
        return VectorTakeAt(Vector, Vector.size() - 1);
    }

    //Получить индекс элемента в векторе
    template <typename T> size_t VectorIndexOf(const std::vector<T> &Vector, T Value)
    {
		auto It = std::find(Vector.begin(), Vector.end(), Value);
        if (It == Vector.end())
        {
            return size_t();
        }
        return std::distance(Vector.begin(), It);
    }

    //Удалить все элементы соответствующие значению
    template <typename T> void VectorRemoveAll(std::vector<T> &Vector, T Value)
    {
        for (size_t i = 0, c = Vector.size(); i < c; ++i)
        {
            if (Vector[i] == Value) //Если попалось значение - удаляем его
            {
                Vector.erase(Vector.begin() + i);
                --i, --c; //Обратно инкрементируем текущий индекс и размер (т.к. удалили один элемент)
            }
        }
    }

	//Удалить первый элемент из вектора
	template <typename T> void VectorRemoveFirst(std::vector<T> &Vector)
	{
		Vector.erase(Vector.begin());
	}

    //Получить все ключи словаря
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

    //Получить все значения словаря
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

    //Извлечь первое значение словаря
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

	//Получить указатель на класс по его имени
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
