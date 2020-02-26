//-----------------------------------------------------------------------------
//Класс для логирования сообщений.
//
//Для использования выполните следующие действия:
//1. Инициализация класса с помощью вызова метода GLLogger::Instance().Initialize()
//   Обязательно удостоверьтесь что метод вернул true, в случае возникновения ошибки
//   инициализации метод вернет false. Получить описание ошибки можно с помощью метода GLLogger::Instance().GetErrorString()
//2. Добавить строку в лог-файл Вы можете вызвав метод GLLogger::Instance().Log()
//
//Пример использования:
// if (GLLogger::Instance().Initialize())
// {
//		std::cout << "OK" << std::endl;
// }
// else
// {
//		std::cerr << GLLogger::Instance().GetErrorString();
// }
//
//Для удобства вызовы обернуты в defines: LG_INFO(const std::string &), LG_DEBUG(const std::string &),
//LG_WARNING(const std::string &) и LG_ERROR(const std::string &)
//-----------------------------------------------------------------------------
#pragma once
#ifndef _ISLOGGER_H_INCLUDED
#define _ISLOGGER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <array>
#include <mutex>
#include <fstream>
//-----------------------------------------------------------------------------
#define LOGGER_TIMEOUT 2000
#define ARRAY_MAX_SIZE 10000000
//-----------------------------------------------------------------------------
class ISLogger
{
public:
	enum MessageType //Тип сообщения
	{
		MT_Info,
		MT_Debug,
		MT_Warning,
		MT_Error
	};

public:
	static ISLogger& Instance(); //Получить ссылку на объект логгера

	std::string GetErrorString() const; //Получить описание ошибки
	bool Initialize(); //Инициализировать логгер

	void Log(MessageType Type, const std::string &String, const char *SourceName, int Line); //Добавить сообщение в лог

private:
	void Worker(); //Обработчик очереди сообщений
	bool CreateDir(); //Создание необходимых директорий
	void UpdateFilePath(); //Обновить путь к файлу

private:
	ISLogger();
	~ISLogger();
	ISLogger(ISLogger const &) {};
	ISLogger& operator=(ISLogger const&) { return *this; };

private:
	std::string ErrorString; //Описание ошибки
	std::mutex Mutex; //Мьютекс
	std::array<std::string, ARRAY_MAX_SIZE> Array; //Массив сообщений
	size_t LastPosition; //Посденяя позиция
	bool Running; //Флаг работы логгера
	std::string PathDirectory; //Путь к папке с исполняемым файлом приложения
	std::string PathLogs; //Путь к папке с логами
	std::string PathFile; //Путь к текущему лог-файлу
	std::ofstream File; //Текущий лог-файл
	size_t CurrentYear; //Текущий год
	size_t CurrentMonth; //Текущий месяц
	size_t CurrentDay; //Текущий день
};
//-----------------------------------------------------------------------------
#define LG_INFO(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Info, MESSAGE, __FILE__, __LINE__) //Логирование информационного сообщения
#define LG_DEBUG(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Debug, MESSAGE, __FILE__, __LINE__) //Логирование отладочного сообщения
#define LG_WARNING(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Warning, MESSAGE, __FILE__, __LINE__) //Логирование предупреждения
#define LG_ERROR(MESSAGE) GLLogger::Instance().Log(GLLogger::MT_Error, MESSAGE, __FILE__, __LINE__) //Логирование ошибки
//-----------------------------------------------------------------------------
#endif
