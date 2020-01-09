#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISObjectInterface : public QObject
{
	Q_OBJECT

public:
	ISObjectInterface(QObject *parent);
	virtual ~ISObjectInterface();

	virtual void RegisterMetaTypes() const = 0; //����������� ����-�����
	virtual void BeforeShowMainWindow() const = 0; //������� ����� �������� �������� ���� ����������
	virtual void InitializePlugin() const = 0; //������������� ������������
	QIcon GetIcon(const QString &IconName) const; //�������� ������ �� �������� ������������
};
//-----------------------------------------------------------------------------
