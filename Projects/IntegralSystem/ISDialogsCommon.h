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

    static QString GetOpenFileNameImage(QWidget *parent); //Получить путь к открываемому изображению
    static QString GetSaveFileNameImage(QWidget *parent, const QString &FileName = QString()); //Получить путь к сохраняемому изображению
    static QString GetOpenFileName(QWidget *parent, const QString &FilePath = QString(), const QString &FilterName = QString()); //Получить путь к открываемому файлу
    static QString GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName = QString()); //Получить путь к сохраняемому файлу
    static QString GetDirectoryPath(QWidget *parent); //Получить путь к директории
    static QStringList GetOpenFileNames(QWidget *parent, const QString &FilterName = QString()); //Получить путь к открываемым файлам
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

    static QString GetString(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //Получить строку
    static QString GetText(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //Получить большую строку
    static QString GetText(const QString &Title, const QString &LabelText, const QVariant &Value, bool &Ok); //Получить большую строку
    static int GetInteger(bool &Ok, const QString &Title = QString(), const QString &LabelText = QString(), int Minimum = 0, int Maximum = 0, const QVariant &Value = QVariant()); //Получить целое число
    static unsigned int GetUInteger(bool &Ok, const QString &Title = QString(), const QString &LabelText = QString(), unsigned int Maximum = 0, const QVariant &Value = QVariant()); //Получить целое беззнаковое число
    static QDateTime GetDateTime(const QString &Title = QString(), const QString &LabelText = QString()); //Получить дату и время
    static QDate GetDate(const QString &Title = QString(), const QString &LabelText = QString()); //Получить дату
    static QTime GetTime(const QString &Title = QString(), const QString &LabelText = QString()); //Получить время
    static QString GetPassword(); //Получить пароль
    static QVariant GetList(const QString &Title, const QString &LabelText, const ISStringToVariantMap &VariantMap); //Получить список

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
    void AddButtons(const std::vector<ISMessageBoxButton> &Buttons); //Добавить кнопку на диалог
    void SetDefaultButton(int ID); //Изменить кнопку по умолчанию

private:
    void ButtonClicked(); //Событие нажатия на кнопку

private:
    int ClickedID;
};
//-----------------------------------------------------------------------------
#endif
