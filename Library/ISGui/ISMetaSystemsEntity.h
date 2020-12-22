#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISMetaSystemsEntity
{
public:
	static ISMetaSystemsEntity& Instance();

	QString GetErrorString() const;
	void Initialize(const QVariantList &VariantList);

	std::vector<ISMetaSystem*> GetSystems(); //�������� ������ ���� ������
	ISMetaSystem* GetSystem(const QString &SystemUID); //�������� ������� �� � ��������������
	ISMetaSubSystem* GetSubSystem(const QString &SubSystemUID); //�������� ���������� �� � ��������������

private:
	ISMetaSystem* CheckExistSystem(const ISUuid &SystemUID);

private:
	ISMetaSystemsEntity();
	~ISMetaSystemsEntity();
	ISMetaSystemsEntity(ISMetaSystemsEntity const &) {};
	ISMetaSystemsEntity& operator=(ISMetaSystemsEntity const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISMetaSystem*> Systems;
};
//-----------------------------------------------------------------------------
