#include "ISExecutorEdit.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISExecutorEdit::ISExecutorEdit(QWidget *parent) : ISListEdit(parent)
{
	ButtonDesignateMe = new ISPushButton(this);
	ButtonDesignateMe->setText(LANG("Task.DesignateMe"));
	ButtonDesignateMe->setIcon(BUFFER_ICONS("User"));
	ButtonDesignateMe->setCursor(CURSOR_POINTING_HAND);
	ButtonDesignateMe->setSizePolicy(QSizePolicy::Maximum, ButtonDesignateMe->sizePolicy().verticalPolicy());
	connect(ButtonDesignateMe, &ISPushButton::clicked, this, &ISExecutorEdit::DesignateMe);
	AddWidgetToRight(ButtonDesignateMe);
}
//-----------------------------------------------------------------------------
ISExecutorEdit::~ISExecutorEdit()
{

}
//-----------------------------------------------------------------------------
void ISExecutorEdit::SetReadOnly(bool read_only)
{
	ISListEdit::SetReadOnly(read_only);
	ButtonDesignateMe->setVisible(!read_only);
}
//-----------------------------------------------------------------------------
void ISExecutorEdit::DesignateMe()
{
	SetValue(ISMetaUser::GetInstance().GetData()->ID);
}
//-----------------------------------------------------------------------------
