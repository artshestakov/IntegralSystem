#pragma once
#ifndef _ISOBJECTFORMBASE_H_INCLUDED
#define _ISOBJECTFORMBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISNamespace.h"
#include "PMetaClass.h"
#include "ISFieldEditBase.h"
#include "ISLineEdit.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
//!Базовый класс формы объекта
class ISObjectFormBase : public ISInterfaceForm
{
	Q_OBJECT

	Q_PROPERTY(bool ModificationFlag READ GetModificationFlag WRITE SetModificationFlag)

signals:
	void SavedObject(int ObjectID);
	void UpdateList();
	void CurrentObjectTab(); //Сигнал для вкладки (установление текущей вкладки)
	void Close();
	void CloseTab(int Index);

public:
	ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISObjectFormBase();

	int GetParentObjectID() const;
	void SetParentObjectID(int parent_object_id);

	ISNamespace::ObjectFormType GetFormType(); //Получить тип формы
	int GetObjectID() const; //Получить идентификатор объекта
	PMetaTable* GetMetaTable(); //Получить мета-таблицу
	bool GetModificationFlag() const; //Получить флаг модицикации данных 

	void SetFieldValue(const QString &FieldName, const QVariant &value); //Изменить значение поля
	QVariant GetFieldValue(const QString &FieldName); //Получить значение поля

	void SetCurrentIndexTab(int current_index_tab);
	void SetVisibleNavigationBar(bool Visible); //Изменить видимость у талбара эскортов
	void SetVisibleField(const QString &FieldName, bool Visible); //Изменить видимость у поля
	void SetVisibleFavorites(bool Visible); //Изменить видимость кнопки "Избранное"
	void SetVisibleDelete(bool Visible); //Изменить видимость кнопки "Удалить"
	void SetVisibleDeleteCascade(bool Visible); //Изменить видимость кнопки "Удалить каскадом"

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	
	void ControlEnterClicked() override;
	void EscapeClicked() override;
	void AfterShowEvent() override;

protected:
	void CreateToolBarEscorts(); //Создание главного тулбара
	void CreateToolBar(); //Создание панели управления объектом
	void CreateWidgetObject(); //Создание главного виджета, на котором будут размещены поля редактирования
	void CreateFieldsWidget(); //Создание полей редактирования информации
	void FillDataFields(); //Заполнение полей данными

	ISFieldEditBase* CreateColumnForField(PMetaField *MetaField); //Создать поле редактирования информации
	void AddColumnForField(PMetaField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *FormLayout); //Добавить поле редактирования информации на форму

	void ToolBarClicked(QAction *ActionClicked); //Событие нажатия на действие в тулбаре
	void SetValueFieldID(int object_id); //Изменить значение поля "Код"

protected:
	void SaveClose(); //Сохранить и закрыть
	virtual void SaveBefore(); //Событие перед сохранением
	virtual bool Save(); //Сохранить
	virtual void SavedEvent(); //Событие при сохранении объекта
	virtual void SaveAfter(); //Событие после сохранения
	void RenameReiconForm(); //Переименование формы
	void DataChanged(); //Собыие при изменении значения одного из полей
	void SetModificationFlag(bool modification); //Изменить флаг модицикации данных
	void ResizeRemove(); //Центрирование формы
	void UpdateObjectActions(); //Обновление видимости действий объекта
	void ShowSystemInfo(); //Показать системную информацию
	void AddFavoite(); //Добавить в избранное
	void Delete();
	void DeleteCascade();
	void CancelChanged(); //Отмена изменений

	void AddActionToolBar(QAction *Action, bool AddingToActionGroup = true); //Добавить кнопку-действие на тулбар
	void AddActionMenu(QAction *Action, bool AddingToActionGroup = true); //Добавить кнопку-действие в меню
	void SetEnabledActions(bool Enabled); //Изменение доступности кнопок действий

	void AddWidgetToBottom(QWidget *Widget); //Добавить виджет в самый низ формы

	QString GetObjectName() const; //Получить наименование объекта
	ISFieldEditBase* GetFieldWidget(const QString &FieldName); //Получиь виджет редактирования по его имени
	QToolBar* GetToolBar(); //Получить указатель на тулбар кнопок-действий

private:
	ISNamespace::ObjectFormType FormType; //Тип формы
	PMetaTable *MetaTable; //Мета-таблица
	int ObjectID; //Идентификатор текущего объекта
	int ParentObjectID; //Идентификатор родителя

	QToolBar *ToolBarEscort; //Тулбар эскортов
	QActionGroup *ActionGroupEscort;
	QWidget *WidgetObject; //Виджет объекта
	QToolBar *ToolBar; //Тулбар действий на объектом
	QFormLayout *FormLayout;
	ISScrollArea *ScrollArea;
	ISInterfaceMetaForm *WidgetEscort;
	QLabel *LabelIsDeleted; //Надпись указывающая на то, что объект помечен на удаление
	QActionGroup *ActionGroup;

	QAction *ActionSaveClose;
	QAction *ActionSave;
	QAction *ActionClose;
	QAction *ActionAdditionally;
	QAction *ActionSystemInfo;
	QAction *ActionFavorites;
	QAction *ActionDelete;
	QAction *ActionDeleteCascade;
	QAction *ActionCancelChange;

	ISLineEdit *EditObjectID; //Виджет отображения идентификатора объекта
	ISFieldEditBase *BeginFieldEdit; //Указатель на первое поле редактирования информации
	std::map<QString, ISFieldEditBase*> FieldsMap;
	std::map<QString, QLabel*> LabelsMap;
	std::map<QString, QHBoxLayout*> Layouts;
	QVariantMap BeginValues;

	bool ModificationFlag;
	bool RecordIsDeleted;
	int CurrentIndexTab;
	QString ObjectName;
};
//-----------------------------------------------------------------------------
#endif
