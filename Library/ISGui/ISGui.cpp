#include "StdAfx.h"
#include "ISGui.h"
#include "EXDefines.h"
#include "EXConstants.h"
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
//-----------------------------------------------------------------------------
ISGui::ISGui() : QObject()
{

}
//-----------------------------------------------------------------------------
ISGui::~ISGui()
{

}
//-----------------------------------------------------------------------------
int ISGui::CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName, int ObjectID)
{
	int CalendarID = ISCore::CalendarInsert(DateTime, Name, Text, TableName, ObjectID);
	if (CalendarID)
	{
		ISNotificationService::ShowNotification(LOCALIZATION("NotifyInsertedDone").arg(Name));
	}
	else
	{
		ISMessageBox::ShowWarning(nullptr, LOCALIZATION("Message.Warning.NotifyNotInserted"));
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
	ISSystem::SetWaitGlobalCursor(true);
	ISObjectFormBase *ObjectForm = nullptr;
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	if (MetaTable->GetObjectForm().length()) //Если у мета-таблицы есть переопределенная форма объекта
	{
		int ObjectType = QMetaType::type((MetaTable->GetObjectForm() + "*").toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("ObjectForm for table \"%1\" is null.").arg(MetaTable->GetObjectForm()));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		ObjectForm = dynamic_cast<ISObjectFormBase*>(MetaObject->newInstance(Q_ARG(ISNamespace::ObjectFormType, FormType), Q_ARG(PMetaClassTable *, MetaTable), Q_ARG(QWidget *, parent), Q_ARG(int, ObjectID)));
		IS_ASSERT(ObjectForm, "ObjectForm not dynamic cast.");
	}
	else //У мета-таблицы нет переопределенной формы объекта - создаем базовую
	{
		ObjectForm = new ISObjectFormBase(FormType, MetaTable, parent, ObjectID);
	}

	ISSystem::SetWaitGlobalCursor(false);
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

	int ObjectType = QMetaType::type((SearchOperatorWidget + "*").toLocal8Bit().constData());
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
	ISSystem::SetWaitGlobalCursor(true);
	ISSelectDialogForm SelectDialogForm(ISNamespace::SLM_Single, TableName, SelectObjectID);
	ISSystem::SetWaitGlobalCursor(false);
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
	ISSystem::SetWaitGlobalCursor(true);
	ISSelectDialogForm SelectDialogForm(ISNamespace::SLM_Multi, TableName, 0);
	ISSystem::SetWaitGlobalCursor(false);
	if (SelectDialogForm.Exec())
	{
		VectorInt = SelectDialogForm.GetSelectedObjects();
	}

	return VectorInt;
}
//-----------------------------------------------------------------------------
void ISGui::ShowSettingsForm(const QString &SettingGroupUID)
{
	ISSystem::SetWaitGlobalCursor(true);
	ISSettingsForm SettingsForm(SettingGroupUID);
	ISSystem::SetWaitGlobalCursor(false);
	SettingsForm.Exec();
}
//-----------------------------------------------------------------------------
bool ISGui::ShowUserPasswordForm(int UserID)
{
	ISSystem::SetWaitGlobalCursor(true);
	ISUserPasswordForm UserPasswordForm(UserID);
	ISSystem::SetWaitGlobalCursor(false);
	return UserPasswordForm.Exec();
}
//-----------------------------------------------------------------------------
void ISGui::ShowSystemInfoRecord(PMetaClassTable *MetaTable, int ObjectID)
{
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_SYSTEM_INFO_OBJECT, MetaTable->GetName(), MetaTable->GetLocalListName(), ObjectID);
	ISSystem::SetWaitGlobalCursor(true);
	ISRecordInfoForm RecordInfoForm(MetaTable, ObjectID);
	ISSystem::SetWaitGlobalCursor(false);
	RecordInfoForm.Exec();
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QPixmap &Pixmap)
{
	ISSystem::SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewerForm = new ISImageViewerForm(Pixmap);
	ImageViewerForm->showMaximized();
	ISSystem::SetWaitGlobalCursor(false);
	return ImageViewerForm;
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QString &FilePath)
{
	ISSystem::SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewerForm = new ISImageViewerForm(FilePath);
	ImageViewerForm->showMaximized();
	ISSystem::SetWaitGlobalCursor(false);
	return ImageViewerForm;
}
//-----------------------------------------------------------------------------
ISImageViewerForm* ISGui::ShowImageForm(const QByteArray &ByteArray)
{
	return ShowImageForm(ISSystem::ByteArrayToPixmap(ByteArray));
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISGui::CreateFieldEditBase(QWidget *ParentWidget, PMetaClassField *MetaField, ISNamespace::FieldType DataType, const QString &ControlWidget)
{
	ISFieldEditBase *FieldEditBase = nullptr;
	QString Temp = ControlWidget;

	if (Temp.length())
	{
		int ObjectType = QMetaType::type((Temp + "*").toLocal8Bit().constData());
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
