#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISIntegralSystem : public QApplication
{
	Q_OBJECT

public:
	ISIntegralSystem(int &argc, char **argv);
	virtual ~ISIntegralSystem();

	bool CheckAdminRole(); //Проверка наличия прав администратора
	bool notify(QObject *Object, QEvent *e);

	static void ShowTaskViewForm(int TaskID); //Показать карточку просмотра задачи
	static void ShowTaskObjectForm(ISNamespace::ObjectFormType FormType, int TaskID = 0); //Показать форму объекта задачи
	static void ShowTaskObjectForm(QWidget *TaskObjectForm); //Показать форму объекта задачи
};
//-----------------------------------------------------------------------------
