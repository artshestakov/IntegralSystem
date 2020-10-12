#pragma once
#ifndef _ISGUI_H_INCLUDED
#define _ISGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISObjectFormBase.h"
#include "ISComboSearchWidgets.h"
#include "ISImageViewerForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISGui
{
public:
	static bool Startup(QString &ErrorString);

	static bool CheckAdminRole(); //�������� ������� ���� ��������������
	static bool CheckPressCapsLook(); //��������� ���������� ������ CapsLook
	static QString GetCurrentLayoutName(); //�������� ������������ ������� ��������� ����������
	static void RepaintWidget(QWidget *Widget, bool AdjustSize = true); //������������ ��������� � ��������� ������
	static void MoveWidgetToDesktop(QWidget *Widget, ISNamespace::MoveWidgetDesktop MovePosition); //����������� ������� �� �������� ����� (�������� ������ ����� ������ ������ show() � �������)
	static void SetWaitGlobalCursor(bool Wait); //�������� ���������� ������ ��� ��������
	static void SetFontWidgetUnderline(QWidget *Widget, bool Underline);
	static void SetFontWidgetItalic(QWidget *Widget, bool Italic);
	static void SetFontWidgetBold(QWidget *Widget, bool Bold);
	static void SetFontWidgetStrikeOut(QWidget *Widget, bool StrikeOut);
	static void SetFontListWidgetItemStrikeOut(QListWidgetItem *ListWidgetItem, bool StrikeOut);
	static QFont StringToFont(const QString &FontText); //����������� ������ � �����, ���� ���������� ������ ������, ������������ ���������� ����� ����������
	static QByteArray IconToByteArray(const QIcon &Icon); //����������� ������ � ������ ����
	static QIcon ByteArrayToIcon(const QByteArray &ByteArray); //����������� ������� ���� � ������
	static QByteArray PixmapToByteArray(const QPixmap &Pixmap); //����������� ����������� Pixmap � ������ ������
	static QPixmap ByteArrayToPixmap(const QByteArray &ByteArray); //����������� ������� ������ � ����������� Pixmap
	static QIcon GetIconFile(const QString &FilePath); //�������� ������ �����
	static bool OpenUrl(const QString &Url); //������� ������ � �������� �� ���������
	static bool OpenFile(const QString &FilePath); //������� ���� ����������� �� ���������
	static QString PrepareLongToolTip(QString ToolTipText, int MaxWidth = 250); //������ ������ ������������ ��������� (��� �������� ���� �� ����� ������)
	static int GetStringWidth(const QString &String, const QFont &Font); //�������� ������ ������ � �������� ��� ������
	static QColor StringToColor(const QString &String); //����������� ������ � ����
	static bool IsStringUrl(const QString &Url); //��������� �������� �� ������ �������
	static bool OpenFolder(const QString &FolderPath); //������� ����������
	static QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat); //�������������� ���� � ����� � ������
	static QString ConvertDateToString(const QDate &Date, const QString &DateFormat); //�������������� ���� � ������

	static void ExitApplication(); //����� �� ���������

	//���������
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0);

	static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, PMetaField *MetaField);
	static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget = QString());
	static ISObjectFormBase* CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID = 0, QWidget *parent = 0);
	static ISComboSearchBase* CreateSearchOperator(QWidget *parent, ISNamespace::FieldType DataType, PMetaForeign *MetaForeign);

	static int SelectObject(const QString &TableName, int SelectObjectID = 0); //����� ����� ������ �� �������
	static ISVectorInt SelectObjects(const QString &TableName); //����� ���������� ������� �� �������

	static void ShowSettingsForm(const QString &SettingGroupUID = QString()); //������� ����� ��������
	static bool ShowUserPasswordForm(int UserID); //������� ����� ��������� ������ ��� ������������
	static void ShowUserPasswordDelete(int UserID, const QString &UserLogin); //�������� ������ ������������
	static void ShowSystemInfoRecord(PMetaTable *MetaTable, int ObjectID); //�������� ��������� ���������� � ������
	static void ShowObjectForm(QWidget *ObjectForm); //�������� ����� �������
	
	static ISImageViewerForm* ShowImageForm(const QPixmap &Pixmap); //�������� �����������
	static ISImageViewerForm* ShowImageForm(const QString &FilePath); //�������� �����������
	static ISImageViewerForm* ShowImageForm(const QByteArray &ByteArray); //�������� �����������

	static void ShowNoteObject(QWidget *parent, const QString &TableName, int ObjectID);
	static void ShowFavoritesForm(const QString &TableName = QString());
	static void ShowHistoryForm();

	static void ShowTaskViewForm(int TaskID); //�������� ����� ��������� ������ �� ��������������
	static void ShowTaskViewForm(QWidget *TaskViewForm); //�������� ����� ��������� ����� �� �������

private:
	static ISFieldEditBase* CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField = nullptr, ISNamespace::FieldType DataType = ISNamespace::FT_Unknown, const QString &ControlWidget = QString());

private:
	static void RegisterMetaType(); //����������� ����-�����
};
//-----------------------------------------------------------------------------
#endif
