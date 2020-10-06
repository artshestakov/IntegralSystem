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
//!������� ����� ����� ������
class ISListBaseForm : public ISInterfaceMetaForm
{
	Q_OBJECT

signals:
	void Updated(); //������ ���������� ����� ���������� �������� ������ � �������
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
	void SetSelectObjectAfterUpdate(int ObjectID); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	void SetSelectObjectAfterUpdate(const ISVectorInt &Objects); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	PMetaTable* GetMetaTable(); //�������� ��������� �� ����-�������
	ISSqlModelCore* GetSqlModel(); //�������� ��������� �� ������

	virtual void Create(); //�������� �������
	virtual void CreateCopy(); //�������� ����� �������
	virtual void Edit(); //��������� �������
	virtual void Delete(); //�������� �������
	virtual bool DeleteCascade(); //�������� ������� ��������
	virtual void Update(); //���������� ������
	virtual void ShowActual(); //���������� ���������� ������
	virtual void ShowDeleted(); //���������� ��������� ������
	virtual void Search(); //�����
	virtual void SearchClear(); //�������� ���������� ������
	virtual void Export(); //��������� �������
	virtual void Print(); //������
	virtual void ShowSystemInfo(); //�������� ����� � ��������� ����������� �� �������
	virtual void ShowFavorites(); //�������� ����� � ���������� ���������
	virtual void NavigationSelectBeginRecord(); //��������� ������ ������ � ������
	virtual void NavigationSelectPreviousRecord(); //��������� ���������� ������
	virtual void NavigationSelectNextRecord(); //��������� ��������� ������
	virtual void NavigationSelectLastRecord(); //��������� ��������� ������
	void NoteObject(); //���������� �������
	void AutoFitColumnWidth(); //���������� ������
	void ResetWidthColumn(); //����� ������ �������
	virtual void LoadData() override; //�������� ������ � �������
	virtual void CornerButtonClicked(); //������� ������� �� cornerButton
	
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //�������� ������-��������
	void InsertAction(QAction *ActionBefore, QAction *ActionAfter, bool AddingToActionGroup = true); //�������� ������-��������

	void HideField(const QString &FieldName); //������� ���� �� ��� �����
	void ShowField(const QString &FieldName); //����������� ���� �� ��� �����

	void Period(); //������� �������
	void PeriodClear(); //������� �������

protected:
	void paintEvent(QPaintEvent *e) override;
	void EscapeClicked() override;

	ISBaseTableView* GetTableView(); //�������� ��������� �� ������ ������
	QToolBar* GetToolBar(); //�������� ��������� �� ������ �������
	QAction* GetAction(ISNamespace::ActionType action_type);
	QAction* GetSpecialAction(ISNamespace::ActionSpecialType action_special);

	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //������� ������ ������ � �������
	virtual void LoadDataAfterEvent(); //������� ������������ ����� �������� ������
	virtual void AfterShowEvent() override;
	
	bool CheckIsSystemObject(); //�������� ������� �� ������ "���������"
	void ResizeColumnsToContents(); //������ ������ ����� � ������������ � ����������
	void SelectRowObject(const ISVectorInt &Objects); //��������� ������ �� �������������� �������
	void SelectRowIndex(int row_index); //��������� ������ �� �������
	void SetEnabledActionObject(bool Enabled); //�������� ����������� �������� ��� ��������
	void SetEnabledPageNavigation(bool Enabled); //�������� ����������� ������������ ���������
	void SearchFast(const QString &SearchValue); //������� �����
	void SearchFastClear(); //�������� ���������� �������� ������

	void ModelThreadStarted(); //������� ������� ������� �� �������� ������
	void ModelThreadLoadingData(); //������� �������� ������ � ������
	void ModelThreadFinished(); //������� ���������� ������� �� �������� ������
	void ModelThreadErrorConnection(const QSqlError &SqlError); //������� ��� ������ ����������� � ��
	void ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText); //������� ��� ������ � �������

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //���������� ������� �������� ������� �� ������ �������

	void FieldResized(bool Include); //��������/��������� ������ ��������� ������� ����
	void FieldResized(int LogicalIndex, int WidthOld, int WidthNew);

	void SortingChanged(int LogicalIndex, Qt::SortOrder Order);
	void SortingDefault(); //��������� ���������� �� ���������
	void VisibleIndicatorWidget(); //��������� ��������� ����������
	void ShowSettingsForm();

private:
	void CreateDelegates(); //�������� ���������
	
	void ShowContextMenu(const QPoint &Point); //����������� ������������ ����

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

	ISVectorInt SelectObjectAfterUpdate; //������ �������, ������� ����� �������� ����� ���������� �������
	bool DelegatesCreated; //��������� �������� ������� �������� ��� ���
	bool IsLoadingData; //���� �������� ������
	bool SearchFlag;
	
	QToolBar *ToolBar;
	QAction *ActionPeriodClear;
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;
	QActionGroup *ActionObjectGroup; //������ �������� ��� ������ �������

	QMenu *ContextMenu;

	ISBaseTableView *TableView;
	ISListIndicatorWidget *ListIndicatorWidget;
};
//-----------------------------------------------------------------------------
#endif
