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
	ISConfigurations(ISConfigurations const &) {};
	ISConfigurations& operator=(ISConfigurations const&) { return *this; };

private:
	std::vector<ISConfigurationInfo> Vector;
	ISConfigurationInfo Current; //Текущая конфигурация
};
//-----------------------------------------------------------------------------
#endif
