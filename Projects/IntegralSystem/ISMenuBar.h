#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISButtons.h"
#include "ISStructsGUI.h"
//-----------------------------------------------------------------------------
class ISMenuBar : public QWidget
{
    Q_OBJECT

signals:
    void ParagraphClicked(const QString &ParagraphUID);
    void ChangeUser();
    void Exit();
    void Favorites();
    void History();
    void DatabaseSettings();
    void ChangePassword();
    void Settings();
    void AboutApplication();

public:
    ISMenuBar(QWidget *parent = 0);
    virtual ~ISMenuBar();

private:
    QToolButton* CreateButton(const QString &ToolTip, const QString &IconName);
    void ParagraphClicked();
    QToolButton* CreateParagraphButton(ISMetaParagraph *MetaParagraph);

private:
    QHBoxLayout *LayoutButtons;
    std::map<QString, QToolButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
