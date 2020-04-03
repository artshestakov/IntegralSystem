#pragma once
#ifndef _ASLOGGER_H_INCLUDED
#define _ASLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <fstream>
#include <sstream>
#include <array>
#include <thread>
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
	static ASLogger& Instance(); //Получить ссылку на объект логгера

	std::string GetErrorString() const; //Получить описание ошибки
	bool Initialize(const std::string &prefix = std::string()); //Инициализировать логгер

	void Log(MessageType type_message, const std::string &string_message, const char *source_name, int source_line);

private:
	bool CreateLogDirectory(); //Создание директории
	void Worker(); //Обработчик очереди сообщений

private:
	bool CheckExistDir(const std::string &PathDir); //Проверка существования директории

private:
	ASLogger();
	~ASLogger();
	ASLogger(ASLogger const &) {};
	ASLogger& operator=(ASLogger const&) { return *this; };

private:
	std::string ErrorString; //Описание ошибки
	std::mutex Mutex; //Мьютекс для массива
	std::array<std::string, ARRAY_SIZE> Array; //Массив сообщений
	size_t LastIndex;
	bool IsRunning; //Флаг работы
	bool IsFinished; //Флаг остановки

	std::ofstream File; //Текущий лог-файл
	std::string PathDirLogs; //Путь к корневой папке с логами
	std::string PathDirCurrent; //Текущий путь к конечной папке с логами
	std::string PathFile; //Путь к текущему лог-файлу

	std::string FilePrefix; //Префикс имени файла
};
//-----------------------------------------------------------------------------
#define ASLOGGER_L(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Lite, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_D(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Debug, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_I(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Info, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_W(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Warning, MESSAGE, __FILE__, __LINE__)
#define ASLOGGER_E(MESSAGE) ASLogger::Instance().Log(ASLogger::MT_Error, MESSAGE, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
