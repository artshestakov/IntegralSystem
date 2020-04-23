#pragma once
#ifndef _ISLISTBASEFORM_H_INCLUDED
#define _ISLISTBASEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
#include "ISQueryModel.h"
#include "ISSqlModelCore.h"
#include "ISModelThreadQuery.h"
#include "ISBaseTableView.h"
#include "ISListIndicatorWidget.h"
#include "ISSearchForm.h"
#include "ISPageNavigation.h"
#include "ISSearchEdit.h"
//-----------------------------------------------------------------------------
//!Базовый класс формы списка
class ISListBaseForm : public ISInterfaceMetaForm
{
	Q_OBJECT

signals:
	void Updated(); //Сигнал посылается после завершения загрузки данных в таблицу
	void DoubleClicked(const QModelIndex &ModelIndex); //Сигнал о двойном нажатии на запись в таблице
	void SelectedRowSignal(); //Сигнал о смене выбора строки

public:
	ISListBaseForm(const QString &TableName, QWidget *parent = 0);
	virtual ~ISListBaseForm();

	int GetCurrentRowIndex(); //Получить индекс текущей строки
	int GetObjectID(); //Получить идентификатор текущего объекта
	int GetObjectID(int RowIndex); //Получить идентификатор объекта по индексу строки
	int GetRowIndex(int object_id); //Получить индекс строки по идентификатору объекта
	QSqlRecord GetCurrentRecord(); //Получить текущую запись
	QVariant GetCurrentRecordValue(const QString &FieldName); //Получить значение из столбца по текущей строке
	QVariant GetCurrentRecordValueDB(const QString &FieldName); //Получить значение из столбца по текущей строке (значение будет взято из базы)
	QVariant GetRecordValue(const QString &FieldName, int RowIndex); //Получить значение из столбца по индексу строки
	ISVectorInt GetSelectedIDs(); //Получить список идентификаторов выделенных строк
	int GetCountSelected(); //Получить количество выделенных записей
	ISVectorInt GetIDs() const; //Получить список идентификатор отображаемых строк в данный момент
	ISVectorInt GetSelectedRowIndexes(); //Получить список индексов выделенных строк
	ISQueryModel* GetQueryModel(); //Получить указатель на модель запроса
	void SetSelectObjectAfterUpdate(int object_id); //Изменить код объекта, который будет выделен после операции (создание, создание копии, изменение) над ним 
	PMetaTable* GetMetaTable(); //Получить указатель на мета-таблицу

	virtual void Create(); //Создание объекта
	virtual void CreateCopy(); //Создание копии объекта
	virtual void Edit(); //Изменение объекта
	virtual void Delete(); //Удаление объекта
	virtual bool DeleteCascade(); //Удаление объекта каскадно
	virtual void Update(); //Обновление данных
	virtual void ShowDeleted(); //Показывать удаленные записи
	virtual void Search(); //Поиск
	virtual void SearchClear(); //Очистить результаты поиска
	virtual void Export(); //Выгрузить таблицу
	virtual void Print(); //Печать
	virtual void ShowSystemInfo(); //Показать форму с системной информацией об объекте
	virtual void Share(); //Поделиться
	virtual void AttachTask(); //Прикрепить запись к задаче
	virtual void ShowFavorites(); //Показать форму с избранными объектами
	virtual void NavigationSelectBeginRecord(); //Выделение первой записи в списке
	virtual void NavigationSelectPreviousRecord(); //Выделение предыдущей записи
	virtual void NavigationSelectNextRecord(); //Выделение следующей записи
	virtual void NavigationSelectLastRecord(); //Выделение последней записи
	void NoteObject(); //Примечание объекта
	void AutoFitColumnWidth(); //Автоподбор ширины
	void ResetWidthColumn(); //Сброс ширины колонок
	void CopyRecord(); //Копирование записи в буфер обмена
	virtual void LoadData() override; //Загрузка данных в таблицу
	virtual void CornerButtonClicked(); //Событие нажатия на cornerButton
	
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //Добавить кнопку-действие
	void InsertAction(QAction *ActionBefore, QAction *ActionAfter, bool AddingToActionGroup = true); //Вставить кнопку-действие

	void HideField(const QString &FieldName); //Скрытие поля по его имени
	void ShowField(const QString &FieldName); //Отображение поля по его имени
	void SetShowOnly(bool show_only);

	void Period(); //Задание периода
	void PeriodClear(); //Очистка периода

protected:
	void paintEvent(QPaintEvent *e);

	ISBaseTableView* GetTableView(); //Получить указатель на виджет списка
	QToolBar* GetToolBar(); //Получить указатель на виджет тулбара
	QStatusBar* GetStatusBar(); //Получить указатель на виджет статус-бара
	QHBoxLayout* GetLayoutTableView(); //Получить указатель на компоновщик таблицы
	QAction* GetAction(ISNamespace::ActionType action_type);
	QAction* GetSpecialAction(ISNamespace::ActionSpecialType action_special);
	ISSqlModelCore* GetSqlModel(); //Получить указатель на модель
	ISModelThreadQuery* GetModelThread(); //Получить поточную модель

	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //Событие выбора строки в таблице
	virtual void LoadDataAfterEvent(); //Событие происходящее после загрузки данных
	virtual void AfterShowEvent() override;
	
	bool CheckIsSystemObject(); //Проверка объекта на статус "Системный"
	void ResizeColumnsToContents(); //Подгон ширины полей в соответствии с содержимым
	void AddWidgetToBottom(QWidget *Widget); //Добавить виджет в нижний виджет формы списка (WidgetBottom)
	void SetVisibleBottom(bool Visible); //Изменить видимость нижнего виджета (WidgetBottom)
	void ClosingObjectForm(); //Срабатывает при закрытии формы объекта (порожденной этой формой списка)
	void SelectRowObject(int object_id); //Выделение строки по идентификатору объекта
	void SelectRowIndex(int row_index); //Выделение строки по индексу
	void SetEnabledActionObject(bool Enabled); //Изменить доступность действий над объектом
	void SetEnabledPageNavigation(bool Enabled); //Изменить доступность постраничной навигации
	void SearchFast(const QString &SearchValue); //Быстрый поиск
	void SearchFastClear(); //Очистить результаты быстрого поиска
	
	void ActionSetVisible(ISNamespace::ActionType action_type, bool visible); //Изменить видимость кнопки-действия
	void ActionSetEnabled(ISNamespace::ActionType action_type, bool enabled); //Изменить доступность кнопки-действия
	void ActionSetText(ISNamespace::ActionType action_type, const QString &text); //Изменить текст кнопки-действия
	void ActionSetToolTip(ISNamespace::ActionType action_type, const QString &tool_tip); //Изменить всплывающий текст кнопки-действия

	void ModelThreadStarted(); //Событие запуска запроса на загрузку данных
	void ModelThreadLoadingData(); //Событие загрузки данных в память
	void ModelThreadFinished(); //Событие завершения запроса на загрузку данных
	void ModelThreadErrorConnection(const QSqlError &SqlError); //Событие при ошибке подключения к БД
	void ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText); //Событие при ошибке в запросе

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //Обработчик события двойного нажатия на строку таблицы

	void SortingChanged(int LogicalIndex, Qt::SortOrder Order);
	void SortingDefault(); //Установка сортировки по умолчанию
	void HideSystemFields(); //Скрытие системных полей
	void VisibleIndicatorWidget(); //Изменение видимости индикатора
	void ShowSettingsForm();

private:
	void CreateActions(); //Создание кнопок-действий
	void CreateSpecialActions(); //Созадние специальных кнопок-действий
	void CreateToolBar(); //Создание тулбара
	void CreateTableView(); //Создание таблицы
	void CreateContextMenu(); //Создание контекстного меню
	void CreateModels(); //Создание моделей
	void CreateStatusBar(); //Создани статус-бара
	void CreateDelegates(); //Создание делегатов
	
	void ShowContextMenu(const QPoint &Point); //Отображение контекстного меню

private:
	PMetaTable *MetaTable;
	ISSqlModelCore *SqlModel;
	QLabel *LabelRowCount;
	QLabel *LabelPeriod;
	QLabel *LabelSelectedRow;
	QLabel *LabelSearchResult;
	ISPageNavigation *PageNavigation;
	ISSearchEdit *EditSearch;
	QStatusBar *StatusBar;

	ISModelThreadQuery *ModelThreadQuery;
	ISQueryModel *QueryModel;
	ISSearchForm *SearchForm;

	int SelectObjectAfterUpdate;
	bool DelegatesCreated; //Индикатор указания созданы делегаты или нет
	bool ShowOnly;
	bool IsLoadingData; //Флаг загрузки данных
	bool SearchFlag;
	
	QToolBar *ToolBar;
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;
	QActionGroup *ActionObjectGroup; //Группа действий для одного объекта

	QMenu *ContextMenu;

	ISBaseTableView *TableView;
	ISListIndicatorWidget *ListIndicatorWidget;
	QHBoxLayout *LayoutTableView;
};
//-----------------------------------------------------------------------------
#endif
