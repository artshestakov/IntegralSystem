#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClass : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString Name READ GetName WRITE SetName)
	Q_PROPERTY(QString UID READ GetUID WRITE SetUID)
	Q_PROPERTY(QString TypeObject READ GetTypeObject WRITE SetTypeObject)

public:
	PMetaClass(QObject *parent = 0);
	virtual ~PMetaClass();

	void SetName(const QString &name); //�������� ��� �������
	QString GetName() const; //�������� ��� �������

	void SetUID(const QString &uid); //�������� ������������� �������
	QString GetUID() const; //�������� ������������� �������

	void SetTypeObject(const QString &type_object); //�������� ��� ���� �������
	QString GetTypeObject() const; //�������� ��� �������

private:
	QString Name; //���
	QString UID; //�������������
	QString TypeObject; //���
};
//-----------------------------------------------------------------------------
