#include "ISLicenseForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISLicense.h"
#include "ISButtonClose.h"
//-----------------------------------------------------------------------------
ISLicenseForm::ISLicenseForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("License"));
	setWindowIcon(BUFFER_ICONS("License"));
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	FormLayout->addRow(LANG("License.Configuration") + ':', new QLabel(ISLicense::GetInstance().GetName(), this));
	FormLayout->addRow(LANG("License.Name") + ':', new QLabel(ISLicense::GetInstance().GetLocalName(), this));

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISLicenseForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISLicenseForm::~ISLicenseForm()
{

}
//-----------------------------------------------------------------------------
