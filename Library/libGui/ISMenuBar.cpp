#include "ISMenuBar.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISSettings.h"
#include "ISMenuFastAccess.h"
#include "ISParagraphEntity.h"
//-----------------------------------------------------------------------------
ISMenuBar::ISMenuBar(QWidget *parent) : QWidget(parent)
{
	CurrentButton = nullptr;

	setAutoFillBackground(true);

	QPalette Palette(palette());
	Palette.setColor(QPalette::Background, DEFINES_GUI.COLOR_MAIN_MENU_BAR);
	setPalette(Palette);

	MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_NULL);
	setLayout(MainLayout);

	ButtonMenu = new ISButtonMainMenu(this);
	connect(ButtonMenu, &QToolButton::clicked, this, &ISMenuBar::ShowMainMenu);
	MainLayout->addWidget(ButtonMenu);

	MainMenu = new ISMainMenu(ButtonMenu);
	connect(MainMenu, &ISMainMenu::aboutToHide, this, &ISMenuBar::HideMainMenu);

	ButtonNotify = new ISButtonNotify(this);
	MainLayout->addWidget(ButtonNotify);

	if (SETTING_BOOL(CONST_UID_SETTING_VIEW_MENUFASTACCESS))
	{
		CreateFastAccessMenu();
	}

	CreateMenuFile();
	CreateMenuService();
	CreateMenuAddition();
	CreateMenuHelp();

	//Выход
	MainMenu->addSeparator();
	MainMenu->addAction(BUFFER_ICONS("PowerOff"), LANG("Exit"), this, &ISMenuBar::Exit, QKeySequence(Qt::AltModifier + Qt::Key_F4));

	CheckVisibleMunuItems();
	InstallFontMenuItems();

	CreateParagraphs();
	
	MainLayout->addStretch();

	LayoutButtons = new QHBoxLayout();
	LayoutButtons->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_4_PX);
	MainLayout->addLayout(LayoutButtons);

	CreateButtonsPanel();
}
//-----------------------------------------------------------------------------
ISMenuBar::~ISMenuBar()
{
	
}
//-----------------------------------------------------------------------------
ISParagraphButton* ISMenuBar::GetParagraphButton(const ISUuid &ParagraphUID)
{
	return ParagraphButtons.value(ParagraphUID);
}
//-----------------------------------------------------------------------------
void ISMenuBar::ButtonParagraphClicked(const ISUuid &ClickedParagraphUID)
{
	ParagraphButtons.value(ClickedParagraphUID)->Clicked();
}
//-----------------------------------------------------------------------------
ISButtonNotify* ISMenuBar::GetbuttonNotify()
{
	return ButtonNotify;
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateFastAccessMenu()
{
	QIcon Icon = BUFFER_ICONS("MainPanel.FastAccess");
	Icon.addPixmap(BUFFER_ICONS("MainPanel.FastAccess.Active").pixmap(DEFINES_GUI.SIZE_24_24), QIcon::Active);

	QToolButton *ButtonFastMenu = new QToolButton(this);
	ButtonFastMenu->setText(LANG("FastAccess"));
	ButtonFastMenu->setFont(DEFINES_GUI.FONT_TAHOMA_10);
	ButtonFastMenu->setAutoRaise(true);
	ButtonFastMenu->setIcon(Icon);
	ButtonFastMenu->setIconSize(DEFINES_GUI.SIZE_25_25);
	ButtonFastMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonFastMenu->setPopupMode(QToolButton::InstantPopup);
	ButtonFastMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ButtonFastMenu->setStyleSheet(STYLE_SHEET("QToolButtonMainMenu"));
	MainLayout->addWidget(ButtonFastMenu);

	ISMenuFastAccess *MenuFastAccess = new ISMenuFastAccess(ButtonFastMenu);
	connect(MenuFastAccess, &ISMenuFastAccess::CreateRecords, this, &ISMenuBar::CreateRecords);
	connect(MenuFastAccess, &ISMenuFastAccess::ExternalTools, this, &ISMenuBar::ExternalTools);
	ButtonFastMenu->setMenu(MenuFastAccess);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuFile()
{
	
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuService()
{
	QMenu *MenuService = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Service"));

	//Избранное
	MenuService->addAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this, &ISMenuBar::Favorites);

	//История
	MenuService->addAction(BUFFER_ICONS("History"), LANG("History"), this, &ISMenuBar::History);
	
	MenuService->addSeparator();

	//Настройка устройств
	MenuService->addAction(BUFFER_ICONS("Device.Settings"), LANG("Device.Settings"), this, &ISMenuBar::DeviceSettings);

	//Смена пароля
	MenuService->addAction(LANG("ChangePassword") + "...", this, &ISMenuBar::ChangePassword);

	//Формирование лога
	MenuService->addAction(BUFFER_ICONS("Log"), LANG("CreateLogToday"), this, &ISMenuBar::CreateLogToday);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuAddition()
{
	QMenu *MenuAddition = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Additionally"));

	//Блокнот
	MenuAddition->addAction(BUFFER_ICONS("Note"), LANG("Notebook"), this, &ISMenuBar::Notebook);

	//Калькулятор
	MenuAddition->addAction(BUFFER_ICONS("Calculator"), LANG("Calculator"), this, &ISMenuBar::Calculator);

	//Адресная книга
	MenuAddition->addAction(BUFFER_ICONS("AddressBook"), LANG("AddressBook"), this, &ISMenuBar::AddressBook);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuHelp()
{
	QMenu *MenuHelp = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Help"));

	//Справочная система
	//Menu->addAction(BUFFER_ICONS("Reference"), LANG("HelpSystem"), this, &ISMenuBar::HelpSystem);
	//Menu->addSeparator();

	//О программе
	MenuHelp->addAction(BUFFER_ICONS("About"), LANG("AboutForm.AboutApplication"), this, &ISMenuBar::AboutApplication);

	//Qt
	MenuHelp->addAction(BUFFER_ICONS("Qt"), LANG("AboutFrameworkDeveloped"), this, &ISMenuBar::AboutQt);
	MenuHelp->addSeparator();
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateButtonsPanel()
{
	connect(CreateButton(LANG("StatusUser"), "MainPanel.UserStatus"), &QToolButton::clicked, this, &ISMenuBar::UserStatus); //Статус пользователя
	connect(CreateButton(LANG("Settings"), "MainPanel.Settings"), &QToolButton::clicked, this, &ISMenuBar::Settings); //Настройки
	connect(CreateButton(LANG("ExitForm.Lock"), "MainPanel.Lock"), &QToolButton::clicked, this, &ISMenuBar::Lock); //Блокировка
	connect(CreateButton(LANG("ChangeUser"), "MainPanel.ChangeUser"), &QToolButton::clicked, this, &ISMenuBar::ChangeUser); //Смена пользователя
}
//-----------------------------------------------------------------------------
QToolButton* ISMenuBar::CreateButton(const QString &ToolTip, const QString &IconName)
{
	QIcon Icon = BUFFER_ICONS(IconName);
	Icon.addPixmap(BUFFER_ICONS(IconName + ".Active").pixmap(DEFINES_GUI.SIZE_24_24), QIcon::Active);

	QToolButton *ToolButton = new QToolButton(this);
	ToolButton->setToolTip(ToolTip);
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ToolButton->setIconSize(DEFINES_GUI.SIZE_24_24);
	ToolButton->setCursor(CURSOR_POINTING_HAND);
	ToolButton->setStyleSheet(STYLE_SHEET("ISMenuBar.Button"));
	LayoutButtons->addWidget(ToolButton);

	return ToolButton;
}
//-----------------------------------------------------------------------------
void ISMenuBar::ShowMainMenu()
{
	ButtonMenu->setChecked(true);
	MainMenu->Show(ButtonMenu->mapToGlobal(QPoint()));
}
//-----------------------------------------------------------------------------
void ISMenuBar::HideMainMenu()
{
	ButtonMenu->setChecked(false);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CheckVisibleMunuItems()
{
	for (QAction *ActionMenu : MainMenu->actions())
	{
		if (ActionMenu->menu())
		{
			if (!ActionMenu->menu()->actions().count()) //Если в пунке меню нет элементов - скрывать этот пункт
			{
				ActionMenu->setVisible(false);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISMenuBar::InstallFontMenuItems()
{
	for (QAction *ActionMainItem : MainMenu->actions())
	{
		ActionMainItem->setFont(DEFINES_GUI.FONT_TAHOMA_10);

		if (ActionMainItem->menu())
		{
			for (QAction *ActionChildItem : ActionMainItem->menu()->actions())
			{
				ActionChildItem->setFont(DEFINES_GUI.FONT_TAHOMA_10);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateParagraphs()
{
	LayoutParagraphs = new QHBoxLayout();
	MainLayout->addLayout(LayoutParagraphs);

	for (int i = 0; i < ISParagraphEntity::GetInstance().GetParagraphs().count(); ++i)
	{
		ISMetaParagraph *MetaParagraph = ISParagraphEntity::GetInstance().GetParagraphs().at(i);

		ISParagraphButton *ParagraphButton = new ISParagraphButton(MetaParagraph, this);
		connect(ParagraphButton, &ISParagraphButton::Clicked, this, static_cast<void(ISMenuBar::*)()>(&ISMenuBar::ButtonParagraphClicked));
		LayoutParagraphs->addWidget(ParagraphButton);
		ParagraphButtons.insert(MetaParagraph->UID, ParagraphButton);
	}
}
//-----------------------------------------------------------------------------
void ISMenuBar::ButtonParagraphClicked()
{
	QList<ISParagraphButton*> Buttons = ParagraphButtons.values();

	for (ISParagraphButton *ParagraphButton : Buttons)
	{
		if (ParagraphButton == sender())
		{
			ParagraphButton->SetVisibleLine(true);
			emit ParagraphClicked(ParagraphButtons.key(ParagraphButton));
		}
		else
		{
			ParagraphButton->SetVisibleLine(false);
		}
	}
}
//-----------------------------------------------------------------------------
