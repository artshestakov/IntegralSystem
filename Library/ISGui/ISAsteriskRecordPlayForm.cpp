#include "ISAsteriskRecordPlayForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtons.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISAsteriskRecordPlayForm::ISAsteriskRecordPlayForm(const QString &FilePath) : ISInterfaceDialogForm()
{
	setWindowTitle(LANG("ReproductionAsteriskRecord"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	ForbidResize();

	PlayerWidget = new ISPlayerWidget(FilePath, true, this);
	GetMainLayout()->addWidget(PlayerWidget);

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISAsteriskRecordPlayForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISAsteriskRecordPlayForm::~ISAsteriskRecordPlayForm()
{

}
//-----------------------------------------------------------------------------
