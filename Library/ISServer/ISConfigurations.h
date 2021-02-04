#pragma once
#ifndef _ISCONFIGURATION_H_INCLUDED
#define _ISCONFIGURATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISConfigurations
{
public:
	static ISConfigurations& Instance();

	ISConfigurationInfo& Get(); //Получить конфигурацию
	bool Set(const QString &configuration_name); //Выбрать конфигурацию

private:
	ISConfigurations();
	~ISConfigurations();
	ISConfigurations(const ISConfigurations&) = delete;
	ISConfigurations(ISConfigurations&&) = delete;
	ISConfigurations& operator=(const ISConfigurations&) = delete;
	ISConfigurations& operator=(ISConfigurations&&) = delete;

private:
	std::vector<ISConfigurationInfo> Vector;
	ISConfigurationInfo Current; //Текущая конфигурация
};
//-----------------------------------------------------------------------------
#endif
