#include "ISProcessForm.h"
#include "ISLocalizationOld.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISConstantsOld.h"
//-----------------------------------------------------------------------------
ISProcessForm::ISProcessForm(const QString &LabelText, QWidget *parent)
    : ISInterfaceForm(parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
    setCursor(CURSOR_WAIT);
    setWindowTitle(LANG("PleaseWait"));
    setSizePolicy(QSizePolicy::Maximum, sizePolicy().verticalPolicy());
    setWindowModality(Qt::WindowModality::WindowModal);
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QHBoxLayout *Layout = new QHBoxLayout();
    GetMainLayout()->addLayout(Layout);

    QLabel *LabelImage = new QLabel(this);
    LabelImage->setPixmap(BUFFER_ICONS("Wait").pixmap(QSize(32, 32)));
    Layout->addWidget(LabelImage);

    Label = new QLabel(this);
    Label->setText(LabelText);
    Label->setSizePolicy(QSizePolicy::Maximum, Label->sizePolicy().verticalPolicy());
    Layout->addWidget(Label);

    Layout->addStretch();
}
//-----------------------------------------------------------------------------
ISProcessForm::~ISProcessForm()
{

}
//-----------------------------------------------------------------------------
void ISProcessForm::SetText(const QString &LabelText)
{
    //ѕоследовательность вызова функций ниже ни в коем случае не мен€ть, иначе
    //возникнут проблемы с изменением размера формы после изменени€ текста в QLabel
    Label->setText(LabelText);
    Label->adjustSize();
    ISGui::RepaintWidget(Label);
    adjustSize();
    ISGui::RepaintWidget(this);
    PROCESS_EVENTS();
}
//-----------------------------------------------------------------------------
void ISProcessForm::showEvent(QShowEvent *ShowEvent)
{
    adjustSize();
    ISGui::RepaintWidget(this);
    PROCESS_EVENTS();
    ISGui::MoveWidgetToDesktop(this, ISNamespace::MoveWidgetDesktop::Center);
    ISInterfaceForm::showEvent(ShowEvent);
}
//-----------------------------------------------------------------------------
