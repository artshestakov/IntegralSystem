#pragma once
#ifndef _ISGUI_H_INCLUDED
#define _ISGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISObjectFormBase.h"
#include "ISComboSearchWidgets.h"
#include "ISImageViewerForm.h"
#include "ISSplashScreen.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------

//��������� �������� ��������
#define DOUBLE_PREPAREN(d, n) QString::number(d, 'f', n)
#define DOUBLE_PREPARE(d) DOUBLE_PREPAREN(d, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//��������� ��������� �������� (���-�� ������ ����� ������� ������ ���� 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//-----------------------------------------------------------------------------
class ISGui
{
public:
    static bool Startup(QString &ErrorString);
    static bool Startup(ISSplashScreen *SplashScreen);
    static void Shutdown(ISSplashScreen *SplashScreen);

    static bool CheckAdminRole(); //�������� ������� ���� ��������������
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
    static bool OpenFolder(const QString &FolderPath); //������� ����������
    static QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat); //�������������� ���� � ����� � ������
    static QString ConvertDateToString(const QDate &Date, const QString &DateFormat); //�������������� ���� � ������
    static bool RecordsDelete(const QString &TableName, const ISVectorUInt &ObjectsID, QString &ErrorString); //������� ������
    static void FavoriteObject(const QString &TableName, unsigned int ObjectID, bool &IsExist); //��������/������� ������ �� ����������

    static void ExitApplication(); //����� �� ���������
    static void ExecLoop(unsigned long Milliseconds); //��������� ������ �������

    static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, PMetaField *MetaField);
    static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget = QString());
    static ISObjectFormBase* CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID = 0, QWidget *parent = 0);

    static ISObjectPair SelectObject(const QString &TableName, const QString &TableLocalName, int SelectObjectID = 0); //����� ����� ������ �� �������
    static ISVectorUInt SelectObjects(const QString &TableName, const QString &TableLocalName); //����� ���������� ������� �� �������

    static void ShowSettingsForm(const QString &SettingGroupUID = QString()); //������� ����� ��������
    static void ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin); //������� ����� ��������� ������ ��� ������������
    static void ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin, bool &Result); //������� ����� ��������� ������ ��� ������������
    static void ShowObjectForm(QWidget *ObjectForm); //�������� ����� �������
    static void ShowRecordInfoForm(QWidget *parent, const QString &TableName, unsigned int ObjectID); //�������� ����� ���������� � ������

    static ISImageViewerForm* ShowImageForm(const QPixmap &Pixmap); //�������� �����������
    static ISImageViewerForm* ShowImageForm(const QString &FilePath); //�������� �����������
    static ISImageViewerForm* ShowImageForm(const QByteArray &ByteArray); //�������� �����������

    static void ShowNoteObject(QWidget *parent, const QString &TableName, int ObjectID);
    static void ShowFavoritesForm(const QString &TableName = QString(), const QString &TableLocalName = QString());
    static void ShowHistoryForm();

    //static void ShowTaskViewForm(int TaskID); //�������� ����� ��������� ������ �� ��������������
    //static void ShowTaskViewForm(QWidget *TaskViewForm); //�������� ����� ��������� ����� �� �������

    static bool CreateDir(const QString &DirPath);
    static bool CreateDir(const QString &DirPath, QString &ErrorString);

    static QVariantMap JsonStringToVariantMap(const QString &JsonString, QJsonParseError &JsonParseError); //����������� JSON-������ � ������ ����-��������
    static QByteArray VariantMapToJsonString(const QVariantMap &VariantMap, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //����������� ������ ����-�������� � JSON-������
    static QByteArray VariantListToJsonString(const QVariantList &VariantList, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //����������� ������ � JSON-������

    static QDomElement GetDomElement(QFile &File); //�������� ������� ����� ����� �� �����
    static QDomElement GetDomElement(const QString &Content); //�������� ������� ����� ����� �� ��������

    //�������� ��������� �� ����� �� ��� �����
    template <typename Class> static Class CreatePointer(const QString &ClassName,
        QGenericArgument val0 = QGenericArgument(nullptr),
        QGenericArgument val1 = QGenericArgument(),
        QGenericArgument val2 = QGenericArgument(),
        QGenericArgument val3 = QGenericArgument(),
        QGenericArgument val4 = QGenericArgument(),
        QGenericArgument val5 = QGenericArgument(),
        QGenericArgument val6 = QGenericArgument(),
        QGenericArgument val7 = QGenericArgument(),
        QGenericArgument val8 = QGenericArgument(),
        QGenericArgument val9 = QGenericArgument())
    {
        Class Pointer = nullptr;
        int ObjectType = QMetaType::type((ClassName + '*').toLocal8Bit().constData());
        const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
        if (ObjectType && MetaObject)
        {
            Pointer = dynamic_cast<Class>(MetaObject->newInstance(val0, val1, val2, val3, val4, val5, val6, val7, val8, val9));
        }

        if (!Pointer)
        {
            IS_ASSERT(Pointer, QString("Not created pointer with class name \"" + ClassName + "\"").toStdString());
        }
        return Pointer;
    }

private:
    static ISFieldEditBase* CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField = nullptr, ISNamespace::FieldType DataType = ISNamespace::FieldType::Unknown, const QString &ControlWidget = QString());

private:
    static void RegisterMetaType(); //����������� ����-�����
};
//-----------------------------------------------------------------------------
#endif
