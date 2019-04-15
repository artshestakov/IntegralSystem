#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISIntegralSystem : public QApplication
{
	Q_OBJECT

public:
	ISIntegralSystem(int &argc, char **argv);
	virtual ~ISIntegralSystem();

	bool CheckAdminRole(); //�������� ������� ���� ��������������
	bool notify(QObject *Object, QEvent *e);

	static void ShowTaskViewForm(int TaskID); //�������� �������� ��������� ������
	static void ShowTaskObjectForm(ISNamespace::ObjectFormType FormType, int TaskID = 0); //�������� ����� ������� ������
	static void ShowTaskObjectForm(QWidget *TaskObjectForm); //�������� ����� ������� ������
};
//-----------------------------------------------------------------------------
