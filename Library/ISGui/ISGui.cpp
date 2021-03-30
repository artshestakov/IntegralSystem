#include "ISGui.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISDialogsForm.h"
#include "ISMetaData.h"
#include "ISDialogsCommon.h"
#include "ISPopupMessage.h"
//#include "ISTaskViewForm.h"
#include "ISConfig.h"
#include "ISBuffer.h"
#include "ISVersionInfo.h"
#include "ISSettings.h"
#include "ISLogger.h"
#include "ISUserObjectForm.h"
#include "ISAlgorithm.h"
#include "ISUserRoleEntity.h"
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
#include "ISServerLogsSubSystem.h"
#include "ISServerInfoSubSystem.h"
#include "ISDelegates.h"
#include "ISFieldEdits.h"
#include "ISComboSearchWidgets.h"
#include "ISSettingFieldWidgets.h"
#include "ISSystem.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
#include "ISObjects.h"
#include "ISMetaSystemsEntity.h"
#include "ISFavorites.h"
#include "ISColumnSizer.h"
#include "ISParagraphEntity.h"
#include "ISQuerySubSystem.h"
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
	ISBuffer::Instance().Initialize();

	//Проверка наличия прав администратора
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
bool ISGui::Startup(ISSplashScreen *SplashScreen)
{
	ISTcpQuery qAuth(API_GET_META_DATA);
	if (!qAuth.Execute()) //Не удалось получить мета-данные - выходим с ошибкой
	{
		ISMessageBox::ShowCritical(SplashScreen, qAuth.GetErrorString());
		return false;
	}
	QVariantMap MetaData = qAuth.TakeAnswer();
	ISObjects::Instance().Initialize(ISBuffer::Instance().ConfigurationInfo.Name);
	ISSettingsDatabase::Instance().Initialize(MetaData["SettingsDB"].toMap());
	ISUserRoleEntity::Instance().InitializeTables(MetaData["AccessTables"].toMap());
	ISUserRoleEntity::Instance().InitializeSpecial(MetaData["AccessSpecial"].toList());
	ISMetaSystemsEntity::Instance().Initialize(MetaData["SystemSubSystem"].toList());
	ISFavorites::Instance().Initialize(MetaData["Favorite"].toMap());
	ISColumnSizer::Instance().Initialize(MetaData["ColumnSize"].toMap());
	ISSettings::Instance().Initialize(MetaData["Settings"].toList());
	ISParagraphEntity::Instance().Initialize(MetaData["Paragraphs"].toList());
	ISBuffer::Instance().SetTaskPriority(MetaData["TaskPriority"].toList());
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	if (!ISMetaData::Instance().Initialize(MetaData["MetaData"].toList())) //Не удалось проинициализировать мета-данные
	{
		ISMessageBox::ShowCritical(SplashScreen, LANG("Message.Error.InitializeMetaData"), ISMetaData::Instance().GetErrorString());
		return false;
	}
	ISObjects::Instance().GetInterface()->BeforeShowMainWindow();
	return true;
}
//-----------------------------------------------------------------------------
void ISGui::Shutdown(ISSplashScreen *SplashScreen)
{
	//Если связь с сервером есть - отправляем запрос на сохранение мета-данных
	//Нужно на случай если пользователь решил закрыть программу после обрыва соединения с сервером
	if (ISTcpConnector::Instance().IsConnected())
	{
		ISTcpQuery qSaveMetaData(API_SAVE_META_DATA);
		qSaveMetaData.BindValue("ColumnSize", ISColumnSizer::Instance().GetColumnSize());
		qSaveMetaData.BindValue("Settings", ISSettings::Instance().GetSettingsChanged());
		if (!qSaveMetaData.Execute())
		{
			ISMessageBox::ShowCritical(SplashScreen, qSaveMetaData.GetErrorString());
		}
	}
	ISGui::ExitApplication();
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
	if (MovePosition == ISNamespace::MoveWidgetDesktop::LeftUp)
	{
		X = 30;
		Y = 30;
	}
	else if (MovePosition == ISNamespace::MoveWidgetDesktop::LeftBottom)
	{
		X = 30;
		Y = QApplication::desktop()->availableGeometry().height() - Widget->height() - 30;
	}
	else if (MovePosition == ISNamespace::MoveWidgetDesktop::RightUp)
	{
		X = QApplication::desktop()->availableGeometry().width() - Widget->width() - 30;
		Y = 30;
	}
	else if (MovePosition == ISNamespace::MoveWidgetDesktop::RightBottom)
	{
		X = QApplication::desktop()->availableGeometry().width() - Widget->width() - 30;
		Y = QApplication::desktop()->availableGeometry().height() - Widget->height() - 30;
	}
	else if (MovePosition == ISNamespace::MoveWidgetDesktop::Center)
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
void ISGui::FavoriteObject(const QString &TableName, unsigned int ObjectID, bool &IsExist)
{
	IsExist = ISFavorites::Instance().Exist(TableName, ObjectID);

	ISTcpQuery qRecordFavorite(IsExist ? API_RECORD_FAVORITE_DELETE : API_RECORD_FAVORITE_ADD);
	qRecordFavorite.BindValue("TableName", TableName);
	qRecordFavorite.BindValue("ObjectID", ObjectID);

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qRecordFavorite.Execute();
	ISGui::SetWaitGlobalCursor(false);

	if (Result)
	{
		IsExist ? ISFavorites::Instance().Delete(TableName, ObjectID) : ISFavorites::Instance().Add(TableName, ObjectID);
		ISPopupMessage::ShowNotification(IsExist ? LANG("RecordRemoveFavorites") : LANG("RecordAddFavorites"));
	}
	else
	{
		ISMessageBox::ShowCritical(nullptr, qRecordFavorite.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISGui::ExitApplication()
{
	ISTcpConnector::Instance().Disconnect();
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
ISObjectPair ISGui::SelectObject(const QString &TableName, int SelectObjectID)
{
	SetWaitGlobalCursor(true);
	ISSelectListDialog SelectDialogForm(ISNamespace::SelectListMode::Single, TableName, SelectObjectID);
	SetWaitGlobalCursor(false);
	return SelectDialogForm.Exec() ? SelectDialogForm.GetSelectedObject() : ISObjectPair();
}
//-----------------------------------------------------------------------------
ISVectorUInt ISGui::SelectObjects(const QString &TableName)
{
	ISVectorUInt VectorInt;
	SetWaitGlobalCursor(true);
	ISSelectListDialog SelectDialogForm(ISNamespace::SelectListMode::Multi, TableName, 0);
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
	MoveWidgetToDesktop(ObjectForm, ISNamespace::MoveWidgetDesktop::Center);
	ObjectForm->show();
}
//-----------------------------------------------------------------------------
void ISGui::ShowRecordInfoForm(QWidget *parent, const QString &TableName, unsigned int ObjectID)
{
	ISGui::SetWaitGlobalCursor(true);
	ISRecordInfoDialog RecordInfoDialog(parent, TableName, ObjectID);
	ISGui::SetWaitGlobalCursor(false);
	RecordInfoDialog.Exec();
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
				Temp = ISMetaData::Instance().GetType(MetaField->Type).ControlWidget;
			}
		}
		else
		{
			Temp = ISMetaData::Instance().GetType(DataType).ControlWidget;
		}
		FieldEditBase = CreateFieldEditBase(ParentWidget, MetaField, DataType, Temp);
	}
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISGui::RegisterMetaType()
{
	qRegisterMetaType<std::vector<QString>>("std::vector<QString>");
	qRegisterMetaType<ISVectorString>("ISVectorString");

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
	qRegisterMetaType<ISServerLogsSubSystem*>("ISServerLogsSubSystem");
	qRegisterMetaType<ISServerInfoSubSystem*>("ISServerInfoSubSystem");
	qRegisterMetaType<ISQuerySubSystem*>("ISQuerySubSystem");

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
