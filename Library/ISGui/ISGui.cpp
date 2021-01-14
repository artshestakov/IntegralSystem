#include "ISGui.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISSettingsDialog.h"
#include "ISUserPasswordDialog.h"
#include "ISMetaData.h"
#include "ISMessageBox.h"
#include "ISPopupMessage.h"
#include "ISCore.h"
#include "ISSelectListDialog.h"
//#include "ISTaskViewForm.h"
#include "ISSplashWidget.h"
#include "ISConfig.h"
#include "ISBuffer.h"
#include "ISVersionInfo.h"
#include "ISQueryPool.h"
#include "ISSettings.h"
#include "ISLogger.h"
#include "ISUserObjectForm.h"
#include "ISAlgorithm.h"
#include "ISUserRoleEntity.h"
#include "ISInputDialog.h"
#include "ISFavoritesForm.h"
#include "ISHistoryForm.h"
#include "ISProtocolObjectListForm.h"
#include "ISDiscussionListForm.h"
#include "ISStorageFilesSubSystem.h"
#include "ISCalendarObjectForm.h"
#include "ISDesktopParagraph.h"
#include "ISWorkspaceParagraph.h"
#include "ISCalendarParagraph.h"
//#include "ISTaskParagraph.h"
#include "ISProtocolSubSystem.h"
#include "ISInternalDirectoriesSubSystem.h"
#include "ISUserGroupSubSystem.h"
#include "ISMonitorActivitySubSystem.h"
#include "ISFullTextSearchSubSystem.h"
#include "ISSettingsDatabaseObjectForm.h"
//#include "ISTaskObjectForm.h"
#include "ISAsteriskCallsSubSystem.h"
#include "ISDelegates.h"
#include "ISFieldEdits.h"
#include "ISComboSearchWidgets.h"
#include "ISSettingFieldWidgets.h"
#include "ISSystem.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
bool ISGui::Startup(QString &ErrorString)
{
	//Инициализируем логгер
	if (!ISLogger::Instance().Initialize())
	{
		ErrorString = ISLogger::Instance().GetErrorString();
		return false;
	}

	//Читаем конфигурационный файл
	if (!ISConfig::Instance().Initialize("Client"))
	{
		ErrorString = ISConfig::Instance().GetErrorString();
		return false;
	}

	//Создание папки для временных файлов
	if (!ISSystem::CreateDir(QCoreApplication::applicationDirPath() + "/Temp", ErrorString))
	{
		return false;
	}

	ISDefines::Gui::Init();

	ISSplashWidget SplashWidget;
	SplashWidget.show();

	//Загрузка трансляций Qt
	if (!ISLocalization::Instance().LoadTraslatorQT())
	{
		ISLOGGER_W("ISLocalization", ISLocalization::Instance().GetErrorString());
	}

	//Загрузка локализации клиента
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_INTEGRAL_SYSTEM))
	{
		ErrorString = QString("Error init localization file \"%1\": %2").arg(LOCALIZATION_FILE_INTEGRAL_SYSTEM).arg(ISLocalization::Instance().GetErrorString());
		return false;
	}

	//Загрузка локализации объектов
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_OBJECTS))
	{
		ErrorString = QString("Error init localization file \"%1\": %2").arg(LOCALIZATION_FILE_OBJECTS).arg(ISLocalization::Instance().GetErrorString());
		return false;
	}

	//Загрузка буфера
	SplashWidget.SetText(LANG("SplashWidget.Buffer"));
	ISBuffer::Instance().Initialize();

	//Проверка наличия прав администратора
	SplashWidget.SetText(LANG("SplashWidget.AdminRoles"));
	if (!CheckAdminRole())
	{
		ErrorString = LANG("NoAdministratorRights");
		return false;
	}

	RegisterMetaType();
	qApp->setStyleSheet(BUFFER_STYLE_SHEET("QToolTip"));
	qApp->setWindowIcon(BUFFER_ICONS("Logo"));
	qApp->setApplicationName("IntegralSystem");
	qApp->setApplicationVersion(QString::number(ISVersionInfo::Instance().Info.Version));
	qApp->setFont(ISDefines::Gui::FONT_APPLICATION);
	QToolTip::setFont(ISDefines::Gui::FONT_APPLICATION);
	return true;
}
//-----------------------------------------------------------------------------
bool ISGui::CheckAdminRole()
{
	QFile File(QCoreApplication::applicationDirPath() + "/Temp/CheckAdmin");
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
bool ISGui::GetUSBDevice(std::vector<ISDeviceInfo> &Vector, QString &ErrorString)
{
	//Очищаем вектор и ошибку
	Vector.clear();
	ErrorString.clear();

	//Получаем объект классов
	HDEVINFO HDeviceInfo = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	bool ResultInit = HDeviceInfo != INVALID_HANDLE_VALUE, Result = true;
	if (!ResultInit)
	{
		ErrorString = LANG("GetUSBDevice.Error.SetupClassDevs").arg(ISAlgorithm::GetLastErrorString());
	}

	if (ResultInit)
	{
		//Обходим устройства пока не дойдём до конца
		for (unsigned i = 0; ; ++i)
		{
			SP_DEVINFO_DATA DeviceInfoData = { 0 };
			DeviceInfoData.cbSize = sizeof(DeviceInfoData);
			if (!SetupDiEnumDeviceInfo(HDeviceInfo, i, &DeviceInfoData)) //Дошли до конца - выходим из цикла
			{
				break;
			}

			//Если класс устройства не USB-носитель - идём дальше
			char Buffer[MAX_DEVICE_ID_LEN] = { 0 };
			sprintf(Buffer, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
				DeviceInfoData.ClassGuid.Data1, DeviceInfoData.ClassGuid.Data2, DeviceInfoData.ClassGuid.Data3,
				DeviceInfoData.ClassGuid.Data4[0], DeviceInfoData.ClassGuid.Data4[1], DeviceInfoData.ClassGuid.Data4[2], DeviceInfoData.ClassGuid.Data4[3],
				DeviceInfoData.ClassGuid.Data4[4], DeviceInfoData.ClassGuid.Data4[5], DeviceInfoData.ClassGuid.Data4[6], DeviceInfoData.ClassGuid.Data4[7]);
			if (strcmp(Buffer, DEVICE_CLASS_UID_USB) != 0)
			{
				continue;
			}

			unsigned long Return = CM_Get_Device_ID(DeviceInfoData.DevInst, Buffer, MAX_PATH, 0);
			Result = Return == CR_SUCCESS;
			if (!Result)
			{
				ErrorString = Return == CR_BUFFER_SMALL ?
					LANG("GetUSBDevice.Error.GettingDeviceID.BufferSmall") :
					LANG("GetUSBDevice.Error.GettingDeviceID").arg(ISAlgorithm::GetLastErrorString());
				break;
			}
			QString DeviceID((const char *)Buffer);

			//Проверяем, действительно ли это USB-накопитель
			//Если идентификатор не содержит VID или PID - идём дальше
			if (!DeviceID.contains("VID") || !DeviceID.contains("PID"))
			{
				continue;
			}

			//Получаем описание устройства
			unsigned long Size = 0,
				PropertyRegDataType = 0;
			unsigned char Description[1024] = { 0 };
			Result = SetupDiGetDeviceRegistryProperty(HDeviceInfo, &DeviceInfoData, SPDRP_DEVICEDESC,
				&PropertyRegDataType, Description, sizeof(Description), &Size) == TRUE;
			if (!Result) //Не удалось получить описание устройства
			{
				ErrorString = LANG("GetUSBDevice.Error.GettingDescription").arg(ISAlgorithm::GetLastErrorString());
				break;
			}

			//Парсим идентификатор устройства
			QString VendorID, ProductID, SerialNumber;
			QStringList StringList = DeviceID.split('\\');
			Result = StringList.size() == 3;
			if (!Result)
			{
				ErrorString = LANG("GetUSBDevice.Error.InvalidFormat");
				break;
			}
			SerialNumber = StringList.back();

			StringList = StringList[1].split('_');
			Result = StringList.size() == 3;
			if (!Result)
			{
				ErrorString = LANG("GetUSBDevice.Error.InvalidFormat");
				break;
			}
			ProductID = StringList.back();

			StringList = StringList[1].split('&');
			Result = StringList.size() == 2;
			if (!Result)
			{
				ErrorString = LANG("GetUSBDevice.Error.InvalidFormat");
				break;
			}
			VendorID = StringList.front();
			Vector.emplace_back(ISDeviceInfo{ VendorID, ProductID, SerialNumber, QString((const char *)Description) });
		}
	}
	
	//Если инициализация выше была успешной - разрушаем HDEVINFO
	if (ResultInit)
	{
		SetupDiDestroyDeviceInfoList(HDeviceInfo);
	}

	//Если результат отрицательный - чистим вектор
	if (!Result)
	{
		Vector.clear();
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISGui::RepaintWidget(QWidget *Widget, bool AdjustSize)
{
	if (Widget)
	{
		if (AdjustSize)
		{
			Widget->adjustSize();
		}
		Widget->repaint();
	}
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
		Y = QApplication::desktop()->availableGeometry().height() - Widget->height() - 30;
	}
	else if (MovePosition == ISNamespace::MWD_RightUp)
	{
		X = QApplication::desktop()->availableGeometry().width() - Widget->width() - 30;
		Y = 30;
	}
	else if (MovePosition == ISNamespace::MWD_RightBottom)
	{
		X = QApplication::desktop()->availableGeometry().width() - Widget->width() - 30;
		Y = QApplication::desktop()->availableGeometry().height() - Widget->height() - 30;
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
void ISGui::SetFontWidgetStrikeOut(QWidget *Widget, bool StrikeOut)
{
	QFont Font = Widget->font();
	Font.setStrikeOut(StrikeOut);
	Widget->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISGui::SetFontListWidgetItemStrikeOut(QListWidgetItem *ListWidgetItem, bool StrikeOut)
{
	QFont Font = ListWidgetItem->font();
	Font.setStrikeOut(StrikeOut);
	ListWidgetItem->setFont(Font);
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
	QByteArray ByteArray;
	QBuffer Buffer(&ByteArray);
	if (Buffer.open(QIODevice::WriteOnly))
	{
		QPixmap ConvertingPixmap = Pixmap;
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
int ISGui::GetStringWidth(const QString &String, const QFont &Font)
{
	return QFontMetrics(Font).width(String);
}
//-----------------------------------------------------------------------------
QColor ISGui::StringToColor(const QString &String)
{
	return QColor(String);
}
//-----------------------------------------------------------------------------
bool ISGui::OpenFolder(const QString &FolderPath)
{
	return QDesktopServices::openUrl(QUrl(FolderPath));
}
//-----------------------------------------------------------------------------
QString ISGui::ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat)
{
	return DateTime.isValid() ? ConvertDateToString(DateTime.date(), DateFormat) + SYMBOL_SPACE + DateTime.time().toString(TimeFormat) : QString();
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
bool ISGui::RecordsDelete(const QString &TableName, const ISVectorUInt &ObjectsID, QString &ErrorString)
{
	ISGui::SetWaitGlobalCursor(true);

	ISTcpQuery qDelete(API_RECORD_DELETE);
	qDelete.BindValue("TableName", TableName);

	QVariantList Objects;
	for (const unsigned int &ObjectID : ObjectsID)
	{
		Objects.push_back(ObjectID);
	}
	qDelete.BindValue("Objects", Objects);

	bool Result = qDelete.Execute();
	ISGui::SetWaitGlobalCursor(false);

	if (!Result)
	{
		ErrorString = qDelete.GetErrorString();
		return false;
	}
	ISPopupMessage::ShowNotification(ObjectsID.size() == 1 ?
		LANG("NotificationForm.Title.Deleted").arg(ObjectsID.front()) :
		LANG("NotificationForm.Title.Deleteds"));
	return true;
}
//-----------------------------------------------------------------------------
void ISGui::ExitApplication()
{
	ISTcpConnector::Instance().Disconnect();
	ISQueryPool::Instance().Shutdown();
	ISLogger::Instance().Shutdown();
	qApp->closeAllWindows();
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
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName);
	if (!MetaTable->ObjectForm.isEmpty()) //Если у мета-таблицы есть переопределенная форма объекта
	{
		ObjectForm = ISAlgorithm::CreatePointer<ISObjectFormBase *>(MetaTable->ObjectForm, Q_ARG(ISNamespace::ObjectFormType, FormType), Q_ARG(PMetaTable *, MetaTable), Q_ARG(QWidget *, parent), Q_ARG(int, ObjectID));
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
	QString SearchOperatorWidget = DataType == ISNamespace::FT_Int && MetaForeign ? "ISComboSearchBase" : ISMetaData::Instance().GetSearchWidget(DataType);
	ISComboSearchBase *ComboSearchBase = ISAlgorithm::CreatePointer<ISComboSearchBase *>(SearchOperatorWidget, Q_ARG(QWidget *, parent));
	return ComboSearchBase;
}
//-----------------------------------------------------------------------------
ISObjectPair ISGui::SelectObject(const QString &TableName, int SelectObjectID)
{
	SetWaitGlobalCursor(true);
	ISSelectListDialog SelectDialogForm(ISNamespace::SLM_Single, TableName, SelectObjectID);
	SetWaitGlobalCursor(false);
	return SelectDialogForm.Exec() ? SelectDialogForm.GetSelectedObject() : ISObjectPair();
}
//-----------------------------------------------------------------------------
ISVectorUInt ISGui::SelectObjects(const QString &TableName)
{
	ISVectorUInt VectorInt;
	SetWaitGlobalCursor(true);
	ISSelectListDialog SelectDialogForm(ISNamespace::SLM_Multi, TableName, 0);
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
	ISSettingsDialog SettingsForm(SettingGroupUID);
	SetWaitGlobalCursor(false);
	SettingsForm.Exec();
}
//-----------------------------------------------------------------------------
void ISGui::ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin)
{
	bool Result = true;
	ShowUserPasswordForm(UserID, UserFIO, UserLogin, Result);
}
//-----------------------------------------------------------------------------
void ISGui::ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin, bool &Result)
{
	SetWaitGlobalCursor(true);
	ISUserPasswordDialog UserPasswordForm(UserID, UserFIO, UserLogin);
	SetWaitGlobalCursor(false);
	Result = UserPasswordForm.Exec();
}
//-----------------------------------------------------------------------------
void ISGui::ShowObjectForm(QWidget *ObjectForm)
{
	ObjectForm->setParent(nullptr);
	QRect Rect = QDesktopWidget().availableGeometry();
	ObjectForm->resize((Rect.width() / 3) * 2, (Rect.height() / 3) * 2);
	MoveWidgetToDesktop(ObjectForm, ISNamespace::MWD_Center);
	ObjectForm->show();
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
void ISGui::ShowNoteObject(QWidget *parent, const QString &TableName, int ObjectID)
{
	ISTcpQuery TcpQuery(API_NOTE_RECORD_GET);
	TcpQuery.BindValue("TableName", TableName);
	TcpQuery.BindValue("ObjectID", ObjectID);
	if (!TcpQuery.Execute())
	{
		ISMessageBox::ShowCritical(parent, TcpQuery.GetErrorString());
		return;
	}
	QVariant Note = TcpQuery.GetAnswer()["Note"];

	bool Ok = true;
	Note = ISInputDialog::GetText(LANG("Note"), LANG("InputNote") + ':', Note, Ok);
	if (!Ok) //Примечание не введено - выходим из функции
	{
		return;
	}

	TcpQuery.BindValue("TableName", TableName);
	TcpQuery.BindValue("ObjectID", ObjectID);
	TcpQuery.BindValue("Note", Note);
	if (!TcpQuery.Execute(API_NOTE_RECORD_SET))
	{
		ISMessageBox::ShowCritical(parent, TcpQuery.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISGui::ShowFavoritesForm(const QString &TableName)
{
	ISGui::SetWaitGlobalCursor(true);
	(new ISFavoritesForm(nullptr, TableName))->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISGui::ShowHistoryForm()
{
	ISGui::SetWaitGlobalCursor(true);
	(new ISHistoryForm())->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
/*void ISGui::ShowTaskViewForm(int TaskID)
{
	ShowTaskViewForm(new ISTaskViewForm(TaskID));
}*/
//-----------------------------------------------------------------------------
/*void ISGui::ShowTaskViewForm(QWidget *TaskViewForm)
{
	if (TaskViewForm)
	{
		TaskViewForm->showMaximized();
	}
}*/
//-----------------------------------------------------------------------------
ISFieldEditBase* ISGui::CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField, ISNamespace::FieldType DataType, const QString &ControlWidget)
{
	ISFieldEditBase *FieldEditBase = nullptr;
	QString Temp = ControlWidget;
	if (!Temp.isEmpty())
	{
		FieldEditBase = ISAlgorithm::CreatePointer<ISFieldEditBase *>(Temp, Q_ARG(QWidget *, ParentWidget));
	}
	else
	{
		if (MetaField)
		{
			if (!MetaField->ControlWidget.isEmpty())
			{
				Temp = MetaField->ControlWidget;
			}
			else if (MetaField->Foreign)
			{
				Temp = CLASS_IS_LIST_EDIT;
			}
			else
			{
				Temp = ISMetaData::Instance().GetNameFieldEdit(MetaField->Type);
			}
		}
		else
		{
			Temp = ISMetaData::Instance().GetNameFieldEdit(DataType);
		}
		FieldEditBase = CreateFieldEditBase(ParentWidget, MetaField, DataType, Temp);
	}
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISGui::RegisterMetaType()
{
	qRegisterMetaType<std::vector<QSqlRecord>>("std::vector<QSqlRecord>");
	qRegisterMetaType<std::vector<QString>>("std::vector<QString>");
	qRegisterMetaType<ISVectorString>("ISVectorString");
	qRegisterMetaType<QSqlError>("QSqlError");
	qRegisterMetaType<ISUuid>("ISUuid");

	qRegisterMetaType<ISUserObjectForm*>("ISUserObjectForm");
	qRegisterMetaType<ISProtocolObjectListForm*>("ISProtocolObjectListForm");
	qRegisterMetaType<ISDiscussionListForm*>("ISDiscussionListForm");
	qRegisterMetaType<ISStorageFilesSubSystem*>("ISStorageFilesListForm");
	qRegisterMetaType<ISCalendarObjectForm*>("ISCalendarObjectForm");
	qRegisterMetaType<ISDesktopParagraph*>("ISDesktopForm");
	qRegisterMetaType<ISWorkspaceParagraph*>("ISWorkspaceForm");
	qRegisterMetaType<ISCalendarParagraph*>("ISCalendarForm");
	//qRegisterMetaType<ISTaskParagraph*>("ISTaskForm");
	qRegisterMetaType<ISProtocolSubSystem*>("ISProtocolListForm");
	qRegisterMetaType<ISInternalDirectoriesSubSystem*>("ISInternalDirectoriesForm");
	qRegisterMetaType<ISUserGroupSubSystem*>("ISUserGroupListForm");
	qRegisterMetaType<ISMonitorActivitySubSystem*>("ISMonitorActivityForm");
	qRegisterMetaType<ISFullTextSearchSubSystem*>("ISFullTextSearchForm");
	qRegisterMetaType<ISSettingsDatabaseObjectForm*>("ISSettingsDatabaseObjectForm");
	//qRegisterMetaType<ISTaskObjectForm*>("ISTaskObjectForm");
	qRegisterMetaType<ISAsteriskCallsSubSystem*>("ISAsteriskCallsListForm");

	qRegisterMetaType<ISDelegateBoolean*>("ISDelegateBoolean");
	qRegisterMetaType<ISDelegateImage*>("ISDelegateImage");
	qRegisterMetaType<ISDelegateColor*>("ISDelegateColor");

	qRegisterMetaType<ISLineEdit*>("ISLineEdit");
	qRegisterMetaType<ISCheckEdit*>("ISCheckEdit");
	qRegisterMetaType<ISIntegerEdit*>("ISIntegerEdit");
	qRegisterMetaType<ISDoubleEdit*>("ISDoubleEdit");
	qRegisterMetaType<ISMoneyEdit*>("ISMoneyEdit");
	qRegisterMetaType<ISTextEdit*>("ISTextEdit");
	qRegisterMetaType<ISUuidEdit*>("ISUuidEdit");
	qRegisterMetaType<ISDateTimeEdit*>("ISDateTimeEdit");
	qRegisterMetaType<ISDateEdit*>("ISDateEdit");
	qRegisterMetaType<ISTimeEdit*>("ISTimeEdit");
	qRegisterMetaType<ISImageEdit*>("ISImageEdit");
	qRegisterMetaType<ISListEdit*>("ISListEdit");
	qRegisterMetaType<ISPathEditFile*>("ISPathEditFile");
	qRegisterMetaType<ISPathEditDir*>("ISPathEditDir");
	qRegisterMetaType<ISColorEdit*>("ISColorEdit");
	qRegisterMetaType<ISPathEditDir*>("ISPathEditDir");
	qRegisterMetaType<ISWeekDayEdit*>("ISWeekDayEdit");
	qRegisterMetaType<ISINNEdit*>("ISINNEdit");
	qRegisterMetaType<ISOKPOEdit*>("ISOKPOEdit");
	qRegisterMetaType<ISKPPEdit*>("ISKPPEdit");
	qRegisterMetaType<ISVINEdit*>("ISVINEdit");
	qRegisterMetaType<ISOGRNEdit*>("ISOGRNEdit");
	qRegisterMetaType<ISYearEdit*>("ISYearEdit");
	qRegisterMetaType<ISComboTimeEdit*>("ISComboTimeEdit");
	qRegisterMetaType<ISPhoneEdit*>("ISPhoneEdit");
	qRegisterMetaType<ISBIKEdit*>("ISBIKEdit");
	qRegisterMetaType<ISEMailEdit*>("ISEMailEdit");
	qRegisterMetaType<ISDivisionCodeEdit*>("ISDivisionCodeEdit");
	qRegisterMetaType<ISVolumeEdit*>("ISVolumeEdit");
	qRegisterMetaType<ISBirthdayEdit*>("ISBirthdayEdit");
	qRegisterMetaType<ISMonthEdit*>("ISMonthEdit");
	qRegisterMetaType<ISTaskPriorityEdit*>("ISTaskPriorityEdit");
	qRegisterMetaType<ISTaskImportantEdit*>("ISTaskImportantEdit");
	qRegisterMetaType<ISTaskDescriptionEdit*>("ISTaskDescriptionEdit");
	qRegisterMetaType<ISTaskExecutorEdit*>("ISTaskExecutorEdit");
	qRegisterMetaType<ISUrlEdit*>("ISUrlEdit");
	qRegisterMetaType<ISFileEdit*>("ISFileEdit");

	qRegisterMetaType<ISComboSearchBase*>("ISComboSearchBase");
	qRegisterMetaType<ISComboSearchString*>("ISComboSearchString");
	qRegisterMetaType<ISComboSearchNumber*>("ISComboSearchNumber");

	qRegisterMetaType<ISComboTableDoubleClickEvent*>("ISComboTableDoubleClickEvent");
	qRegisterMetaType<ISComboCalendarNoteType*>("ISComboCalendarNoteType");
	qRegisterMetaType<ISComboTabBarSelectionBehavior*>("ISComboTabBarSelectionBehavior");
}
//-----------------------------------------------------------------------------
