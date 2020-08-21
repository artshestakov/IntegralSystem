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

	void Initialize(); //�������������

	std::vector<ISMetaSystem*> GetSystems(); //�������� ������ ���� ������
	ISMetaSystem* GetSystem(const QString &SystemUID); //�������� ������� �� � ��������������
	ISMetaSubSystem* GetSubSystem(const QString &SubSystemUID); //�������� ���������� �� � ��������������

private:
	ISMetaSystem* CheckExistSystem(const ISUuid &SystemUID);

private:
	ISMetaSystemsEntity();

	std::vector<ISMetaSystem*> Systems;
};
//-----------------------------------------------------------------------------
