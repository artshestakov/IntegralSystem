#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISMetaSystemsEntity
{
public:
	ISMetaSystemsEntity(const ISMetaSystemsEntity &) = delete;
	ISMetaSystemsEntity(ISMetaSystemsEntity &&) = delete;
	ISMetaSystemsEntity &operator=(const ISMetaSystemsEntity &) = delete;
	ISMetaSystemsEntity &operator=(ISMetaSystemsEntity &&) = delete;
	~ISMetaSystemsEntity();

	static ISMetaSystemsEntity& GetInstance();

	void Initialize(); //Инициализация

	QVector<ISMetaSystem*> GetSystems(); //Получить список всех систем
	ISMetaSystem* GetSystem(const QString &SystemUID); //Получить систему по её идентификатору
	ISMetaSubSystem* GetSubSystem(const QString &SubSystemUID); //Получить подсистему по её идентификатору

protected:
	ISMetaSystem* CheckExistSystem(const ISUuid &SystemUID);

private:
	ISMetaSystemsEntity();

	QVector<ISMetaSystem*> Systems;
};
//-----------------------------------------------------------------------------
