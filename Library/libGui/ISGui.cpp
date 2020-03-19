#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISSettingsForm.h"
#include "ISUserPasswordForm.h"
#include "ISMetaData.h"
#include "ISMessageBox.h"
#include "ISNotificationService.h"
#include "ISCore.h"
#include "ISRecordInfoForm.h"
#include "ISProtocol.h"
#include "ISSelectDialogForm.h"
#include "ISTaskViewForm.h"
#include "ISMemoryObjects.h"
#include "ISSplashWidget.h"
#include "ISConfig.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISRegisterMetaType.h"
#include "ISVersion.h"
#include "ISQuery.h"
#include "ISQueryPool.h"
#include "ISSettings.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static QString QS_SETTING_DATABASE_ID = PREPARE_QUERY("SELECT sgdb_id FROM _settingsdatabase WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static QString Q_DELETE_OR_RECOVERY_OBJECT = "UPDATE %1 SET %2_isdeleted = :IsDeleted WHERE %2_id = :ObjectID";
//-----------------------------------------------------------------------------
static QString QD_OBJECT_CASCADE = "DELETE FROM %1 WHERE %2_id = :ObjectID";
//-----------------------------------------------------------------------------
static QString QU_OBJECT = "UPDATE %1 SET %2_deletiondate = now(), %2_deletionuser = CURRENT_USER WHERE %2_id = %3";
//-----------------------------------------------------------------------------
bool ISGui::Startup(QString &ErrorString)
{
	bool Result = ISCore::Startup(true, ErrorString);
	if (!Result)
	{
		return Result;
	}

	ISDefines::Gui::Init();

	ISSplashWidget SplashWidget;
	SplashWidget.show();

	//Загрузка локализации
	ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CORE);
	ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_INTEGRAL_SYSTEM);
	ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_OBJECTS);

	//Загрузка стилей интерфейса
	SplashWidget.SetText(LANG("SplashWidget.Styles"));
	ISStyleSheet::GetInstance().Initialize();

	//Загрузка буфера
	SplashWidget.SetText(LANG("SplashWidget.Buffer"));
	ISBuffer::GetInstance().Initialize();

	//Проверка наличия прав администратора
	SplashWidget.SetText(LANG("SplashWidget.AdminRoles"));
	Result = CheckAdminRole();
	if (!Result)
	{
		ErrorString = LANG("NoAdministratorRights");
		return Result;
	}

	ISRegisterMetaType::RegisterMetaType();
	
	qApp->setStyleSheet(STYLE_SHEET("QToolTip"));
	qApp->setWindowIcon(BUFFER_ICONS("Logo"));
	qApp->setApplicationName("IntegralSystem");
	qApp->setApplicationVersion(ISVersion::Instance().ToString());
	qApp->setFont(ISDefines::Gui::FONT_APPLICATION);
	QToolTip::setFont(ISDefines::Gui::FONT_APPLICATION);

	return Result;
}
//-----------------------------------------------------------------------------
bool ISGui::CheckAdminRole()
{
	QFile File(ISDefines::Core::PATH_TEMP_DIR + "/CheckAdmin");
	bool Result = File.open(QIODevice::WriteOnly);
	if (Result)
	{
		File.close();
		File.remove();
	}
	else
	{
		if (File.error() != QFileDevice::OpenError)
		{
			Result = true;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISGui::CheckPressCapsLook()
{
#ifdef WIN32
	return GetKeyState(VK_CAPITAL) == 1 ? true : false;
#endif
}
//-----------------------------------------------------------------------------
QString ISGui::GetCurrentLayoutName()
{
	QString Result;

#ifdef WIN32
	char LayoutName[KL_NAMELENGTH];
	BOOL Ok = GetKeyboardLayoutName(LayoutName);
	if (Ok == TRUE)
	{
		switch (std::atoi(LayoutName))
		{
		case 409: Result = "ENG"; break;
		case 419: Result = "RUS"; break;
		default: Result = "UNKNOWN"; break;
		}
	}
#endif

	return Result;
}
//-----------------------------------------------------------------------------
void ISGui::RepaintWidget(QWidget *Widget, bool adjust_size)
{
	if (Widget)
	{
		if (adjust_size)
		{
			Widget->adjustSize();
		}
		Widget->repaint();
	}
}
//-----------------------------------------------------------------------------
void ISGui::ProcessEvents()
{
	qApp->processEvents();
}
//-----------------------------------------------------------------------------
void ISGui::MoveWidgetToDesktop(QWidget *Widget, ISNamespace::MoveWidgetDesktop MovePosition)
{
	int X = 0;
	int Y = 0;
	if (MovePosition == ISNamespace::MWD_LeftUp)
	{
		X = 30;
		Y = 30;
	}
	else if (MovePosition == ISNamespace::MWD_LeftBottom)
	{
		X = 30;

		Y = QApplication::desktop()->availableGeometry().height();
		Y = Y - Widget->height();
		Y = Y - 30;
	}
	else if (MovePosition == ISNamespace::MWD_RightUp)
	{
		X = QApplication::desktop()->availableGeometry().width();
		X = X - Widget->width();
		X = X - 30;

		Y = 30;
	}
	else if (MovePosition == ISNamespace::MWD_RightBottom)
	{
		X = QApplication::desktop()->availableGeometry().width();
		X = X - Widget->width();
		X = X - 30;

		Y = QApplication::desktop()->availableGeometry().height();
		Y = Y - Widget->height();
		Y = Y - 30;
	}
	else if (MovePosition == ISNamespace::MWD_Center)
	{
		QRect Rect = Widget->frameGeometry();
		Rect.moveCenter(QDesktopWidget().availableGeometry().center());

		X = Rect.topLeft().x();
		Y = Rect.topLeft().y();
	}
	Widget->move(X, Y);
}
//-----------------------------------------------------------------------------
void ISGui::SetWaitGlobalCursor(bool Wait)
{
	if (Wait)
	{
		QApplication::setOverrideCursor(CURSOR_WAIT);
	}
	else
	{
		QApplication::restoreOverrideCursor();
		if (QApplication::overrideCursor())
		{
			if (QApplication::overrideCursor()->shape() == CURSOR_WAIT)
			{
				QApplication::restoreOverrideCursor();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISGui::SetFontWidgetUnderline(QWidget *Widget, bool Underline)
{
	QFont Font = Widget->font();
	Font.setUnderline(Underline);
	Widget->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISGui::SetFontWidgetItalic(QWidget *Widget, bool Italic)
{
	QFont Font = Widget->font();
	Font.setItalic(Italic);
	Widget->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISGui::SetFontWidgetBold(QWidget *Widget, bool Bold)
{
	QFont Font = Widget->font();
	Font.setBold(Bold);
	Widget->setFont(Font);
}
//-----------------------------------------------------------------------------
QFont ISGui::StringToFont(const QString &FontText)
{
	return !FontText.isEmpty() ? qvariant_cast<QFont>(FontText) : QApplication::font().toString();
}
//-----------------------------------------------------------------------------
QByteArray ISGui::IconToByteArray(const QIcon &Icon)
{
	QPixmap PixmapIcon = Icon.pixmap(ISDefines::Gui::SIZE_16_16);
	QByteArray ByteArray;
	QBuffer Buffer(&ByteArray);
	if (Buffer.open(QIODevice::WriteOnly))
	{
		PixmapIcon.save(&Buffer, "PNG");
	}
	return ByteArray;
}
//-----------------------------------------------------------------------------
QIcon ISGui::ByteArrayToIcon(const QByteArray &ByteArray)
{
	QIcon Icon;
	QPixmap Pixmap;
	if (Pixmap.loadFromData(ByteArray, "PNG"))
	{
		Icon = QIcon(Pixmap);
	}
	return Icon;
}
//-----------------------------------------------------------------------------
QByteArray ISGui::PixmapToByteArray(const QPixmap &Pixmap)
{
	QPixmap ConvertingPixmap = Pixmap;
	QByteArray ByteArray;
	QBuffer Buffer(&ByteArray);
	if (Buffer.open(QIODevice::WriteOnly));
	{
		ConvertingPixmap.save(&Buffer, "PNG");
	}
	return ByteArray;
}
//-----------------------------------------------------------------------------
QPixmap ISGui::ByteArrayToPixmap(const QByteArray &ByteArray)
{
	QPixmap Pixmap;
	Pixmap.loadFromData(ByteArray);
	return Pixmap;
}
//-----------------------------------------------------------------------------
QIcon ISGui::GetIconFile(const QString &FilePath)
{
	return QFileIconProvider().icon(QFileInfo(FilePath));
}
//-----------------------------------------------------------------------------
bool ISGui::OpenUrl(const QString &Url)
{
	SetWaitGlobalCursor(true);
	bool Result = QDesktopServices::openUrl(QUrl(Url));
	SetWaitGlobalCursor(false);
	return Result;
}
//-----------------------------------------------------------------------------
bool ISGui::OpenFile(const QString &FilePath)
{
	SetWaitGlobalCursor(true);
	bool Result = QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(FilePath)));
	SetWaitGlobalCursor(false);
	return Result;
}
//-----------------------------------------------------------------------------
QString ISGui::PrepareLongToolTip(QString ToolTipText, int MaxWidth)
{
	QFontMetrics FontMetrics(QToolTip::font());
	QString Result;
	for (;;)
	{
		int i = 0;
		while (i < ToolTipText.length())
		{
			if (FontMetrics.width(ToolTipText.left(++i + 1)) > MaxWidth)
			{
				int j = ToolTipText.lastIndexOf(' ', i);
				if (j > 0)
				{
					i = j;
				}

				Result += ToolTipText.left(i);
				Result += '\n';
				ToolTipText = ToolTipText.mid(i + 1);
				break;
			}
		}

		if (i >= ToolTipText.length())
		{
			break;
		}
	}
	return Result + ToolTipText;
}
//-----------------------------------------------------------------------------
QColor ISGui::StringToColor(const QString &String)
{
	return QColor(String);
}
//-----------------------------------------------------------------------------
bool ISGui::IsStringUrl(const QString &Url)
{
	int Pos = 0;
	QString Temp = Url;
	return QRegExpValidator(QRegExp(REG_EXP_URL)).validate(Temp, Pos) == QValidator::Acceptable;
}
//-----------------------------------------------------------------------------
bool ISGui::OpenFolder(const QString &FolderPath)
{
	return QDesktopServices::openUrl(QUrl(FolderPath));
}
//-----------------------------------------------------------------------------
QString ISGui::ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat)
{
	return ConvertDateToString(DateTime.date(), DateFormat) + SYMBOL_SPACE + LANG("At") + SYMBOL_SPACE + DateTime.time().toString(TimeFormat);
}
//-----------------------------------------------------------------------------
QString ISGui::ConvertDateToString(const QDate &Date, const QString &DateFormat)
{
	QString Result;
	if (Date == QDate::currentDate().addDays(-1)) //Вчера
	{
		Result = LANG("Yesterday");
	}
	else if (Date == QDate::currentDate()) //Сегодня
	{
		Result = LANG("Today");
	}
	else if (Date == QDate::currentDate().addDays(1)) //Завтра
	{
		Result = LANG("Tomorrow");
	}
	else
	{
		Result = Date.toString(DateFormat);
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISGui::ChangeUser()
{
	RestartApplication();
}
//-----------------------------------------------------------------------------
void ISGui::RestartApplication()
{
	ExitApplication();
	ISSystem::SleepSeconds(1);
	QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_FILE);
}
//-----------------------------------------------------------------------------
void ISGui::ExitApplication()
{
	if (ISMemoryObjects::GetInstance().GetMainWindow())
	{
		ISMemoryObjects::GetInstance().GetMainWindow()->setProperty("CloseEvent", false);
	}
	qApp->closeAllWindows();
	ISCore::ExitApplication();
}
//-----------------------------------------------------------------------------
bool ISGui::DeleteOrRecoveryObject(ISNamespace::DeleteRecoveryObject DeleteOrRecovery, const QString &TableName, const QString &TableAlias, int ID, const QString &LocalListName)
{
	QString QueryText = Q_DELETE_OR_RECOVERY_OBJECT.arg(TableName).arg(TableAlias);

	ISQuery qDeleteOrRecovery(QueryText);
	qDeleteOrRecovery.BindValue(":ObjectID", ID);
	qDeleteOrRecovery.BindValue(":IsDeleted", DeleteOrRecovery == ISNamespace::DRO_Delete ? true : false);
	bool Result = qDeleteOrRecovery.Execute();
	if (Result && DeleteOrRecovery == ISNamespace::DRO_Delete)
	{
		QString UpdateQuery = QU_OBJECT;
		UpdateQuery = UpdateQuery.arg(TableName);
		UpdateQuery = UpdateQuery.arg(TableAlias);
		UpdateQuery = UpdateQuery.arg(ID);
		ISQueryPool::GetInstance().AddQuery(UpdateQuery);
		DeleteOrRecovery == ISNamespace::DRO_Delete ? ISProtocol::DeleteObject(TableName, LocalListName, ID) : ISProtocol::RecoveryObject(TableName, LocalListName, ID);
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISGui::DeleteCascadeObject(const QString &TableName, const QString &TableAlias, int ObjectID)
{
	QString QueryText = QD_OBJECT_CASCADE.arg(TableName).arg(TableAlias);
	ISQuery qDeleteCascade(QueryText);
	qDeleteCascade.BindValue(":ObjectID", ObjectID);
	return qDeleteCascade.Execute();
}
//-----------------------------------------------------------------------------
void ISGui::ExecuteStartCommand()
{
	if (SETTING_BOOL(CONST_UID_SETTING_EVENTS_EVENT_AT_STARTUP))
	{
		QString CommandText = SETTING_STRING(CONST_UID_SETTING_EVENTS_STARTUP_COMMAND);
		if (CommandText.length())
		{
			ISLOGGER_DEBUG(QString("Executing command: %1").arg(CommandText));
			int ExitCode = QProcess::execute("cmd.exe", QStringList() << "/C" << CommandText);
			if (ExitCode == 0)
			{
				ISLOGGER_DEBUG("Executed command done.");
			}
			else
			{
				ISLOGGER_DEBUG("Executed command error.");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISGui::ExecuteExitComamnd()
{
	if (SETTING_BOOL(CONST_UID_SETTING_EVENTS_EVENT_ON_EXIT))
	{
		QString CommandText = SETTING_STRING(CONST_UID_SETTING_EVENTS_EXIT_COMMAND);
		if (CommandText.length())
		{
			ISLOGGER_DEBUG(QString("Executing command: %1").arg(CommandText));
			int ExitCode = QProcess::execute("cmd.exe", QStringList() << "/C" << CommandText);
			if (ExitCode == 0)
			{
				ISLOGGER_DEBUG("Executed command done.");
			}
			else
			{
				ISLOGGER_DEBUG("Executed command error.");
			}
		}
	}
}
//-----------------------------------------------------------------------------
int ISGui::CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName, int ObjectID)
{
	int CalendarID = ISCore::CalendarInsert(DateTime, Name, Text, TableName, ObjectID);
	if (CalendarID)
	{
		ISNotificationService::ShowNotification(LANG("NotifyInsertedDone").arg(Name));
	}
	else
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotifyNotInserted"));
	}
	return CalendarID;
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISGui::CreateColumnForField(QWidget *ParentWidget, PMetaField *MetaField)
{
	return CreateFieldEditBase(ParentWidget, MetaField);
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISGui::CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget)
{
	return CreateFieldEditBase(ParentWidget, nullptr, DataType, ControlWidget);
}
//-----------------------------------------------------------------------------
ISObjectFormBase* ISGui::CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID, QWidget *parent)
{
	SetWaitGlobalCursor(true);
	ISObjectFormBase *ObjectForm = nullptr;
	PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	if (!MetaTable->ObjectForm.isEmpty()) //Если у мета-таблицы есть переопределенная форма объекта
	{
		int ObjectType = QMetaType::type((MetaTable->ObjectForm + SYMBOL_STAR).toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("ObjectForm for table \"%1\" is null.").arg(MetaTable->ObjectForm));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		ObjectForm = dynamic_cast<ISObjectFormBase*>(MetaObject->newInstance(Q_ARG(ISNamespace::ObjectFormType, FormType), Q_ARG(PMetaTable *, MetaTable), Q_ARG(QWidget *, parent), Q_ARG(int, ObjectID)));
		IS_ASSERT(ObjectForm, "ObjectForm not dynamic cast.");
	}
	else //У мета-таблицы нет переопределенной формы объекта - создаем базовую
	{
		ObjectForm = new ISObjectFormBase(FormType, MetaTable, parent, ObjectID);
	}
	SetWaitGlobalCursor(false);
	return ObjectForm;
}
//-----------------------------------------------------------------------------
ISComboSearchBase* ISGui::CreateSearchOperator(QWidget *parent, ISNamespace::FieldType DataType, PMetaForeign *MetaForeign)
{
	QString SearchOperatorWidget;

	if (DataType == ISNamespace::FT_Int && MetaForeign)
	{
		SearchOperatorWidget = "ISComboSearchBase";
	}
	else
	{
		SearchOperatorWidget = ISMetaData::GetInstanse().GetSearch(DataType);
	}

	int ObjectType = QMetaType::type((SearchOperatorWidget + SYMBOL_STAR).toLocal8Bit().constData());
	IS_ASSERT(ObjectType, "Widget search operator is null.");

	const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
	ISComboSearchBase *ComboSearchBase = dynamic_cast<ISComboSearchBase*>(MetaObject->newInstance(Q_ARG(QWidget *, parent)));
	IS_ASSERT(ComboSearchBase, "Widget search operator not dynamic cast.");
	return ComboSearchBase;
}
//-----------------------------------------------------------------------------
int ISGui::SelectObject(const QString &TableName, int SelectObjectID)
{
	int SelectedObjectID = 0;
	SetWaitGlobalCursor(true);
	ISSelectDialogForm SelectDialogForm(ISNamespace::SLM_Single, TableName, SelectObjectID);
	SetWaitGlobalCursor(false);
	if (SelectDialogForm.Exec())
	{
		SelectedObjectID = SelectDialogForm.GetSelectedObject();
	}
	return SelectedObjectID;
}
//-----------------------------------------------------------------------------
ISVectorInt ISGui::SelectObjects(const QString &TableName)
{
	ISVectorInt VectorInt;
	SetWaitGlobalCursor(true);
	ISSelectDialogForm SelectDialogForm(ISNamespace::SLM_Multi, TableName, 0);
	SetWaitGlobalCursor(false);
	if (SelectDialogForm.Exec())
	{
		VectorInt = SelectDialogForm.GetSelectedObjects();
	}
	return VectorInt;
}
//-----------------------------------------------------------------------------
void ISGui::ShowSettingsForm(const QString &SettingGroupUID)
{
	SetWaitGlobalCursor(true);
	ISSettingsForm SettingsForm(SettingGroupUID);
	SetWaitGlobalCursor(false);
	SettingsForm.Exec();
}
//-----------------------------------------------------------------------------
bool ISGui::ShowUserPasswordForm(int UserID)
{
	SetWaitGlobalCursor(true);
	ISUserPasswordForm UserPasswordForm(UserID);
	SetWaitGlobalCursor(false);
	return UserPasswordForm.Exec();
}
//-----------------------------------------------------------------------------
void ISGui::ShowSystemInfoRecord(PMetaTable *MetaTable, int ObjectID)
{
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_SYSTEM_INFO_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID);
	SetWaitGlobalCursor(true);
	ISRecordInfoForm RecordInfoForm(MetaTable, ObjectID);
	SetWaitGlobalCursor(false);
	RecordInfoForm.Exec();
}
//-----------------------------------------------------------------------------
void ISGui::ShowDatabaseSettings()
{
	ISGui::SetWaitGlobalCursor(true);
	ISQuery qSelectID(QS_SETTING_DATABASE_ID);
	qSelectID.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	bool Result = qSelectID.ExecuteFirst();
	if (Result)
	{
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_SettingsDatabase", qSelectID.ReadColumn("sgdb_id").toInt())->showMaximized();
	}
	ISGui::SetWaitGlobalCursor(false);
	if (!Result)
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.SettingsDatabaseNotExist"));
	}
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QPixmap &Pixmap)
{
	SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewerForm = new ISImageViewerForm(Pixmap);
	ImageViewerForm->showMaximized();
	SetWaitGlobalCursor(false);
	return ImageViewerForm;
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QString &FilePath)
{
	SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewerForm = new ISImageViewerForm(FilePath);
	ImageViewerForm->showMaximized();
	SetWaitGlobalCursor(false);
	return ImageViewerForm;
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QByteArray &ByteArray)
{
	return ShowImageForm(ByteArrayToPixmap(ByteArray));
}
//-----------------------------------------------------------------------------
void ISGui::ShowTaskViewForm(int TaskID)
{
	SetWaitGlobalCursor(true);
	ISTaskViewForm *TaskViewForm = new ISTaskViewForm(TaskID);
	TaskViewForm->showMaximized();
	SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISGui::ShowTaskObjectForm(ISNamespace::ObjectFormType FormType, int TaskID)
{
	ISObjectFormBase *TaskObjectForm = CreateObjectForm(FormType, "_Task", TaskID);
	ShowTaskObjectForm(TaskObjectForm);
}
//-----------------------------------------------------------------------------
void ISGui::ShowTaskObjectForm(QWidget *TaskObjectForm)
{
	TaskObjectForm->setParent(nullptr);
	TaskObjectForm->resize(ISDefines::Gui::SIZE_TASK_OBJECT_FORM);
	MoveWidgetToDesktop(TaskObjectForm, ISNamespace::MWD_Center);
	TaskObjectForm->show();
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISGui::CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField, ISNamespace::FieldType DataType, const QString &ControlWidget)
{
	ISFieldEditBase *FieldEditBase = nullptr;
	QString Temp = ControlWidget;
	if (!Temp.isEmpty())
	{
		int ObjectType = QMetaType::type((Temp + SYMBOL_STAR).toLocal8Bit().constData());
		if (ObjectType)
		{
			const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
			QObject *Object = MetaObject->newInstance((Q_ARG(QWidget *, ParentWidget)));
			if (Object)
			{
				FieldEditBase = dynamic_cast<ISFieldEditBase*>(Object);
			}
		}
	}
	else
	{
		if (MetaField)
		{
			if (MetaField && !MetaField->ControlWidget.isEmpty()) //??? нужна ли двойная проверка MetaField
			{
				Temp = MetaField->ControlWidget;
			}
			else if (MetaField->Foreign)
			{
				Temp = CLASS_IS_LIST_EDIT;
			}
			else
			{
				Temp = ISMetaData::GetInstanse().GetNameFieldEdit(MetaField->Type);
			}
		}
		else
		{
			Temp = ISMetaData::GetInstanse().GetNameFieldEdit(DataType);
		}
		FieldEditBase = CreateFieldEditBase(ParentWidget, MetaField, DataType, Temp);
	}
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
