#include "ISDesktopForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISObjects.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISDesktopForm::ISDesktopForm(QWidget *parent)
	: ISParagraphBaseForm(parent),
	DesktopWidget(nullptr)
{
	MainLayout = new QVBoxLayout();
	setLayout(MainLayout);
	
	QString DesktopFormName = ISObjects::GetInstance().GetInfo().DesktopForm;
	if (DesktopFormName.isEmpty())
	{
		MainLayout->addStretch();

		QLabel *LabelLogo = new QLabel(this);
		LabelLogo->setPixmap(BUFFER_PIXMAPS("DesktopLogo"));
		MainLayout->addWidget(LabelLogo, 0, Qt::AlignCenter);

		QLabel *LabelLocalName = new QLabel(this);
		LabelLocalName->setText(ISObjects::GetInstance().GetInfo().LocalName);
		LabelLocalName->setFont(ISDefines::Gui::FONT_TAHOMA_15_BOLD);
		LabelLocalName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
		MainLayout->addWidget(LabelLocalName, 0, Qt::AlignCenter);

		MainLayout->addStretch();
	}
	else
	{
		if (ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP))
		{
			DesktopWidget = ISAlgorithm::CreatePointer<QWidget *>(DesktopFormName, Q_ARG(QWidget *, this));
			MainLayout->addWidget(DesktopWidget);
		}
		else
		{
			QLabel *Label = new QLabel(this);
			Label->setText(LANG("NotAccessSpecialDesktop"));
			Label->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
			MainLayout->addWidget(Label, 0, Qt::AlignCenter);
		}
	}
}
//-----------------------------------------------------------------------------
ISDesktopForm::~ISDesktopForm()
{

}
//-----------------------------------------------------------------------------
void ISDesktopForm::Invoke()
{
	ISParagraphBaseForm::Invoke();
	if (DesktopWidget)
	{
		DesktopWidget->show();
	}
}
//-----------------------------------------------------------------------------
