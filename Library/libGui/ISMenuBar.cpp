#include "StdAfx.h"
#include "ISMenuBar.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISDefines.h"
#include "ISStyleSheet.h"
#include "ISMetaUser.h"
#include "ISSettings.h"
#include "ISFastAccessEntity.h"
#include "ISMenuFastAccess.h"
#include "ISParagraphEntity.h"
//-----------------------------------------------------------------------------
ISMenuBar::ISMenuBar(QWidget *parent) : QWidget(parent)
{
	CurrentButton = nullptr;

	setAutoFillBackground(true);

	QPalette Palette(palette());
	Palette.setColor(QPalette::Background, COLOR_MAIN_MENU_BAR);
	setPalette(Palette);

	MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(LAYOUT_MARGINS_NULL);
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
	MainMenu->addAction(BUFFER_ICONS("PowerOff"), LOCALIZATION("Exit"), this, &ISMenuBar::Exit, QKeySequence(Qt::AltModifier + Qt::Key_F4));

	CheckVisibleMunuItems();
	InstallFontMenuItems();

	CreateParagraphs();
	
	MainLayout->addStretch();

	LayoutButtons = new QHBoxLayout();
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
	Icon.addPixmap(BUFFER_ICONS("MainPanel.FastAccess.Active").pixmap(SIZE_24_24), QIcon::Active);

	QToolButton *ButtonFastMenu = new QToolButton(this);
	ButtonFastMenu->setText(LOCALIZATION("FastAccess"));
	ButtonFastMenu->setFont(FONT_TAHOMA_10);
	ButtonFastMenu->setAutoRaise(true);
	ButtonFastMenu->setIcon(Icon);
	ButtonFastMenu->setIconSize(SIZE_25_25);
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
	QMenu *MenuService = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LOCALIZATION("Service"));

	//Избранное
	MenuService->addAction(BUFFER_ICONS("Favorites"), LOCALIZATION("Favorites"), this, &ISMenuBar::Favorites);

	//История
	MenuService->addAction(BUFFER_ICONS("History"), LOCALIZATION("History"), this, &ISMenuBar::History);
	
	MenuService->addSeparator();

	//Настройка устройств
	MenuService->addAction(BUFFER_ICONS("Device.Settings"), LOCALIZATION("Device.Settings"), this, &ISMenuBar::DeviceSettings);

	//Смена пароля
	MenuService->addAction(LOCALIZATION("ChangePassword") + "...", this, &ISMenuBar::ChangePassword);

	//Формирование лога
	MenuService->addAction(BUFFER_ICONS("Log"), LOCALIZATION("CreateLogToday"), this, &ISMenuBar::CreateLogToday);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuAddition()
{
	QMenu *MenuAddition = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LOCALIZATION("Additionally"));

	//Блокнот
	MenuAddition->addAction(BUFFER_ICONS("Note"), LOCALIZATION("Notebook"), this, &ISMenuBar::Notebook);

	//Калькулятор
	MenuAddition->addAction(BUFFER_ICONS("Calculator"), LOCALIZATION("Calculator"), this, &ISMenuBar::Calculator);

	//Адресная книга
	MenuAddition->addAction(BUFFER_ICONS("AddressBook"), LOCALIZATION("AddressBook"), this, &ISMenuBar::AddressBook);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuHelp()
{
	QMenu *MenuHelp = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LOCALIZATION("Help"));

	//Справочная система
	//Menu->addAction(BUFFER_ICONS("Reference"), LOCALIZATION("HelpSystem"), this, &ISMenuBar::HelpSystem);
	//Menu->addSeparator();

	//О программе
	MenuHelp->addAction(BUFFER_ICONS("About"), LOCALIZATION("AboutForm.AboutApplication"), this, &ISMenuBar::AboutApplication);

	//Обратная форма
	MenuHelp->addAction(BUFFER_ICONS("FeedbackForm"), LOCALIZATION("Feedback"), this, &ISMenuBar::FeedbackForm);

	//Qt
	MenuHelp->addAction(BUFFER_ICONS("Qt"), LOCALIZATION("AboutFrameworkDeveloped"), this, &ISMenuBar::AboutQt);
	MenuHelp->addSeparator();

	MenuHelp->addAction(BUFFER_ICONS("License"), LOCALIZATION("License"), this, &ISMenuBar::License);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateButtonsPanel()
{
	//Отладка
	if (SETTING_BOOL(CONST_UID_SETTING_DEBUG_ACCESSTODEBUGVIEW))
	{
		QToolButton *ButtonDebug = CreateButton(LOCALIZATION("Debug"), "MainPanel.Debug");
		connect(ButtonDebug, &QToolButton::clicked, this, &ISMenuBar::Debug);
	}

	//Статус пользователя
	QToolButton *ButtonUserStatus = CreateButton(LOCALIZATION("StatusUser"), "MainPanel.UserStatus");
	connect(ButtonUserStatus, &QToolButton::clicked, this, &ISMenuBar::UserStatus);

	//Настройки
	QToolButton *ButtonSettings = CreateButton(LOCALIZATION("Settings"), "MainPanel.Settings");
	connect(ButtonSettings, &QToolButton::clicked, this, &ISMenuBar::Settings);

	//Блокировка
	QToolButton *ButtonLook = CreateButton(LOCALIZATION("ExitForm.Lock"), "MainPanel.Lock");
	connect(ButtonLook, &QToolButton::clicked, this, &ISMenuBar::Lock);

	//Смена пользователя
	QToolButton *ButtonChangeUser = CreateButton(LOCALIZATION("ChangeUser"), "MainPanel.ChangeUser");
	connect(ButtonChangeUser, &QToolButton::clicked, this, &ISMenuBar::ChangeUser);
}
//-----------------------------------------------------------------------------
QToolButton* ISMenuBar::CreateButton(const QString &ToolTip, const QString &IconName)
{
	QIcon Icon = BUFFER_ICONS(IconName);
	Icon.addPixmap(BUFFER_ICONS(IconName + ".Active").pixmap(SIZE_24_24), QIcon::Active);

	QToolButton *ToolButton = new QToolButton(this);
	ToolButton->setToolTip(ToolTip);
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ToolButton->setIconSize(SIZE_24_24);
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
		ActionMainItem->setFont(FONT_TAHOMA_10);

		if (ActionMainItem->menu())
		{
			for (QAction *ActionChildItem : ActionMainItem->menu()->actions())
			{
				ActionChildItem->setFont(FONT_TAHOMA_10);
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
		ParagraphButtons.insert(MetaParagraph->GetUID(), ParagraphButton);
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
