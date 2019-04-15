#include "StdAfx.h"
#include "ISAsteriskRecordPlayForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtonClose.h"
//-----------------------------------------------------------------------------
ISAsteriskRecordPlayForm::ISAsteriskRecordPlayForm(const QString &FilePath, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LOCALIZATION("ReproductionAsteriskRecord"));
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);
	ForbidResize();

	PlayerWidget = new ISPlayerWidget(FilePath, true, this);
	GetMainLayout()->addWidget(PlayerWidget);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISAsteriskRecordPlayForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISAsteriskRecordPlayForm::~ISAsteriskRecordPlayForm()
{

}
//-----------------------------------------------------------------------------
