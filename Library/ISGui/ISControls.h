#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISControls
{
public:
	ISControls();
	virtual ~ISControls();

	static void SetBackgroundColorWidget(QWidget *Widget, const QColor &Color); //Изменить цвет фона виджета

	static QFrame* CreateHorizontalLine(QWidget *ParentWidget); //Создание горизонтальной линии
	static QFrame* CreateHorizontalLinePlain(QWidget *ParentWidget); //Создание черной горизонтальной линии
	static QFrame* CreateVerticalLine(QWidget *ParentWidget); //Создание вертикальной линии
	static QFrame* CreateVerticalLinePlain(QWidget *ParentWidget); //Создание вертикальной линии

//ДЕЙСТВИЯ
	static QAction* CreateActionClose(QObject *ParentObject); //Действие "Закрыть"
	static QAction* CreateActionSave(QObject *ParentObject); //Действие "Сохранить"
	static QAction* CreateActionSaveAndClose(QObject *ParentObject); //Действие "Сохранить и закрыть"
	static QAction* CreateActionExit(QObject *ParentObject); //Действие "Выход"

	static QAction* CreateSeparator(QObject *ParentObject); //Создать и получить сепаратор

//КОНТЕКСТНОЕ МЕНЮ
	static QAction* GetActionContextUndo(QObject *ParentObject); //Действие отменить
	static QAction* GetActionContextRedo(QObject *ParentObject);  //Действие вернуть
	static QAction* GetActionContextCut(QObject *ParentObject); //Действие вырезать
	static QAction* GetActionContextCopy(QObject *ParentObject); //Действие копировать
	static QAction* GetActionContextPaste(QObject *ParentObject); //Действие вставить
	static QAction* GetActionContextDelete(QObject *ParentObject); //Действие удалить
	static QAction* GetActionContextSelectAll(QObject *ParentObject); //Действие выделить все
	static QAction* GetActionContextToUpper(QObject *ParentObject); //Действие верхний регистр
	static QAction* GetActionContextToLower(QObject *ParentObject); //Действие нижний регистр

//БАЗОВАЯ ФОРМА СПИСКА
	static QAction* CreateActionCreate(QObject *ParentObject);
	static QAction* CreateActionCreateCopy(QObject *ParentObject);
	static QAction* CreateActionEdit(QObject *ParentObject);
	static QAction* CreateActionDelete(QObject *ParentObject);
	static QAction* CreateActionUpdate(QObject *ParentObject);
	static QAction* CreateActionSearch(QObject *ParentObject);
	static QAction* CreateActionSearchClearResults(QObject *ParentObject);
	static QAction* CreateActionPrint(QObject *ParentObject);
	static QAction* CreateActionRecordInformartion(QObject *ParentObject);
	static QAction* CreateActionNoteObject(QObject *ParentObject);
};
//-----------------------------------------------------------------------------
