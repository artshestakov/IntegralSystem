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

	void SetSelectionScroll(bool selection_scroll);
	void SetCornerText(const QString &text);
	void SetCornerToolTip(const QString &tool_tip);
	
	void SetVisibleVerticalHeader(bool visible);
	void SetVisibleHorizontalHeader(bool visible);

protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void wheelEvent(QWheelEvent *WheelEvent);
	void paintEvent(QPaintEvent *PaintEvent);
	
private:
	bool SelectionScroll;
	ISPushButton *ButtonCorner;
	bool CtrlClicked;
};
//-----------------------------------------------------------------------------