#include "ISUuidEdit.h"
#include "ISUuid.h"
#include "ISButtons.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISUuidEdit::ISUuidEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetUppercase(true);
	SetPlaceholderText("{00000000-0000-0000-0000-000000000000}");

	ISServiceButton *ButtonGenerate = new ISServiceButton(this);
	ButtonGenerate->setToolTip(LANG("ClickFromGenerateNewUID"));
	ButtonGenerate->setIcon(BUFFER_ICONS("UUID.Generate"));
	AddWidgetToRight(ButtonGenerate);
}
//-----------------------------------------------------------------------------
ISUuidEdit::~ISUuidEdit()
{

}
//-----------------------------------------------------------------------------
void ISUuidEdit::SetValue(const QVariant &value)
{
	ISLineEdit::SetValue(ISUuid(value.toString()));
}
//-----------------------------------------------------------------------------
bool ISUuidEdit::IsValid() const
{
	return ISUuid(GetValue().toString()).length();
}
//-----------------------------------------------------------------------------
void ISUuidEdit::Generate()
{
	SetValue(ISSystem::GenerateUuid());
}
//-----------------------------------------------------------------------------
