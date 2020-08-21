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

	//���� "������"
	QMenu *MenuService = ButtonMenu->menu()->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Service"));

	//���������
	MenuService->addAction(BUFFER_ICONS("Favorites"), LANG("Favorites"), this, &ISMenuBar::Favorites);

	//�������
	MenuService->addAction(BUFFER_ICONS("History"), LANG("History"), this, &ISMenuBar::History);

	MenuService->addSeparator();

	//����� ������
	MenuService->addAction(LANG("ChangePassword"), this, &ISMenuBar::ChangePassword);

	//���� "������"
	QMenu *MenuHelp = ButtonMenu->menu()->addMenu(BUFFER_ICONS("MainMenuItem"), LANG("Help"));

	//� ���������
	MenuHelp->addAction(BUFFER_ICONS("About"), LANG("AboutForm.AboutApplication"), this, &ISMenuBar::AboutApplication);

	//Qt
	MenuHelp->addAction(BUFFER_ICONS("Qt"), LANG("AboutFrameworkDeveloped"), this, &ISMenuBar::AboutQt);
	MenuHelp->addSeparator();

	//�����
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(BUFFER_ICONS("PowerOff"), LANG("Exit"), this, &ISMenuBar::Exit, QKeySequence(Qt::AltModifier + Qt::Key_F4));

	//��������� ������ ��� ���� ��������� �������� ����
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

	//�������� ���������
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

	//�������� ������ � �������� ������
	connect(CreateButton(LANG("Settings"), "MainPanel.Settings"), &QToolButton::clicked, this, &ISMenuBar::Settings); //���������
	connect(CreateButton(LANG("ChangeUser"), "MainPanel.ChangeUser"), &QToolButton::clicked, this, &ISMenuBar::ChangeUser); //����� ������������
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

	QIcon IconDefault = BUFFER_ICONS(MetaParagraph->Icon), IconActive = BUFFER_ICONS(MetaParagraph->Icon + ".Active");
	QIcon Icon;
	Icon.addPixmap(IconDefault.pixmap(QSize(45, 45), QIcon::Normal, QIcon::Off));
	Icon.addPixmap(IconActive.pixmap(ISDefines::Gui::SIZE_45_45), QIcon::Normal, QIcon::On);
	Icon.addPixmap(IconActive.pixmap(ISDefines::Gui::SIZE_45_45), QIcon::Active, QIcon::Off);
	Icon.addPixmap(IconActive.pixmap(ISDefines::Gui::SIZE_45_45), QIcon::Selected, QIcon::On);

	QToolButton *ToolButton = new QToolButton(this);
	ToolButton->setToolTip(MetaParagraph->ToolTip);
	ToolButton->setCheckable(true);
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ToolButton->setIconSize(ISDefines::Gui::SIZE_45_45);
	ToolButton->setCursor(CURSOR_POINTING_HAND);
	ToolButton->setStyleSheet(STYLE_SHEET("ISParagraphButton"));
	LayoutWidget->addWidget(ToolButton, 0, Qt::AlignHCenter);
	return ToolButton;
}
//-----------------------------------------------------------------------------
