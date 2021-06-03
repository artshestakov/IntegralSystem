#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISObjectInterface : public QObject
{
    Q_OBJECT

public:
    ISObjectInterface();
    virtual ~ISObjectInterface();

    void SetConfigurationName(const QString &configuration_name);

    virtual void RegisterMetaTypes() const = 0; //����������� ����-�����
    virtual void BeforeShowMainWindow() const = 0; //������� ����� �������� �������� ���� ����������
    virtual void InitializePlugin() const = 0; //������������� ������������
    QIcon GetIcon(const QString &IconName) const; //�������� ������ �� �������� ������������

private:
    QString ConfigurationName;
};
//-----------------------------------------------------------------------------
