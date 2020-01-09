#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
#include "ISNamespace.h"
#include "PMetaClassTable.h"
#include "ISToolBarObject.h"
#include "ISFieldEditBase.h"
#include "ISLineEdit.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
#include "ISTabWidgetObject.h"
//-----------------------------------------------------------------------------
//!Базовый класс формы объекта
class LIBGUI_EXPORT ISObjectFormBase : public ISInterfaceForm
{
	Q_OBJECT

	Q_PROPERTY(bool ModificationFlag READ GetModificationFlag WRITE SetModificationFlag)

signals:
	void SavedObject(int ObjectID);
	void UpdateList();
	void CurrentObjectTab(); //Сигнал для вкладки (установление текущей вкладки)
	void Close();
	void CloseTab(int Index);
	void SaveAndCreate(PMetaClassTable *MetaTable);

public:
	ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISObjectFormBase();

	int GetParentObjectID() const;
	void SetParentObjectID(int parent_object_id);

	ISNamespace::ObjectFormType GetFormType(); //Получить тип формы
	int GetObjectID() const; //Получить идентификатор объекта
	PMetaClassTable* GetMetaTable(); //Получить мета-таблицу
	bool GetModificationFlag() const; //Получить флаг модицикации данных 

	void SetFieldValue(const QString &FieldName, const QVariant &value); //Изменить значение поля
	QVariant GetFieldValue(const QString &FieldName) const; //Получить значение поля

	void SetCurrentIndexTab(int current_index_tab);
	void SetVisibleNavigationBar(bool Visible); //Изменить видимость у талбара эскортов
	void SetVisibleField(const QString &FieldName, bool Visible); //Изменить видимость у поля
	void SetVisibleFavorites(bool Visible); //Изменить видимость кнопки "Избранное"
	void SetVisibleDelete(bool Visible); //Изменить видимость кнопки "Удалить"
	void SetVisibleDeleteCascade(bool Visible); //Изменить видимость кнопки "Удалить каскадом"
	void SetVisibleReRead(bool Visible); //Изменить видимость кнопки "Перечитать карточку"

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	
	void ControlEnterClicked() override;
	void EscapeClicked() override;
	void AfterShowEvent() override;

protected:
	void CreateToolBarEscorts(); //Создание главного тулбара
	void CreateMainTabWidget(); //Создание главного таба
	void CreateToolBar(); //Создание панели управления объектом
	void CreateFieldsWidget(); //Создание полей редактирования информации
	void FillDataFields(); //Заполнение полей данными

	void CreateFieldID(QFormLayout *FormLayout); //Создание виджета с системной иформацией
	ISFieldEditBase* CreateColumnForField(PMetaClassField *MetaField); //Создать поле редактирования информации
	void AddColumnForField(PMetaClassField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *FormLayout); //Добавить поле редактирования информации на форму
	void AddObjectEscort(QWidget *ObjectForm);

	void ToolBarClicked(QAction *ActionClicked); //Событие нажатия на действие в тулбаре
	void SetValueFieldID(int object_id); //Изменить значение поля "Код"

protected:
	void SaveCreate(); //Сохранить и создать новую запись
	void SaveClose(); //Сохранить и закрыть
	virtual bool Save(); //Сохранить
	virtual void SaveEvent(); //Событие при сохранении объекта
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
	void ReRead(); //Перечитать
	void ShowSettingsForm();

	void AddActionToolBar(QAction *Action, bool AddingToActionGroup = true); //Добавить кнопку-действие на тулбар
	void AddActionMenu(QAction *Action, bool AddingToActionGroup = true); //Добавить кнопку-действие в меню
	void SetEnabledActions(bool Enabled); //Изменение доступности кнопок действий
	void TimeoutAutoSave();

	QString GetObjectName() const; //Получить наименование объекта
	ISFieldEditBase* GetFieldWidget(const QString &FieldName); //Получиь виджет редактирования по его имени
	ISTabWidgetObject* GetTabWidget(); //Получить указатель на таб-виджет
	QVBoxLayout* GetLayoutWidgetObject(); //Получить указатель на компоновщик объекта
	QToolBar* GetToolBar(); //Получить указатель на тулбар кнопок-действий

private:
	ISNamespace::ObjectFormType FormType; //Тип формы
	PMetaClassTable *MetaTable; //Мета-таблица
	int ObjectID; //Идентификатор текущего объекта
	int ParentObjectID; //Идентификатор родителя
	QTimer *TimerAutoSave;

	ISToolBarObject *ToolBarNavigation; //Тулбар эскортов
	ISTabWidgetObject *TabWidgetMain; //Главный таб
	QStackedWidget *StackedWidget;
	QWidget *WidgetObject; //Виджет объекта
	QVBoxLayout *WidgetObjectLayout;
	QWidget *WidgetTabEscort;
	ISInterfaceMetaForm *WidgetEscort;
	QToolBar *ToolBar;
	QFormLayout *FormLayout;
	ISScrollArea *ScrollAreaMain;
	QLabel *LabelIsDeleted;
	QActionGroup *ActionGroup;

	QAction *ActionSaveAndNew;
	QAction *ActionSaveClose;
	QAction *ActionSave;
	QAction *ActionClose;
	QAction *ActionAdditionally;
	QAction *ActionSystemInfo;
	QAction *ActionFavorites;
	QAction *ActionDelete;
	QAction *ActionDeleteCascade;
	QAction *ActionCancelChange;
	QAction *ActionReRead;


	ISLineEdit *EditObjectID;
	ISFieldEditBase *BeginFieldEdit; //Указатель на первое поле редактирования информации
	QMap<QString, ISFieldEditBase*> FieldsMap;
	QMap<QString, QLabel*> LabelsMap;
	QMap<QString, QHBoxLayout*> Layouts;
	QVariantMap BeginValues;

	bool ModificationFlag;
	bool RecordIsDeleted;
	int CurrentIndexTab;
	QString ObjectName;
};
//-----------------------------------------------------------------------------
