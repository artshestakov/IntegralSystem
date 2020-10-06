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
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
//!Базовый класс формы списка
class ISListBaseForm : public ISInterfaceMetaForm
{
	Q_OBJECT

signals:
	void Updated(); //Сигнал посылается после завершения загрузки данных в таблицу
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
	void SetSelectObjectAfterUpdate(int ObjectID); //Изменить код объекта, который будет выделен после операции (создание, создание копии, изменение) над ним 
	void SetSelectObjectAfterUpdate(const ISVectorInt &Objects); //Изменить код объекта, который будет выделен после операции (создание, создание копии, изменение) над ним 
	PMetaTable* GetMetaTable(); //Получить указатель на мета-таблицу
	ISSqlModelCore* GetSqlModel(); //Получить указатель на модель

	virtual void Create(); //Создание объекта
	virtual void CreateCopy(); //Создание копии объекта
	virtual void Edit(); //Изменение объекта
	virtual void Delete(); //Удаление объекта
	virtual bool DeleteCascade(); //Удаление объекта каскадно
	virtual void Update(); //Обновление данных
	virtual void ShowActual(); //Показывать актуальные записи
	virtual void ShowDeleted(); //Показывать удаленные записи
	virtual void Search(); //Поиск
	virtual void SearchClear(); //Очистить результаты поиска
	virtual void Export(); //Выгрузить таблицу
	virtual void Print(); //Печать
	virtual void ShowSystemInfo(); //Показать форму с системной информацией об объекте
	virtual void ShowFavorites(); //Показать форму с избранными объектами
	virtual void NavigationSelectBeginRecord(); //Выделение первой записи в списке
	virtual void NavigationSelectPreviousRecord(); //Выделение предыдущей записи
	virtual void NavigationSelectNextRecord(); //Выделение следующей записи
	virtual void NavigationSelectLastRecord(); //Выделение последней записи
	void NoteObject(); //Примечание объекта
	void AutoFitColumnWidth(); //Автоподбор ширины
	void ResetWidthColumn(); //Сброс ширины колонок
	virtual void LoadData() override; //Загрузка данных в таблицу
	virtual void CornerButtonClicked(); //Событие нажатия на cornerButton
	
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //Добавить кнопку-действие
	void InsertAction(QAction *ActionBefore, QAction *ActionAfter, bool AddingToActionGroup = true); //Вставить кнопку-действие

	void HideField(const QString &FieldName); //Скрытие поля по его имени
	void ShowField(const QString &FieldName); //Отображение поля по его имени

	void Period(); //Задание периода
	void PeriodClear(); //Очистка периода

protected:
	void paintEvent(QPaintEvent *e) override;
	void EscapeClicked() override;

	ISBaseTableView* GetTableView(); //Получить указатель на виджет списка
	QToolBar* GetToolBar(); //Получить указатель на виджет тулбара
	QAction* GetAction(ISNamespace::ActionType action_type);
	QAction* GetSpecialAction(ISNamespace::ActionSpecialType action_special);

	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //Событие выбора строки в таблице
	virtual void LoadDataAfterEvent(); //Событие происходящее после загрузки данных
	virtual void AfterShowEvent() override;
	
	bool CheckIsSystemObject(); //Проверка объекта на статус "Системный"
	void ResizeColumnsToContents(); //Подгон ширины полей в соответствии с содержимым
	void SelectRowObject(const ISVectorInt &Objects); //Выделение строки по идентификатору объекта
	void SelectRowIndex(int row_index); //Выделение строки по индексу
	void SetEnabledActionObject(bool Enabled); //Изменить доступность действий над объектом
	void SetEnabledPageNavigation(bool Enabled); //Изменить доступность постраничной навигации
	void SearchFast(const QString &SearchValue); //Быстрый поиск
	void SearchFastClear(); //Очистить результаты быстрого поиска

	void ModelThreadStarted(); //Событие запуска запроса на загрузку данных
	void ModelThreadLoadingData(); //Событие загрузки данных в память
	void ModelThreadFinished(); //Событие завершения запроса на загрузку данных
	void ModelThreadErrorConnection(const QSqlError &SqlError); //Событие при ошибке подключения к БД
	void ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText); //Событие при ошибке в запросе

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //Обработчик события двойного нажатия на строку таблицы

	void FieldResized(bool Include); //Включить/выключить сигнал изменения размера поля
	void FieldResized(int LogicalIndex, int WidthOld, int WidthNew);

	void SortingChanged(int LogicalIndex, Qt::SortOrder Order);
	void SortingDefault(); //Установка сортировки по умолчанию
	void VisibleIndicatorWidget(); //Изменение видимости индикатора
	void ShowSettingsForm();

private:
	void CreateDelegates(); //Создание делегатов
	
	void ShowContextMenu(const QPoint &Point); //Отображение контекстного меню

private:
	PMetaTable *MetaTable;
	ISSqlModelCore *SqlModel;
	QLabel *LabelRowCount;
	QLabel *LabelPeriod;
	QLabel *LabelSelectedRow;
	ISPageNavigation *PageNavigation;
	ISSearchEdit *EditSearch;
	QStatusBar *StatusBar;

	ISModelThreadQuery *ModelThreadQuery;
	ISQueryModel *QueryModel;
	ISSearchForm *SearchForm;

	ISVectorInt SelectObjectAfterUpdate; //Вектор записей, которые нужно выделить после обновления таблицы
	bool DelegatesCreated; //Индикатор указания созданы делегаты или нет
	bool IsLoadingData; //Флаг загрузки данных
	bool SearchFlag;
	
	QToolBar *ToolBar;
	QAction *ActionPeriodClear;
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;
	QActionGroup *ActionObjectGroup; //Группа действий для одного объекта

	QMenu *ContextMenu;

	ISBaseTableView *TableView;
	ISListIndicatorWidget *ListIndicatorWidget;
};
//-----------------------------------------------------------------------------
#endif
