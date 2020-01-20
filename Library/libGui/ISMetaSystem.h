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
	bool IsSystem; //Движковая система (если нет - значит пользовательская)
	int ID;
	ISUuid UID; //Идентификатор
	QString LocalName; //Имя системы
	int OrderID; //Порядок системы
	QString IconName; //Имя иконки
	QByteArray IconImage; //Пользовательская иконка
	QString Hint;

	QVector<ISMetaSubSystem*> SubSystems; //Список подсистем
};
//-----------------------------------------------------------------------------
