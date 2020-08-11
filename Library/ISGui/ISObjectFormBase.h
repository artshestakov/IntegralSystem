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
//!������� ����� ����� �������
class ISObjectFormBase : public ISInterfaceForm
{
	Q_OBJECT

	Q_PROPERTY(bool ModificationFlag READ GetModificationFlag WRITE SetModificationFlag)

signals:
	void SavedObject(int ObjectID);
	void UpdateList();
	void CurrentObjectTab(); //������ ��� ������� (������������ ������� �������)
	void Close();
	void CloseTab(int Index);

public:
	ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISObjectFormBase();

	int GetParentObjectID() const;
	void SetParentObjectID(int parent_object_id);

	ISNamespace::ObjectFormType GetFormType(); //�������� ��� �����
	int GetObjectID() const; //�������� ������������� �������
	PMetaTable* GetMetaTable(); //�������� ����-�������
	bool GetModificationFlag() const; //�������� ���� ����������� ������ 

	void SetFieldValue(const QString &FieldName, const QVariant &value); //�������� �������� ����
	QVariant GetFieldValue(const QString &FieldName); //�������� �������� ����

	void SetCurrentIndexTab(int current_index_tab);
	void SetVisibleNavigationBar(bool Visible); //�������� ��������� � ������� ��������
	void SetVisibleField(const QString &FieldName, bool Visible); //�������� ��������� � ����
	void SetVisibleFavorites(bool Visible); //�������� ��������� ������ "���������"
	void SetVisibleDelete(bool Visible); //�������� ��������� ������ "�������"
	void SetVisibleDeleteCascade(bool Visible); //�������� ��������� ������ "������� ��������"

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	
	void ControlEnterClicked() override;
	void EscapeClicked() override;
	void AfterShowEvent() override;

protected:
	void CreateToolBarEscorts(); //�������� �������� �������
	void CreateToolBar(); //�������� ������ ���������� ��������
	void CreateWidgetObject(); //�������� �������� �������, �� ������� ����� ��������� ���� ��������������
	void CreateFieldsWidget(); //�������� ����� �������������� ����������
	void FillDataFields(); //���������� ����� �������

	ISFieldEditBase* CreateColumnForField(PMetaField *MetaField); //������� ���� �������������� ����������
	void AddColumnForField(PMetaField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *FormLayout); //�������� ���� �������������� ���������� �� �����

	void ToolBarClicked(QAction *ActionClicked); //������� ������� �� �������� � �������
	void SetValueFieldID(int object_id); //�������� �������� ���� "���"

protected:
	void SaveClose(); //��������� � �������
	virtual void SaveBefore(); //������� ����� �����������
	virtual bool Save(); //���������
	virtual void SavedEvent(); //������� ��� ���������� �������
	virtual void SaveAfter(); //������� ����� ����������
	void RenameReiconForm(); //�������������� �����
	void DataChanged(); //������ ��� ��������� �������� ������ �� �����
	void SetModificationFlag(bool modification); //�������� ���� ����������� ������
	void ResizeRemove(); //������������� �����
	void UpdateObjectActions(); //���������� ��������� �������� �������
	void ShowSystemInfo(); //�������� ��������� ����������
	void AddFavoite(); //�������� � ���������
	void Delete();
	void DeleteCascade();
	void CancelChanged(); //������ ���������

	void AddActionToolBar(QAction *Action, bool AddingToActionGroup = true); //�������� ������-�������� �� ������
	void AddActionMenu(QAction *Action, bool AddingToActionGroup = true); //�������� ������-�������� � ����
	void SetEnabledActions(bool Enabled); //��������� ����������� ������ ��������

	void AddWidgetToBottom(QWidget *Widget); //�������� ������ � ����� ��� �����

	QString GetObjectName() const; //�������� ������������ �������
	ISFieldEditBase* GetFieldWidget(const QString &FieldName); //������� ������ �������������� �� ��� �����
	QToolBar* GetToolBar(); //�������� ��������� �� ������ ������-��������

private:
	ISNamespace::ObjectFormType FormType; //��� �����
	PMetaTable *MetaTable; //����-�������
	int ObjectID; //������������� �������� �������
	int ParentObjectID; //������������� ��������

	QToolBar *ToolBarEscort; //������ ��������
	QActionGroup *ActionGroupEscort;
	QWidget *WidgetObject; //������ �������
	QToolBar *ToolBar; //������ �������� �� ��������
	QFormLayout *FormLayout;
	ISScrollArea *ScrollArea;
	ISInterfaceMetaForm *WidgetEscort;
	QLabel *LabelIsDeleted; //������� ����������� �� ��, ��� ������ ������� �� ��������
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

	ISLineEdit *EditObjectID; //������ ����������� �������������� �������
	ISFieldEditBase *BeginFieldEdit; //��������� �� ������ ���� �������������� ����������
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
