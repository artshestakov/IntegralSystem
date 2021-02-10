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
	ISObjectPair GetObject(); //�������� ������
	unsigned int GetObjectID(); //�������� ������������� �������� �������
	unsigned int GetObjectID(int RowIndex); //�������� ������������� ������� �� ������� ������
	int GetRowIndex(int object_id); //�������� ������ ������ �� �������������� �������
	const ISModelRecord& GetCurrentRecord(); //�������� ������� ������
	QVariant GetCurrentRecordValue(const QString &FieldName); //�������� �������� �� ������� �� ������� ������
	QVariant GetCurrentRecordValueDB(const QString &FieldName); //�������� �������� �� ������� �� ������� ������ (�������� ����� ����� �� ����)
	ISVectorUInt GetSelectedIDs(); //�������� ������ ��������������� ���������� �����
	int GetCountSelected(); //�������� ���������� ���������� �������
	ISVectorUInt GetIDs() const; //�������� ������ ������������� ������������ ����� � ������ ������
	ISVectorInt GetSelectedRowIndexes(); //�������� ������ �������� ���������� �����
	void SetSelectObjectAfterUpdate(unsigned int ObjectID); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	void SetSelectObjectAfterUpdate(const ISVectorUInt &Objects); //�������� ��� �������, ������� ����� ������� ����� �������� (��������, �������� �����, ���������) ��� ��� 
	PMetaTable* GetMetaTable(); //�������� ��������� �� ����-�������
	ISTcpModel* GetTcpModel(); //�������� ��������� �� ������
	QStatusBar* GetStatusBar(); //�������� ��������� �� ������-���
	ISTcpQueryTable* GetTcpQuery(); //�������� ��������� �� ������� �������

	virtual void Create(); //�������� �������
	virtual void CreateCopy(); //�������� ����� �������
	virtual void Edit(); //��������� �������
	virtual bool Delete(); //�������� �������
	virtual bool Update(); //���������� ������
	virtual void SearchShow(); //�����
	virtual void SearchClear(); //�������� ���������� ������
	virtual void Export(); //��������� �������
	virtual void ShowFavorites(); //�������� ����� � ���������� ���������
	void RecordInfo(); //���������� � ������
	void FavoriteObject(); //���������
	void NoteObject(); //���������� �������
	virtual void LoadData() override; //�������� ������ � �������
	virtual void CornerButtonClicked(); //������� ������� �� cornerButton
	
	void AddAction(QAction *Action, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //�������� ������-��������
	void InsertAction(QAction *Action, QAction *ActionBefore, bool AddingToActionGroup = true, bool AddingToContextMenu = false); //�������� ������-��������

	void HideField(const QString &FieldName); //������� ���� �� ��� �����
	void ShowField(const QString &FieldName); //����������� ���� �� ��� �����

	void SetShowOnly(bool ShowOnly);

protected:
	void paintEvent(QPaintEvent *e) override;
	void EscapeClicked() override;

	ISBaseTableView* GetTableView(); //�������� ��������� �� ������ ������
	QToolBar* GetToolBar(); //�������� ��������� �� ������ �������
	QAction* GetAction(ISNamespace::ActionType action_type);
	QAction* GetSpecialAction(ISNamespace::ActionSpecialType action_special);

	virtual void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected); //������� ������ ������ � �������
	virtual void AfterShowEvent() override;
	
	bool CheckIsSystemObject(); //�������� ������� �� ������ "���������"
	void SelectRowObject(const ISVectorUInt &Objects); //��������� ������ �� �������������� �������
	void SelectRowIndex(int row_index); //��������� ������ �� �������
	void SetEnabledActionObject(bool Enabled); //�������� ����������� �������� ��� ��������
	void SetEnabledActions(bool Enabled); //�������� ����������� ���� �������� �� �������

	virtual void DoubleClickedTable(const QModelIndex &ModelIndex); //���������� ������� �������� ������� �� ������ �������

	void FieldResized(bool Include); //��������/��������� ������ ��������� ������� ����
	void FieldResized(int LogicalIndex, int WidthOld, int WidthNew);

	void SortingChanged(int LogicalIndex, Qt::SortOrder SortingOrder);
	void ShowSettingsForm();

private:
	void Search(const QVariantList &VariantList);
	void CreateDelegates(); //�������� ���������
	void ShowContextMenu(const QPoint &Point); //����������� ������������ ����

private:
	PMetaTable *MetaTable;
	ISTcpQueryTable *TcpQuery;
	bool IsLoadingData; //���� �������� ������
	ISVectorUInt SelectObjectAfterUpdate; //������ �������, ������� ����� �������� ����� ���������� �������

private:
	QActionGroup *ActionObjectGroup; //������ �������� ��� ������ �������
	QActionGroup *ActionGroupPageNavigation; //������ �������� ��� ������������ ���������
	std::map<ISNamespace::ActionType, QAction*> Actions;
	std::map<ISNamespace::ActionSpecialType, QAction *> ActionsSpecial;

	QToolBar *ToolBar; //������
	ISBaseTableView *TableView; //������ �������
	ISTcpModel *TcpModel; //������ ������������� ������
	QStatusBar *StatusBar; //������-���
	QLabel *LabelRowCount; //������� � ����������� �����
	QLabel *LabelSelectedRow; //������� � ����������� ��������
	QMenu *ContextMenu; //����������� ����
	ISListIndicatorWidget *ListIndicatorWidget; //���������
	ISSearchForm *SearchForm; //����� ������
};
//-----------------------------------------------------------------------------
#endif
