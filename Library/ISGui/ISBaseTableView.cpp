#include "ISBaseTableView.h"
#include "ISDefinesGui.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISBaseTableView::ISBaseTableView(QWidget *parent)
	: QTableView(parent),
	SelectionScroll(false),
	CtrlClicked(false)
{
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
	horizontalHeader()->setSortIndicatorShown(false);
	horizontalHeader()->setTextElideMode(Qt::ElideRight);
	horizontalHeader()->setMinimumHeight(25);
	horizontalHeader()->setHighlightSections(false);

	ButtonCorner = new ISPushButton(this);
	ButtonCorner->setCursor(CURSOR_POINTING_HAND);
	ButtonCorner->setFlat(true);
	ButtonCorner->setFont(ISDefines::Gui::FONT_TAHOMA_8);
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
	CtrlClicked = false;
}
//-----------------------------------------------------------------------------
void ISBaseTableView::wheelEvent(QWheelEvent *WheelEvent)
{
	if (SelectionScroll)
	{
		emit WheelEvent->delta() > 0 ? WheelUp() : WheelDown();
	}
	else
	{
		QTableView::wheelEvent(CtrlClicked ? &QWheelEvent(WheelEvent->pos(), WheelEvent->delta(), WheelEvent->buttons(), WheelEvent->modifiers(), Qt::Horizontal) : WheelEvent);
	}
}
//-----------------------------------------------------------------------------
void ISBaseTableView::paintEvent(QPaintEvent *PaintEvent)
{
	QTableView::paintEvent(PaintEvent);
	ButtonCorner->move(1, 1);
	ButtonCorner->setFixedWidth(verticalHeader()->width() - 1);
	ButtonCorner->setFixedHeight(horizontalHeader()->height());
}
//-----------------------------------------------------------------------------