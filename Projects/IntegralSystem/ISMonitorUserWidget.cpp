#include "ISMonitorUserWidget.h"
#include "ISConstantsOld.h"
#include "ISLocalizationOld.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISInterfaceDialogForm.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISMonitorUserWidget::ISMonitorUserWidget(const QVariantMap &VariantMap, QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setAutoFillBackground(true);
    setCursor(CURSOR_POINTING_HAND);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    IsOnline = VariantMap["IsOnline"].toBool();
    UserID = VariantMap["UserID"].toUInt();
    UserFIO = VariantMap["FIO"].toString();
    UserGroupName = VariantMap["GroupName"].toString();
    IPAddress = VariantMap["Address"].toString();
    Port = VariantMap["Port"].toInt();
    DateTimeConnected = VariantMap["DateTimeConnected"].toString();
    DateTimeLastQuery = VariantMap["DateTimeLastQuery"].toString();

    setProperty("UserID", UserID);
    setProperty("UserName", UserFIO);

    QPalette Palette(palette());
    Palette.setColor(QPalette::Background, QColor(210, 210, 210));
    setPalette(Palette);

    QVBoxLayout *Layout = new QVBoxLayout();
    Layout->setContentsMargins(QMargins(5, 5, 5, 5));
    setLayout(Layout);

    QLabel *LabelIcon = new QLabel(this);
    LabelIcon->setPixmap(BUFFER_ICONS("User").pixmap(QSize(32, 32)));
    Layout->addWidget(LabelIcon, 0, Qt::AlignHCenter);

    QLabel *LabelUserFIO = new QLabel(UserFIO.split(SYMBOL_SPACE, QString::SkipEmptyParts).join('\n'), this);
    LabelUserFIO->setText(UserID == CURRENT_USER_ID ? LabelUserFIO->text() + "\n(" + LANG("You") + ')' : LabelUserFIO->text() + "\n(" + (IsOnline ? LANG("Online") : LANG("Offline")) + ')');
    LabelUserFIO->setAlignment(Qt::AlignHCenter);
    LabelUserFIO->setWordWrap(true);
    Layout->addWidget(LabelUserFIO, 0, Qt::AlignHCenter);
    ISGui::SetFontWidgetBold(LabelUserFIO, UserID == CURRENT_USER_ID);

    QAction *ActionUser = new QAction(BUFFER_ICONS("User"), UserFIO, this);
    ActionUser->setFont(FONT_APPLICATION_BOLD);
    connect(ActionUser, &QAction::triggered, this, &ISMonitorUserWidget::ShowUserCard);
    addAction(ActionUser);

    QAction *ActionProtocol = new QAction(BUFFER_ICONS("Protocol"), LANG("ProtocolUser"), this);
    connect(ActionProtocol, &QAction::triggered, this, &ISMonitorUserWidget::ShowProtocol);
    addAction(ActionProtocol);

    QAction *ActionDetails = new QAction(LANG("ISMonitorUserWidget.Details"), this);
    connect(ActionDetails, &QAction::triggered, this, &ISMonitorUserWidget::ServiceClicked);
    addAction(ActionDetails);
}
//-----------------------------------------------------------------------------
ISMonitorUserWidget::~ISMonitorUserWidget()
{

}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);
    setFrameShape(QFrame::WinPanel);
}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    setFrameShape(QFrame::Box);
}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    QWidget::mouseDoubleClickEvent(e);
    emit ShowUserCard();
}
//-----------------------------------------------------------------------------
void ISMonitorUserWidget::ServiceClicked()
{
    ISInterfaceDialogForm InterfaceDialogForm;
    InterfaceDialogForm.setWindowTitle(LANG("ISMonitorUserWidget.SessionDetails"));
    dynamic_cast<QVBoxLayout*>(InterfaceDialogForm.layout())->setContentsMargins(QMargins(10, 10, 10, 10));

    QFormLayout FormLayout;
    dynamic_cast<QVBoxLayout*>(InterfaceDialogForm.layout())->addLayout(&FormLayout);

    FormLayout.addRow(LANG("ISMonitorUserWidget.Status"), new QLabel(IsOnline ? LANG("ISMonitorUserWidget.Status.Online") : LANG("ISMonitorUserWidget.Status.Offline"), &InterfaceDialogForm));
    FormLayout.addRow(LANG("ISMonitorUserWidget.ID"), new QLabel(QString::number(UserID), &InterfaceDialogForm));
    FormLayout.addRow(LANG("ISMonitorUserWidget.FIO"), new QLabel(UserFIO, &InterfaceDialogForm));
    FormLayout.addRow(LANG("ISMonitorUserWidget.Group"), new QLabel(UserGroupName, &InterfaceDialogForm));
    if (IsOnline)
    {
        FormLayout.addRow(LANG("ISMonitorUserWidget.IPAddress"), new QLabel(IPAddress, &InterfaceDialogForm));
        FormLayout.addRow(LANG("ISMonitorUserWidget.Port"), new QLabel(QString::number(Port), &InterfaceDialogForm));
        FormLayout.addRow(LANG("ISMonitorUserWidget.DateTimeConnected"), new QLabel(DateTimeConnected, &InterfaceDialogForm));
        FormLayout.addRow(LANG("ISMonitorUserWidget.DateTimeLastQuery"), new QLabel(DateTimeLastQuery, &InterfaceDialogForm));
    }

    InterfaceDialogForm.Exec();
}
//-----------------------------------------------------------------------------
