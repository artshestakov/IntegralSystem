#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISBaseTableView : public QTableView
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
	void wheelEvent(QWheelEvent *e);
	void paintEvent(QPaintEvent *e);
	
private:
	bool SelectionScroll;
	ISPushButton *ButtonCorner;
	bool CtrlClicked;
};
//-----------------------------------------------------------------------------
