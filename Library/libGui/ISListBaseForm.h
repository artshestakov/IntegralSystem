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
//!������� ����� ����� ������
class ISListBaseForm : public ISInterfaceMetaForm
{
	Q_OBJECT

signals:
	void Updated(); //������ ���������� ����� ���������� �������� ������ � �������
	void DoubleClicked(const QModelIndex &ModelIndex); //������ � ������� ������� �� ������ � �������
	void SelectedRowSignal(); //������ � ����� ������ ������

public:
	ISListBaseForm(const QString &TableName, QWidget *parent = 0);
	virtual ~ISListBaseForm();

	int GetCurrentRowIndex(); //�������� ������ ������� ������
	int GetObjectID(); //�������� ������������� �������� �������
	int GetObjectID(int RowIndex); //�������� ������������� ������� �� ������� ������
	int GetRowIndex(int object_id); //�������� ������ ������ �� �������������� �������
	QSqlRecord GetCurrentRecord(); //�������� ������� ������
	QVariant GetCurrentRecordValue(const QString &FieldName); //�������� �������� �� ������� �� ������� ������
	QVariant GetCurrentRecordValueDB(const QString &FieldName); //�������� �������� �� ������� �� ������� ������ (�������� ����� ����� �� ����)
	QVariant GetRecordValue(const QString &FieldName, int RowIndex); //�������� �������� �� ������� �� ������� ������
	ISVectorInt GetSelectedIDs(); //�������� ������ ��������������� ���������� �����
	int GetCountSelected(); //�������� ���������� ���������� �������
	ISVectorInt GetIDs() const; //�������� ������ ������������� ������������ ����� � ������ ������
	ISVectorInt GetSelectedRowIndexes(); //�������� ������ �������� ���������� �����
	ISQueryModel* GetQueryModel(); //�������� ��������� �� ������ �������
	void SetSelectObjectAfterUpdate(int object_id); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	PMetaTable* GetMetaTable(); //�������� ��������� �� ����-�������

	virtual void Create(); //�������� �������
	virtual void CreateCopy(); //�������� ����� �������
	virtual void Edit(); //��������� �������
	virtual void Delete(); //�������� �������
	virtual bool DeleteCascade(); //�������� ������� ��������
	virtual void Update(); //���������� ������
	virtual void ShowDeleted(); //���������� ��������� ������
	virtual void Search(); //�����
	virtual void SearchClear(); //�������� ���������� ������
	virtual void Export(); //��������� �������
	virtual void Print(); //������
	virtual void ShowSystemInfo(); //�������� ����� � ��������� ����������� �� �������
	virtual void Share(); //����������
	virtual void AttachTask(); //���������� ������ � ������
	virtual void ShowFavorites(); //�������� ����� � ���������� ���������
	virtual void NavigationSelectBeginRecord(); //��������� ������ ������ � ������
	virtual void NavigationSelectPreviousRecord(); //��������� ���������� ������
	virtual void NavigationSelectNextRecord(); //��������� ��������� ������
	virtual void NavigationSelectLastRecord(); //��������� ��������� ������
	void NoteObject(); //���������� �������
	void AutoFitColumnWidth(); //���������� ������
	void ResetWidthColumn(); //����� ������ �������
	void CopyRecord(); //����������� ������ � ����� ������
	virtual void LoadData() override; //�������� ������ � �������
	virtual void CornerButtonClicked(); //������� ������� �� cornerButton
	
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //�������� ������-��������
	void InsertAction(QAction *ActionBefore, QAction *ActionAfter, bool AddingToActionGroup = true); //�������� ������-��������

	void HideField(const QString &FieldName); //������� ���� �� ��� �����
	void ShowField(const QString &FieldName); //����������� ���� �� ��� �����
	void SetShowOnly(bool show_only);

	void Period(); //������� �������
	void PeriodClear(); //������� �������

protected:
	void paintEvent(QPaintEvent *e);

	ISBaseTableView* GetTableView(); //�������� ��������� �� ������ ������
	QToolBar* GetToolBar(); //�������� ��������� �� ������ �������
	QStatusBar* GetStatusBar(); //�������� ��������� �� ������ ������-����
	QHBoxLayout* GetLayoutTableView(); //�������� ��������� �� ����������� �������
	QAction* GetAction(ISNamespace::ActionType action_type);
	QAction* GetSpecialAction(ISNamespace::ActionSpecialType action_special);
	ISSqlModelCore* GetSqlModel(); //�������� ��������� �� ������
	ISModelThreadQuery* GetModelThread(); //�������� �������� ������

	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //������� ������ ������ � �������
	virtual void LoadDataAfterEvent(); //������� ������������ ����� �������� ������
	virtual void AfterShowEvent() override;
	
	bool CheckIsSystemObject(); //�������� ������� �� ������ "���������"
	void ResizeColumnsToContents(); //������ ������ ����� � ������������ � ����������
	void AddWidgetToBottom(QWidget *Widget); //�������� ������ � ������ ������ ����� ������ (WidgetBottom)
	void SetVisibleBottom(bool Visible); //�������� ��������� ������� ������� (WidgetBottom)
	void ClosingObjectForm(); //����������� ��� �������� ����� ������� (����������� ���� ������ ������)
	void SelectRowObject(int object_id); //��������� ������ �� �������������� �������
	void SelectRowIndex(int row_index); //��������� ������ �� �������
	void SetEnabledActionObject(bool Enabled); //�������� ����������� �������� ��� ��������
	void SetEnabledPageNavigation(bool Enabled); //�������� ����������� ������������ ���������
	void SearchFast(const QString &SearchValue); //������� �����
	void SearchFastClear(); //�������� ���������� �������� ������
	
	void ActionSetVisible(ISNamespace::ActionType action_type, bool visible); //�������� ��������� ������-��������
	void ActionSetEnabled(ISNamespace::ActionType action_type, bool enabled); //�������� ����������� ������-��������
	void ActionSetText(ISNamespace::ActionType action_type, const QString &text); //�������� ����� ������-��������
	void ActionSetToolTip(ISNamespace::ActionType action_type, const QString &tool_tip); //�������� ����������� ����� ������-��������

	void ModelThreadStarted(); //������� ������� ������� �� �������� ������
	void ModelThreadLoadingData(); //������� �������� ������ � ������
	void ModelThreadFinished(); //������� ���������� ������� �� �������� ������
	void ModelThreadErrorConnection(const QSqlError &SqlError); //������� ��� ������ ����������� � ��
	void ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText); //������� ��� ������ � �������

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //���������� ������� �������� ������� �� ������ �������

	void SortingChanged(int LogicalIndex, Qt::SortOrder Order);
	void SortingDefault(); //��������� ���������� �� ���������
	void HideSystemFields(); //������� ��������� �����
	void VisibleIndicatorWidget(); //��������� ��������� ����������
	void ShowSettingsForm();

private:
	void CreateActions(); //�������� ������-��������
	void CreateSpecialActions(); //�������� ����������� ������-��������
	void CreateToolBar(); //�������� �������
	void CreateTableView(); //�������� �������
	void CreateContextMenu(); //�������� ������������ ����
	void CreateModels(); //�������� �������
	void CreateStatusBar(); //������� ������-����
	void CreateDelegates(); //�������� ���������
	
	void ShowContextMenu(const QPoint &Point); //����������� ������������ ����

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
	bool DelegatesCreated; //��������� �������� ������� �������� ��� ���
	bool ShowOnly;
	bool IsLoadingData; //���� �������� ������
	bool SearchFlag;
	
	QToolBar *ToolBar;
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;
	QActionGroup *ActionObjectGroup; //������ �������� ��� ������ �������

	QMenu *ContextMenu;

	ISBaseTableView *TableView;
	ISListIndicatorWidget *ListIndicatorWidget;
	QHBoxLayout *LayoutTableView;
};
//-----------------------------------------------------------------------------
#endif
