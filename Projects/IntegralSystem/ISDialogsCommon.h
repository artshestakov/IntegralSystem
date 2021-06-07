#pragma once
#ifndef _ISDIALOGSCOMMON_H_INCLUDED
#define _ISDIALOGSCOMMON_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISFieldEditBase.h"
#include "ISStructsGUI.h"
//-----------------------------------------------------------------------------
class ISFileDialog : public QFileDialog
{
    Q_OBJECT

public:
    ISFileDialog(QWidget *parent = 0);
    virtual ~ISFileDialog();

    static QString GetOpenFileNameImage(QWidget *parent); //�������� ���� � ������������ �����������
    static QString GetSaveFileNameImage(QWidget *parent, const QString &FileName = QString()); //�������� ���� � ������������ �����������
    static QString GetOpenFileName(QWidget *parent, const QString &FilePath = QString(), const QString &FilterName = QString()); //�������� ���� � ������������ �����
    static QString GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName = QString()); //�������� ���� � ������������ �����
    static QString GetDirectoryPath(QWidget *parent); //�������� ���� � ����������
    static QStringList GetOpenFileNames(QWidget *parent, const QString &FilterName = QString()); //�������� ���� � ����������� ������
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISInputDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISInputDialog(ISNamespace::FieldType DataType, const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant());
    virtual ~ISInputDialog();

    static QString GetString(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //�������� ������
    static QString GetText(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //�������� ������� ������
    static QString GetText(const QString &Title, const QString &LabelText, const QVariant &Value, bool &Ok); //�������� ������� ������
    static int GetInteger(bool &Ok, const QString &Title = QString(), const QString &LabelText = QString(), int Minimum = 0, int Maximum = 0, const QVariant &Value = QVariant()); //�������� ����� �����
    static unsigned int GetUInteger(bool &Ok, const QString &Title = QString(), const QString &LabelText = QString(), unsigned int Maximum = 0, const QVariant &Value = QVariant()); //�������� ����� ����������� �����
    static QDateTime GetDateTime(const QString &Title = QString(), const QString &LabelText = QString()); //�������� ���� � �����
    static QDate GetDate(const QString &Title = QString(), const QString &LabelText = QString()); //�������� ����
    static QTime GetTime(const QString &Title = QString(), const QString &LabelText = QString()); //�������� �����
    static QString GetPassword(); //�������� ������
    static QVariant GetList(const QString &Title, const QString &LabelText, const ISStringToVariantMap &VariantMap); //�������� ������

    QVariant GetValue() const;
    ISFieldEditBase* GetEditWidget();
    void SetEditWidget(ISFieldEditBase *field_edit_base);

protected:
    void AfterShowEvent() override;

private:
    void Apply();
    void EnterClicked() override;
    void ControlEnterClicked() override;

private:
    QLabel *Label;
    ISFieldEditBase *FieldEditBase;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    ISMessageBox(ISMessageBox::Icon Icon, const QString &Title, const QString &Message, const QString &DetailedText, const std::vector<ISMessageBoxButton> &Buttons, QWidget *parent = 0);
    virtual ~ISMessageBox();

    static void ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
    static void ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
    static void ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
    static bool ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText = QString());
    static int ShowQuestion(QWidget *parent, const QString &Message, const std::vector<ISMessageBoxButton> &Buttons, const QString &DetailedText = QString());

public:
    int Exec();
    void AddButtons(const std::vector<ISMessageBoxButton> &Buttons); //�������� ������ �� ������
    void SetDefaultButton(int ID); //�������� ������ �� ���������

private:
    void ButtonClicked(); //������� ������� �� ������

private:
    int ClickedID;
};
//-----------------------------------------------------------------------------
#endif
