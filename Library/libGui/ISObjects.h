#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISObjectInterface.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISObjects
{
public:
	ISObjects(const ISObjects &) = delete;
	ISObjects(ISObjects &&) = delete;
	ISObjects &operator=(const ISObjects &) = delete;
	ISObjects &operator=(ISObjects &&) = delete;
	~ISObjects();

	static ISObjects& GetInstance();

	QString GetErrorString() const;
	bool IsInitialized() const;
	bool Initialize(); //Инициализация

	ISObjectInterface* GetInterface(); //Получить указатель на ядро конфигурации
	ISConfigurationItem GetInfo(); //Получить информацию о конфигурации

private:
	ISObjects();

	QString ErrorString;
	QString ConfigurationName;
	ISObjectInterface *ObjectInterface;
	ISConfigurationItem Info;
};
//-----------------------------------------------------------------------------
