#pragma once
#ifndef _ISCONFIGURATIONS_H_INCLUDED
#define _ISCONFIGURATIONS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISConfigurations
{
public:
	static ISConfigurations& Instance();

	const ISConfigurationInfo& Get(); //Получить конфигурацию
	bool Set(const std::string &configuration_name); //Выбрать конфигурацию

private:
	ISConfigurations();
	~ISConfigurations();
	ISConfigurations(const ISConfigurations&) = delete;
	ISConfigurations(ISConfigurations&&) = delete;
	ISConfigurations& operator=(const ISConfigurations&) = delete;
	ISConfigurations& operator=(ISConfigurations&&) = delete;

private:
	std::vector<ISConfigurationInfo> Vector;
    size_t Index; //Индекс текущей конфигурации
};
//-----------------------------------------------------------------------------
#endif
