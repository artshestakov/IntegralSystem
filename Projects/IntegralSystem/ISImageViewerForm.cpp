#include "ISImageViewerForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISImageViewerForm::ISImageViewerForm(const QPixmap &Pixmap, QWidget *parent) : ISInterfaceForm(parent)
{
    ScaleFactor = 1;

    setWindowTitle(LANG("ViewImage"));
    setWindowIcon(BUFFER_ICONS("Image"));

    QToolBar *ToolBar = new QToolBar(this);
    ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    GetMainLayout()->addWidget(ToolBar);

    ActionCopy = ToolBar->addAction(BUFFER_ICONS("Copy"), LANG("Copy"), this, &ISImageViewerForm::Copy);
    ActionCopy->setShortcut(QKeySequence::Copy);
    ActionCopy->setEnabled(false);

    ToolBar->addSeparator();

    ActionZoomIn = ToolBar->addAction(BUFFER_ICONS("ZoomIn"), LANG("ZoomIn"), this, &ISImageViewerForm::ZoomIn);
    ActionZoomIn->setShortcut(QKeySequence::ZoomIn);
    ActionZoomIn->setEnabled(false);

    ActionZoomOut = ToolBar->addAction(BUFFER_ICONS("ZoomOut"), LANG("ZoomOut"), this, &ISImageViewerForm::ZoomOut);
    ActionZoomOut->setShortcut(QKeySequence::ZoomOut);
    ActionZoomOut->setEnabled(false);

    ActionNormalSize = ToolBar->addAction(BUFFER_ICONS("NormalSize"), LANG("NormalSize"), this, &ISImageViewerForm::NormalSize);
    ActionNormalSize->setShortcut(QKeySequence("Ctrl+S"));
    ActionNormalSize->setEnabled(false);

    ToolBar->addSeparator();

    ActionFitToWindow = ToolBar->addAction(BUFFER_ICONS("FitToWindow"), LANG("FitToWindow"), this, &ISImageViewerForm::FitToWindow);
    ActionFitToWindow->setEnabled(false);
    ActionFitToWindow->setCheckable(true);
    ActionFitToWindow->setShortcut(QKeySequence("Ctrl+F"));

    LabelImage = new QLabel(this);
    LabelImage->setBackgroundRole(QPalette::Base);
    LabelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    LabelImage->setScaledContents(true);

    ScrollArea = new ISScrollArea(this);
    ScrollArea->setBackgroundRole(QPalette::Dark);
    ScrollArea->setWidget(LabelImage);
    ScrollArea->setVisible(false);
    GetMainLayout()->addWidget(ScrollArea);

    QStatusBar *StatusBar = new QStatusBar(this);
    GetMainLayout()->addWidget(StatusBar);

    LabelSize = new QLabel(StatusBar);
    StatusBar->addWidget(LabelSize);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    if (!Pixmap.isNull())
    {
        SetPixmap(Pixmap);
        FitToWindow();
    }
}
//-----------------------------------------------------------------------------
ISImageViewerForm::ISImageViewerForm(const QString &FilePath, QWidget *parent) : ISImageViewerForm(QPixmap(FilePath), parent)
{

}
//-----------------------------------------------------------------------------
ISImageViewerForm::~ISImageViewerForm()
{

}
//-----------------------------------------------------------------------------
void ISImageViewerForm::keyPressEvent(QKeyEvent *e)
{
    ISInterfaceForm::keyPressEvent(e);
    if (e->key() == Qt::Key_Control)
    {
        ScrollArea->SetWheelTracking(false);
    }
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::keyReleaseEvent(QKeyEvent *e)
{
    ISInterfaceForm::keyPressEvent(e);
    if (!ScrollArea->IsWheelTracking())
    {
        ScrollArea->SetWheelTracking(true);
    }
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::wheelEvent(QWheelEvent *e)
{
    ISInterfaceForm::wheelEvent(e);
    if (!ScrollArea->IsWheelTracking())
    {
        if (e->delta() > 0)
        {
            ZoomIn();
        }
        else
        {
            ZoomOut();
        }
    }
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::EscapeClicked()
{
    close();
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::Copy()
{
    ISGui::SetWaitGlobalCursor(true);
    QGuiApplication::clipboard()->setImage(LabelImage->pixmap()->toImage());
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::ZoomIn()
{
    ISGui::SetWaitGlobalCursor(true);
    ScaleImage(1.25);
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::ZoomOut()
{
    ISGui::SetWaitGlobalCursor(true);
    ScaleImage(0.8);
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::NormalSize()
{
    ISGui::SetWaitGlobalCursor(true);
    LabelImage->adjustSize();
    ScaleFactor = 1.0;
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::FitToWindow()
{
    bool FitChecked = ActionFitToWindow->isChecked();
    ScrollArea->setWidgetResizable(FitChecked);
    if (!FitChecked)
    {
        NormalSize();
    }
    UpdateActions();
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::UpdateActions()
{
    ActionCopy->setEnabled(!LabelImage->pixmap()->isNull());
    ActionZoomIn->setEnabled(!ActionFitToWindow->isChecked());
    ActionZoomOut->setEnabled(!ActionFitToWindow->isChecked());
    ActionNormalSize->setEnabled(!ActionFitToWindow->isChecked());
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::SetPixmap(const QPixmap &NewPixmap)
{
    LabelImage->setPixmap(NewPixmap);
    LabelSize->setText(ISAlgorithm::CStringF(LANG("ImageSize"), NewPixmap.width(), NewPixmap.height()));
    ScaleFactor = 1.0;

    ScrollArea->setVisible(true);
    ActionFitToWindow->setEnabled(true);
    UpdateActions();

    if (!ActionFitToWindow->isChecked())
    {
        LabelImage->adjustSize();
    }
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::ScaleImage(double Factor)
{
    ScaleFactor *= Factor;
    LabelImage->resize(ScaleFactor * LabelImage->pixmap()->size());

    AdjustScrollBar(ScrollArea->horizontalScrollBar(), Factor);
    AdjustScrollBar(ScrollArea->verticalScrollBar(), Factor);

    ActionZoomIn->setEnabled(ScaleFactor < 3.0);
    ActionZoomOut->setEnabled(ScaleFactor > 0.333);
}
//-----------------------------------------------------------------------------
void ISImageViewerForm::AdjustScrollBar(QScrollBar *ScrollBar, double Factor)
{
    ScrollBar->setValue(int(Factor * ScrollBar->value() + ((Factor - 1) * ScrollBar->pageStep() / 2)));
}
//-----------------------------------------------------------------------------
