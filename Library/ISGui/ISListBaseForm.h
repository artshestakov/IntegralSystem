#pragma once
#ifndef _ISLISTBASEFORM_H_INCLUDED
#define _ISLISTBASEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
#include "ISQueryModel.h"
//#include "ISSqlModelCore.h"
#include "ISTcpModel.h"
//#include "ISModelThreadQuery.h"
#include "ISBaseTableView.h"
#include "ISListIndicatorWidget.h"
#include "ISSearchForm.h"
#include "ISPageNavigation.h"
#include "ISFieldEdits.h"
#include "ISTcpQuery.h"
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
	unsigned int GetObjectID(); //�������� ������������� �������� �������
	unsigned int GetObjectID(int RowIndex); //�������� ������������� ������� �� ������� ������
	int GetRowIndex(int object_id); //�������� ������ ������ �� �������������� �������
	const ISModelRecord& GetCurrentRecord(); //�������� ������� ������
	QVariant GetCurrentRecordValue(const QString &FieldName); //�������� �������� �� ������� �� ������� ������
	QVariant GetCurrentRecordValueDB(const QString &FieldName); //�������� �������� �� ������� �� ������� ������ (�������� ����� ����� �� ����)
	ISVectorUInt GetSelectedIDs(); //�������� ������ ��������������� ���������� �����
	int GetCountSelected(); //�������� ���������� ���������� �������
	ISVectorInt GetIDs() const; //�������� ������ ������������� ������������ ����� � ������ ������
	ISVectorInt GetSelectedRowIndexes(); //�������� ������ �������� ���������� �����
	ISQueryModel* GetQueryModel(); //�������� ��������� �� ������ �������
	void SetSelectObjectAfterUpdate(unsigned int ObjectID); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	void SetSelectObjectAfterUpdate(const ISVectorUInt &Objects); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	PMetaTable* GetMetaTable(); //�������� ��������� �� ����-�������
	ISTcpModel* GetTcpModel(); //�������� ��������� �� ������
	QStatusBar* GetStatusBar(); //�������� ��������� �� ������-���

	virtual void Create(); //�������� �������
	virtual void CreateCopy(); //�������� ����� �������
	virtual void Edit(); //��������� �������
	virtual bool Delete(); //�������� �������
	virtual bool Update(); //���������� ������
	virtual void Search(); //�����
	virtual void SearchClear(); //�������� ���������� ������
	virtual void Export(); //��������� �������
	virtual void Print(); //������
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
	void InsertAction(QAction *Action, QAction *ActionBefore, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //�������� ������-��������

	void HideField(const QString &FieldName); //������� ���� �� ��� �����
	void ShowField(const QString &FieldName); //����������� ���� �� ��� �����

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
	void SelectRowObject(const ISVectorUInt &Objects); //��������� ������ �� �������������� �������
	void SelectRowIndex(int row_index); //��������� ������ �� �������
	void SetEnabledActionObject(bool Enabled); //�������� ����������� �������� ��� ��������
	void SetEnabledPageNavigation(bool Enabled); //�������� ����������� ������������ ���������

	void ModelThreadStarted(); //������� ������� ������� �� �������� ������
	void ModelThreadLoadingData(); //������� �������� ������ � ������
	void ModelThreadFinished(); //������� ���������� ������� �� �������� ������
	void ModelThreadErrorConnection(const QSqlError &SqlError); //������� ��� ������ ����������� � ��
	void ModelThreadErrorQuery(const QSqlError &SqlError, const QString &QueryText); //������� ��� ������ � �������

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //���������� ������� �������� ������� �� ������ �������

	void FieldResized(bool Include); //��������/��������� ������ ��������� ������� ����
	void FieldResized(int LogicalIndex, int WidthOld, int WidthNew);

	void SortingChanged(int LogicalIndex, Qt::SortOrder SortingOrder);
	void ShowSettingsForm();

private:
	void CreateDelegates(); //�������� ���������
	
	void ShowContextMenu(const QPoint &Point); //����������� ������������ ����

private:
	PMetaTable *MetaTable;
	//ISSqlModelCore *SqlModel;
	ISTcpModel *TcpModel;
	QLabel *LabelRowCount;
	QLabel *LabelSelectedRow;
	ISPageNavigation *PageNavigation;
	QStatusBar *StatusBar;

	//ISModelThreadQuery *ModelThreadQuery;
	ISTcpQuery *TcpQueryUpdate;
	ISQueryModel *QueryModel;
	ISSearchForm *SearchForm;

	ISVectorUInt SelectObjectAfterUpdate; //������ �������, ������� ����� �������� ����� ���������� �������
	bool IsLoadingData; //���� �������� ������
	
	QToolBar *ToolBar;
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;
	QActionGroup *ActionObjectGroup; //������ �������� ��� ������ �������

	QMenu *ContextMenu;

	ISBaseTableView *TableView;
	ISListIndicatorWidget *ListIndicatorWidget;
};
//-----------------------------------------------------------------------------
#endif
