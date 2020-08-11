#include "ISObjectFormBase.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaData.h"
#include "ISListEdit.h"
#include "ISSettings.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISMessageBox.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISNotificationService.h"
#include "ISProtocol.h"
#include "ISListObjectForm.h"
#include "ISDatabaseHelper.h"
#include "ISStyleSheet.h"
#include "ISFavorites.h"
#include "ISCreatedObjectsEntity.h"
#include "ISUserRoleEntity.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISObjectFormBase::ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISInterfaceForm(parent),
	FormType(form_type),
	MetaTable(meta_table),
	ObjectID(object_id),
	ParentObjectID(0),
	WidgetEscort(nullptr),
	EditObjectID(nullptr),
	BeginFieldEdit(nullptr),
	ModificationFlag(false),
	RecordIsDeleted(false),
	CurrentIndexTab(-1),
	ActionDelete(nullptr),
	ActionGroup(new QActionGroup(this))
{
	GetMainLayout()->setContentsMargins(0, 3, 0, 0);
	GetMainLayout()->setSpacing(0);

	CreateToolBarEscorts();
	CreateToolBar();
	CreateWidgetObject();
	CreateFieldsWidget();
	FillDataFields();
	ISCreatedObjectsEntity::Instance().RegisterForm(this);
}
//-----------------------------------------------------------------------------
ISObjectFormBase::~ISObjectFormBase()
{
	ISCreatedObjectsEntity::Instance().UnregisterForm(GetFormUID());
}
//-----------------------------------------------------------------------------
int ISObjectFormBase::GetParentObjectID() const
{
	return ParentObjectID;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetParentObjectID(int parent_object_id)
{
	ParentObjectID = parent_object_id;
}
//-----------------------------------------------------------------------------
ISNamespace::ObjectFormType ISObjectFormBase::GetFormType()
{
	return FormType;
}
//-----------------------------------------------------------------------------
int ISObjectFormBase::GetObjectID() const
{
	return ObjectID;
}
//-----------------------------------------------------------------------------
PMetaTable* ISObjectFormBase::GetMetaTable()
{
	return MetaTable;
}
//-----------------------------------------------------------------------------
bool ISObjectFormBase::GetModificationFlag() const
{
	return ModificationFlag;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetFieldValue(const QString &FieldName, const QVariant &value)
{
	GetFieldWidget(FieldName)->SetValue(value);
}
//-----------------------------------------------------------------------------
QVariant ISObjectFormBase::GetFieldValue(const QString &FieldName)
{
	return GetFieldWidget(FieldName)->GetValue();
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetCurrentIndexTab(int current_index_tab)
{
	CurrentIndexTab = current_index_tab;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleNavigationBar(bool Visible)
{
	ToolBarEscort->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleField(const QString &FieldName, bool Visible)
{
	LabelsMap[FieldName]->setVisible(Visible);
	FieldsMap[FieldName]->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleFavorites(bool Visible)
{
	ActionFavorites->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleDelete(bool Visible)
{
	if (ActionDelete)
	{
		ActionDelete->setVisible(Visible);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleDeleteCascade(bool Visible)
{
	if (ActionDeleteCascade)
	{
		ActionDeleteCascade->setVisible(Visible);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::closeEvent(QCloseEvent *e)
{
	if (ModificationFlag)
	{
		emit CurrentObjectTab();

		ISMessageBox MessageBox(QMessageBox::Warning, LANG("SavingProcess"), LANG("Message.Question.SaveObjectChanged").arg(MetaTable->LocalName), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this);
		MessageBox.setWindowIcon(BUFFER_ICONS("Save"));
		MessageBox.setDefaultButton(QMessageBox::No);
        QMessageBox::StandardButtons ClickedButton = static_cast<QMessageBox::StandardButtons>(MessageBox.Exec());

		switch (ClickedButton)
		{
		case ISMessageBox::Save:
			if (Save())
			{
				ISInterfaceForm::closeEvent(e);
			}
			else
			{
				e->ignore();
			}
			break;

		case ISMessageBox::Discard:
			ISInterfaceForm::closeEvent(e);
			break;

		case ISMessageBox::Cancel:
			e->ignore();
			break;
		}
	}
	else
	{
		ISInterfaceForm::closeEvent(e);
	}

	emit Close();
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::keyPressEvent(QKeyEvent *e)
{
	ISInterfaceForm::keyPressEvent(e);
	if (e->modifiers() == Qt::CTRL && e->key() == Qt::Key_S)
	{
		if (ModificationFlag)
		{
			Save();
		}
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ControlEnterClicked()
{
	SaveClose();
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();

	//�� ��������� ��� �������� ��������� �������� ������ ���� ��������� (����� ������� �������� - ��������)
	for (int i = 1; i < ToolBarEscort->actions().size(); ++i)
	{
		ToolBarEscort->actions()[i]->setEnabled(!(FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy));
	}

	if (FormType == ISNamespace::OFT_Edit)
	{
		ISProtocol::ShowObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
	}

	if (BeginFieldEdit)
	{
		BeginFieldEdit->SetFocus();
	}

	if (FieldsMap.count(MetaTable->ClassFilterField))
	{
		FieldsMap[MetaTable->ClassFilterField]->SetValue(ParentObjectID);
	}

	RenameReiconForm();
	UpdateObjectActions();
	SetModificationFlag(false);
	ResizeRemove();
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateToolBarEscorts()
{
	ToolBarEscort = new QToolBar(this);
	ToolBarEscort->setIconSize(ISDefines::Gui::SIZE_20_20);
	ToolBarEscort->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	connect(ToolBarEscort, &QToolBar::actionTriggered, this, &ISObjectFormBase::ToolBarClicked);
	GetMainLayout()->addWidget(ToolBarEscort);

	ActionGroupEscort = new QActionGroup(this);

	//�������� �������
	QAction *ActionObject = ToolBarEscort->addAction(BUFFER_ICONS("Document"), MetaTable->LocalName);
	ActionObject->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	ActionObject->setCheckable(true);
	ActionObject->setChecked(true);
	ActionObject->setProperty("IsChecked", true);
	ActionObject->setProperty("IsObject", true);
	ActionGroupEscort->addAction(ActionObject);

	ToolBarEscort->addSeparator();

	QAction *ActionProtocol = ToolBarEscort->addAction(BUFFER_ICONS("Protocol"), LANG("ProtocolCard"));
	ActionProtocol->setCheckable(true);
	ActionProtocol->setProperty("IsChecked", false);
	ActionProtocol->setProperty("ClassName", "ISProtocolObjectListForm");
	ActionGroupEscort->addAction(ActionProtocol);

	QAction *ActionDiscussion = ToolBarEscort->addAction(BUFFER_ICONS("Discussion"), LANG("Discussion"));
	ActionDiscussion->setCheckable(true);
	ActionDiscussion->setProperty("IsChecked", false);
	ActionDiscussion->setProperty("ClassName", "ISDiscussionListForm");
	ActionGroupEscort->addAction(ActionDiscussion);

	ToolBarEscort->addSeparator();

	for (PMetaEscort *MetaEscort : MetaTable->Escorts) //����� ��������� ����-������
	{		
		QAction *ActionEscort = ToolBarEscort->addAction(BUFFER_ICONS("Table"), MetaEscort->LocalName);
		ActionEscort->setCheckable(true);
		ActionEscort->setProperty("IsChecked", false);
		ActionEscort->setProperty("TableName", MetaEscort->TableName);
		ActionEscort->setProperty("ClassName", MetaEscort->ClassName);
		ActionEscort->setProperty("ClassFilter", MetaEscort->ClassFilter);
		ActionGroupEscort->addAction(ActionEscort);
	}

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateToolBar()
{
	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	//��������� � ������� ��������
	ActionSaveClose = ISControls::CreateActionSaveAndClose(ToolBar);
	ActionSaveClose->setToolTip(LANG("SaveChangeClose"));
	connect(ActionSaveClose, &QAction::triggered, this, &ISObjectFormBase::SaveClose);
	ToolBar->addAction(ActionSaveClose);

	//��������� ��������
	ActionSave = ISControls::CreateActionSave(ToolBar);
	ActionSave->setToolTip(LANG("SaveChange"));
	connect(ActionSave, &QAction::triggered, this, &ISObjectFormBase::Save);
	ToolBar->addAction(ActionSave);

	//������� ��������
	ActionClose = ISControls::CreateActionClose(ToolBar);
	ActionClose->setToolTip(LANG("CloseCard"));
	connect(ActionClose, &QAction::triggered, this, &ISObjectFormBase::close);
	ToolBar->addAction(ActionClose);

	ActionAdditionally = new QAction(ToolBar);
	ActionAdditionally->setText(LANG("Additionally"));
	ActionAdditionally->setToolTip(LANG("Additionally"));
	ActionAdditionally->setIcon(BUFFER_ICONS("AdditionallyActions"));
	ActionAdditionally->setPriority(QAction::LowPriority);
	ActionAdditionally->setMenu(new QMenu(ToolBar));
	ToolBar->addAction(ActionAdditionally);

	QToolButton *ButtonAdditionally = dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionAdditionally));
	ButtonAdditionally->setPopupMode(QToolButton::InstantPopup);
	ButtonAdditionally->setCursor(CURSOR_POINTING_HAND);
	ButtonAdditionally->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));

	//��������� ����������
	ActionSystemInfo = ISControls::CreateActionRecordInformartion(this);
	ActionSystemInfo->setPriority(QAction::LowPriority);
	connect(ActionSystemInfo, &QAction::triggered, this, &ISObjectFormBase::ShowSystemInfo);
	AddActionMenu(ActionSystemInfo);

	//���������
	ActionFavorites = new QAction(ToolBar);
	ActionFavorites->setText(LANG("AddToFavorites"));
	ActionFavorites->setToolTip(LANG("AddToFavorites"));
	ActionFavorites->setIcon(BUFFER_ICONS("Favorites"));
	ActionFavorites->setPriority(QAction::LowPriority);
	ActionFavorites->setCheckable(true);
	connect(ActionFavorites, &QAction::triggered, this, &ISObjectFormBase::AddFavoite);
	AddActionToolBar(ActionFavorites);

	//�������/������������ ��������
	ActionDelete = ISControls::CreateActionDelete(ToolBar);
	ActionDelete->setPriority(QAction::LowPriority);
	connect(ActionDelete, &QAction::triggered, this, &ISObjectFormBase::Delete);
	AddActionMenu(ActionDelete);

	ActionDeleteCascade = ISControls::CreateActionDeleteCascade(ToolBar);
	ActionDeleteCascade->setPriority(QAction::LowPriority);
	connect(ActionDeleteCascade, &QAction::triggered, this, &ISObjectFormBase::DeleteCascade);
	AddActionMenu(ActionDeleteCascade);

	//�������� ���������
	ActionCancelChange = new QAction(ToolBar);
	ActionCancelChange->setEnabled(false);
	ActionCancelChange->setText(LANG("CancelChanged"));
	ActionCancelChange->setToolTip(LANG("CancelChanged"));
	ActionCancelChange->setIcon(BUFFER_ICONS("CancelChangedObject"));
	ActionCancelChange->setPriority(QAction::LowPriority);
	connect(ActionCancelChange, &QAction::triggered, this, &ISObjectFormBase::CancelChanged);
	AddActionMenu(ActionCancelChange);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateWidgetObject()
{
	QVBoxLayout *LayoutObject = new QVBoxLayout();
	LayoutObject->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	WidgetObject = new QWidget(this);
	WidgetObject->setLayout(LayoutObject);
	WidgetObject->setSizePolicy(WidgetObject->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	GetMainLayout()->addWidget(WidgetObject);

	LabelIsDeleted = new QLabel(this);
	LabelIsDeleted->setVisible(false);
	LabelIsDeleted->setStyleSheet(STYLE_SHEET("QLabel.Color.Red"));
	LabelIsDeleted->setText(LANG("RecordMarkerIsDeleted"));
	LabelIsDeleted->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	ISGui::SetFontWidgetUnderline(LabelIsDeleted, true);
	LayoutObject->addWidget(LabelIsDeleted);

	FormLayout = new QFormLayout();

	ScrollArea = new ISScrollArea(WidgetObject); //������ �������� (�������) ������ �����
	ScrollArea->widget()->setLayout(FormLayout);
	LayoutObject->addWidget(ScrollArea);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateFieldsWidget()
{
	//���� ��������� ���������� ���� � �����
	if (SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		QLabel *LabelSystemInfoObject = new QLabel(this);
		LabelSystemInfoObject->setText(LANG("SystemField.ID") + ':');
		LabelSystemInfoObject->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

		EditObjectID = new ISLineEdit(this);
		EditObjectID->SetValue(LANG("SystemField.ID.NotAssigned"));
		EditObjectID->SetToolTip(LANG("AutoFillField"));
		EditObjectID->SetReadOnly(true); //������������������ setReadOnly � setCursor ������ ���� ������ �����, ����� ������ �� ��������
		EditObjectID->SetCursor(CURSOR_WHATS_THIS); //������������������ setReadOnly � setCursor ������ ���� ������ �����, ����� ������ �� ��������
		EditObjectID->SetVisibleClear(false);
		EditObjectID->setSizePolicy(QSizePolicy::Maximum, EditObjectID->sizePolicy().verticalPolicy());
		FormLayout->addRow(LabelSystemInfoObject, EditObjectID);
	}

	for (PMetaField *MetaField : MetaTable->Fields) //����� �����
	{
		if (!MetaField->QueryText.isEmpty()) //���� ���� �������� �������� - ���������� ���
		{
			continue;
		}

		//���� ��� ���� ByteArray � ��� ���� �� ������������ ������ �������������� - ���������� ���
		if (MetaField->Type == ISNamespace::FT_ByteArray && MetaField->ControlWidget.isEmpty())
		{
			continue;
		}

		if (!MetaField->LayoutName.isEmpty()) //���� ���� ������ ���� � ������������� ������������
		{
			if (!Layouts.count(MetaField->LayoutName))
			{
				QHBoxLayout *LayoutHorizontal = new QHBoxLayout();
				LayoutHorizontal->setProperty("Inserted", false);
				Layouts.emplace(MetaField->LayoutName, LayoutHorizontal);
			}
		}

		ISFieldEditBase *FieldEditBase = CreateColumnForField(MetaField);
		if (dynamic_cast<ISListEdit*>(FieldEditBase))
		{
			dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
		}
		AddColumnForField(MetaField, FieldEditBase, FormLayout);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::FillDataFields()
{
	if (FormType == ISNamespace::OFT_Edit || FormType == ISNamespace::OFT_Copy)
	{
		ObjectName = ISCore::GetObjectName(MetaTable, ObjectID);

		//���������� �������
		ISQueryModel QueryModel(MetaTable, ISNamespace::QMT_Object);
		QueryModel.SetClassFilter(MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + "_id = " + QString::number(ObjectID));

		//���������� �������
		ISQuery qSelect(QueryModel.GetQueryText());
		if (qSelect.ExecuteFirst())
		{
			if (qSelect.GetCountResultRows() == 1)
			{
				QSqlRecord SqlRecord = qSelect.GetRecord();
				RecordIsDeleted = SqlRecord.value("IsDeleted").toBool();
				LabelIsDeleted->setVisible(RecordIsDeleted);

				if (FormType == ISNamespace::OFT_Edit)
				{
					SetValueFieldID(ObjectID);
				}

				for (int i = 0; i < SqlRecord.count(); ++i) //����� ����� ������
				{
					QString FieldName = SqlRecord.fieldName(i);
					QVariant Value = SqlRecord.value(FieldName);
					if (Value.isNull()) //���� �������� ������, ������� �� ��������� ��� �����
					{
						BeginValues.insert(FieldName, QVariant());
						continue;
					}

					if (FieldsMap.count(FieldName))
					{
						ISFieldEditBase *FieldEditWidget = FieldsMap[FieldName];
						disconnect(FieldEditWidget, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);

						if (MetaTable->GetField(FieldName)->Foreign)
						{
							QVariant ListObjectID = ISDatabaseHelper::GetObjectIDToList(MetaTable, MetaTable->GetField(FieldName), ObjectID);
							FieldEditWidget->SetValue(ListObjectID);
							BeginValues.insert(FieldName, ListObjectID);
						}
						else
						{
							FieldEditWidget->SetValue(Value);
							BeginValues.insert(FieldName, Value);
						}

						//���� ����� ����������� ��� �������� �����
						FieldEditWidget->SetModificationFlag(FormType == ISNamespace::OFT_Copy);
						connect(FieldEditWidget, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
					}
				}
				ISCore::AddHistory(MetaTable->Name, MetaTable->LocalListName, ObjectName, ObjectID);
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.QueryRecordIsNull"));
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.QueryRecord"), qSelect.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISObjectFormBase::CreateColumnForField(PMetaField *MetaField)
{
	ISFieldEditBase	*FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
	FieldsMap.emplace(MetaField->Name, FieldEditBase);
	connect(FieldEditBase, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddColumnForField(PMetaField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *FormLayout)
{
	QLabel *LabelField = new QLabel(this);
	LabelField->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	LabelsMap.emplace(MetaField->Name, LabelField);

	if (MetaField->DefaultValueWidget.isValid()) //���� � ���� ������� �������� �� ��������� ��� �������-���������
	{
		FieldEditBase->SetValue(MetaField->DefaultValueWidget);
		FieldEditBase->SetModificationFlag(true);
	}

	if (MetaField->NotNull && MetaField->DefaultValue.toString().isEmpty()) //���� ���� ����������� ��� ����������
	{
		LabelField->setTextFormat(Qt::RichText);
		LabelField->setText(QString("<font>%1:</font><font color=#FF0000 size=4>*</font>").arg(MetaField->LabelName));
		LabelField->setToolTip(LANG("FieldNotNull"));
		LabelField->setCursor(CURSOR_WHATS_THIS);
	}
	else if (MetaField->ReadOnly) //���� �������� ������ ��� ���������
	{
		LabelField->setTextFormat(Qt::RichText);
		LabelField->setText(QString("<font>%1:</font><font color=#808080 size=4>*</font>").arg(MetaField->LabelName));
		LabelField->setToolTip(LANG("FieldReadOnly"));
		LabelField->setCursor(CURSOR_WHATS_THIS);
		FieldEditBase->SetReadOnly(MetaField->ReadOnly);
	}
	else
	{
		LabelField->setText(MetaField->LabelName + ':');
	}

	if (!MetaField->PlaceholderText.isEmpty()) //���� ���� �����-�����������
	{
		FieldEditBase->SetPlaceholderText(MetaField->PlaceholderText);
	}

	if (!MetaField->Hint.isEmpty()) //���� ���� ��������� ��� ����
	{
		FieldEditBase->CreateHint(MetaField->Hint);
	}

	if (!MetaField->RegExp.isEmpty()) //���� � ���� ��������� ���������� ���������
	{
		FieldEditBase->SetRegExp(MetaField->RegExp);
	}

	if (MetaField->HideFromObject) //���� ���� ����� ������ - ������� �� ������ � ������ ��� �� ��������� �� �����
	{
		SetVisibleField(MetaField->Name, false);
		return;
	}
	
	if (!BeginFieldEdit && !MetaField->ReadOnly)
	{
		BeginFieldEdit = FieldEditBase;
	}

	if (!MetaField->SeparatorName.isEmpty())
	{
		QLabel *LabelTab = new QLabel(this);
		LabelTab->setText(MetaField->SeparatorName);
		LabelTab->setFont(ISDefines::Gui::FONT_TAHOMA_10_BOLD);

		QHBoxLayout *LayoutLine = new QHBoxLayout();
		LayoutLine->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

		QWidget *WidgetLine = new QWidget(this);
		WidgetLine->setLayout(LayoutLine);
		LayoutLine->addWidget(ISControls::CreateHorizontalLinePlain(this));

		FormLayout->addRow(LabelTab, WidgetLine);
	}

	QHBoxLayout *LayoutHorizontal = Layouts[MetaField->LayoutName];
	if (LayoutHorizontal)
	{
		if (LayoutHorizontal->property("Inserted").toBool())
		{
			LayoutHorizontal->addWidget(LabelField);
			LayoutHorizontal->addWidget(FieldEditBase);
		}
		else
		{
			LayoutHorizontal->setProperty("Inserted", true);
			FormLayout->addRow(LabelField, LayoutHorizontal);
			LayoutHorizontal->addWidget(FieldEditBase);
		}
	}
	else
	{
		FormLayout->addRow(LabelField, FieldEditBase);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ToolBarClicked(QAction *ActionClicked)
{
	//������� ��� ��������� ��������
	for (QAction *Action: ToolBarEscort->actions())
	{
		if (Action == ActionClicked) //������� ��������
		{
			if (Action->property("IsChecked").toBool()) //���� ��� ��� �������� - ������� �� �������
			{
				return;
			}
			else //����� - ������������� ������ �����
			{
				Action->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
				Action->setProperty("IsChecked", true);
			}
		}
		else //������ �������� - ��������� ��������� � ������������� ������� �����
		{
			Action->setFont(ISDefines::Gui::FONT_APPLICATION);
			Action->setChecked(false);
			Action->setProperty("IsChecked", false);
		}
	}

	if (WidgetEscort)
	{
		delete WidgetEscort;
		WidgetEscort = nullptr;
	}

	bool IsObjectClicked = ActionClicked->property("IsObject").toBool();
	ToolBar->setVisible(IsObjectClicked);
	WidgetObject->setVisible(IsObjectClicked);

	if (!IsObjectClicked)
	{
		QString TableName = ActionClicked->property("TableName").toString(),
			ClassName = ActionClicked->property("ClassName").toString(),
			ClassFilter = ActionClicked->property("ClassFilter").toString();
		if (!ClassName.isEmpty()) //�������� �������
		{
			WidgetEscort = ISAlgorithm::CreatePointer<ISInterfaceMetaForm *>(ClassName, Q_ARG(QWidget *, this));
		}
		else //�������� �������
		{
			ISListObjectForm *ListObjectForm = new ISListObjectForm(TableName, ObjectID, this);
			ListObjectForm->SetUID(ISMetaData::Instance().GetMetaTable(TableName)->UID);
			if (!ClassFilter.isEmpty())
			{
				ListObjectForm->GetQueryModel()->SetClassFilter(ClassFilter);
			}
			WidgetEscort = ListObjectForm;
		}
		connect(WidgetEscort, &ISInterfaceMetaForm::AddFormFromTab, &ISGui::ShowObjectForm);
		GetMainLayout()->addWidget(WidgetEscort);

		WidgetEscort->SetParentTableName(MetaTable->Name);
		WidgetEscort->SetParentObjectID(ObjectID);
		WidgetEscort->LoadData();
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetValueFieldID(int object_id)
{
	if (SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		EditObjectID->SetValue(object_id);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SaveClose()
{
	if (ModificationFlag)
	{
		if (Save())
		{
			close();
		}
	}
	else
	{
		close();
	}
	emit CloseTab(CurrentIndexTab);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SaveBefore()
{

}
//-----------------------------------------------------------------------------
bool ISObjectFormBase::Save()
{
	SaveBefore();
	QVariantMap ValuesMap;
	ISVectorString FieldsVector;
	QString QueryText;

	for (const auto &Field : FieldsMap) //����� ������������ ����� �� �����
	{
		QString FieldName = Field.first; //��� ����
		PMetaField *MetaField = MetaTable->GetField(FieldName); //����-����
		ISFieldEditBase *FieldEditBase = Field.second; //��������� �� ������ �������������� ����
		QVariant Value = FieldEditBase->GetValue(); //�������� ����
		if (Value.isNull()) //���� �������� � ���� �����������, ��������� ����������� �� ���� ��� ����������
		{
			if (MetaField->NotNull && !MetaField->HideFromObject && MetaField->DefaultValue.toString().isEmpty()) //���� ���� ����������� ��� ����������
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(MetaField->LabelName));
				FieldEditBase->BlinkRed();
				return false;
			}
		}

		if (!FieldEditBase->GetModificationFlag()) //���� �������� ���� ������������� �� ����������, ���������� � ����������
		{
			if (MetaTable->ClassFilterField != FieldName) //���� ������� ���� �� �������� ����������� (������� ������� �� �������� ���������)
			{
				continue;
			}
		}

		if (!FieldEditBase->IsValid()) //���� ���� �� ������ ���������
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ValueFieldEditInvalid").arg(MetaField->LabelName));
			FieldEditBase->BlinkRed();
			return false;
		}

		FieldEditBase->Invoke();
		ValuesMap.insert(FieldName, Value);
		FieldsVector.emplace_back(FieldName);
	}

	//������������ ������� �� ����������/���������/�����������
	if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
	{
		QString InsertFields = "INSERT INTO " + MetaTable->Name + " (";
		QString InsertValues = "VALUES (";

		for (const QString &String : FieldsVector)
		{
			InsertFields += MetaTable->Alias + '_' + String + ", ";
			InsertValues += ':' + String + ", ";
		}

		InsertFields.chop(2);
		InsertValues.chop(2);

		QueryText += InsertFields + ") \n";
		QueryText += InsertValues + ") \n";
		QueryText += "RETURNING " + MetaTable->Alias + "_id";
	}
	else if (FormType == ISNamespace::OFT_Edit)
	{
		QueryText += "UPDATE " + MetaTable->Name + " SET \n";

		//���������� ����� ����������� � ������������� � ������� ����
		QueryText += MetaTable->Alias + "_updationdate = now(), \n";
		QueryText += MetaTable->Alias + "_updationuser = CURRENT_USER, \n";

		for (const QString &String : FieldsVector)
		{
			QueryText += MetaTable->Alias + '_' + String + " = :" + String + ", \n";
		}

		QueryText.chop(3);
		QueryText += " \n";
		QueryText += "WHERE " + MetaTable->Alias + "_id = " + QString::number(ObjectID);
	}

	ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).transaction(); //�������� ����������
	SavedEvent();

	//���������� ������� ����������
	ISQuery SqlQuery(QueryText);
	for (const auto &Value : ValuesMap.toStdMap())
	{
		IS_ASSERT(SqlQuery.BindValue(':' + Value.first, Value.second), "Not bind value");
	}

	if (SqlQuery.Execute()) //������ �������� �������
	{
		ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).commit(); //������ ����������
		if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
		{
			IS_ASSERT(SqlQuery.First(), "Not first SqlQuery");
			ObjectID = SqlQuery.ReadColumn(MetaTable->Alias + "_id").toInt();
		}

		ObjectName = ISCore::GetObjectName(MetaTable, ObjectID);
		switch (FormType)
		{
		case ISNamespace::OFT_New:
			FormType = ISNamespace::OFT_Edit;
			ISNotificationService::ShowNotification(ISNamespace::NFT_Create, MetaTable->LocalName, ObjectName);
			ISProtocol::CreateObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
			break;
		case ISNamespace::OFT_Copy:
			FormType = ISNamespace::OFT_Edit;
			ISNotificationService::ShowNotification(ISNamespace::NFT_CreateCopy, MetaTable->LocalName, ObjectName);
			ISProtocol::CreateCopyObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
			break;
		case ISNamespace::OFT_Edit:
			ISNotificationService::ShowNotification(ISNamespace::NFT_Edit, MetaTable->LocalName, ObjectName);
			ISProtocol::EditObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
			break;
		}

		RenameReiconForm();
		SetModificationFlag(false);
		UpdateObjectActions();
		ActionFavorites->setEnabled(true);
		SetValueFieldID(ObjectID);

		for (QAction *Action : ToolBarEscort->actions())
		{
			Action->setEnabled(true);
		}

		SaveAfter();
		emit SavedObject(ObjectID);
		emit UpdateList();
		return true;
	}
	else
	{
		ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).rollback(); //����� ����������
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ErrorQuerySQL"), LANG("Message.Error.ErrorQuerySQL.Details").arg(SqlQuery.GetErrorString()).arg(SqlQuery.GetSqlText()));
	}
	return false;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SavedEvent()
{
	
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SaveAfter()
{

}
//-----------------------------------------------------------------------------
void ISObjectFormBase::RenameReiconForm()
{
	switch (FormType)
	{
	case ISNamespace::OFT_New:
		setWindowTitle(LANG("Creating") + " (" + MetaTable->LocalName + ')');
		setWindowIcon(BUFFER_ICONS("Add"));
		break;
	case ISNamespace::OFT_Edit:
		setWindowTitle(MetaTable->LocalName + ": " + ObjectName);
		setWindowIcon(BUFFER_ICONS("Edit"));
		break;
	case ISNamespace::OFT_Copy:
		setWindowTitle(LANG("Coping") + " (" + MetaTable->LocalName + "): " + ObjectName);
		setWindowIcon(BUFFER_ICONS("AddCopy"));
		break;
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::DataChanged()
{
	SetModificationFlag(true);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetModificationFlag(bool modification)
{
	ModificationFlag = modification;
	ActionSave->setEnabled(ModificationFlag);
	ActionCancelChange->setEnabled(ModificationFlag);

	QString WindowTitle = windowTitle();
	if (ModificationFlag)
	{
		if (!WindowTitle.contains(ISDefines::Core::SYMBOL_OBJECT_CHANGED))
		{
			WindowTitle += ISDefines::Core::SYMBOL_OBJECT_CHANGED;
		}
	}
	else
	{
		if (WindowTitle.contains(ISDefines::Core::SYMBOL_OBJECT_CHANGED))
		{
			WindowTitle.remove(ISDefines::Core::SYMBOL_OBJECT_CHANGED);
		}
	}
	setWindowTitle(WindowTitle);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ResizeRemove()
{
	if (!parentWidget())
	{
		QRect Rect = QDesktopWidget().availableGeometry();
		int Width = Rect.width() / 3;
		Width *= 2;
		int Height = Rect.height() / 3;
		Height *= 2;

		resize(Width, Height);
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::UpdateObjectActions()
{
	if (FormType == ISNamespace::OFT_New)
	{
		SetEnabledActions(false);
	}
	else if (FormType == ISNamespace::OFT_Copy)
	{
		SetEnabledActions(false);
		ActionSave->setEnabled(true);
	}
	else if (FormType == ISNamespace::OFT_Edit)
	{
		SetEnabledActions(true);
		ActionSave->setEnabled(false);
		ActionFavorites->setChecked(ISFavorites::GetInstance().CheckExistFavoriteObject(MetaTable->Name, ObjectID));
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ShowSystemInfo()
{
	ISGui::ShowSystemInfoRecord(MetaTable, ObjectID);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddFavoite()
{
	bool IsExist = ISFavorites::GetInstance().CheckExistFavoriteObject(MetaTable->Name, ObjectID);
	IsExist ? ISFavorites::GetInstance().DeleteFavorite(MetaTable->Name, ObjectID) : ISFavorites::GetInstance().AddFavorite(MetaTable->Name, MetaTable->LocalListName, ObjectName, ObjectID);
	ActionFavorites->setCheckable(!IsExist);
	IsExist ? ISNotificationService::ShowNotification(LANG("RecordRemoveFavorites").arg(ObjectName)) : ISNotificationService::ShowNotification(LANG("RecordAddFavorites").arg(ObjectName));
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::Delete()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	if (RecordIsDeleted)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.RecoveryThisRecord")))
		{
			if (ISGui::DeleteOrRecoveryObject(ISNamespace::DRO_Recovery, MetaTable->Name, MetaTable->Alias, GetObjectID(), MetaTable->LocalListName))
			{
				emit UpdateList();
				close();
			}
		}
	}
	else
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteThisRecord")))
		{
			if (ISGui::DeleteOrRecoveryObject(ISNamespace::DRO_Delete, MetaTable->Name, MetaTable->Alias, GetObjectID(), MetaTable->LocalListName))
			{
				emit UpdateList();
				close();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::DeleteCascade()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete.Cascade"), LANG("Message.Object.Delete.Cascade.Help")))
	{
		if (ISGui::DeleteCascadeObject(MetaTable->Name, MetaTable->Alias, GetObjectID()))
		{
			ISNotificationService::ShowNotification(LANG("NotificationForm.Title.Deleted.Cascade").arg(GetObjectID()));
			ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
			
			emit UpdateList();
			close();
		}
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CancelChanged()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CancelChanged")))
	{
		ISGui::SetWaitGlobalCursor(true);

		if (FormType == ISNamespace::OFT_New)
		{
			for (const auto &MapItem : FieldsMap)
			{
				ISFieldEditBase *FieldEditBase = MapItem.second;
				if (FieldEditBase->GetModificationFlag())
				{
					FieldEditBase->Clear();
				}
			}
		}

		for (const auto &FieldItem : BeginValues.toStdMap()) //����� ���� �������������� ��������
		{
			QString FieldName = FieldItem.first;
			QVariant BeginValue = FieldItem.second;
			if (!MetaTable->GetField(FieldName)->QueryText.isEmpty()) //���� ���� �������� ����-��������
			{
				continue;
			}

			QVariant CurrentValue = GetFieldValue(FieldName);
			if (CurrentValue != BeginValue) //���� ������� �������� ���� �� ����� ���������� - �������� �������� ���� �� ���������
			{
				GetFieldWidget(FieldName)->SetValue(BeginValue);
			}
		}

		ISGui::SetWaitGlobalCursor(false);
		SetModificationFlag(false);
		BeginFieldEdit->SetFocus();
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddActionToolBar(QAction *Action, bool AddingToActionGroup)
{
	ToolBar->addAction(Action);
	if (AddingToActionGroup)
	{
		ActionGroup->addAction(Action);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddActionMenu(QAction *Action, bool AddingToActionGroup)
{
	ActionAdditionally->menu()->addAction(Action);
	if (AddingToActionGroup)
	{
		ActionGroup->addAction(Action);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetEnabledActions(bool Enabled)
{
	for (QAction *Action : ActionGroup->actions())
	{
		Action->setEnabled(Enabled);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddWidgetToBottom(QWidget *Widget)
{
	WidgetObject->layout()->addWidget(Widget);
}
//-----------------------------------------------------------------------------
QString ISObjectFormBase::GetObjectName() const
{
	return ObjectName;
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISObjectFormBase::GetFieldWidget(const QString &FieldName)
{
	return FieldsMap[FieldName];
}
//-----------------------------------------------------------------------------
QToolBar* ISObjectFormBase::GetToolBar()
{
	return ToolBar;
}
//-----------------------------------------------------------------------------
