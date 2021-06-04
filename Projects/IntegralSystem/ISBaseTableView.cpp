#include "ISBaseTableView.h"
#include "ISConstantsOld.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISBaseTableView::ISBaseTableView(QWidget *parent)
    : QTableView(parent),
    CtrlClicked(false)
{
    setSortingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setCornerButtonEnabled(false);
    setStyleSheet(BUFFER_STYLE_SHEET("ISBaseTableView"));
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);

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
    ButtonCorner->setFont(QFont("Tahoma", 8));
    connect(ButtonCorner, &QPushButton::clicked, this, &ISBaseTableView::CornerClicked);
}
//-----------------------------------------------------------------------------
ISBaseTableView::~ISBaseTableView()
{

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
void ISBaseTableView::ShowRows()
{
    if (model())
    {
        for (int i = 0, c = model()->rowCount(); i < c; ++i)
        {
            showRow(i);
        }
    }
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
void ISBaseTableView::paintEvent(QPaintEvent *PaintEvent)
{
    QTableView::paintEvent(PaintEvent);
    ButtonCorner->move(1, 1);
    ButtonCorner->setFixedWidth(verticalHeader()->width() - 1);
    ButtonCorner->setFixedHeight(horizontalHeader()->height());
}
//-----------------------------------------------------------------------------
