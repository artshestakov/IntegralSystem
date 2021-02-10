#pragma once
#ifndef _ISLISTBASEFORM_H_INCLUDED
#define _ISLISTBASEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
#include "ISTcpModels.h"
#include "ISBaseTableView.h"
#include "ISListIndicatorWidget.h"
#include "ISSearchForm.h"
#include "ISFieldEdits.h"
#include "ISTcpQuery.h"
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
	ISObjectPair GetObject(); //Получить объект
	unsigned int GetObjectID(); //Получить идентификатор текущего объекта
	unsigned int GetObjectID(int RowIndex); //Получить идентификатор объекта по индексу строки
	int GetRowIndex(int object_id); //Получить индекс строки по идентификатору объекта
	const ISModelRecord& GetCurrentRecord(); //Получить текущую запись
	QVariant GetCurrentRecordValue(const QString &FieldName); //Получить значение из столбца по текущей строке
	QVariant GetCurrentRecordValueDB(const QString &FieldName); //Получить значение из столбца по текущей строке (значение будет взято из базы)
	ISVectorUInt GetSelectedIDs(); //Получить список идентификаторов выделенных строк
	int GetCountSelected(); //Получить количество выделенных записей
	ISVectorUInt GetIDs() const; //Получить список идентификатор отображаемых строк в данный момент
	ISVectorInt GetSelectedRowIndexes(); //Получить список индексов выделенных строк
	void SetSelectObjectAfterUpdate(unsigned int ObjectID); //Изменить код объекта, который будет выделен после операции (создание, создание копии, изменение) над ним 
	void SetSelectObjectAfterUpdate(const ISVectorUInt &Objects); //Изменить код объекта, который будет выделен после операции (создание, создание копии, изменение) над ним 
	PMetaTable* GetMetaTable(); //Получить указатель на мета-таблицу
	ISTcpModel* GetTcpModel(); //Получить указатель на модель
	QStatusBar* GetStatusBar(); //Получить указатель на статус-бар
	ISTcpQueryTable* GetTcpQuery(); //Получить указатель на объекта запроса

	virtual void Create(); //Создание объекта
	virtual void CreateCopy(); //Создание копии объекта
	virtual void Edit(); //Изменение объекта
	virtual bool Delete(); //Удаление объекта
	virtual bool Update(); //Обновление данных
	virtual void SearchShow(); //Поиск
	virtual void SearchClear(); //Очистить результаты поиска
	virtual void Export(); //Выгрузить таблицу
	virtual void ShowFavorites(); //Показать форму с избранными объектами
	void RecordInfo(); //Информация о записи
	void FavoriteObject(); //Избранное
	void NoteObject(); //Примечание объекта
	virtual void LoadData() override; //Загрузка данных в таблицу
	virtual void CornerButtonClicked(); //Событие нажатия на cornerButton
	
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //Добавить кнопку-действие
	void InsertAction(QAction *Action, QAction *ActionBefore, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //Вставить кнопку-действие

	void HideField(const QString &FieldName); //Скрытие поля по его имени
	void ShowField(const QString &FieldName); //Отображение поля по его имени

	void SetShowOnly(bool ShowOnly);

protected:
	void paintEvent(QPaintEvent *e) override;
	void EscapeClicked() override;

	ISBaseTableView* GetTableView(); //Получить указатель на виджет списка
	QToolBar* GetToolBar(); //Получить указатель на виджет тулбара
	QAction* GetAction(ISNamespace::ActionType action_type);
	QAction* GetSpecialAction(ISNamespace::ActionSpecialType action_special);

	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //Событие выбора строки в таблице
	virtual void AfterShowEvent() override;
	
	bool CheckIsSystemObject(); //Проверка объекта на статус "Системный"
	void SelectRowObject(const ISVectorUInt &Objects); //Выделение строки по идентификатору объекта
	void SelectRowIndex(int row_index); //Выделение строки по индексу
	void SetEnabledActionObject(bool Enabled); //Изменить доступность действий над объектом
	void SetEnabledActions(bool Enabled); //Изменить доступность всех действий на тулбаре

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //Обработчик события двойного нажатия на строку таблицы

	void FieldResized(bool Include); //Включить/выключить сигнал изменения размера поля
	void FieldResized(int LogicalIndex, int WidthOld, int WidthNew);

	void SortingChanged(int LogicalIndex, Qt::SortOrder SortingOrder);
	void ShowSettingsForm();

private:
	void Search(const QVariantList &VariantList);
	void CreateDelegates(); //Создание делегатов
	void ShowContextMenu(const QPoint &Point); //Отображение контекстного меню

private:
	PMetaTable *MetaTable;
	ISTcpQueryTable *TcpQuery;
	bool IsLoadingData; //Флаг загрузки данных
	ISVectorUInt SelectObjectAfterUpdate; //Вектор записей, которые нужно выделить после обновления таблицы

private:
	QActionGroup *ActionObjectGroup; //Группа действий для одного объекта
	QActionGroup *ActionGroupPageNavigation; //Группа действий для постраничной навигации
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;

	QToolBar *ToolBar; //Тулбар
	ISBaseTableView *TableView; //Виджет таблицы
	ISTcpModel *TcpModel; //Модель представления данных
	QStatusBar *StatusBar; //Статус-бар
	QLabel *LabelRowCount; //Надпись с количеством строк
	QLabel *LabelSelectedRow; //Надпись с выделенными записями
	QMenu *ContextMenu; //Контекстное меню
	ISListIndicatorWidget *ListIndicatorWidget; //Индикатор
	ISSearchForm *SearchForm; //Форма поиска
};
//-----------------------------------------------------------------------------
#endif
