#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISObjects : public QObject
{
	Q_OBJECT

public:
	ISObjects(const ISObjects &) = delete;
	ISObjects(ISObjects &&) = delete;
	ISObjects &operator=(const ISObjects &) = delete;
	ISObjects &operator=(ISObjects &&) = delete;
	~ISObjects();

	static ISObjects& GetInstance();
	void Initialize(); //Инициализация
	ISObjectInterface* GetInterface(); //Получить указатель на ядро конфигурации

private:
	ISObjects();

	ISObjectInterface *ObjectInterface;
};
//-----------------------------------------------------------------------------
