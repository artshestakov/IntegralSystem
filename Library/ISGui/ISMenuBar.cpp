#include "ISMenuBar.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISSettings.h"
#include "ISParagraphEntity.h"
//-----------------------------------------------------------------------------
ISMenuBar::ISMenuBar(QWidget *parent) : QWidget(parent)
{
	setAutoFillBackground(true);

	QPalette Palette(palette());
	Palette.setColor(QPalette::Background, ISDefines::Gui::COLOR_MAIN_MENU_BAR);
	setPalette(Palette);

	QHBoxLayout *MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(MainLayout);

	QIcon IconMainMenu;
	IconMainMenu.addPixmap(BUFFER_ICONS("MainPanel.Menu").pixmap(ISDefines::Gui::SIZE_25_25), QIcon::Normal, QIcon::Off);
	IconMainMenu.addPixmap(BUFFER_ICONS("MainPanel.Menu.Hover").pixmap(ISDefines::Gui::SIZE_25_25), QIcon::Active, QIcon::Off);

	QToolButton *ButtonMenu = new QToolButton(this);
	ButtonMenu->setText(LANG("MainMenu"));
	ButtonMenu->setFont(ISDefines::Gui::FONT_TAHOMA_10);
	ButtonMenu->setAutoRaise(true);
	ButtonMenu->setIcon(IconMainMenu);
	ButtonMenu->setIconSize(ISDefines::Gui::SIZE_25_25);
	ButtonMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ButtonMenu->setStyleSheet(STYLE_SHEET("QToolButtonMainMenu"));
	ButtonMenu->setPopupMode(QToolButton::InstantPopup);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	MainLayout->addWidget(ButtonMenu);

	//Меню "Сервис"
	QMenu *MenuService = ButtonMenu->menu()->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Service"));

	//Избранное
	MenuService->addAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this, &ISMenuBar::Favorites);

	//История
	MenuService->addAction(BUFFER_ICONS("History"), LANG("History"), this, &ISMenuBar::History);

	MenuService->addSeparator();

	//Смена пароля
	MenuService->addAction(LANG("ChangePassword"), this, &ISMenuBar::ChangePassword);

	//Меню "Помощь"
	QMenu *MenuHelp = ButtonMenu->menu()->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Help"));

	//О программе
	MenuHelp->addAction(BUFFER_ICONS("About"), LANG("AboutForm.AboutApplication"), this, &ISMenuBar::AboutApplication);

	//Qt
	MenuHelp->addAction(BUFFER_ICONS("Qt"), LANG("AboutFrameworkDeveloped"), this, &ISMenuBar::AboutQt);
	MenuHelp->addSeparator();

	//Выход
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(BUFFER_ICONS("PowerOff"), LANG("Exit"), this, &ISMenuBar::Exit, QKeySequence(Qt::AltModifier + Qt::Key_F4));

	//Установка шрифта для всех элементов главного меню
	for (QAction *ActionMainItem : ButtonMenu->menu()->actions())
	{
		ActionMainItem->setFont(ISDefines::Gui::FONT_TAHOMA_10);
		if (ActionMainItem->menu())
		{
			for (QAction *ActionChildItem : ActionMainItem->menu()->actions())
			{
				ActionChildItem->setFont(ISDefines::Gui::FONT_TAHOMA_10);
			}
		}
	}

	QHBoxLayout *LayoutParagraphs = new QHBoxLayout();
	MainLayout->addLayout(LayoutParagraphs);

	//Создание парграфов
	for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::Instance().GetParagraphs())
	{
		QToolButton *ButtonParagraph = CreateParagraphButton(MetaParagraph);
		connect(ButtonParagraph, &QToolButton::clicked, this, static_cast<void(ISMenuBar::*)()>(&ISMenuBar::ParagraphClicked));
		LayoutParagraphs->addWidget(ButtonParagraph->parentWidget());
		ParagraphButtons[MetaParagraph->UID] = ButtonParagraph;
	}
	MainLayout->addStretch();

	LayoutButtons = new QHBoxLayout();
	LayoutButtons->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_4_PX);
	MainLayout->addLayout(LayoutButtons);

	//Создание панели с кнопками справа
	connect(CreateButton(LANG("Settings"), "MainPanel.Settings"), &QToolButton::clicked, this, &ISMenuBar::Settings); //Настройки
	connect(CreateButton(LANG("ChangeUser"), "MainPanel.ChangeUser"), &QToolButton::clicked, this, &ISMenuBar::ChangeUser); //Смена пользователя
}
//-----------------------------------------------------------------------------
ISMenuBar::~ISMenuBar()
{
	
}
//-----------------------------------------------------------------------------
void ISMenuBar::ParagraphClick(const ISUuid &ParagraphUID)
{
	ParagraphButtons[ParagraphUID]->clicked();
}
//-----------------------------------------------------------------------------
QToolButton* ISMenuBar::CreateButton(const QString &ToolTip, const QString &IconName)
{
	QIcon Icon;
	Icon.addPixmap(BUFFER_ICONS(IconName).pixmap(ISDefines::Gui::SIZE_25_25));
	Icon.addPixmap(BUFFER_ICONS(IconName + ".Hover").pixmap(ISDefines::Gui::SIZE_25_25), QIcon::Active);

	QToolButton *ToolButton = new QToolButton(this);
	ToolButton->setToolTip(ToolTip);
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setIconSize(ISDefines::Gui::SIZE_25_25);
	ToolButton->setCursor(CURSOR_POINTING_HAND);
	ToolButton->setStyleSheet(STYLE_SHEET("ISMenuBar.Button"));
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
			MapItem.second->parentWidget()->setStyleSheet("background-color: white;");
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
	QPixmap PixmapDeSelected = IconDeSelected.pixmap(ISDefines::Gui::SIZE_45_45),
		PixmapSelected = IconSelected.pixmap(ISDefines::Gui::SIZE_45_45),
		PixmapHover = IconHover.pixmap(ISDefines::Gui::SIZE_45_45);
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
	ToolButton->setIconSize(ISDefines::Gui::SIZE_45_45);
	ToolButton->setCursor(CURSOR_POINTING_HAND);
	ToolButton->setStyleSheet(STYLE_SHEET("ISParagraphButton"));
	LayoutWidget->addWidget(ToolButton, 0, Qt::AlignHCenter);
	return ToolButton;
}
//-----------------------------------------------------------------------------
