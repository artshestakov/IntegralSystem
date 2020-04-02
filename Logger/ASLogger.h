#pragma once
#ifndef _ASLOGGER_H_INCLUDED
#define _ASLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <fstream>
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
		MT_Debug, //Отладка
		MT_Info, //Информация
		MT_Warning, //Предупреждение
		MT_Error //Ошибка
	};

public:
	static ASLogger& Instance(); //Получить ссылку на объект логгера

	std::string GetErrorString() const; //Получить описание ошибки
	bool Initialize(const std::string &Prefix = std::string()); //Инициализировать логгер
	void Shutdown(); //Остановка логгера

	void Log(MessageType message_type, const std::string &message_string, const char *SourceName, int Line);

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
	bool Running; //Флаг работы логгера

	std::ofstream File; //Текущий лог-файл
	std::string PathDirLogs; //Путь к корневой папке с логами
	std::string PathDirCurrent; //Текущий путь к конечной папке с логами
	std::string PathFile; //Путь к текущему лог-файлу

	std::string FilePrefix; //Префикс имени файла
};
//-----------------------------------------------------------------------------
#endif
