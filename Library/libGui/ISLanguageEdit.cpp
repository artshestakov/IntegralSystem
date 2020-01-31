#include "StdAfx.h"
#include "ISLanguageEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISLanguageEdit::ISLanguageEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	//AddItem(BUFFER_ICONS("Language.Canada"), LANG("Language.Canada"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Germany"), LANG("Language.Germany"), QVariant());
	AddItem(BUFFER_ICONS("Language.English"), LANG("Language.English"), "en");
	//AddItem(BUFFER_ICONS("Language.Finland"), LANG("Language.Finland"), QVariant());
	//AddItem(BUFFER_ICONS("Language.France"), LANG("Language.France"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Hungary"), LANG("Language.Hungary"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Italy"), LANG("Language.Italy"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Japan"), LANG("Language.Japan"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Latvia"), LANG("Language.Latvia"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Poland"), LANG("Language.Poland"), QVariant());
	AddItem(BUFFER_ICONS("Language.Russia"), LANG("Language.Russia"), "ru");
	//AddItem(BUFFER_ICONS("Language.Slovakia"), LANG("Language.Slovakia"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Ukraine"), LANG("Language.Ukraine"), QVariant());
}
//-----------------------------------------------------------------------------
ISLanguageEdit::~ISLanguageEdit()
{

}
//-----------------------------------------------------------------------------
