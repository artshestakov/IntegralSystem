#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
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
