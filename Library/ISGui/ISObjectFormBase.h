#pragma once
#ifndef _ISOBJECTFORMBASE_H_INCLUDED
#define _ISOBJECTFORMBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISNamespace.h"
#include "PMetaClass.h"
#include "ISFieldEditBase.h"
#include "ISFieldEdits.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
//!������� ����� ����� �������
class ISObjectFormBase : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void SavedObject(unsigned int ObjectID);
	void UpdateList();
	void CurrentObjectTab(); //������ ��� ������� (������������ ������� �������)
	void Close();

public:
	ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISObjectFormBase();

	int GetParentObjectID() const;
	void SetParentObjectID(int parent_object_id, const QString &parent_filter_field);

	ISNamespace::ObjectFormType GetFormType(); //�������� ��� �����
	unsigned int GetObjectID() const; //�������� ������������� �������
	PMetaTable* GetMetaTable(); //�������� ����-�������
	bool GetModificationFlag() const; //�������� ���� ����������� ������ 

	void SetFieldValue(const QString &FieldName, const QVariant &value); //�������� �������� ����
	QVariant GetFieldValue(const QString &FieldName); //�������� �������� ����
	void AddVirtualField(const QString &FieldName, const QVariant &Value); //�������� ����������� ����

	void SetVisibleNavigationBar(bool Visible); //�������� ��������� � ������� ��������
	void SetVisibleField(const QString &FieldName, bool Visible); //�������� ��������� � ����
	void SetVisibleFavorites(bool Visible); //�������� ��������� ������ "���������"
	void SetVisibleDelete(bool Visible); //�������� ��������� ������ "������� ��������"

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

	ISFieldEditBase* CreateColumnForField(PMetaField *MetaField); //������� ���� �������������� ����������
	void AddColumnForField(PMetaField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *form_layout); //�������� ���� �������������� ���������� �� �����

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
	void UpdateObjectActions(); //���������� ��������� �������� �������
	void AddFavoite(); //�������� � ���������
	void Delete();
	void CancelChanged(); //������ ���������
	void NoteObject(); //���������� ������

	void AddActionToolBar(QAction *Action, bool AddingToActionGroup = true); //�������� ������-�������� �� ������
	void AddActionMenu(QAction *Action, bool AddingToActionGroup = true); //�������� ������-�������� � ����
	void SetEnabledActions(bool Enabled); //��������� ����������� ������ ��������

	ISFieldEditBase* GetFieldWidget(const QString &FieldName); //������� ������ �������������� �� ��� �����
	QToolBar* GetToolBar(); //�������� ��������� �� ������ ������-��������

private:
	ISNamespace::ObjectFormType FormType; //��� �����
	PMetaTable *MetaTable; //����-�������
	unsigned int ObjectID; //������������� �������� �������
	int ParentObjectID; //������������� ��������
	QString ParentFilterField; //��� ���� ��� ����������

	QToolBar *ToolBarEscort; //������ ��������
	QFrame *ToolBarEscortSeparator; //����������� ������� ��������
	QActionGroup *ActionGroupEscort;
	QToolBar *ToolBar; //������ �������� �� ��������
	QWidget *CentralWidget; //������� ����������� ������
	QActionGroup *ActionGroup;

	QAction *ActionSaveClose;
	QAction *ActionSave;
	QAction *ActionClose;
	QAction *ActionAdditionally;
	QAction *ActionSystemInfo;
	QAction *ActionFavorites;
	QAction *ActionDelete;
	QAction *ActionCancelChange;

	ISLineEdit *EditObjectID; //������ ����������� �������������� �������
	ISFieldEditBase *BeginFieldEdit; //��������� �� ������ ���� �������������� ����������
	std::map<QString, ISFieldEditBase*> FieldsMap;
	std::map<QString, QLabel*> LabelsMap;
	std::map<QString, QHBoxLayout*> Layouts;
	std::map<QString, QVariant> VirtualFields; //����������� ����

	bool ModificationFlag;
	QString ObjectName;
};
//-----------------------------------------------------------------------------
#endif
