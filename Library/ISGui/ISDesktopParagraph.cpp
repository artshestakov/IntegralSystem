#include "ISDesktopParagraph.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISObjects.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISDesktopParagraph::ISDesktopParagraph(QWidget *parent)
	: ISParagraphBaseForm(parent),
	DesktopWidget(nullptr)
{
	MainLayout = new QVBoxLayout();
	setLayout(MainLayout);
	
	QString DesktopFormName = ISBuffer::Instance().ConfigurationInfo.DesktopForm;
	if (DesktopFormName.isEmpty())
	{
		MainLayout->addStretch();
		
		QLabel *LabelLogo = new QLabel(this);
		LabelLogo->setPixmap(ISBuffer::Instance().ConfigurationInfo.LogoName.isEmpty() ?
			BUFFER_PIXMAPS("DesktopLogo") :
			QPixmap(":_" + ISBuffer::Instance().ConfigurationInfo.Name + '/' + ISBuffer::Instance().ConfigurationInfo.LogoName));
		MainLayout->addWidget(LabelLogo, 0, Qt::AlignCenter);

		if (!ISBuffer::Instance().ConfigurationInfo.LocalName.isEmpty())
		{
			QLabel *LabelLocalName = new QLabel(this);
			LabelLocalName->setText(ISBuffer::Instance().ConfigurationInfo.LocalName);
			LabelLocalName->setFont(ISDefines::Gui::FONT_TAHOMA_15_BOLD);
			LabelLocalName->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
			MainLayout->addWidget(LabelLocalName, 0, Qt::AlignCenter);
		}
		MainLayout->addStretch();
	}
	else
	{
		if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP))
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
ISDesktopParagraph::~ISDesktopParagraph()
{

}
//-----------------------------------------------------------------------------
void ISDesktopParagraph::Invoke()
{
	ISParagraphBaseForm::Invoke();
	if (DesktopWidget)
	{
		DesktopWidget->show();
	}
}
//-----------------------------------------------------------------------------
