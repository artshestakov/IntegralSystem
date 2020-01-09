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
//!������� ����� ����� �������
class LIBGUI_EXPORT ISObjectFormBase : public ISInterfaceForm
{
	Q_OBJECT

	Q_PROPERTY(bool ModificationFlag READ GetModificationFlag WRITE SetModificationFlag)

signals:
	void SavedObject(int ObjectID);
	void UpdateList();
	void CurrentObjectTab(); //������ ��� ������� (������������ ������� �������)
	void Close();
	void CloseTab(int Index);
	void SaveAndCreate(PMetaClassTable *MetaTable);

public:
	ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISObjectFormBase();

	int GetParentObjectID() const;
	void SetParentObjectID(int parent_object_id);

	ISNamespace::ObjectFormType GetFormType(); //�������� ��� �����
	int GetObjectID() const; //�������� ������������� �������
	PMetaClassTable* GetMetaTable(); //�������� ����-�������
	bool GetModificationFlag() const; //�������� ���� ����������� ������ 

	void SetFieldValue(const QString &FieldName, const QVariant &value); //�������� �������� ����
	QVariant GetFieldValue(const QString &FieldName) const; //�������� �������� ����

	void SetCurrentIndexTab(int current_index_tab);
	void SetVisibleNavigationBar(bool Visible); //�������� ��������� � ������� ��������
	void SetVisibleField(const QString &FieldName, bool Visible); //�������� ��������� � ����
	void SetVisibleFavorites(bool Visible); //�������� ��������� ������ "���������"
	void SetVisibleDelete(bool Visible); //�������� ��������� ������ "�������"
	void SetVisibleDeleteCascade(bool Visible); //�������� ��������� ������ "������� ��������"
	void SetVisibleReRead(bool Visible); //�������� ��������� ������ "���������� ��������"

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	
	void ControlEnterClicked() override;
	void EscapeClicked() override;
	void AfterShowEvent() override;

protected:
	void CreateToolBarEscorts(); //�������� �������� �������
	void CreateMainTabWidget(); //�������� �������� ����
	void CreateToolBar(); //�������� ������ ���������� ��������
	void CreateFieldsWidget(); //�������� ����� �������������� ����������
	void FillDataFields(); //���������� ����� �������

	void CreateFieldID(QFormLayout *FormLayout); //�������� ������� � ��������� ����������
	ISFieldEditBase* CreateColumnForField(PMetaClassField *MetaField); //������� ���� �������������� ����������
	void AddColumnForField(PMetaClassField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *FormLayout); //�������� ���� �������������� ���������� �� �����
	void AddObjectEscort(QWidget *ObjectForm);

	void ToolBarClicked(QAction *ActionClicked); //������� ������� �� �������� � �������
	void SetValueFieldID(int object_id); //�������� �������� ���� "���"

protected:
	void SaveCreate(); //��������� � ������� ����� ������
	void SaveClose(); //��������� � �������
	virtual bool Save(); //���������
	virtual void SaveEvent(); //������� ��� ���������� �������
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
	void ReRead(); //����������
	void ShowSettingsForm();

	void AddActionToolBar(QAction *Action, bool AddingToActionGroup = true); //�������� ������-�������� �� ������
	void AddActionMenu(QAction *Action, bool AddingToActionGroup = true); //�������� ������-�������� � ����
	void SetEnabledActions(bool Enabled); //��������� ����������� ������ ��������
	void TimeoutAutoSave();

	QString GetObjectName() const; //�������� ������������ �������
	ISFieldEditBase* GetFieldWidget(const QString &FieldName); //������� ������ �������������� �� ��� �����
	ISTabWidgetObject* GetTabWidget(); //�������� ��������� �� ���-������
	QVBoxLayout* GetLayoutWidgetObject(); //�������� ��������� �� ����������� �������
	QToolBar* GetToolBar(); //�������� ��������� �� ������ ������-��������

private:
	ISNamespace::ObjectFormType FormType; //��� �����
	PMetaClassTable *MetaTable; //����-�������
	int ObjectID; //������������� �������� �������
	int ParentObjectID; //������������� ��������
	QTimer *TimerAutoSave;

	ISToolBarObject *ToolBarNavigation; //������ ��������
	ISTabWidgetObject *TabWidgetMain; //������� ���
	QStackedWidget *StackedWidget;
	QWidget *WidgetObject; //������ �������
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
	ISFieldEditBase *BeginFieldEdit; //��������� �� ������ ���� �������������� ����������
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
