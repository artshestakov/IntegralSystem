#pragma once
#ifndef _ASLOGGER_H_INCLUDED
#define _ASLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <thread>
#include <windows.h>
#include <minwindef.h>
#include <Shlwapi.h>
#include <ShlObj.h>
//-----------------------------------------------------------------------------
#define ARRAY_SIZE 20000
//-----------------------------------------------------------------------------
class ASLogger
{
public:
	enum MessageType //Типы сообщений
	{
		MT_Lite, //Упрощенное
		MT_Debug, //Отладка
		MT_Info, //Информация
		MT_Warning, //Предупреждение
		MT_Error //Ошибка
	};

public:

	//Получить ссылку на объект логгера
	static ASLogger& Instance();

	//Получить описание ошибки
	std::string GetErrorString() const;

	//Инициализировать логгер
	bool Initialize(const std::string &prefix = std::string());

	//Остановка логгера
	void Shutdown();

	//Добавить сообщение в лог-файл
	void Log(MessageType type_message, const std::string &string_message, const char *source_name, int source_line);

private:

	//Создать директорию
	bool CreateLogDirectory(const SYSTEMTIME &st);

	//Получить путь к текущему файлу
	std::string GetPathFile(const SYSTEMTIME &st) const;

	//Обработчик очереди сообщений
	void Worker();

private:
	ASLogger();
	~ASLogger();
	ASLogger(ASLogger const &) {};
	ASLogger& operator=(ASLogger const&) { return *this; };

private:
	std::string ErrorString; //Описание ошибки
	CRITICAL_SECTION CriticalSection; //Ктирическая секция для синхронизации
	std::array<std::string, ARRAY_SIZE> Array; //Массив сообщений (очередь)
	size_t LastIndex; //Последняя позиция в очереди
	bool IsRunning; //Флаг работы
	bool IsFinished; //Флаг остановки

	std::ofstream File; //Текущий лог-файл
	std::string PathCurrentDirectory; //Путь к текущей папке приложения
	std::string PathDirCurrent; //Текущий путь к конечной папке с логами
	//std::string PathFile; //Путь к текущему лог-файлу
	std::string FilePrefix; //Префикс имени файла

	//Текущая дата
	size_t CurrentDay, CurrentMonth, CurrentYear;
};
//-----------------------------------------------------------------------------
#define ASLOGGER_L(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Lite, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_D(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Debug, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_I(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Info, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_W(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Warning, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_E(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Error, MESSAGE, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
