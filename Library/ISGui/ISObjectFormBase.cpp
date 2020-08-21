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
#include "ISPopupMessage.h"
#include "ISProtocol.h"
#include "ISListBaseForm.h"
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
void ISObjectFormBase::SetParentObjectID(int parent_object_id, const QString &parent_filter_field)
{
	ParentObjectID = parent_object_id;
	ParentFilterField = parent_filter_field;
	if (FieldsMap.count(parent_filter_field))
	{
		FieldsMap[parent_filter_field]->SetValue(parent_object_id);
	}
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

	//По умолчанию все действия навигации эскортов должны быть отключены (кроме первого действия - карточки)
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

	//Действие объекта
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
	ActionProtocol->setProperty("ClassName", "ISProtocolObjectListForm");
	ActionGroupEscort->addAction(ActionProtocol);

	QAction *ActionDiscussion = ToolBarEscort->addAction(BUFFER_ICONS("Discussion"), LANG("Discussion"));
	ActionDiscussion->setCheckable(true);
	ActionDiscussion->setProperty("ClassName", "ISDiscussionListForm");
	ActionGroupEscort->addAction(ActionDiscussion);

	ToolBarEscort->addSeparator();

	for (PMetaEscort *MetaEscort : MetaTable->Escorts) //Обход эскортных мета-таблиц
	{		
		QAction *ActionEscort = ToolBarEscort->addAction(BUFFER_ICONS("Table"), MetaEscort->LocalName);
		ActionEscort->setCheckable(true);
		ActionEscort->setProperty("TableName", MetaEscort->TableName);
		ActionEscort->setProperty("ClassName", MetaEscort->ClassName);
		ActionEscort->setProperty("FilterField", MetaEscort->FilterField);
		ActionGroupEscort->addAction(ActionEscort);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateToolBar()
{
	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	//Сохранить и закрыть карточку
	ActionSaveClose = ISControls::CreateActionSaveAndClose(ToolBar);
	ActionSaveClose->setToolTip(LANG("SaveChangeClose"));
	connect(ActionSaveClose, &QAction::triggered, this, &ISObjectFormBase::SaveClose);
	ToolBar->addAction(ActionSaveClose);

	//Сохранить карточку
	ActionSave = ISControls::CreateActionSave(ToolBar);
	ActionSave->setToolTip(LANG("SaveChange"));
	connect(ActionSave, &QAction::triggered, this, &ISObjectFormBase::Save);
	ToolBar->addAction(ActionSave);

	//Закрыть карточку
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

	//Системная информация
	ActionSystemInfo = ISControls::CreateActionRecordInformartion(this);
	ActionSystemInfo->setPriority(QAction::LowPriority);
	connect(ActionSystemInfo, &QAction::triggered, this, &ISObjectFormBase::ShowSystemInfo);
	AddActionMenu(ActionSystemInfo);

	//Избранное
	ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("AddToFavorites"), ToolBar);
	ActionFavorites->setPriority(QAction::LowPriority);
	ActionFavorites->setCheckable(true);
	connect(ActionFavorites, &QAction::triggered, this, &ISObjectFormBase::AddFavoite);
	AddActionToolBar(ActionFavorites);

	//Удалить/восстановить карточку
	ActionDelete = ISControls::CreateActionDelete(ToolBar);
	ActionDelete->setPriority(QAction::LowPriority);
	connect(ActionDelete, &QAction::triggered, this, &ISObjectFormBase::Delete);
	AddActionMenu(ActionDelete);

	//Удалить каскадом
	ActionDeleteCascade = ISControls::CreateActionDeleteCascade(ToolBar);
	ActionDeleteCascade->setPriority(QAction::LowPriority);
	connect(ActionDeleteCascade, &QAction::triggered, this, &ISObjectFormBase::DeleteCascade);
	AddActionMenu(ActionDeleteCascade);

	//Отменить изменения
	ActionCancelChange = new QAction(BUFFER_ICONS("CancelChangedObject"), LANG("CancelChanged"), ToolBar);
	ActionCancelChange->setEnabled(false);
	ActionCancelChange->setPriority(QAction::LowPriority);
	connect(ActionCancelChange, &QAction::triggered, this, &ISObjectFormBase::CancelChanged);
	AddActionMenu(ActionCancelChange);

	//Примечание записи
	QAction *ActionNoteObject = ISControls::CreateActionNoteObject(ToolBar);
	ActionNoteObject->setPriority(QAction::LowPriority);
	connect(ActionNoteObject, &QAction::triggered, this, &ISObjectFormBase::NoteObject);
	AddActionMenu(ActionNoteObject);
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
	LabelIsDeleted->setMargin(10);
	ISGui::SetFontWidgetUnderline(LabelIsDeleted, true);
	LayoutObject->addWidget(LabelIsDeleted);

	FormLayout = new QFormLayout();

	ScrollArea = new ISScrollArea(WidgetObject); //Панель основной (главной) группы полей
	ScrollArea->widget()->setLayout(FormLayout);
	LayoutObject->addWidget(ScrollArea);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateFieldsWidget()
{
	//Если настроено добавление поле с кодом
	if (SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		QLabel *LabelSystemInfoObject = new QLabel(this);
		LabelSystemInfoObject->setText(LANG("SystemField.ID") + ':');
		LabelSystemInfoObject->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

		EditObjectID = new ISLineEdit(this);
		EditObjectID->SetValue(LANG("SystemField.ID.NotAssigned"));
		EditObjectID->SetToolTip(LANG("AutoFillField"));
		EditObjectID->SetReadOnly(true); //Последовательность setReadOnly и setCursor должна быть именно такой, иначе курсор не меняется
		EditObjectID->SetCursor(CURSOR_WHATS_THIS); //Последовательность setReadOnly и setCursor должна быть именно такой, иначе курсор не меняется
		EditObjectID->SetVisibleClear(false);
		EditObjectID->setSizePolicy(QSizePolicy::Maximum, EditObjectID->sizePolicy().verticalPolicy());
		FormLayout->addRow(LabelSystemInfoObject, EditObjectID);
	}

	for (PMetaField *MetaField : MetaTable->Fields) //Обход полей
	{
		if (!MetaField->QueryText.isEmpty()) //Если поле является запросом - пропускать его
		{
			continue;
		}

		//Если тип поля ByteArray и для него не предусмотрен виджет редактирования - пропускать его
		if (MetaField->Type == ISNamespace::FT_ByteArray && MetaField->ControlWidget.isEmpty())
		{
			continue;
		}

		if (!MetaField->LayoutName.isEmpty()) //Если поле должно быть в горизональном компоновщике
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

		//Подготовка запроса
		ISQueryModel QueryModel(MetaTable, ISNamespace::QMT_Object);
		QueryModel.SetClassFilter(MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + "_id = " + QString::number(ObjectID));

		//Выполнение запроса
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

				for (int i = 0; i < SqlRecord.count(); ++i) //Обход полей записи
				{
					QString FieldName = SqlRecord.fieldName(i);
					QVariant Value = SqlRecord.value(FieldName);
					if (Value.isNull()) //Если значение пустое, перейти на следующий шаг цикла
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

						//Если формы открывается для создания копии
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
void ISObjectFormBase::AddColumnForField(PMetaField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *form_layout)
{
	QLabel *LabelField = new QLabel(this);
	LabelField->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	LabelsMap.emplace(MetaField->Name, LabelField);

	if (MetaField->DefaultValueWidget.isValid()) //Если в поле указано значение по умолчанию для виджета-редактора
	{
		FieldEditBase->SetValue(MetaField->DefaultValueWidget);
		FieldEditBase->SetModificationFlag(true);
	}

	if (MetaField->ReadOnly) //Если доступно только для просмотра
	{
		LabelField->setTextFormat(Qt::RichText);
		LabelField->setText(QString("<font>%1:</font><font color=#3D77B2 size=4>*</font>").arg(MetaField->LabelName));
		LabelField->setToolTip(LANG("FieldReadOnly"));
		LabelField->setCursor(CURSOR_WHATS_THIS);
		FieldEditBase->SetReadOnly(MetaField->ReadOnly);
	}
	else if (MetaField->NotNull) //Если поле обязательно для заполнения
	{
		LabelField->setTextFormat(Qt::RichText);
		LabelField->setText(QString("<font>%1:</font><font color=#FF0000 size=4>*</font>").arg(MetaField->LabelName));
		LabelField->setToolTip(LANG("FieldNotNull"));
		LabelField->setCursor(CURSOR_WHATS_THIS);
	}
	else
	{
		LabelField->setText(MetaField->LabelName + ':');
	}

	if (!MetaField->PlaceholderText.isEmpty()) //Если есть текст-заполнитель
	{
		FieldEditBase->SetPlaceholderText(MetaField->PlaceholderText);
	}

	if (!MetaField->Hint.isEmpty()) //Если есть подсказка для поля
	{
		FieldEditBase->CreateHint(MetaField->Hint);
	}

	if (!MetaField->RegExp.isEmpty()) //Если у поля настроено регулярное выражение
	{
		FieldEditBase->SetRegExp(MetaField->RegExp);
	}

	if (MetaField->HideFromObject) //Если поле нужно скрыть - выходим из метода и просто его не добавляем на форму
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

		form_layout->addRow(LabelTab, WidgetLine);
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
			form_layout->addRow(LabelField, LayoutHorizontal);
			LayoutHorizontal->addWidget(FieldEditBase);
		}
	}
	else
	{
		form_layout->addRow(LabelField, FieldEditBase);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ToolBarClicked(QAction *ActionClicked)
{
	//Обходим все эскортные действия
	for (QAction *Action: ToolBarEscort->actions())
	{
		if (Action == ActionClicked) //Текущее действие
		{
			if (Action->property("IsChecked").toBool()) //Если оно уже выделено - выходим из фукнции
			{
				return;
			}
			else //Иначе - устанавливаем жирный шрифт
			{
				Action->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
				Action->setProperty("IsChecked", true);
			}
		}
		else //Другое действие - отключаем выдлеение и устанавливаем обычный шрифт
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
			parent_filter_field = ActionClicked->property("FilterField").toString(),
			ClassName = ActionClicked->property("ClassName").toString(),
			ClassFilter = ActionClicked->property("ClassFilter").toString();
		if (ClassName.isEmpty()) //Открытие таблицы
		{
			ISListBaseForm *ListBaseForm = new ISListBaseForm(TableName, this);
			if (!ClassFilter.isEmpty())
			{
				ListBaseForm->GetQueryModel()->SetClassFilter(ClassFilter);
			}
			WidgetEscort = ListBaseForm;
		}
		else //Открытие виджета
		{
			WidgetEscort = ISAlgorithm::CreatePointer<ISInterfaceMetaForm *>(ClassName, Q_ARG(QWidget *, this));
		}
		WidgetEscort->SetParentObjectID(ObjectID);
		WidgetEscort->SetParentFilterField(parent_filter_field);
		WidgetEscort->SetParentTableName(MetaTable->Name);
		connect(WidgetEscort, &ISInterfaceMetaForm::AddFormFromTab, &ISGui::ShowObjectForm);
		GetMainLayout()->addWidget(WidgetEscort);
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
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SaveBefore()
{

}
//-----------------------------------------------------------------------------
bool ISObjectFormBase::Save()
{
	SaveBefore();
	ISStringToVariantMap ValuesMap;
	ISVectorString FieldsVector;
	QString QueryText;

	for (const auto &Field : FieldsMap) //Обход существующих полей на форме
	{
		QString FieldName = Field.first; //Имя поля
		PMetaField *MetaField = MetaTable->GetField(FieldName); //Мета-поле
		ISFieldEditBase *FieldEditBase = Field.second; //Указатель на виджет редактирования поля
		QVariant Value = FieldEditBase->GetValue(); //Значение поля
		if (Value.isNull()) //Если значение в поле отсутствует, проверить обязательно ли поле для заполнения
		{
			if (MetaField->NotNull && !MetaField->HideFromObject && MetaField->DefaultValue.toString().isEmpty()) //Если поле обязательно для заполнения
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(MetaField->LabelName));
				FieldEditBase->BlinkRed();
				return false;
			}
		}

		if (!FieldEditBase->GetModificationFlag()) //Если значения поля редактированя не изменялось, переходить к следующему
		{
			if (ParentFilterField != FieldName) //Если текущее поле не является фильтруемым (текущая таблица не является эскортной)
			{
				continue;
			}
		}

		if (!FieldEditBase->IsValid()) //Если поле не прошло валидацию
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ValueFieldEditInvalid").arg(MetaField->LabelName));
			FieldEditBase->BlinkRed();
			return false;
		}

		FieldEditBase->Invoke();
		ValuesMap[FieldName] = Value;
		FieldsVector.emplace_back(FieldName);
	}

	//Формирование запроса на добавление/изменение/копирование
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

		//Заполнение полей информацией о опользователе и текущей дате
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

	ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).transaction(); //Открытие транзакции
	SavedEvent();

	//Заполнение запроса значениями
	ISQuery SqlQuery(QueryText);
	for (const auto &Value : ValuesMap)
	{
		IS_ASSERT(SqlQuery.BindValue(':' + Value.first, Value.second), "Not bind value");
	}

	if (SqlQuery.Execute()) //Запрос выполнен успешно
	{
		ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).commit(); //Коммит транзакции
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
			ISProtocol::CreateObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
			if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
			{
				ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Created") + " - " + MetaTable->LocalName.toLower() + ':', ObjectName);
			}
			break;
		case ISNamespace::OFT_Copy:
			FormType = ISNamespace::OFT_Edit;
			ISProtocol::CreateCopyObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
			if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
			{
				ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.CreatedCopy") + " - " + MetaTable->LocalName.toLower() + ':', ObjectName);
			}
			break;
		case ISNamespace::OFT_Edit:
			ISProtocol::EditObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
			if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
			{
				ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Edited") + " - " + MetaTable->LocalName.toLower() + ':', ObjectName);
			}
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
		ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).rollback(); //Откат транзакции
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
		ActionFavorites->setChecked(ISFavorites::Instance().CheckExistFavoriteObject(MetaTable->Name, ObjectID));
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
	bool IsExist = ISFavorites::Instance().CheckExistFavoriteObject(MetaTable->Name, ObjectID);
	IsExist ? ISFavorites::Instance().DeleteFavorite(MetaTable->Name, ObjectID) : ISFavorites::Instance().AddFavorite(MetaTable->Name, ObjectID);
	ActionFavorites->setCheckable(!IsExist);
	if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
	{
		IsExist ? ISPopupMessage::ShowNotification(LANG("RecordRemoveFavorites").arg(ObjectName)) : ISPopupMessage::ShowNotification(LANG("RecordAddFavorites").arg(ObjectName));
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::Delete()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	QString ErrorString;
	if (RecordIsDeleted)
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.RecoveryThisRecord")))
		{
			if (ISCore::SetIsDeletedObject(false, MetaTable, GetObjectID(), ErrorString))
			{
				emit UpdateList();
				close();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.SetNotIsDeletedObject"), ErrorString);
			}
		}
	}
	else
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteThisRecord")))
		{
			if (ISCore::SetIsDeletedObject(true, MetaTable, GetObjectID(), ErrorString))
			{
				emit UpdateList();
				close();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.SetIsDeletedObject"), ErrorString);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::DeleteCascade()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete.Cascade"), LANG("Message.Object.Delete.Cascade.Help")))
	{
		QString ErrorString;
		if (ISCore::DeleteCascadeObject(MetaTable, GetObjectID(), ErrorString))
		{
			if (SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
			{
				ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Deleted.Cascade").arg(GetObjectID()));
			}
			ISProtocol::DeleteCascadeObject(MetaTable->Name, MetaTable->LocalListName, GetObjectID());
			emit UpdateList();
			close();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.CascadeDeleteObject"), ErrorString);
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

		for (const auto &FieldItem : BeginValues.toStdMap()) //Обход всех первоначальных значений
		{
			QString FieldName = FieldItem.first;
			QVariant BeginValue = FieldItem.second;
			if (!MetaTable->GetField(FieldName)->QueryText.isEmpty()) //Если поле является мета-запросом
			{
				continue;
			}

			QVariant CurrentValue = GetFieldValue(FieldName);
			if (CurrentValue != BeginValue) //Если текущее значение поля не равно первичному - изменить значение поля на первичное
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
void ISObjectFormBase::NoteObject()
{
	ISGui::ShowNoteObject(this, MetaTable->Name, ObjectID);
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
