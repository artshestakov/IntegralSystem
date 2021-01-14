#include "ISObjectFormBase.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaData.h"
#include "ISSettings.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISMessageBox.h"
#include "ISDatabase.h"
#include "ISAssert.h"
#include "ISPopupMessage.h"
#include "ISProtocol.h"
#include "ISListBaseForm.h"
#include "ISFavorites.h"
#include "ISCreatedObjectsEntity.h"
#include "ISUserRoleEntity.h"
#include "ISAlgorithm.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISObjectFormBase::ISObjectFormBase(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISInterfaceForm(parent),
	FormType(form_type),
	MetaTable(meta_table),
	ObjectID(object_id),
	ParentObjectID(0),
	EditObjectID(nullptr),
	BeginFieldEdit(nullptr),
	ModificationFlag(false),
	RightEdit(ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_EDIT)),
	CentralWidget(nullptr),
	ActionGroup(new QActionGroup(this))
{
	GetMainLayout()->setContentsMargins(0, 3, 0, 0);
	GetMainLayout()->setSpacing(0);

	CreateToolBarEscorts();
	CreateToolBar();
	CreateWidgetObject();
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
}
//-----------------------------------------------------------------------------
ISNamespace::ObjectFormType ISObjectFormBase::GetFormType()
{
	return FormType;
}
//-----------------------------------------------------------------------------
unsigned int ISObjectFormBase::GetObjectID() const
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
void ISObjectFormBase::AddVirtualField(const QString &FieldName, const QVariant &Value)
{
	VirtualFields[FieldName] = Value;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleNavigationBar(bool Visible)
{
	ToolBarEscort->setVisible(Visible);
	ToolBarEscortSeparator->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::SetVisibleField(const QString &FieldName, bool Visible)
{
	LabelsMap.at(FieldName)->setVisible(Visible);
	FieldsMap.at(FieldName)->setVisible(Visible);
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
void ISObjectFormBase::SetVisibleNote(bool Visible)
{
	if (ActionNoteObject)
	{
		ActionNoteObject->setVisible(Visible);
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::closeEvent(QCloseEvent *e)
{
	if (ModificationFlag)
	{
		emit CurrentObjectTab();
		ISMessageBox MessageBox(QMessageBox::Warning, LANG("SavingProcess"), LANG("Message.Question.SaveObjectChanged").arg(MetaTable->LocalName), QString(),
		{
			{ 1, LANG("Yes") },
			{ 2, LANG("No") },
			{ 3, LANG("Cancel"), true }
		}, this);
		MessageBox.setWindowIcon(BUFFER_ICONS("Save"));
        switch (MessageBox.Exec())
		{
		case 1: Save() ? ISInterfaceForm::closeEvent(e) : e->ignore(); break;
		case 2: ISInterfaceForm::closeEvent(e); break;
		case 3: e->ignore(); break;
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

	if (BeginFieldEdit)
	{
		BeginFieldEdit->SetFocus();
	}

	RenameReiconForm();
	UpdateObjectActions();
	SetModificationFlag(FormType == ISNamespace::OFT_Copy);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateToolBarEscorts()
{
	ToolBarEscort = new QToolBar(this);
	ToolBarEscort->setIconSize(ISDefines::Gui::SIZE_20_20);
	ToolBarEscort->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	connect(ToolBarEscort, &QToolBar::actionTriggered, this, &ISObjectFormBase::ToolBarClicked);
	GetMainLayout()->addWidget(ToolBarEscort);

	ToolBarEscortSeparator = ISControls::CreateHorizontalLine(this);
	GetMainLayout()->addWidget(ToolBarEscortSeparator);

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
	ActionSaveClose->setVisible(RightEdit);
	connect(ActionSaveClose, &QAction::triggered, this, &ISObjectFormBase::SaveClose);
	ToolBar->addAction(ActionSaveClose);

	//Сохранить карточку
	ActionSave = ISControls::CreateActionSave(ToolBar);
	ActionSave->setToolTip(LANG("SaveChange"));
	ActionSave->setVisible(RightEdit);
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
	ButtonAdditionally->setStyleSheet(BUFFER_STYLE_SHEET("QToolButtonMenu"));

	//Избранное
	ActionFavorites = new QAction(BUFFER_ICONS("Favorites"), LANG("AddToFavorites"), ToolBar);
	ActionFavorites->setPriority(QAction::LowPriority);
	ActionFavorites->setCheckable(true);
	connect(ActionFavorites, &QAction::triggered, this, &ISObjectFormBase::FavoriteClicked);
	AddActionToolBar(ActionFavorites);

	//Удалить карточку
	ActionDelete = ISControls::CreateActionDelete(ToolBar);
	ActionDelete->setPriority(QAction::LowPriority);
	connect(ActionDelete, &QAction::triggered, this, &ISObjectFormBase::Delete);
	AddActionMenu(ActionDelete);

	//Отменить изменения
	ActionCancelChange = new QAction(BUFFER_ICONS("CancelChangedObject"), LANG("CancelChanged"), ToolBar);
	ActionCancelChange->setPriority(QAction::LowPriority);
	connect(ActionCancelChange, &QAction::triggered, this, &ISObjectFormBase::CancelChanged);
	AddActionMenu(ActionCancelChange);

	//Примечание записи
	ActionNoteObject = ISControls::CreateActionNoteObject(ToolBar);
	ActionNoteObject->setPriority(QAction::LowPriority);
	connect(ActionNoteObject, &QAction::triggered, this, &ISObjectFormBase::NoteObject);
	AddActionMenu(ActionNoteObject);
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CreateWidgetObject()
{
	//Выполняем предварительные очистки (на случай, если виджет карточки объекта создаётся повторно)
	FieldsMap.clear();
	Layouts.clear();
	LabelsMap.clear();
	BeginFieldEdit = nullptr;

	QVBoxLayout *LayoutObject = new QVBoxLayout();
	LayoutObject->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	POINTER_DELETE(CentralWidget); //Удаляем центральный видджет (если есть)
	CentralWidget = new QWidget(this);
	CentralWidget->setLayout(LayoutObject);
	CentralWidget->setSizePolicy(CentralWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	GetMainLayout()->addWidget(CentralWidget);

	QFormLayout *FormLayout = new QFormLayout();

	ISScrollArea *ScrollArea = new ISScrollArea(CentralWidget);
	ScrollArea->widget()->setLayout(FormLayout);
	LayoutObject->addWidget(ScrollArea);

	//Если настроено добавление поле с кодом
	if (SETTING_BOOL(CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID))
	{
		QLabel *LabelObjectID = new QLabel(CentralWidget);
		LabelObjectID->setText(LANG("SystemField.ID") + ':');
		LabelObjectID->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

		EditObjectID = new ISLineEdit(CentralWidget);
		EditObjectID->SetValue(LANG("SystemField.ID.NotAssigned"));
		EditObjectID->SetToolTip(LANG("AutoFillField"));
		EditObjectID->SetReadOnly(true); //Последовательность setReadOnly и setCursor должна быть именно такой, иначе курсор не меняется
		EditObjectID->SetCursor(CURSOR_WHATS_THIS); //Последовательность setReadOnly и setCursor должна быть именно такой, иначе курсор не меняется
		EditObjectID->SetVisibleClear(false);
		EditObjectID->setSizePolicy(QSizePolicy::Maximum, EditObjectID->sizePolicy().verticalPolicy());
		FormLayout->addRow(LabelObjectID, EditObjectID);
	}

	//Создаём и размещаем на форме поля редактирования
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (MetaField->IsSystem) //Пропускаем поле "Код"
		{
			continue;
		}

		//Если тип поля ByteArray и для него не предусмотрен виджет редактирования - пропускать его
		if (MetaField->Type == ISNamespace::FT_ByteArray && MetaField->ControlWidget.isEmpty())
		{
			continue;
		}

		//Если поле отключено на форме объекта
		if (MetaField->HideFromObject)
		{
			continue;
		}

		//Если поле является виртуальным и сейчас режим создания копии - поле не размещаем
		if (!MetaField->QueryText.isEmpty() && FormType == ISNamespace::OFT_Copy)
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

	//Заполняем поля
	if (FormType == ISNamespace::OFT_Edit || FormType == ISNamespace::OFT_Copy)
	{
		ISTcpQuery qRecordGet(API_RECORD_GET);
		qRecordGet.BindValue("TableName", MetaTable->Name);
		qRecordGet.BindValue("ObjectID", ObjectID);
		if (qRecordGet.Execute())
		{
			//Если форма открыта на изменение - устанавливаем код в виджет
			if (FormType == ISNamespace::AT_Edit)
			{
				SetValueFieldID(ObjectID);
			}

			//Анализируем ответ
			QVariantMap AnswerMap = qRecordGet.GetAnswer();
			ObjectName = AnswerMap["ObjectName"].toString();
			AnswerMap = AnswerMap["Values"].toMap();

			//Обходим карту значений
			for (const auto &MapItem : AnswerMap.toStdMap())
			{
				//Если значение невалидное - перехолим к следующему
				if (!MapItem.second.isValid())
				{
					continue;
				}

				//Если такого поля нет (возможно это поле ID и т.д.) - переходим к следующему
				if (!FieldsMap.count(MapItem.first))
				{
					continue;
				}
				FieldsMap.at(MapItem.first)->SetValue(MapItem.second);
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, qRecordGet.GetErrorString());
		}
	}

	//Подключаем необходиые сигналы к полям
	for (ISFieldEditBase *FieldEditBase : ISAlgorithm::ConvertMapToValues(FieldsMap))
	{
		connect(FieldEditBase, &ISFieldEditBase::DataChanged, this, &ISObjectFormBase::DataChanged);
	}
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISObjectFormBase::CreateColumnForField(PMetaField *MetaField)
{
	ISFieldEditBase	*FieldEditBase = ISGui::CreateColumnForField(CentralWidget, MetaField);
	FieldsMap.emplace(MetaField->Name, FieldEditBase);
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::AddColumnForField(PMetaField *MetaField, ISFieldEditBase *FieldEditBase, QFormLayout *form_layout)
{
	QLabel *LabelField = new QLabel(CentralWidget);
	LabelField->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	LabelsMap.emplace(MetaField->Name, LabelField);

	if (MetaField->DefaultValueWidget.isValid()) //Если в поле указано значение по умолчанию для виджета-редактора
	{
		FieldEditBase->SetValue(MetaField->DefaultValueWidget);
	}

	if (MetaField->ReadOnly || !RightEdit) //Если доступно только для просмотра или прав на изменение нет
	{
		LabelField->setTextFormat(Qt::RichText);
		LabelField->setText(QString("<font>%1:</font><font color=#3D77B2 size=4>*</font>").arg(MetaField->LabelName));
		LabelField->setToolTip(LANG("FieldReadOnly"));
		LabelField->setCursor(CURSOR_WHATS_THIS);
		FieldEditBase->SetReadOnly(true);
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
		QLabel *LabelTab = new QLabel(CentralWidget);
		LabelTab->setText(MetaField->SeparatorName);
		LabelTab->setFont(ISDefines::Gui::FONT_TAHOMA_10_BOLD);

		QHBoxLayout *LayoutLine = new QHBoxLayout();
		LayoutLine->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

		QWidget *WidgetLine = new QWidget(CentralWidget);
		WidgetLine->setLayout(LayoutLine);
		LayoutLine->addWidget(ISControls::CreateHorizontalLinePlain(this));

		form_layout->addRow(LabelTab, WidgetLine);
	}

	if (Layouts.count(MetaField->LayoutName))
	{
		QHBoxLayout *LayoutHorizontal = Layouts[MetaField->LayoutName];
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
	if (GetModificationFlag()) //Если данные были изменены - просить сохранить
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SaveObjectFromContinue"));
		ToolBarEscort->actions()[0]->setChecked(true); //Выделяем кнопку-действие (ответственную за карточку объекта) обратно
		return;
	}

	for (QAction *Action: ToolBarEscort->actions()) //Обходим все эскортные действия
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

	ISGui::SetWaitGlobalCursor(true);
	bool IsObjectClicked = ActionClicked->property("IsObject").toBool();
	ToolBar->setVisible(IsObjectClicked);

	if (IsObjectClicked) //Выбор карточки объекта
	{
		CreateWidgetObject();
	}
	else //Выбор эскорта
	{
		POINTER_DELETE(CentralWidget);
		ISInterfaceMetaForm *WidgetEscort = nullptr;
		QString TableName = ActionClicked->property("TableName").toString(),
			parent_filter_field = ActionClicked->property("FilterField").toString(),
			ClassName = ActionClicked->property("ClassName").toString();
		if (ClassName.isEmpty()) //Открытие таблицы
		{
			ISListBaseForm *ListBaseForm = new ISListBaseForm(TableName, this);
			if (!parent_filter_field.isEmpty())
			{
				ListBaseForm->GetTcpQuery()->AddFilter(parent_filter_field, ObjectID);
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
		CentralWidget = WidgetEscort;
		WidgetEscort->LoadData();
	}
	ISGui::SetWaitGlobalCursor(false);
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
	QVariantMap ValuesMap;
	ISVectorString FieldsVector;
	QString QueryText;

	if (ParentObjectID && !ParentFilterField.isEmpty()) //Если форма является дочерней
	{
		ValuesMap[ParentFilterField] = ParentObjectID;
		FieldsVector.emplace_back(ParentFilterField);
	}

	for (const auto &MapItem : VirtualFields) //Обходим виртуальные поля
	{
		ValuesMap[MapItem.first] = MapItem.second;
		FieldsVector.emplace_back(MapItem.first);
	}

	for (const auto &Field : FieldsMap) //Обход существующих полей на форме
	{
		PMetaField *MetaField = MetaTable->GetField(Field.first); //Мета-поле
		if (!MetaField->QueryText.isEmpty()) //Если поле является виртуальным
		{
			continue;
		}

		//Получаем указатель на поле редактирования и вытаскиваем его значение
		ISFieldEditBase *FieldEditBase = Field.second;
		QVariant Value = FieldEditBase->GetValue();

		if (Value.isNull()) //Если значение в поле отсутствует, проверить обязательно ли поле для заполнения
		{
			if (MetaField->NotNull && !MetaField->HideFromObject && MetaField->DefaultValue.toString().isEmpty()) //Если поле обязательно для заполнения
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(MetaField->LabelName));
				FieldEditBase->BlinkRed();
				return false;
			}
		}

		//Если значения поля редактированя не изменялось, переходить к следующему
		if (!FieldEditBase->GetModificationFlag())
		{
			continue;
		}

		if (!FieldEditBase->IsValid()) //Если поле не прошло валидацию
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ValueFieldEditInvalid").arg(MetaField->LabelName));
			FieldEditBase->BlinkRed();
			return false;
		}

		FieldEditBase->Invoke();
		ValuesMap[Field.first] = Value;
		FieldsVector.emplace_back(Field.first);
	}

	//Формирование запроса на добавление/изменение/копирование
	ISTcpQuery TcpQuery(FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy
		? API_RECORD_ADD : API_RECORD_EDIT);
	TcpQuery.BindValue("TableName", MetaTable->Name);
	TcpQuery.BindValue("Values", ValuesMap);

	//Если форма открыта на редактирование - дополняем запрос идентификатором
	if (FormType == ISNamespace::OFT_Edit)
	{
		TcpQuery.BindValue("ObjectID", ObjectID);
	}

	SavedEvent();

	if (!TcpQuery.Execute()) //Запрос выполнен с ошибкой
	{
		ISMessageBox::ShowCritical(this, TcpQuery.GetErrorString());
		return false;
	}
	QVariantMap AnswerMap = TcpQuery.GetAnswer();

	if (FormType == ISNamespace::OFT_New || FormType == ISNamespace::OFT_Copy)
	{
		ObjectID = AnswerMap["ObjectID"].toUInt();
	}

	ObjectName = AnswerMap["ObjectName"].toString();
	switch (FormType)
	{
	case ISNamespace::OFT_New:
		FormType = ISNamespace::OFT_Edit;
		ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Created") + " - " + MetaTable->LocalName.toLower() + ':', ObjectName);
		break;
	case ISNamespace::OFT_Copy:
		FormType = ISNamespace::OFT_Edit;
		ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.CreatedCopy") + " - " + MetaTable->LocalName.toLower() + ':', ObjectName);
		break;
	case ISNamespace::OFT_Edit:
		ISPopupMessage::ShowNotification(LANG("NotificationForm.Title.Edited") + " - " + MetaTable->LocalName.toLower() + ':', ObjectName);
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
		ActionFavorites->setChecked(ISFavorites::Instance().Exist(MetaTable->Name, ObjectID));
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::FavoriteClicked()
{
	bool IsExist = ISFavorites::Instance().Exist(MetaTable->Name, ObjectID);

	ISTcpQuery qRecordFavorite(IsExist ? API_RECORD_FAVORITE_DELETE : API_RECORD_FAVORITE_ADD);
	qRecordFavorite.BindValue("TableName", MetaTable->Name);
	qRecordFavorite.BindValue("ObjectID", ObjectID);

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qRecordFavorite.Execute();
	ISGui::SetWaitGlobalCursor(false);

	if (Result)
	{
		IsExist ? ISFavorites::Instance().Delete(MetaTable->Name, ObjectID) : ISFavorites::Instance().Add(MetaTable->Name, ObjectID);
		ActionFavorites->setChecked(!IsExist);
		IsExist ? ISPopupMessage::ShowNotification(LANG("RecordRemoveFavorites").arg(ObjectName)) : ISPopupMessage::ShowNotification(LANG("RecordAddFavorites").arg(ObjectName));
	}
	else
	{
		ISMessageBox::ShowCritical(this, qRecordFavorite.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::Delete()
{
	if (!ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_DELETE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Delete").arg(MetaTable->LocalListName));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Object.Delete")))
	{
		QString ErrorString;
		if (ISGui::RecordsDelete(MetaTable->Name, { GetObjectID() }, ErrorString))
		{
			emit UpdateList();
			close();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteObject"), ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void ISObjectFormBase::CancelChanged()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CancelChanged")))
	{
		ISGui::SetWaitGlobalCursor(true);
		CreateWidgetObject();
		SetModificationFlag(false);
		BeginFieldEdit->SetFocus();
		ISGui::SetWaitGlobalCursor(false);
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
ISFieldEditBase* ISObjectFormBase::GetFieldWidget(const QString &FieldName)
{
	return FieldsMap.at(FieldName);
}
//-----------------------------------------------------------------------------
QToolBar* ISObjectFormBase::GetToolBar()
{
	return ToolBar;
}
//-----------------------------------------------------------------------------
