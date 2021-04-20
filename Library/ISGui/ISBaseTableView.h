#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISBaseTableView : public QTableView
{
    Q_OBJECT

signals:
    void WheelUp();
    void WheelDown();
    void CornerClicked();

public:
    ISBaseTableView(QWidget *parent = 0);
    virtual ~ISBaseTableView();

    void SetCornerText(const QString &text);
    void SetCornerToolTip(const QString &tool_tip);

    void SetVisibleVerticalHeader(bool visible);
    void SetVisibleHorizontalHeader(bool visible);

    void ShowRows(); //Отобразить все скрытые строки

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *PaintEvent);

private:
    ISPushButton *ButtonCorner;
    bool CtrlClicked;
};
//-----------------------------------------------------------------------------
