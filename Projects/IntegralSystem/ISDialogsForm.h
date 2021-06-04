#pragma once
#ifndef _ISDIALOGSFORM_H_INCLUDED
#define _ISDIALOGSFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISStructsOld.h"
#include "ISFieldEdits.h"
#include "ISSelectListForm.h"
//-----------------------------------------------------------------------------
class ISAboutDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISAboutDialog();
    virtual ~ISAboutDialog();

private:
    void CreateCommonTab(); //Общие
    void CreateContactsTab(); //Контакты
    void CreateModuleTab(); //Текущий модуль
    void CreateDescriptionTab(); //Описание
    void CreateLicenseTab(); //Лицензионное соглашение
    void CreateOtherTab(); //Прочее

    void AddLabel(QWidget *parent, const QString &LabelText, const QString &Text);
    void Feedback();

private:
    QVBoxLayout *LayoutRight;
    QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISAuthDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISAuthDialog();
    virtual ~ISAuthDialog();

protected:
    void closeEvent(QCloseEvent *CloseEvent);
    void EnterClicked() override;

private:
    void ShowConnectionForm(); //Открытие формы настроек подключения
    void ShowAboutForm(); //Открыть форму "О программе"

    void Input(); //Вход в программу
    void SetConnecting(bool Connecting); //Изменить рисовку интерфейса

private:
    ISLineEdit *EditLogin;
    ISLineEdit *EditPassword;
    ISCheckEdit *CheckRememberUser;
    QLabel *LabelIndicator;
    ISServiceButton *ButtonMenu;
    ISWaitWidget *WaitWidget;
    ISPushButton *ButtonInput;
    ISPushButton *ButtonExit;
    bool ConnectingState;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISConnectDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISConnectDialog();
    virtual ~ISConnectDialog();

protected:
    void AfterShowEvent() override;
    void EnterClicked() override;

private:
    void Save(); //Программное сохранение настроек

private:
    ISLineEdit *EditServer;
    ISIntegerEdit *EditPort;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExportDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISExportDialog(ISTcpModel *TcpModel);
    virtual ~ISExportDialog();

    ISNamespace::ExportType GetSelectedType();
    QString GetSelectTypeName() const;
    ISVectorUInt GetSelectedFields() const;
    bool GetHeader() const;

private:
    void CreateTabSettings();
    void CreateTabFields(ISTcpModel *TcpModel);
    void Select();

    void CreateFieldItem(const ISModelField &ModelField);
    void FieldsPositionChanged();
    void ItemDoubleClicked(QListWidgetItem *ListWidgetItem);

    void EnterClicked();
    void TypeChanged(const QVariant &Value);

private:
    ISNamespace::ExportType SelectedType;
    ISVectorUInt SelectedFields;
    ISComboEdit *ComboBoxType;
    QTabWidget *TabWidget;
    ISButtonDialog *ButtonDialog;
    ISListWidget *ListFields;

    ISCheckEdit *CheckHeader;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISReconnectDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISReconnectDialog();
    ~ISReconnectDialog();

private:
    void Timeout();

private:
    QLabel *LabelAttempts;
    QTimer *Timer;

    unsigned int Attempts;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISSelectListDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISSelectListDialog(ISNamespace::SelectListMode SelectMode, const QString &TableName, const QString &TableLocalName, int SelectObjectID);
    virtual ~ISSelectListDialog();

    bool Exec() override;
    ISObjectPair GetSelectedObject() const;
    ISVectorUInt GetSelectedObjects() const;

protected:
    void AfterShowEvent() override;

private:
    void Selected();

private:
    ISSelectListForm *SelectListForm;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISSettingsDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISSettingsDialog(const QString &SettingGroupUID);
    virtual ~ISSettingsDialog();

protected:
    void CreateCentralPanel(); //Создание центральной панели
    void CreateSettings();
    void Save();
    void DefaultSettings(); //Установить настройки по умолчанию
    QListWidgetItem* CreateItemGroup(ISMetaSettingsGroup *MetaGroup);
    void ItemSelectionChanged();
    void DataChanged();

private:
    QHBoxLayout *Layout;
    ISListWidget *ListWidget;
    QVBoxLayout *LayoutCentral;
    QLabel *LabelCurrentGroup;
    QLabel *LabelCurrentGroupHint;
    QTabWidget *TabWidget;
    ISButtonDialog *ButtonDialog;

    std::map<QListWidgetItem*, ISMetaSettingsGroup*> Groups;
    std::map<QString, ISFieldEditBase*> Fields;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUserGroupRightDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISUserGroupRightDialog(int group_id, const QString &group_name);
    virtual ~ISUserGroupRightDialog();

protected:
    void AfterShowEvent() override;

private:
    void CreateSubSystems(const QVariantList &Systems);
    void CreateTables(const QVariantList &RightTableType, const QVariantList &Tables);
    void CreateSpecial(const QVariantList &SpecialRights);

private:
    void SubSystemClicked(const QVariant &value); //Обработка нажатия на право для подсистемы
    void TableClicked(QAction *Action); //Обработка нажатия на право для таблицы
    void SpecialClicked(const QVariant &value); //Обработка нажатия на специальное право

private:
    QTabWidget *TabWidget;

private:
    int GroupID;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUserPasswordDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISUserPasswordDialog(unsigned int user_id, const QString &user_fio, const QString &user_login);
    virtual ~ISUserPasswordDialog();

protected:
    void AfterShowEvent() override;
    void EnterClicked() override;

private:
    void Apply();

private:
    ISLineEdit *EditPasswordCurrent;
    ISLineEdit *EditPassword;
    ISLineEdit *EditPasswordCheck;
    QLabel *LabelText;
    ISButtonDialog *ButtonDialog;

    bool PasswordExist;
    unsigned int UserID;
    QString UserFIO;
    QString UserLogin;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISRecordInfoDialog : public ISInterfaceDialogForm
{
    Q_OBJECT

public:
    ISRecordInfoDialog(QWidget *parent, const QString &TableName, unsigned int ObjectID);
    virtual ~ISRecordInfoDialog();
};
//-----------------------------------------------------------------------------
#endif
