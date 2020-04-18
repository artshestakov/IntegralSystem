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
#include "ISSystem.h"
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
//-----------------------------------------------------------------------------
ISObjectFormBase::ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISInterfaceForm(parent),
	FormType(form_type),
	MetaTable(meta_table),
	ObjectID(object_id),
	ParentObjectID(0),
	WidgetObject(nullptr),
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
	CreateMainTabWidget();
	CreateToolBar();
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
	FieldsMap[FieldName]->SetValue(value);
}
//-----------------------------------------------------------------------------
QVariant ISObjectFormBase::GetFieldValue(const QString &FieldName) const
{
	return FieldsMap.at(FieldName)->GetValue();
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetCurrentIndexTab(int current_index_tab)
{
	CurrentIndexTab = current_index_tab;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleNavigationBar(bool Visible)
{
	ToolBarNavigation->setVisible(Visible);
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
void ISObjectFormBase::SetVisibleReRead(bool Visible)
{
	ActionReRead->setVisible(Visible);
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

	if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
	{
		ToolBarNavigation->UpdateEnabledActionsList(false);
	}
	else
	{
		ToolBarNavigation->UpdateEnabledActionsList(true);
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
	ToolBarNavigation = new ISToolBarObject(this);
	ToolBarNavigation->setIconSize(ISDefines::Gui::SIZE_20_20);
	GetMainLayout()->addWidget(ToolBarNavigation);

	//Действие объекта
	QAction *ActionObject = ToolBarNavigation->CreateAction(BUFFER_ICONS("Document"), MetaTable->LocalName, ISNamespace::OAT_Object);
	ActionObject->setChecked(true);
	ToolBarNavigation->addAction(ActionObject);
	ToolBarNavigation->actionTriggered(ActionObject);

	//Список системных эскортов
	QAction *ActionOther = ToolBarNavigation->CreateAction(BUFFER_ICONS("AdditionallyActions"), LANG("ObjectOther"), ISNamespace::OAT_Other);
	ToolBarNavigation->addAction(ActionOther);

	QToolButton *ToolButtonOther = dynamic_cast<QToolButton*>(ToolBarNavigation->widgetForAction(ActionOther));
	ToolButtonOther->setPopupMode(QToolButton::InstantPopup);
	ToolButtonOther->setMenu(new QMenu(ToolButtonOther));
	ToolButtonOther->setCursor(CURSOR_POINTING_HAND);
	ToolButtonOther->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
	ToolButtonOther->menu()->addAction(ToolBarNavigation->CreateAction(BUFFER_ICONS("Protocol"), LANG("ProtocolCard"), ISNamespace::OAT_Service, QString(), "ISProtocolObjectListForm"));
	ToolButtonOther->menu()->addAction(ToolBarNavigation->CreateAction(BUFFER_ICONS("Discussion"), LANG("Discussion"), ISNamespace::OAT_Service, QString(), "ISDiscussionListForm"));

	for (int i = 0; i < MetaTable->Escorts.size(); ++i) //Обход эскортных мета-таблиц
	{
		PMetaEscort *MetaEscort = MetaTable->Escorts[i];
		
		QAction *ActionEscort = ToolBarNavigation->CreateAction(BUFFER_ICONS("Table"), MetaEscort->LocalName, ISNamespace::OAT_Escort, MetaEscort->TableName, MetaEscort->ClassName);
		ActionEscort->setProperty("ClassFilter", MetaEscort->ClassFilter);
		ToolBarNavigation->addAction(ActionEscort);
	}

	connect(ToolBarNavigation, &ISToolBarObject::ActionClicked, this, &ISObjectFormBase::ToolBarClicked);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateMainTabWidget()
{
	TabWidgetMain = new ISTabWidgetObject(this);
	GetMainLayout()->addWidget(TabWidgetMain);

	StackedWidget = new QStackedWidget(TabWidgetMain);
	TabWidgetMain->addTab(StackedWidget, LANG("Card"));

	WidgetObjectLayout = new QVBoxLayout();
	WidgetObjectLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	WidgetObject = new QWidget(TabWidgetMain);
	WidgetObject->setLayout(WidgetObjectLayout);
	StackedWidget->addWidget(WidgetObject);

	WidgetTabEscort = new QWidget(TabWidgetMain);
	WidgetTabEscort->setLayout(new QVBoxLayout());
	StackedWidget->addWidget(WidgetTabEscort);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateToolBar()
{
	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	WidgetObjectLayout->addWidget(ToolBar);

	LabelIsDeleted = new QLabel(this);
	LabelIsDeleted->setVisible(false);
	LabelIsDeleted->setStyleSheet(STYLE_SHEET("QLabel.Color.Red"));
	LabelIsDeleted->setText(LANG("RecordMarkerIsDeleted"));
	LabelIsDeleted->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	ISGui::SetFontWidgetUnderline(LabelIsDeleted, true);
	WidgetObjectLayout->addWidget(LabelIsDeleted, 0, Qt::AlignHCenter);

	//Сохранить и создать новую запись
	ActionSaveAndNew = new QAction(ToolBar);
	ActionSaveAndNew->setText(LANG("SaveAndCreate"));
	ActionSaveAndNew->setToolTip(LANG("SaveChangeCloseAndCreateNew"));
	ActionSaveAndNew->setIcon(BUFFER_ICONS("SaveAndNew"));
	ActionSaveAndNew->setVisible(SETTING_BOOL(CONST_UID_SETTING_OBJECTS_SAVECLOSENEW));
	connect(ActionSaveAndNew, &QAction::triggered, this, &ISObjectFormBase::SaveCreate);
	ToolBar->addAction(ActionSaveAndNew);

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
	ActionFavorites = new QAction(ToolBar);
	ActionFavorites->setText(LANG("AddToFavorites"));
	ActionFavorites->setToolTip(LANG("AddToFavorites"));
	ActionFavorites->setIcon(BUFFER_ICONS("Favorites"));
	ActionFavorites->setPriority(QAction::LowPriority);
	ActionFavorites->setCheckable(true);
	connect(ActionFavorites, &QAction::triggered, this, &ISObjectFormBase::AddFavoite);
	AddActionToolBar(ActionFavorites);

	//Удалить/восстановить карточку
	ActionDelete = ISControls::CreateActionDelete(ToolBar);
	ActionDelete->setPriority(QAction::LowPriority);
	connect(ActionDelete, &QAction::triggered, this, &ISObjectFormBase::Delete);
	AddActionMenu(ActionDelete);

	ActionDeleteCascade = ISControls::CreateActionDeleteCascade(ToolBar);
	ActionDeleteCascade->setPriority(QAction::LowPriority);
	connect(ActionDeleteCascade, &QAction::triggered, this, &ISObjectFormBase::DeleteCascade);
	AddActionMenu(ActionDeleteCascade);

	//Отменить изменения
	ActionCancelChange = new QAction(ToolBar);
	ActionCancelChange->setEnabled(false);
	ActionCancelChange->setText(LANG("CancelChanged"));
	ActionCancelChange->setToolTip(LANG("CancelChanged"));
	ActionCancelChange->setIcon(BUFFER_ICONS("CancelChangedObject"));
	ActionCancelChange->setPriority(QAction::LowPriority);
	connect(ActionCancelChange, &QAction::triggered, this, &ISObjectFormBase::CancelChanged);
	AddActionMenu(ActionCancelChange);

	//Перечитать
	ActionReRead = new QAction(ToolBar);
	ActionReRead->setEnabled(false);
	ActionReRead->setText(LANG("ReReadCard"));
	ActionReRead->setToolTip(LANG("ReReadCard"));
	ActionReRead->setIcon(BUFFER_ICONS("Update"));
	ActionReRead->setPriority(QAction::LowPriority);
	connect(ActionReRead, &QAction::triggered, this, &ISObjectFormBase::ReRead);
	AddActionMenu(ActionReRead);

	QAction *ActionSettings = new QAction(ToolBar);
	ActionSettings->setText(LANG("Settings"));
	connect(ActionSettings, &QAction::triggered, this, &ISObjectFormBase::ShowSettingsForm);
	AddActionMenu(ActionSettings);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateFieldsWidget()
{
	FormLayout = new QFormLayout(); //Компоновщик для главного таба
	
	ScrollAreaMain = new ISScrollArea(this); //Панель основной (главной) группы полей
	ScrollAreaMain->setSizePolicy(ScrollAreaMain->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	ScrollAreaMain->widget()->setLayout(FormLayout);
	WidgetObjectLayout->addWidget(ScrollAreaMain);

	CreateFieldID(FormLayout);
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
		QString QueryText = QueryModel.GetQueryText();

		//Выполнение запроса
		ISQuery qSelect(QueryText);
		IS_ASSERT(qSelect.ExecuteFirst(), QString("Not executed query:\n%1\n%2").arg(QueryText).arg(qSelect.GetErrorString()));
		IS_ASSERT(qSelect.GetCountResultRows() == 1, "Error count result rows");
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
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateFieldID(QFormLayout *FormLayout)
{
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

	if (MetaField->DefaultValueWidget.isValid()) //Если в поле указано значение по умолчанию для виджета-редактора
	{
		FieldEditBase->SetValue(MetaField->DefaultValueWidget);
		FieldEditBase->SetModificationFlag(true);
	}

	if (MetaField->NotNull) //Если поле обязательно для заполнения
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

	if (MetaField->ReadOnly) //Если доступно только для просмотра
	{
		FieldEditBase->SetReadOnly(MetaField->ReadOnly);
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
void ISObjectFormBase::AddObjectEscort(QWidget *ObjectForm)
{
	//Изменение наименования заголовка вкладки формы объекта
	connect(ObjectForm, &QWidget::windowTitleChanged, [=](const QString &WindowTitle)
	{
		TabWidgetMain->setTabText(TabWidgetMain->indexOf(ObjectForm), WindowTitle);
	});

	//Изменение иконки заголовка вкладки формы объекта
	connect(ObjectForm, &QWidget::windowIconChanged, [=](const QIcon &WindowIcon)
	{
		TabWidgetMain->setTabIcon(TabWidgetMain->indexOf(ObjectForm), WindowIcon);
	});

	ObjectForm->setParent(this);
	TabWidgetMain->addTab(ObjectForm, ObjectForm->windowIcon(), ObjectForm->windowTitle());
	TabWidgetMain->setCurrentWidget(ObjectForm);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ToolBarClicked(QAction *ActionClicked)
{
	if (WidgetEscort)
	{
		delete WidgetEscort;
		WidgetEscort = nullptr;
	}

	TabWidgetMain->setCurrentWidget(StackedWidget);

	if (qvariant_cast<ISNamespace::ObjectActionType>(ActionClicked->property("Type")) == ISNamespace::OAT_Object) //Выбран объект
	{
		TabWidgetMain->setTabText(0, LANG("Card"));
		StackedWidget->setCurrentWidget(WidgetObject);
	}
	else //Выбран эскорт
	{
		TabWidgetMain->setTabText(TabWidgetMain->indexOf(StackedWidget), ActionClicked->text());
		StackedWidget->setCurrentWidget(WidgetTabEscort);

		QString TableName = ActionClicked->property("TableName").toString();
		QString ClassName = ActionClicked->property("ClassName").toString();
		QString ClassFilter = ActionClicked->property("ClassFilter").toString();

		if (ClassName.length()) //Открытие виджета
		{
			int ObjectType = QMetaType::type((ClassName + SYMBOL_STAR).toLocal8Bit().constData());
			IS_ASSERT(ObjectType, "Class for SybSystem is NULL. ClassName: " + ClassName);

			const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
			IS_ASSERT(MetaObject, "Error opening subsystem widget.");

			WidgetEscort = dynamic_cast<ISInterfaceMetaForm*>(MetaObject->newInstance(Q_ARG(QWidget *, WidgetTabEscort)));
			IS_ASSERT(WidgetEscort, "Error instance escort class.");
		}
		else //Открытие таблицы
		{
			ISListObjectForm *ListForm = new ISListObjectForm(TableName, ObjectID, WidgetTabEscort);
			ListForm->SetUID(ISMetaData::GetInstanse().GetMetaTable(TableName)->UID);
			
			if (!ClassFilter.isEmpty())
			{
				ListForm->GetQueryModel()->SetClassFilter(ClassFilter);
			}
			WidgetEscort = ListForm;
		}

		connect(WidgetEscort, &ISInterfaceMetaForm::AddFormFromTab, this, &ISObjectFormBase::AddObjectEscort);
		WidgetTabEscort->layout()->addWidget(WidgetEscort);

		//Присвоения ниже должны быть перед вызовом LoadData()
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
void ISObjectFormBase::SaveCreate()
{
	bool Result = true;
	if (ModificationFlag)
	{
		Result = Save();
	}

	if (Result)
	{
		if (parent())
		{
			emit SaveAndCreate(MetaTable);
		}
		else
		{
			ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name);
			ObjectFormBase->show();
		}

		close();
		emit CloseTab(CurrentIndexTab);
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
bool ISObjectFormBase::Save()
{
	QVariantMap ValuesMap;
	ISVectorString FieldsVector;
	QString QueryText;
	bool Executed = false;

	for (const auto &Field : FieldsMap) //Обход существующих полей на форме
	{
		QString FieldName = Field.first;
		PMetaField *MetaField = MetaTable->GetField(FieldName);
		ISFieldEditBase *FieldEditBase = Field.second;

		QVariant Value = FieldEditBase->GetValue();
		if (Value.isNull()) //Если значение в поле отсутствует, проверить обязательно ли поле для заполнения
		{
			if (MetaField->NotNull && !MetaField->HideFromObject) //Если поле обязательно для заполнения
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(MetaField->LabelName));
				FieldEditBase->BlinkRed();
				return false;
			}
		}

		if (!FieldEditBase->GetModificationFlag()) //Если значения поля редактированя не изменялось, переходить к следующему
		{
			if (MetaTable->ClassFilterField != FieldName) //Если текущее поле не является фильтруемым (текущая таблица не является эскортной)
			{
				continue;
			}
		}

		if (!FieldEditBase->IsValid())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ValueFieldEditInvalid").arg(MetaField->LabelName));
			FieldEditBase->BlinkRed();
			return false;
		}

		FieldEditBase->Invoke();
		ValuesMap.insert(FieldName, Value);
		FieldsVector.emplace_back(FieldName);
	}

	//Формирование запроса на добавление/изменение/копирование
	if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
	{
		QString InsertFields = "INSERT INTO " + MetaTable->Name + " (";
		QString InsertValues = "VALUES (";

		for (int i = 0; i < FieldsVector.size(); ++i)
		{
			InsertFields += MetaTable->Alias + '_' + FieldsVector.at(i) + ", ";
			InsertValues += ':' + FieldsVector.at(i) + ", ";
		}

		ISSystem::RemoveLastSymbolFromString(InsertFields, 2);
		ISSystem::RemoveLastSymbolFromString(InsertValues, 2);

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

		for (int i = 0; i < FieldsVector.size(); ++i)
		{
			QueryText += MetaTable->Alias + '_' + FieldsVector.at(i) + " = :" + FieldsVector.at(i) + ", \n";
		}

		ISSystem::RemoveLastSymbolFromString(QueryText, 3);

		QueryText += " \n";
		QueryText += "WHERE " + MetaTable->Alias + "_id = " + QString::number(ObjectID);
	}

	ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).transaction(); //Открытие транзакции
	SavedEvent();

	//Заполнение запроса значениями
	ISQuery SqlQuery(QueryText);
	for (const auto &Value : ValuesMap.toStdMap())
	{
		IS_ASSERT(SqlQuery.BindValue(':' + Value.first, Value.second), "Not bind value");
	}

	Executed = SqlQuery.Execute(); //Исполнение запроса
	if (Executed) //Запрос выполнен успешно
	{
		ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).commit(); //Коммит транзакции
		if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
		{
			IS_ASSERT(SqlQuery.First(), "Not first SqlQuery");
			ObjectID = SqlQuery.ReadColumn(MetaTable->Alias + "_id").toInt();
		}

		ObjectName = ISCore::GetObjectName(MetaTable, ObjectID);
		if (FormType == ISNamespace::OFT_New)
		{
			FormType = ISNamespace::OFT_Edit;
			ISNotificationService::ShowNotification(ISNamespace::NFT_Create, MetaTable->LocalName, ObjectName);
			ISProtocol::CreateObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
		}
		else if (FormType == ISNamespace::OFT_Copy)
		{
			FormType = ISNamespace::OFT_Edit;
			ISNotificationService::ShowNotification(ISNamespace::NFT_CreateCopy, MetaTable->LocalName, ObjectName);
			ISProtocol::CreateCopyObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
		}
		else if (FormType == ISNamespace::OFT_Edit)
		{
			ISNotificationService::ShowNotification(ISNamespace::NFT_Edit, MetaTable->LocalName, ObjectName);
			ISProtocol::EditObject(MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
		}

		RenameReiconForm();
		SetModificationFlag(false);
		UpdateObjectActions();
		ToolBarNavigation->UpdateEnabledActionsList(true);
		ActionFavorites->setEnabled(true);
		SetValueFieldID(ObjectID);

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
	ActionSaveAndNew->setEnabled(ModificationFlag);
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
		ActionSaveAndNew->setEnabled(true);
		ActionSave->setEnabled(true);
	}
	else if (FormType == ISNamespace::OFT_Edit)
	{
		SetEnabledActions(true);
		ActionSaveAndNew->setEnabled(false);
		ActionSave->setEnabled(false);
		ActionReRead->setEnabled(true);
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
void ISObjectFormBase::ReRead()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ReReadCard")))
	{
		ISGui::SetWaitGlobalCursor(true);
		for (const auto &FieldItem : FieldsMap)
		{
			QString FieldName = FieldItem.first;
			ISFieldEditBase *FieldWidget = FieldItem.second;

			ISQuery qSelect("SELECT " + MetaTable->Alias + '_' + FieldName + " FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID");
			qSelect.BindValue(":ObjectID", ObjectID);
			if (qSelect.ExecuteFirst())
			{
				QVariant ValueDB = qSelect.ReadColumn(MetaTable->Alias + '_' + FieldName);
				disconnect(FieldWidget, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
				FieldWidget->SetValue(ValueDB);
				FieldWidget->SetModificationFlag(false);
				connect(FieldWidget, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
			}
		}

		RenameReiconForm();
		BeginFieldEdit->SetFocus();
		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::ShowSettingsForm()
{
	ISGui::ShowSettingsForm(CONST_UID_SETTING_GROUP_OBJECTS);
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
	for (int i = 0; i < ActionGroup->actions().count(); ++i)
	{
		ActionGroup->actions().at(i)->setEnabled(Enabled);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddWidgetToBottom(QWidget *Widget)
{
	WidgetObjectLayout->addWidget(Widget);
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
ISTabWidgetObject* ISObjectFormBase::GetTabWidget()
{
	return TabWidgetMain;
}
//-----------------------------------------------------------------------------
QVBoxLayout* ISObjectFormBase::GetLayoutWidgetObject()
{
	return WidgetObjectLayout;
}
//-----------------------------------------------------------------------------
QToolBar* ISObjectFormBase::GetToolBar()
{
	return ToolBar;
}
//-----------------------------------------------------------------------------
