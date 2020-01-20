#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISObjectFormBase.h"
#include "ISComboSearchBase.h"
#include "ISImageViewerForm.h"
//-----------------------------------------------------------------------------
class ISGui : public QObject
{
	Q_OBJECT

public:
	ISGui();
	virtual ~ISGui();

	//���������
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0);

	static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, PMetaClassField *MetaField);
	static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget = QString());
	static ISObjectFormBase* CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID = 0, QWidget *parent = 0);
	static ISComboSearchBase* CreateSearchOperator(QWidget *parent, ISNamespace::FieldType DataType, QObject *MetaForeign);

	static int SelectObject(const QString &TableName, int SelectObjectID = 0); //����� ����� ������ �� �������
	static QVectorInt SelectObjects(const QString &TableName); //����� ���������� ������� �� �������

	static void ShowSettingsForm(const QString &SettingGroupUID = QString()); //������� ����� ��������
	static bool ShowUserPasswordForm(int UserID); //������� ����� ��������� ������ ��� ������������
	static void ShowSystemInfoRecord(PMetaClassTable *MetaTable, int ObjectID); //�������� ��������� ���������� � ������
	
	static ISImageViewerForm* ShowImageForm(const QPixmap &Pixmap); //�������� �����������
	static ISImageViewerForm* ShowImageForm(const QString &FilePath); //�������� �����������
	static ISImageViewerForm* ShowImageForm(const QByteArray &ByteArray); //�������� �����������

protected:
	static ISFieldEditBase* CreateFieldEditBase(QWidget *ParentWidget, PMetaClassField *MetaField = nullptr, ISNamespace::FieldType DataType = ISNamespace::FT_Unknown, const QString &ControlWidget = QString());
};
//-----------------------------------------------------------------------------
