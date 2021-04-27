#pragma once
#ifndef _ISSUBSYSTEM_H_INCLUDED
#define _ISSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
#include "ISScrollArea.h"
#include "ISMonitorUserWidget.h"
//-----------------------------------------------------------------------------
class ISAsteriskCallsSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISAsteriskCallsSubSystem(QWidget *parent = 0);
    virtual ~ISAsteriskCallsSubSystem();

private:
    void PlayRecordCall();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISFullTextSearchSubSystem : public ISInterfaceMetaForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISFullTextSearchSubSystem(QWidget *parent = 0);
    virtual ~ISFullTextSearchSubSystem();

    void LoadData() override;

private:
    void SetSearchInProgress(bool InProgress);
    void Search();
    void ClickedRecord();

private:
    QWidgetList WidgetList;

    ISSearchEdit *EditSearch;
    ISWaitWidget *WaitWidget;
    ISPushButton *ButtonSearch;
    QLabel *LabelResult;
    QFrame *Frame;
    ISScrollArea *ScrollArea;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISInternalDirectoriesSubSystem : public ISInterfaceMetaForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISInternalDirectoriesSubSystem(QWidget *parent = 0);
    virtual ~ISInternalDirectoriesSubSystem();

    void LoadData() override;

protected:
    void ItemSelectionChanged();

private:
    ISListBaseForm *ListBaseForm;

    QHBoxLayout *Layout;
    ISListWidget *ListWidget;
    QLabel *Label;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISProtocolSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISProtocolSubSystem(QWidget *parent = 0);
    virtual ~ISProtocolSubSystem();

protected:
    void DoubleClickedTable(const QModelIndex &ModelIndex);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISMonitorActivitySubSystem : public ISInterfaceMetaForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISMonitorActivitySubSystem(QWidget *parent = 0);
    virtual ~ISMonitorActivitySubSystem();

    void LoadData() override;

protected:
    void ShowUserCard(); //Показать карточку пользователя
    void ShowProtocol(); //Просмотр протокола

private:
    QLabel *LabelCount;
    ISScrollArea *ScrollArea;
    std::vector<ISMonitorUserWidget*> VectorUsers;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISServerInfoSubSystem : public ISInterfaceMetaForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISServerInfoSubSystem(QWidget *parent);
    virtual ~ISServerInfoSubSystem();

    void LoadData() override;

private:
    QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISStorageFilesSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISStorageFilesSubSystem(QWidget *parent = 0);
    virtual ~ISStorageFilesSubSystem();

protected:
    void Create() override;
    void CreateCopy() override;

private:
    void SaveFile(); //Сохранить файл
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUserGroupSubSystem : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISUserGroupSubSystem(QWidget *parent = 0);
    virtual ~ISUserGroupSubSystem();

private:
    void SelectedGroup();
};
//-----------------------------------------------------------------------------
#endif
