#include "ISWeekDayEdit.h"
#include "ISSystem.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISWeekDayEdit::ISWeekDayEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	AddItem(LANG("Everyday"), 0);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Monday), Qt::Monday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Tuesday), Qt::Tuesday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Wednesday), Qt::Wednesday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Thursday), Qt::Thursday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Friday), Qt::Friday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Saturday), Qt::Saturday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Sunday), Qt::Sunday);
}
//-----------------------------------------------------------------------------
ISWeekDayEdit::~ISWeekDayEdit()
{

}
//-----------------------------------------------------------------------------
