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

	ButtonMenu = new ISButtonMainMenu(this);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	MainLayout->addWidget(ButtonMenu);

	ButtonNotify = new ISButtonNotify(this);
	MainLayout->addWidget(ButtonNotify);

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
		ISParagraphButton *ParagraphButton = new ISParagraphButton(MetaParagraph, this);
		connect(ParagraphButton, &ISParagraphButton::Clicked, this, static_cast<void(ISMenuBar::*)()>(&ISMenuBar::ButtonParagraphClicked));
		LayoutParagraphs->addWidget(ParagraphButton);
		ParagraphButtons.insert(MetaParagraph->UID, ParagraphButton);
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
void ISMenuBar::ButtonParagraphClicked()
{
	for (ISParagraphButton *ParagraphButton : ParagraphButtons.values())
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
