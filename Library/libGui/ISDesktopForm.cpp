#include "StdAfx.h"
#include "ISDesktopForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISUserRoleEntity.h"
#include "ISAssert.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
ISDesktopForm::ISDesktopForm(QWidget *parent) : ISParagraphBaseForm(parent)
{
	DesktopWidget = nullptr;

	MainLayout = new QVBoxLayout();
	setLayout(MainLayout);
	
	QString DesktopFormName = ISLicense::GetInstance().GetDesktopForm();
	if (DesktopFormName.length())
	{
		if (ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP))
		{
			int ObjectType = QMetaType::type((DesktopFormName + "*").toLocal8Bit().constData());
			IS_ASSERT(ObjectType, QString("Class for desktop form is NULL. DesktopFormName: %1").arg(DesktopFormName));

			const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
			IS_ASSERT(MetaObject, "Error opening desktop widget.");

			DesktopWidget = dynamic_cast<QWidget*>(MetaObject->newInstance(Q_ARG(QWidget *, this)));
			IS_ASSERT(DesktopWidget, QString("Error instance desktop widget. DesktopFormName: %1").arg(DesktopFormName));
			MainLayout->addWidget(DesktopWidget);
		}
		else
		{
			QLabel *Label = new QLabel(this);
			Label->setText(LANG("NotAccessSpecialDesktop"));
			Label->setFont(FONT_TAHOMA_12_BOLD);
			MainLayout->addWidget(Label, 0, Qt::AlignCenter);
		}
	}
	else
	{
		MainLayout->addStretch();

		QLabel *LabelLogo = new QLabel(this);
		LabelLogo->setPixmap(BUFFER_PIXMAPS("DesktopLogo"));
		MainLayout->addWidget(LabelLogo, 0, Qt::AlignCenter);

		QLabel *LabelLocalName = new QLabel(this);
		LabelLocalName->setText(ISLicense::GetInstance().GetLocalName());
		LabelLocalName->setFont(FONT_TAHOMA_15_BOLD);
		LabelLocalName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
		MainLayout->addWidget(LabelLocalName, 0, Qt::AlignCenter);

		MainLayout->addStretch();
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
