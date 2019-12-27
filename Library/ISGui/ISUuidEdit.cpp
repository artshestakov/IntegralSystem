#include "StdAfx.h"
#include "ISUuidEdit.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
ISUuidEdit::ISUuidEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetUppercase(true);
	SetPlaceholderText("{00000000-0000-0000-0000-000000000000}");
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
