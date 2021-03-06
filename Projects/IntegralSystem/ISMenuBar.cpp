#include "ISMenuBar.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISSettings.h"
#include "ISParagraphEntity.h"
#include "ISUserRoleEntity.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISMenuBar::ISMenuBar(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);

    QPalette Palette(palette());
    Palette.setColor(QPalette::Background, COLOR_MAIN_MENU_BAR);
    setPalette(Palette);

    QHBoxLayout *MainLayout = new QHBoxLayout();
    MainLayout->setContentsMargins(QMargins());
    setLayout(MainLayout);

    QIcon IconMainMenu;
    IconMainMenu.addPixmap(BUFFER_ICONS("MainPanel.Menu").pixmap(QSize(25, 25)), QIcon::Normal, QIcon::Off);
    IconMainMenu.addPixmap(BUFFER_ICONS("MainPanel.Menu.Hover").pixmap(QSize(25, 25)), QIcon::Active, QIcon::Off);

    QToolButton *ButtonMenu = new QToolButton(this);
    ButtonMenu->setText(LANG("MainMenu"));
    ButtonMenu->setFont(FONT_TAHOMA_10);
    ButtonMenu->setAutoRaise(true);
    ButtonMenu->setIcon(IconMainMenu);
    ButtonMenu->setIconSize(QSize(25, 25));
    ButtonMenu->setCursor(CURSOR_POINTING_HAND);
    ButtonMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ButtonMenu->setStyleSheet(BUFFER_STYLE_SHEET("QToolButtonMainMenu"));
    ButtonMenu->setPopupMode(QToolButton::InstantPopup);
    ButtonMenu->setMenu(new QMenu(ButtonMenu));
    MainLayout->addWidget(ButtonMenu);

    //���� "������"
    QMenu *MenuService = ButtonMenu->menu()->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Service"));

    //���������
    MenuService->addAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this, &ISMenuBar::Favorites);

    //�������
    MenuService->addAction(BUFFER_ICONS("History"), LANG("History"), this, &ISMenuBar::History);

    //��������� �� (���� ���� �����)
    if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_SETTINGS_DB))
    {
        MenuService->addAction(BUFFER_ICONS("DatabaseSettings"), LANG("DatabaseSettings"), this, &ISMenuBar::DatabaseSettings);
    }

    MenuService->addSeparator();

    //����� ������
    MenuService->addAction(LANG("PasswordManagement"), this, &ISMenuBar::ChangePassword);

    //� ���������
    ButtonMenu->menu()->addAction(BUFFER_ICONS("About"), LANG("AboutForm.AboutApplication"), this, &ISMenuBar::AboutApplication);

    //�����
    ButtonMenu->menu()->addSeparator();
    ButtonMenu->menu()->addAction(BUFFER_ICONS("PowerOff"), LANG("Exit"), this, &ISMenuBar::Exit, QKeySequence(Qt::AltModifier + Qt::Key_F4));

    //��������� ������ ��� ���� ��������� �������� ����
    for (QAction *ActionMainItem : ButtonMenu->menu()->actions())
    {
        ActionMainItem->setFont(FONT_TAHOMA_10);
        if (ActionMainItem->menu())
        {
            for (QAction *ActionChildItem : ActionMainItem->menu()->actions())
            {
                ActionChildItem->setFont(FONT_TAHOMA_10);
            }
        }
    }

    QHBoxLayout *LayoutParagraphs = new QHBoxLayout();
    MainLayout->addLayout(LayoutParagraphs);

    //�������� ���������
    bool clicked_begin = false;
    for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::Instance().GetParagraphs())
    {
        QToolButton *ButtonParagraph = CreateParagraphButton(MetaParagraph);
        connect(ButtonParagraph, &QToolButton::clicked, this, static_cast<void(ISMenuBar::*)()>(&ISMenuBar::ParagraphClicked));
        LayoutParagraphs->addWidget(ButtonParagraph->parentWidget());
        ParagraphButtons[MetaParagraph->UID] = ButtonParagraph;

        if (!clicked_begin) //���� ������ �������� ��� �� ������� - ��������
        {
            ButtonParagraph->clicked();
            clicked_begin = true;
        }
    }
    MainLayout->addStretch();

    LayoutButtons = new QHBoxLayout();
    LayoutButtons->setContentsMargins(QMargins(4, 4, 4, 4));
    MainLayout->addLayout(LayoutButtons);

    //�������� ������ � �������� ������
    connect(CreateButton(LANG("SettingsUser"), "MainPanel.Settings"), &QToolButton::clicked, this, &ISMenuBar::Settings); //���������
    connect(CreateButton(LANG("ChangeUser"), "MainPanel.ChangeUser"), &QToolButton::clicked, this, &ISMenuBar::ChangeUser); //����� ������������
}
//-----------------------------------------------------------------------------
ISMenuBar::~ISMenuBar()
{

}
//-----------------------------------------------------------------------------
QToolButton* ISMenuBar::CreateButton(const QString &ToolTip, const QString &IconName)
{
    QIcon Icon;
    Icon.addPixmap(BUFFER_ICONS(IconName).pixmap(QSize(25, 25)));
    Icon.addPixmap(BUFFER_ICONS(IconName + ".Hover").pixmap(QSize(25, 25)), QIcon::Active);

    QToolButton *ToolButton = new QToolButton(this);
    ToolButton->setToolTip(ToolTip);
    ToolButton->setIcon(Icon);
    ToolButton->setAutoRaise(true);
    ToolButton->setIconSize(QSize(25, 25));
    ToolButton->setCursor(CURSOR_POINTING_HAND);
    ToolButton->setStyleSheet(BUFFER_STYLE_SHEET("ISMenuBar.Button"));
    LayoutButtons->addWidget(ToolButton);
    return ToolButton;
}
//-----------------------------------------------------------------------------
void ISMenuBar::ParagraphClicked()
{
    for (const auto &MapItem : ParagraphButtons)
    {
        if (MapItem.second == sender())
        {
            MapItem.second->parentWidget()->setStyleSheet(BUFFER_STYLE_SHEET("QWidgetParagraph.Checked"));
            MapItem.second->setChecked(true);
            emit ParagraphClicked(MapItem.first);
        }
        else
        {
            MapItem.second->parentWidget()->setStyleSheet(QString());
            MapItem.second->setChecked(false);
        }
    }
}
//-----------------------------------------------------------------------------
QToolButton* ISMenuBar::CreateParagraphButton(ISMetaParagraph *MetaParagraph)
{
    QVBoxLayout *LayoutWidget = new QVBoxLayout();
    LayoutWidget->setContentsMargins(25, 5, 25, 5);

    QWidget *Widget = new QWidget(this);
    Widget->setLayout(LayoutWidget);

    QIcon IconDeSelected = BUFFER_ICONS(MetaParagraph->Icon),
        IconSelected = BUFFER_ICONS(MetaParagraph->Icon + ".Selected"),
        IconHover = BUFFER_ICONS(MetaParagraph->Icon + ".Hover");
    QPixmap PixmapDeSelected = IconDeSelected.pixmap(QSize(45, 45)),
        PixmapSelected = IconSelected.pixmap(QSize(45, 45)),
        PixmapHover = IconHover.pixmap(QSize(45, 45));
    QIcon Icon;
    Icon.addPixmap(PixmapDeSelected, QIcon::Normal, QIcon::Off);
    Icon.addPixmap(PixmapSelected, QIcon::Normal, QIcon::On);
    Icon.addPixmap(PixmapHover, QIcon::Active, QIcon::Off);
    Icon.addPixmap(PixmapHover, QIcon::Selected, QIcon::On);

    QToolButton *ToolButton = new QToolButton(this);
    ToolButton->setToolTip(MetaParagraph->ToolTip);
    ToolButton->setCheckable(true);
    ToolButton->setIcon(Icon);
    ToolButton->setAutoRaise(true);
    ToolButton->setIconSize(QSize(45, 45));
    ToolButton->setCursor(CURSOR_POINTING_HAND);
    ToolButton->setStyleSheet(BUFFER_STYLE_SHEET("ISParagraphButton"));
    LayoutWidget->addWidget(ToolButton, 0, Qt::AlignHCenter);
    return ToolButton;
}
//-----------------------------------------------------------------------------
