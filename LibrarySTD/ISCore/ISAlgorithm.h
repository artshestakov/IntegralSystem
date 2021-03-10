#pragma once
#ifndef _ISALGORITHM_H_INCLUDED
#define _ISALGORITHM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
//Заснуть на Msec миллисекунд
#define ISSleep(MSec) std::this_thread::sleep_for(std::chrono::milliseconds(MSec))

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
	//! Получить имя класса
	//! \param FunctionName в качестве этого параметра необходимо передавать макрос
	//! \return возвращает имя класса в случае успеха, иначе пустую строку
	ISCORE_EXPORT std::string GetClassName(const std::string &FunctionName);

	//! Получить описание последней ошибки
	ISCORE_EXPORT std::string GetLastErrorS();

	//! Установить кодовую страницу
	//! \param CodePage номер кодовой страницы
	//! \param ErrorString ссылка на строку с ошибкой
	//! \return возвращает true в случае успешной установки, иначе - false
	ISCORE_EXPORT bool ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString);

	//! Проверка существования папки
	//! \param DirPath путь к папке
	//! \return возвращает true в случае существования папки, иначе - false
	ISCORE_EXPORT bool DirExist(const std::string &DirPath);

	//! Создание папки рекусивно
	//! \param DirPath путь к папке
	//! \return возвращает true в случае успешного создания папки, иначе - false
	ISCORE_EXPORT bool DirCreate(const std::string &DirPath);

	//! Создание папки рекусивно
	//! \param DirPath путь к папке
	//! \param ErrorString ссылка на строку с ошибкой
	//! \return возвращает true в случае успешного создания папки, иначе - false
	ISCORE_EXPORT bool DirCreate(const std::string &DirPath, std::string &ErrorString);

	//! Проверка существования файла
	//! \param FilePath путь к файлу
	//! \return возвращает true в случае существования файла, иначе - false
	ISCORE_EXPORT bool FileExist(const std::string &FilePath);

	//! Удаление файла
	//! \param FilePath путь к файлу
	//! \return возвращает true в сулчае успешного удаления файла, иначе - false
	ISCORE_EXPORT bool FileDelete(const std::string &FilePath);

	//! Удаление файла
	//! \param FilePath путь к файлу
	//! \param ErrorString ссылка на строку с ошибкой
	//! \return возвращает true в сулчае успешного удаления файла, иначе - false
	ISCORE_EXPORT bool FileDelete(const std::string &FilePath, std::string &ErrorString);

	//! Получить путь к исполняемому файлу
	//! \return возвращает путь к испролняемому файлу
	ISCORE_EXPORT std::string GetApplicationPath();

	//! Получить путь к папке приложения
	//! \return возвращает путь к папке приложения
	ISCORE_EXPORT std::string GetApplicationDir();

	//! Получить имя приложения
	//! \return возвращает имя приложения
	ISCORE_EXPORT std::string GetApplicationName();

	//! Парсинг аргументов командной строки
	//! \param argc количество аргументов
	//! \param argv массив аргументов
	//! \return возвращает вектор аргументов
	ISCORE_EXPORT ISVectorString ParseArgs(int argc, char **argv);

	//! Получить текущую дату и время
	//! \return возвращает структуру с текущей датой и временем
	ISCORE_EXPORT ISDateTime GetCurrentDate();

    //! Разделить строку
    //! \param Separator разделитель
    //! \return возвращает вектор разделённых строк
    ISCORE_EXPORT ISVectorString StringSplit(const std::string &String, char Separator);
}
//-----------------------------------------------------------------------------
#endif
