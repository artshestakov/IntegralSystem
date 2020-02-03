#include "StdAfx.h"
#include "ISGui.h"
#include "ISDefines.h"
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
	QPixmap PixmapIcon = Icon.pixmap(SIZE_16_16);
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
	bool Result = false;
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
ISFieldEditBase* ISGui::CreateColumnForField(QWidget *ParentWidget, PMetaClassField *MetaField)
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
	ISGui::SetWaitGlobalCursor(true);
	ISObjectFormBase *ObjectForm = nullptr;
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	if (MetaTable->GetObjectForm().length()) //Если у мета-таблицы есть переопределенная форма объекта
	{
		int ObjectType = QMetaType::type((MetaTable->GetObjectForm() + '*').toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("ObjectForm for table \"%1\" is null.").arg(MetaTable->GetObjectForm()));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		ObjectForm = dynamic_cast<ISObjectFormBase*>(MetaObject->newInstance(Q_ARG(ISNamespace::ObjectFormType, FormType), Q_ARG(PMetaClassTable *, MetaTable), Q_ARG(QWidget *, parent), Q_ARG(int, ObjectID)));
		IS_ASSERT(ObjectForm, "ObjectForm not dynamic cast.");
	}
	else //У мета-таблицы нет переопределенной формы объекта - создаем базовую
	{
		ObjectForm = new ISObjectFormBase(FormType, MetaTable, parent, ObjectID);
	}

	ISGui::SetWaitGlobalCursor(false);
	return ObjectForm;
}
//-----------------------------------------------------------------------------
ISComboSearchBase* ISGui::CreateSearchOperator(QWidget *parent, ISNamespace::FieldType DataType, QObject *MetaForeign)
{
	QString SearchOperatorWidget;

	if (DataType == ISNamespace::FT_Int && MetaForeign)
	{
		SearchOperatorWidget = "ISComboSearchBase";
	}
	else
	{
		SearchOperatorWidget = ISMetaData::GetInstanse().GetAssociationTypes().GetSearchOperatorWidget(DataType);
	}

	int ObjectType = QMetaType::type((SearchOperatorWidget + '*').toLocal8Bit().constData());
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
	ISGui::SetWaitGlobalCursor(true);
	ISSelectDialogForm SelectDialogForm(ISNamespace::SLM_Single, TableName, SelectObjectID);
	ISGui::SetWaitGlobalCursor(false);
	if (SelectDialogForm.Exec())
	{
		SelectedObjectID = SelectDialogForm.GetSelectedObject();
	}

	return SelectedObjectID;
}
//-----------------------------------------------------------------------------
QVectorInt ISGui::SelectObjects(const QString &TableName)
{
	QVectorInt VectorInt;
	ISGui::SetWaitGlobalCursor(true);
	ISSelectDialogForm SelectDialogForm(ISNamespace::SLM_Multi, TableName, 0);
	ISGui::SetWaitGlobalCursor(false);
	if (SelectDialogForm.Exec())
	{
		VectorInt = SelectDialogForm.GetSelectedObjects();
	}

	return VectorInt;
}
//-----------------------------------------------------------------------------
void ISGui::ShowSettingsForm(const QString &SettingGroupUID)
{
	ISGui::SetWaitGlobalCursor(true);
	ISSettingsForm SettingsForm(SettingGroupUID);
	ISGui::SetWaitGlobalCursor(false);
	SettingsForm.Exec();
}
//-----------------------------------------------------------------------------
bool ISGui::ShowUserPasswordForm(int UserID)
{
	ISGui::SetWaitGlobalCursor(true);
	ISUserPasswordForm UserPasswordForm(UserID);
	ISGui::SetWaitGlobalCursor(false);
	return UserPasswordForm.Exec();
}
//-----------------------------------------------------------------------------
void ISGui::ShowSystemInfoRecord(PMetaClassTable *MetaTable, int ObjectID)
{
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_SYSTEM_INFO_OBJECT, MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectID);
	ISGui::SetWaitGlobalCursor(true);
	ISRecordInfoForm RecordInfoForm(MetaTable, ObjectID);
	ISGui::SetWaitGlobalCursor(false);
	RecordInfoForm.Exec();
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QPixmap &Pixmap)
{
	ISGui::SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewerForm = new ISImageViewerForm(Pixmap);
	ImageViewerForm->showMaximized();
	ISGui::SetWaitGlobalCursor(false);
	return ImageViewerForm;
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QString &FilePath)
{
	ISGui::SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewerForm = new ISImageViewerForm(FilePath);
	ImageViewerForm->showMaximized();
	ISGui::SetWaitGlobalCursor(false);
	return ImageViewerForm;
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QByteArray &ByteArray)
{
	return ShowImageForm(ISGui::ByteArrayToPixmap(ByteArray));
}
//-----------------------------------------------------------------------------
void ISGui::ShowTaskViewForm(int TaskID)
{
	ISGui::SetWaitGlobalCursor(true);
	ISTaskViewForm *TaskViewForm = new ISTaskViewForm(TaskID);
	TaskViewForm->showMaximized();
	ISGui::SetWaitGlobalCursor(false);
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
	TaskObjectForm->resize(SIZE_TASK_OBJECT_FORM);
	ISGui::MoveWidgetToDesktop(TaskObjectForm, ISNamespace::MWD_Center);
	TaskObjectForm->show();
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISGui::CreateFieldEditBase(QWidget *ParentWidget, PMetaClassField *MetaField, ISNamespace::FieldType DataType, const QString &ControlWidget)
{
	ISFieldEditBase *FieldEditBase = nullptr;
	QString Temp = ControlWidget;

	if (Temp.length())
	{
		int ObjectType = QMetaType::type((Temp + '*').toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("ObjectType for field edit \"%1\" NULL.").arg(Temp));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		QObject *Object = MetaObject->newInstance((Q_ARG(QWidget *, ParentWidget)));
		IS_ASSERT(Object, QString("Not created QObject for ClassName: %1").arg(Temp));

		FieldEditBase = dynamic_cast<ISFieldEditBase*>(Object);
		IS_ASSERT(FieldEditBase, QString("Not created ISFieldEditBase for ClassName: %1").arg(Temp));
	}
	else
	{
		if (MetaField)
		{
			if (MetaField && MetaField->GetControlWidget().length())
			{
				Temp = MetaField->GetControlWidget();
			}
			else if (MetaField->GetForeign())
			{
				Temp = CLASS_IS_LIST_EDIT;
			}
			else
			{
				Temp = ISMetaData::GetInstanse().GetAssociationTypes().GetControlWidgetFromType(MetaField->GetType());
			}
		}
		else
		{
			Temp = ISMetaData::GetInstanse().GetAssociationTypes().GetControlWidgetFromType(DataType);
		}

		FieldEditBase = CreateFieldEditBase(ParentWidget, MetaField, DataType, Temp);
	}

	return FieldEditBase;
}
//-----------------------------------------------------------------------------
