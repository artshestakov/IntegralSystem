#pragma once
#ifndef _ISPARAGRAPHS_H_INCLUDED
#define _ISPARAGRAPHS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISCalendarEventItem.h"
#include "ISCalendarEventsWidget.h"
#include "ISCalendarPanel.h"
#include "ISTabWidgetMain.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISParagraphBaseForm : public QWidget
{
    Q_OBJECT

public:
    ISParagraphBaseForm(QWidget *parent = 0);
    virtual ~ISParagraphBaseForm();

    virtual void Invoke() = 0;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDesktopParagraph : public ISParagraphBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISDesktopParagraph(QWidget *parent = 0);
    virtual ~ISDesktopParagraph();

    void Invoke() override;

private:
    QVBoxLayout *MainLayout;
    QWidget *DesktopWidget;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISWorkspaceParagraph : public ISParagraphBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISWorkspaceParagraph(QWidget *parent = 0);
    virtual ~ISWorkspaceParagraph();

    void Invoke() override;

    public slots:
    void AddObjectForm(QWidget *ObjectForm); //Добавить форму объекта на панель навигации

private:
    void ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem); //Обработчик события клика по подсистеме

private:
    QVBoxLayout *Layout;
    ISTabWidgetMain *TabWidget;
    ISInterfaceMetaForm *CentralForm; //Центральная форма (рабочая)
    QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISCalendarParagraph : public ISParagraphBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISCalendarParagraph(QWidget *parent = 0);
    virtual ~ISCalendarParagraph();

    void Invoke() override;

protected:
    void ReloadEvents(); //Загрузка событий на текущий месяц
    void ReloadEvents(int Year, int Month); //Загрузка событий на месяц
    void SelectedDateChanged(); //Событие смены выбранной даты

    void Create(); //Создание нового события календаря
    void DateTo(); //Переход на дату календаря
    void ToCurrentDate(); //Переход на текущую дату
    void ShowSettingsForm();
    void Edit(); //Изменение события календаря
    void Delete(); //Удаление события календаря
    void CloseEvent(); //Завершение события

    void ItemDoubleClicked(QListWidgetItem *ListWidgetItem); //Двойное нажатие на событие
    void ItemSelectionChanged();

    void EditEvent(int CalendarID);
    void DeleteEvent(int CalendarID);

    ISCalendarEventItem* AddEventItem(int CalendarID, const QString &Name, const QString &Text, const QTime &Time, bool Closed); //Добавить элемент события в список

private:
    bool FirstUpdate;
    std::map<unsigned int, QVariantList> EventsMap;

private:
    QHBoxLayout *MainLayout;

    ISCalendarPanel *CalendarPanel;
    QLabel *LabelDayNumber;
    QLabel *LabelDayName;
    QLabel *LabelMonthYear;

    QAction *ActionEdit;
    QAction *ActionDelete;
    QAction *ActionCloseEvent;

    ISCalendarEventsWidget *ListWidget;
};
//-----------------------------------------------------------------------------
#endif
