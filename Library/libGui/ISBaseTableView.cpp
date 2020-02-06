#include "ISBaseTableView.h"
#include "ISDefinesGui.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISBaseTableView::ISBaseTableView(QWidget *parent) : QTableView(parent)
{
	SelectionScroll = false;
	CtrlClicked = false;

	setSortingEnabled(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setCornerButtonEnabled(false);
	setStyleSheet(STYLE_SHEET("ISBaseTableView"));
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	verticalHeader()->setMinimumWidth(40);
	verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
	horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
	horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
	horizontalHeader()->setTextElideMode(Qt::ElideRight);
	horizontalHeader()->setMinimumHeight(25);
	horizontalHeader()->setHighlightSections(false);

	ButtonCorner = new ISPushButton(this);
	ButtonCorner->setCursor(CURSOR_POINTING_HAND);
	ButtonCorner->setFlat(true);
	ButtonCorner->setFont(DEFINES_GUI.FONT_TAHOMA_8);
	connect(ButtonCorner, &QPushButton::clicked, this, &ISBaseTableView::CornerClicked);
}
//-----------------------------------------------------------------------------
ISBaseTableView::~ISBaseTableView()
{

}
//-----------------------------------------------------------------------------
void ISBaseTableView::SetSelectionScroll(bool selection_scroll)
{
	SelectionScroll = selection_scroll;
}
//-----------------------------------------------------------------------------
void ISBaseTableView::SetCornerText(const QString &text)
{
	ButtonCorner->setText(text);
}
//-----------------------------------------------------------------------------
void ISBaseTableView::SetCornerToolTip(const QString &tool_tip)
{
	ButtonCorner->setToolTip(tool_tip);
}
//-----------------------------------------------------------------------------
void ISBaseTableView::SetVisibleVerticalHeader(bool visible)
{
	verticalHeader()->setVisible(visible);
	ButtonCorner->setVisible(visible);
}
//-----------------------------------------------------------------------------
void ISBaseTableView::SetVisibleHorizontalHeader(bool visible)
{
	horizontalHeader()->setVisible(visible);
	ButtonCorner->setVisible(visible);
}
//-----------------------------------------------------------------------------
void ISBaseTableView::keyPressEvent(QKeyEvent *e)
{
	QTableView::keyPressEvent(e);
	if (e->key() == Qt::Key_Control)
	{
		CtrlClicked = true;
	}
}
//-----------------------------------------------------------------------------
void ISBaseTableView::keyReleaseEvent(QKeyEvent *e)
{
	QTableView::keyReleaseEvent(e);
	if (CtrlClicked)
	{
		CtrlClicked = false;
	}
}
//-----------------------------------------------------------------------------
void ISBaseTableView::wheelEvent(QWheelEvent *e)
{
	if (SelectionScroll)
	{
		if (e->delta() > 0)
		{
			emit WheelUp();
		}
		else
		{
			emit WheelDown();
		}
	}
	else
	{
		if (CtrlClicked)
		{
			QWheelEvent WheelEvent(e->pos(), e->delta(), e->buttons(), e->modifiers(), Qt::Horizontal);
			QTableView::wheelEvent(&WheelEvent);
		}
		else
		{
			QTableView::wheelEvent(e);
		}
	}
}
//-----------------------------------------------------------------------------
void ISBaseTableView::paintEvent(QPaintEvent *e)
{
	QTableView::paintEvent(e);

	ButtonCorner->move(1, 1);
	ButtonCorner->setFixedWidth(verticalHeader()->width() - 1);
	ButtonCorner->setFixedHeight(horizontalHeader()->height());
}
//-----------------------------------------------------------------------------
