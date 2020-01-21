#include "StdAfx.h"
#include "ISObjectFormBase.h"
#include "ISDefines.h"
#include "ISTrace.h"
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
#include "ISCountingTime.h"
#include "ISDebug.h"
#include "ISDatabaseHelper.h"
#include "ISStyleSheet.h"
#include "ISFavorites.h"
#include "ISConstants.h"
#include "ISCreatedObjectsEntity.h"
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
ISObjectFormBase::ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISInterfaceForm(parent)
{
	FormType = form_type;
	MetaTable = meta_table;
	ObjectID = object_id;
	ParentObjectID = 0;
	TimerAutoSave = nullptr;
	WidgetObject = nullptr;
	WidgetEscort = nullptr;
	EditObjectID = nullptr;
	BeginFieldEdit = nullptr;
	ModificationFlag = false;
	RecordIsDeleted = false;
	CurrentIndexTab = -1;

	ActionDelete = nullptr;

	GetMainLayout()->setContentsMargins(0, 3, 0, 0);
	GetMainLayout()->setSpacing(0);

	ActionGroup = new QActionGroup(this);

	CreateToolBarEscorts();
	CreateMainTabWidget();
	CreateToolBar();
	CreateFieldsWidget();
	FillDataFields();

	ISCreatedObjectsEntity::GetInstance().RegisterForm(this);
}
//-----------------------------------------------------------------------------
ISObjectFormBase::~ISObjectFormBase()
{
	ISCreatedObjectsEntity::GetInstance().UnregisterForm(GetFormUID());
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
PMetaClassTable* ISObjectFormBase::GetMetaTable()
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
	FieldsMap.value(FieldName)->SetValue(value);
}
//-----------------------------------------------------------------------------
QVariant ISObjectFormBase::GetFieldValue(const QString &FieldName) const
{
	return FieldsMap.value(FieldName)->GetValue();
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
	LabelsMap.value(FieldName)->setVisible(Visible);
	FieldsMap.value(FieldName)->setVisible(Visible);
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
	if (TimerAutoSave)
	{
		TimerAutoSave->stop();
	}

	if (ModificationFlag)
	{
		emit CurrentObjectTab();

		ISMessageBox MessageBox(QMessageBox::Warning, LOCALIZATION("SavingProcess"), LOCALIZATION("Message.Question.SaveObjectChanged").arg(MetaTable->GetLocalName()), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this);
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
		ISProtocol::ShowObject(MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectID, ObjectName);
	}

	if (BeginFieldEdit)
	{
		BeginFieldEdit->SetFocus();
	}

	ISFieldEditBase *FieldEditWidget = FieldsMap.value(MetaTable->GetClassFilterField());
	if (FieldEditWidget)
	{
		FieldEditWidget->SetValue(ParentObjectID);
	}

	RenameReiconForm();
	UpdateObjectActions();
	SetModificationFlag(false);
	ResizeRemove();

	if (SETTING_BOOL(CONST_UID_SETTING_OBJECTS_AUTOSAVE) && FormType == ISNamespace::OFT_Edit)
	{
		TimerAutoSave = new QTimer(this);
		TimerAutoSave->setInterval(SETTING_INT(CONST_UID_SETTING_OBJECTS_AUTOSAVE_TIMEOUT) * 1000);
		connect(TimerAutoSave, &QTimer::timeout, this, &ISObjectFormBase::TimeoutAutoSave);
		TimerAutoSave->start();
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateToolBarEscorts()
{
	ToolBarNavigation = new ISToolBarObject(this);
	ToolBarNavigation->setIconSize(SIZE_20_20);
	GetMainLayout()->addWidget(ToolBarNavigation);

	//Действие объекта
	QAction *ActionObject = ToolBarNavigation->CreateAction(BUFFER_ICONS("Document"), MetaTable->GetLocalName(), ISNamespace::OAT_Object);
	ActionObject->setChecked(true);
	ToolBarNavigation->addAction(ActionObject);
	ToolBarNavigation->actionTriggered(ActionObject);

	//Список системных эскортов
	QAction *ActionOther = ToolBarNavigation->CreateAction(BUFFER_ICONS("AdditionallyActions"), LOCALIZATION("ObjectOther"), ISNamespace::OAT_Other);
	ToolBarNavigation->addAction(ActionOther);

	QToolButton *ToolButtonOther = dynamic_cast<QToolButton*>(ToolBarNavigation->widgetForAction(ActionOther));
	ToolButtonOther->setPopupMode(QToolButton::InstantPopup);
	ToolButtonOther->setMenu(new QMenu(ToolButtonOther));
	ToolButtonOther->setCursor(CURSOR_POINTING_HAND);
	ToolButtonOther->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));
	ToolButtonOther->menu()->addAction(ToolBarNavigation->CreateAction(BUFFER_ICONS("Protocol"), LOCALIZATION("ProtocolCard"), ISNamespace::OAT_Service, QString(), "ISProtocolObjectListForm"));
	ToolButtonOther->menu()->addAction(ToolBarNavigation->CreateAction(BUFFER_ICONS("Discussion"), LOCALIZATION("Discussion"), ISNamespace::OAT_Service, QString(), "ISDiscussionListForm"));

	for (int i = 0; i < MetaTable->GetEscorts().count(); i++) //Обход эскортных мета-таблиц
	{
		PMetaClassEscort *MetaEscort = MetaTable->GetEscorts().at(i);
		
		QAction *ActionEscort = ToolBarNavigation->CreateAction(BUFFER_ICONS("Table"), MetaEscort->GetLocalName(), ISNamespace::OAT_Escort, MetaEscort->GetTableName(), MetaEscort->GetClassName());
		ActionEscort->setProperty("ClassFilter", MetaEscort->GetClassFilter());
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
	TabWidgetMain->addTab(StackedWidget, LOCALIZATION("Card"));

	WidgetObjectLayout = new QVBoxLayout();
	WidgetObjectLayout->setContentsMargins(LAYOUT_MARGINS_NULL);

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
	LabelIsDeleted->setText(LOCALIZATION("RecordMarkerIsDeleted"));
	LabelIsDeleted->setFont(FONT_TAHOMA_12_BOLD);
	ISGui::SetFontWidgetUnderline(LabelIsDeleted, true);
	WidgetObjectLayout->addWidget(LabelIsDeleted, 0, Qt::AlignHCenter);

	//Сохранить и создать новую запись
	ActionSaveAndNew = new QAction(ToolBar);
	ActionSaveAndNew->setText(LOCALIZATION("SaveAndCreate"));
	ActionSaveAndNew->setToolTip(LOCALIZATION("SaveChangeCloseAndCreateNew"));
	ActionSaveAndNew->setIcon(BUFFER_ICONS("SaveAndNew"));
	ActionSaveAndNew->setVisible(SETTING_BOOL(CONST_UID_SETTING_OBJECTS_SAVECLOSENEW));
	connect(ActionSaveAndNew, &QAction::triggered, this, &ISObjectFormBase::SaveCreate);
	ToolBar->addAction(ActionSaveAndNew);

	//Сохранить и закрыть карточку
	ActionSaveClose = ISControls::CreateActionSaveAndClose(ToolBar);
	ActionSaveClose->setToolTip(LOCALIZATION("SaveChangeClose"));
	connect(ActionSaveClose, &QAction::triggered, this, &ISObjectFormBase::SaveClose);
	ToolBar->addAction(ActionSaveClose);

	//Сохранить карточку
	ActionSave = ISControls::CreateActionSave(ToolBar);
	ActionSave->setToolTip(LOCALIZATION("SaveChange"));
	connect(ActionSave, &QAction::triggered, this, &ISObjectFormBase::Save);
	ToolBar->addAction(ActionSave);

	//Закрыть карточку
	ActionClose = ISControls::CreateActionClose(ToolBar);
	ActionClose->setToolTip(LOCALIZATION("CloseCard"));
	connect(ActionClose, &QAction::triggered, this, &ISObjectFormBase::close);
	ToolBar->addAction(ActionClose);

	ActionAdditionally = new QAction(ToolBar);
	ActionAdditionally->setText(LOCALIZATION("Additionally"));
	ActionAdditionally->setToolTip(LOCALIZATION("Additionally"));
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
	ActionFavorites->setText(LOCALIZATION("AddToFavorites"));
	ActionFavorites->setToolTip(LOCALIZATION("AddToFavorites"));
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
	ActionCancelChange->setText(LOCALIZATION("CancelChanged"));
	ActionCancelChange->setToolTip(LOCALIZATION("CancelChanged"));
	ActionCancelChange->setIcon(BUFFER_ICONS("CancelChangedObject"));
	ActionCancelChange->setPriority(QAction::LowPriority);
	connect(ActionCancelChange, &QAction::triggered, this, &ISObjectFormBase::CancelChanged);
	AddActionMenu(ActionCancelChange);

	//Перечитать
	ActionReRead = new QAction(ToolBar);
	ActionReRead->setEnabled(false);
	ActionReRead->setText(LOCALIZATION("ReReadCard"));
	ActionReRead->setToolTip(LOCALIZATION("ReReadCard"));
	ActionReRead->setIcon(BUFFER_ICONS("Update"));
	ActionReRead->setPriority(QAction::LowPriority);
	connect(ActionReRead, &QAction::triggered, this, &ISObjectFormBase::ReRead);
	AddActionMenu(ActionReRead);

	QAction *ActionSettings = new QAction(ToolBar);
	ActionSettings->setText(LOCALIZATION("Settings"));
	connect(ActionSettings, &QAction::triggered, this, &ISObjectFormBase::ShowSettingsForm);
	AddActionMenu(ActionSettings);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateFieldsWidget()
{
	FormLayout = new QFormLayout(); //Компоновщик для главного таба

	ScrollAreaMain = new ISScrollArea(this); //Панель основной (главной) группы полей
	ScrollAreaMain->widget()->setLayout(FormLayout);
	WidgetObjectLayout->addWidget(ScrollAreaMain);

	CreateFieldID(FormLayout);
	for (PMetaClassField *MetaField : MetaTable->GetFields()) //Обход полей
	{
		if (MetaField->GetQueryText().length()) //Если поле является запросом - пропускать его
		{
			continue;
		}

		//Если тип поля ByteArray и для него не предусмотрен виджет редактирования - пропускать его
		if (MetaField->GetType() == ISNamespace::FT_ByteArray && !MetaField->GetControlWidget().length())
		{
			continue;
		}

		QString LayoutName = MetaField->GetLayoutName();
		if (LayoutName.length()) //Если поле должно быть в горизональном компоновщике
		{
			if (!Layouts.contains(LayoutName))
			{
				QHBoxLayout *LayoutHorizontal = new QHBoxLayout();
				LayoutHorizontal->setProperty("Inserted", false);
				Layouts.insert(LayoutName, LayoutHorizontal);
			}
		}

		ISFieldEditBase *FieldEditBase = CreateColumnForField(MetaField);
		if (dynamic_cast<ISListEdit*>(FieldEditBase))
		{
			dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->GetForeign());
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
		QueryModel.SetClassFilter(MetaTable->GetAlias() + "." + MetaTable->GetAlias() + "_id = " + QString::number(ObjectID));
		QString QueryText = QueryModel.GetQueryText();

		//Выполнение запроса
		ISDebug::ShowDebugString(QString("Start select query from object %1 in table \"%2\"").arg(ObjectID).arg(MetaTable->GetName()));
		ISCountingTime Time;
		ISQuery qSelect(QueryText);
		IS_ASSERT(qSelect.ExecuteFirst(), QString("Not executed query:\n%1\n%2").arg(QueryText).arg(qSelect.GetErrorText()));
		IS_ASSERT(qSelect.GetCountResultRows() == 1, "Error count result rows");
		ISDebug::ShowDebugString(QString("Finished select query from object %1 in table \"%2\". %3 msec.").arg(ObjectID).arg(MetaTable->GetName()).arg(Time.GetElapsed()));
		QSqlRecord SqlRecord = qSelect.GetRecord();
		
		RecordIsDeleted = SqlRecord.value("IsDeleted").toBool();
		LabelIsDeleted->setVisible(RecordIsDeleted);

		if (FormType == ISNamespace::OFT_Edit)
		{
			SetValueFieldID(ObjectID);
		}

		for (int i = 0; i < SqlRecord.count(); i++) //Обход полей записи
		{
			QString FieldName = SqlRecord.fieldName(i);
			QVariant Value = SqlRecord.value(FieldName);
			if (Value.isNull()) //Если значение пустое, перейти на следующий шаг цикла
			{
				BeginValues.insert(FieldName, QVariant());
				continue;
			}

			ISFieldEditBase *FieldEditWidget = FieldsMap.value(FieldName);
			if (FieldEditWidget)
			{
				disconnect(FieldEditWidget, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);

				if (MetaTable->GetField(FieldName)->GetForeign())
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

				if (FormType == ISNamespace::OFT_Copy) //Если формы открывается для создания копии
				{
					FieldEditWidget->SetModificationFlag(true);
				}
				else
				{
					FieldEditWidget->SetModificationFlag(false);
				}

				connect(FieldEditWidget, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
			}
		}

		ISCore::AddHistory(MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectName, ObjectID);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateFieldID(QFormLayout *FormLayout)
{
	if (SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		QLabel *LabelSystemInfoObject = new QLabel(this);
		LabelSystemInfoObject->setText(LOCALIZATION("SystemField.ID") + ":");
		LabelSystemInfoObject->setFont(FONT_APPLICATION_BOLD);

		EditObjectID = new ISLineEdit(this);
		EditObjectID->SetValue(LOCALIZATION("SystemField.ID.NotAssigned"));
		EditObjectID->SetToolTip(LOCALIZATION("AutoFillField"));
		EditObjectID->SetReadOnly(true); //Последовательность setReadOnly и setCursor должна быть именно такой, иначе курсор не меняется
		EditObjectID->SetCursor(CURSOR_WHATS_THIS); //Последовательность setReadOnly и setCursor должна быть именно такой, иначе курсор не меняется
		EditObjectID->SetVisibleClear(false);
		EditObjectID->setSizePolicy(QSizePolicy::Maximum, EditObjectID->sizePolicy().verticalPolicy());

		FormLayout->addRow(LabelSystemInfoObject, EditObjectID);
	}
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISObjectFormBase::CreateColumnForField(PMetaClassField *MetaField)
{
	ISFieldEditBase	*FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
	FieldsMap.insert(MetaField->GetName(), FieldEditBase);
	connect(FieldEditBase, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddColumnForField(PMetaClassField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *FormLayout)
{
	QLabel *LabelField = new QLabel(this);
	LabelField->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	LabelsMap.insert(MetaField->GetName(), LabelField);

	if (MetaField->GetDefaultValueWidget().isValid()) //Если в поле указано значение по умолчанию для виджета-редактора
	{
		FieldEditBase->SetValue(MetaField->GetDefaultValueWidget());
		FieldEditBase->SetModificationFlag(true);
	}

	if (MetaField->GetNotNull()) //Если поле обязательно для заполнения
	{
		LabelField->setTextFormat(Qt::RichText);
		LabelField->setText(QString("<font>%1:</font><font color=#FF0000 size=4>*</font>").arg(MetaField->GetLabelName()));
		LabelField->setToolTip(LOCALIZATION("FieldNotNull"));
		LabelField->setCursor(CURSOR_WHATS_THIS);
		FieldEditBase->SetToolTip(LOCALIZATION("FillThisField"));
	}
	else
	{
		LabelField->setText(MetaField->GetLabelName() + ":");
	}

	if (MetaField->GetReadOnly()) //Если доступно только для просмотра
	{
		FieldEditBase->SetReadOnly(MetaField->GetReadOnly());
	}

	if (MetaField->GetPlaceholderText().length()) //Если есть текст-заполнитель
	{
		FieldEditBase->SetPlaceholderText(MetaField->GetPlaceholderText());
	}

	if (MetaField->GetHint().length()) //Если есть подсказка для поля
	{
		FieldEditBase->CreateHint(MetaField->GetHint());
	}

	if (MetaField->GetRegExp().length()) //Если у поля настроено регулярное выражение
	{
		FieldEditBase->SetRegExp(MetaField->GetRegExp());
	}

	if (MetaField->GetHideFromObject()) //Если поле нужно скрыть - выходим из метода и просто его не добавляем на форму
	{
		SetVisibleField(MetaField->GetName(), false);
		return;
	}
	
	if (!BeginFieldEdit && !MetaField->GetReadOnly())
	{
		BeginFieldEdit = FieldEditBase;
	}

	QString Separator = MetaField->GetSeparatorName();
	if (Separator.length())
	{
		QLabel *LabelTab = new QLabel(this);
		LabelTab->setText(Separator);
		LabelTab->setFont(FONT_TAHOMA_10_BOLD);

		QHBoxLayout *LayoutLine = new QHBoxLayout();
		LayoutLine->setContentsMargins(LAYOUT_MARGINS_NULL);

		QWidget *WidgetLine = new QWidget(this);
		WidgetLine->setLayout(LayoutLine);
		LayoutLine->addWidget(ISControls::CreateHorizontalLinePlain(this));

		FormLayout->addRow(LabelTab, WidgetLine);
	}

	QHBoxLayout *LayoutHorizontal = Layouts.value(MetaField->GetLayoutName());
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
		TabWidgetMain->setTabText(0, LOCALIZATION("Card"));
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
			int ObjectType = QMetaType::type((ClassName + "*").toLocal8Bit().constData());
			IS_ASSERT(ObjectType, "Class for SybSystem is NULL. ClassName: " + ClassName);

			const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
			IS_ASSERT(MetaObject, "Error opening subsystem widget.");

			WidgetEscort = dynamic_cast<ISInterfaceMetaForm*>(MetaObject->newInstance(Q_ARG(QWidget *, WidgetTabEscort)));
			IS_ASSERT(WidgetEscort, "Error instance escort class.");
		}
		else //Открытие таблицы
		{
			ISListObjectForm *ListForm = new ISListObjectForm(TableName, ObjectID, WidgetTabEscort);
			ListForm->SetUID(ISMetaData::GetInstanse().GetMetaTable(TableName)->GetUID());
			
			if (ClassFilter.length())
			{
				ListForm->GetQueryModel()->SetClassFilter(ClassFilter);
			}

			WidgetEscort = ListForm;
		}

		connect(WidgetEscort, &ISInterfaceMetaForm::AddFormFromTab, this, &ISObjectFormBase::AddObjectEscort);
		WidgetTabEscort->layout()->addWidget(WidgetEscort);

		//Присвоения ниже должны быть перед вызовом LoadData()
		WidgetEscort->SetParentTableName(MetaTable->GetName());
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
			ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->GetName());
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
	QVectorString FieldsVector;
	QString QueryText;
	bool Executed = false;

	for (const auto &Field : FieldsMap.toStdMap()) //Обход существующих полей на форме
	{
		QString FieldName = Field.first;
		PMetaClassField *MetaField = MetaTable->GetField(FieldName);
		ISFieldEditBase *FieldEditBase = Field.second;

		QVariant Value = FieldEditBase->GetValue();
		if (Value.isNull()) //Если значение в поле отсутствует, проверить обязательно ли поле для заполнения
		{
			if (MetaField->GetNotNull() && !MetaField->GetHideFromObject()) //Если поле обязательно для заполнения
			{
				ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Field.NullValue").arg(MetaField->GetLabelName()));
				FieldEditBase->BlinkRed();
				return false;
			}
		}

		if (!FieldEditBase->GetModificationFlag()) //Если значения поля редактированя не изменялось, переходить к следующему
		{
			if (MetaTable->GetClassFilterField() != FieldName) //Если текущее поле не является фильтруемым (текущая таблица не является эскортной)
			{
				continue;
			}
		}

		if (!FieldEditBase->IsValid())
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.ValueFieldEditInvalid").arg(MetaField->GetLabelName()));
			FieldEditBase->BlinkRed();
			return false;
		}

		FieldEditBase->Invoke();
		ValuesMap.insert(FieldName, Value);
		FieldsVector.append(FieldName);
	}

	//Формирование запроса на добавление/изменение/копирование
	if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
	{
		QString InsertFields = "INSERT INTO " + MetaTable->GetName() + " (";
		QString InsertValues = "VALUES (";

		for (int i = 0; i < FieldsVector.count(); i++)
		{
			InsertFields += MetaTable->GetAlias() + "_" + FieldsVector.at(i) + ", ";
			InsertValues += ":" + FieldsVector.at(i) + ", ";
		}

		ISSystem::RemoveLastSymbolFromString(InsertFields, 2);
		ISSystem::RemoveLastSymbolFromString(InsertValues, 2);

		QueryText += InsertFields + ") \n";
		QueryText += InsertValues + ") \n";
		QueryText += "RETURNING " + MetaTable->GetAlias() + "_id";
	}
	else if (FormType == ISNamespace::OFT_Edit)
	{
		QueryText += "UPDATE " + MetaTable->GetName() + " SET \n";

		//Заполнение полей информацией о опользователе и текущей дате
		QueryText += MetaTable->GetAlias() + "_updationdate = now(), \n";
		QueryText += MetaTable->GetAlias() + "_updationuser = CURRENT_USER, \n";

		for (int i = 0; i < FieldsVector.count(); i++)
		{
			QueryText += MetaTable->GetAlias() + "_" + FieldsVector.at(i) + " = :" + FieldsVector.at(i) + ", \n";
		}

		ISSystem::RemoveLastSymbolFromString(QueryText, 3);

		QueryText += " \n";
		QueryText += "WHERE " + MetaTable->GetAlias() + "_id = " + QString::number(ObjectID);
	}

	ISDatabase::GetInstance().GetDefaultDB().transaction(); //Открытие транзакции
	SaveEvent();

	//Заполнение запроса значениями
	ISQuery SqlQuery(QueryText);
	for (const auto &Value : ValuesMap.toStdMap())
	{
		bool BindValue = SqlQuery.BindValue(":" + Value.first, Value.second);
		IS_ASSERT(BindValue, "Not bind value");
	}

	try
	{
		Executed = SqlQuery.Execute(); //Исполнение запроса

		if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
		{
			IS_ASSERT(SqlQuery.First(), "Not first SqlQuery");
			ObjectID = SqlQuery.ReadColumn(MetaTable->GetAlias() + "_id").toInt();
		}
	}
	catch (ISQueryException &e)
	{
		ISDatabase::GetInstance().GetDefaultDB().rollback(); //Откат транзакции
		ISMessageBox::ShowWarning(this, LOCALIZATION(QString("PostgreSQL.Error.%1").arg(SqlQuery.GetErrorNumber())), e.GetWhat());
	}

	if (Executed) //Запрос выполнен успешно
	{
		ISDatabase::GetInstance().GetDefaultDB().commit();
		ObjectName = ISCore::GetObjectName(MetaTable, ObjectID);

		if (FormType == ISNamespace::OFT_New)
		{
			FormType = ISNamespace::OFT_Edit;
			ISNotificationService::ShowNotification(ISNamespace::NFT_Create, MetaTable->GetLocalName(), ObjectName);
			ISProtocol::CreateObject(MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectID, ObjectName);
		}
		else if (FormType == ISNamespace::OFT_Copy)
		{
			FormType = ISNamespace::OFT_Edit;
			ISNotificationService::ShowNotification(ISNamespace::NFT_CreateCopy, MetaTable->GetLocalName(), ObjectName);
			ISProtocol::CreateCopyObject(MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectID, ObjectName);
		}
		else if (FormType == ISNamespace::OFT_Edit)
		{
			ISNotificationService::ShowNotification(ISNamespace::NFT_Edit, MetaTable->GetLocalName(), ObjectName);
			ISProtocol::EditObject(MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectID, ObjectName);
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

	return false;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SaveEvent()
{

}
//-----------------------------------------------------------------------------
void ISObjectFormBase::RenameReiconForm()
{
	switch (FormType)
	{
	case ISNamespace::OFT_New:
		setWindowTitle(LOCALIZATION("Creating") + " (" + MetaTable->GetLocalName() + ")");
		setWindowIcon(BUFFER_ICONS("Add"));
		break;

	case ISNamespace::OFT_Edit:
		setWindowTitle(MetaTable->GetLocalName() + ": " + ObjectName);
		setWindowIcon(BUFFER_ICONS("Edit"));
		break;

	case ISNamespace::OFT_Copy:
		setWindowTitle(LOCALIZATION("Coping") + " (" + MetaTable->GetLocalName() + "): " + ObjectName);
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
		if (!WindowTitle.contains(SYMBOL_OBJECT_CHANGED))
		{
			WindowTitle += SYMBOL_OBJECT_CHANGED;
		}
	}
	else
	{
		if (WindowTitle.contains(SYMBOL_OBJECT_CHANGED))
		{
			WindowTitle.remove(SYMBOL_OBJECT_CHANGED);
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
		ActionFavorites->setChecked(ISFavorites::GetInstance().CheckExistFavoriteObject(MetaTable->GetName(), ObjectID));
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
	if (ISFavorites::GetInstance().CheckExistFavoriteObject(MetaTable->GetName(), ObjectID))
	{
		ISFavorites::GetInstance().DeleteFavorite(MetaTable->GetName(), ObjectID);
		ActionFavorites->setChecked(false);
		ISNotificationService::ShowNotification(LOCALIZATION("RecordRemoveFavorites").arg(ObjectName));
	}
	else
	{
		ISFavorites::GetInstance().AddFavorite(MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectName, ObjectID);
		ActionFavorites->setChecked(true);
		ISNotificationService::ShowNotification(LOCALIZATION("RecordAddFavorites").arg(ObjectName));
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::Delete()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->GetUID(), CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Edit").arg(MetaTable->GetLocalListName()));
		return;
	}

	if (RecordIsDeleted)
	{
		if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.RecoveryThisRecord")))
		{
			if (ISCore::DeleteOrRecoveryObject(ISNamespace::DRO_Recovery, MetaTable->GetName(), MetaTable->GetAlias(), GetObjectID(), MetaTable->GetLocalListName()))
			{
				emit UpdateList();
				close();
			}
		}
	}
	else
	{
		if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.DeleteThisRecord")))
		{
			if (ISCore::DeleteOrRecoveryObject(ISNamespace::DRO_Delete, MetaTable->GetName(), MetaTable->GetAlias(), GetObjectID(), MetaTable->GetLocalListName()))
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
	if (!ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->GetUID(), CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Edit").arg(MetaTable->GetLocalListName()));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Object.Delete.Cascade"), LOCALIZATION("Message.Object.Delete.Cascade.Help")))
	{
		if (ISCore::DeleteCascadeObject(MetaTable->GetName(), MetaTable->GetAlias(), GetObjectID()))
		{
			ISNotificationService::ShowNotification(LOCALIZATION("NotificationForm.Title.Deleted.Cascade").arg(GetObjectID()));
			ISProtocol::DeleteCascadeObject(MetaTable->GetName(), MetaTable->GetLocalListName(), GetObjectID());
			
			emit UpdateList();
			close();
		}
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CancelChanged()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.CancelChanged")))
	{
		ISGui::SetWaitGlobalCursor(true);

		if (FormType == ISNamespace::OFT_New)
		{
			for (const auto &MapItem : FieldsMap.toStdMap())
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
			if (MetaTable->GetField(FieldName)->GetQueryText().length()) //Если поле является мета-запросом
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
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.ReReadCard")))
	{
		ISGui::SetWaitGlobalCursor(true);
		for (const auto &FieldItem : FieldsMap.toStdMap())
		{
			QString FieldName = FieldItem.first;
			ISFieldEditBase *FieldWidget = FieldItem.second;

			ISQuery qSelect("SELECT " + MetaTable->GetAlias() + "_" + FieldName + " FROM " + MetaTable->GetName() + " WHERE " + MetaTable->GetAlias() + "_id = :ObjectID");
			qSelect.BindValue(":ObjectID", ObjectID);
			if (qSelect.ExecuteFirst())
			{
				QVariant ValueDB = qSelect.ReadColumn(MetaTable->GetAlias() + "_" + FieldName);
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
	for (int i = 0; i < ActionGroup->actions().count(); i++)
	{
		ActionGroup->actions().at(i)->setEnabled(Enabled);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::TimeoutAutoSave()
{
	if (GetModificationFlag())
	{
		TimerAutoSave->stop();
		Save();
		TimerAutoSave->start();
	}
}
//-----------------------------------------------------------------------------
QString ISObjectFormBase::GetObjectName() const
{
	return ObjectName;
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISObjectFormBase::GetFieldWidget(const QString &FieldName)
{
	return FieldsMap.value(FieldName);
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
