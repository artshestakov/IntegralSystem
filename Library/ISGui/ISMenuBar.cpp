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

	MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(MainLayout);

	ButtonMenu = new ISButtonMainMenu(this);
	connect(ButtonMenu, &QToolButton::clicked, this, &ISMenuBar::ShowMainMenu);
	MainLayout->addWidget(ButtonMenu);

	MainMenu = new ISMainMenu(ButtonMenu);
	connect(MainMenu, &ISMainMenu::aboutToHide, this, &ISMenuBar::HideMainMenu);

	ButtonNotify = new ISButtonNotify(this);
	MainLayout->addWidget(ButtonNotify);

	CreateMenuFile();
	CreateMenuService();
	CreateMenuHelp();

	//Выход
	MainMenu->addSeparator();
	MainMenu->addAction(BUFFER_ICONS("PowerOff"), LANG("Exit"), this, &ISMenuBar::Exit, QKeySequence(Qt::AltModifier + Qt::Key_F4));

	CheckVisibleMunuItems();
	InstallFontMenuItems();

	CreateParagraphs();
	
	MainLayout->addStretch();

	LayoutButtons = new QHBoxLayout();
	LayoutButtons->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_4_PX);
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
	MenuService->addAction(LANG("ChangePassword"), this, &ISMenuBar::ChangePassword);
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateMenuHelp()
{
	QMenu *MenuHelp = MainMenu->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Help"));

	//О программе
	MenuHelp->addAction(BUFFER_ICONS("About"), LANG("AboutForm.AboutApplication"), this, &ISMenuBar::AboutApplication);

	//Qt
	MenuHelp->addAction(BUFFER_ICONS("Qt"), LANG("AboutFrameworkDeveloped"), this, &ISMenuBar::AboutQt);
	MenuHelp->addSeparator();
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateButtonsPanel()
{
	connect(CreateButton(LANG("Settings"), "MainPanel.Settings"), &QToolButton::clicked, this, &ISMenuBar::Settings); //Настройки
	connect(CreateButton(LANG("ChangeUser"), "MainPanel.ChangeUser"), &QToolButton::clicked, this, &ISMenuBar::ChangeUser); //Смена пользователя
}
//-----------------------------------------------------------------------------
QToolButton* ISMenuBar::CreateButton(const QString &ToolTip, const QString &IconName)
{
	QIcon Icon = BUFFER_ICONS(IconName);
	Icon.addPixmap(BUFFER_ICONS(IconName + ".Active").pixmap(ISDefines::Gui::SIZE_24_24), QIcon::Active);

	QToolButton *ToolButton = new QToolButton(this);
	ToolButton->setToolTip(ToolTip);
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ToolButton->setIconSize(ISDefines::Gui::SIZE_24_24);
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
		ActionMainItem->setFont(ISDefines::Gui::FONT_TAHOMA_10);

		if (ActionMainItem->menu())
		{
			for (QAction *ActionChildItem : ActionMainItem->menu()->actions())
			{
				ActionChildItem->setFont(ISDefines::Gui::FONT_TAHOMA_10);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISMenuBar::CreateParagraphs()
{
	LayoutParagraphs = new QHBoxLayout();
	MainLayout->addLayout(LayoutParagraphs);

	for (ISMetaParagraph *MetaParagraph : ISParagraphEntity::GetInstance().GetParagraphs())
	{
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
