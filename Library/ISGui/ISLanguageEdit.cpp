#include "StdAfx.h"
#include "ISLanguageEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISLanguageEdit::ISLanguageEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	//AddItem(BUFFER_ICONS("Language.Canada"), LOCALIZATION("Language.Canada"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Germany"), LOCALIZATION("Language.Germany"), QVariant());
	AddItem(BUFFER_ICONS("Language.English"), LOCALIZATION("Language.English"), "en");
	//AddItem(BUFFER_ICONS("Language.Finland"), LOCALIZATION("Language.Finland"), QVariant());
	//AddItem(BUFFER_ICONS("Language.France"), LOCALIZATION("Language.France"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Hungary"), LOCALIZATION("Language.Hungary"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Italy"), LOCALIZATION("Language.Italy"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Japan"), LOCALIZATION("Language.Japan"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Latvia"), LOCALIZATION("Language.Latvia"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Poland"), LOCALIZATION("Language.Poland"), QVariant());
	AddItem(BUFFER_ICONS("Language.Russia"), LOCALIZATION("Language.Russia"), "ru");
	//AddItem(BUFFER_ICONS("Language.Slovakia"), LOCALIZATION("Language.Slovakia"), QVariant());
	//AddItem(BUFFER_ICONS("Language.Ukraine"), LOCALIZATION("Language.Ukraine"), QVariant());
}
//-----------------------------------------------------------------------------
ISLanguageEdit::~ISLanguageEdit()
{

}
//-----------------------------------------------------------------------------
