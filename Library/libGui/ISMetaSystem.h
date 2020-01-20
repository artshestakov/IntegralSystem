#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMetaSubSystem.h"
//-----------------------------------------------------------------------------
class ISMetaSystem : public QObject
{
	Q_OBJECT

public:
	ISMetaSystem(QObject *parent = 0);
	virtual ~ISMetaSystem();

	void SetIsSystem(bool is_system);
	bool GetIsSystem() const;

	void SetID(int id);
	int GetID() const;

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetOrderID(int order_id);
	int GetOrderID() const;

	void SetIconName(const QString &icon_name);
	QString GetIconName() const;

	void SetHint(const QString &hint);
	QString GetHint() const;

	void AppendSubSystem(ISMetaSubSystem *MetaSubSystem);
	QVector<ISMetaSubSystem*> GetSubSystems();

private:
	bool IsSystem; //��������� ������� (���� ��� - ������ ����������������)
	int ID;
	ISUuid UID; //�������������
	QString LocalName; //��� �������
	int OrderID; //������� �������
	QString IconName; //��� ������
	QByteArray IconImage; //���������������� ������
	QString Hint;

	QVector<ISMetaSubSystem*> SubSystems; //������ ���������
};
//-----------------------------------------------------------------------------
