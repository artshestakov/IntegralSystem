#include "StdAfx.h"
#include "ISTaskPriorityEdit.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISTaskPriorityEdit::ISTaskPriorityEdit(QWidget *parent) : ISRadioEdit(parent)
{
	QRadioButton *ButtonLow = new QRadioButton(this);
	ButtonLow->setText(LANG("Task.Priority.Low"));
	ButtonLow->setToolTip(LANG("Task.Priority.Low.ToolTip"));
	ButtonLow->setStyleSheet(STYLE_SHEET("Task.Priority.Low"));
	ButtonLow->setIcon(BUFFER_ICONS("Task.Priority.Low"));
	ButtonLow->setFont(FONT_APPLICATION_BOLD);
	AddButton(ButtonLow, 1);

	QRadioButton *ButtonAverage = new QRadioButton(this);
	ButtonAverage->setText(LANG("Task.Priority.Average"));
	ButtonAverage->setToolTip(LANG("Task.Priority.Average.ToolTip"));
	ButtonAverage->setStyleSheet(STYLE_SHEET("Task.Priority.Average"));
	ButtonAverage->setIcon(BUFFER_ICONS("Task.Priority.Average"));
	ButtonAverage->setFont(FONT_APPLICATION_BOLD);
	AddButton(ButtonAverage, 2);

	QRadioButton *ButtonTall = new QRadioButton(this);
	ButtonTall->setText(LANG("Task.Priority.Tall"));
	ButtonTall->setToolTip(LANG("Task.Priority.Tall.ToolTip"));
	ButtonTall->setStyleSheet(STYLE_SHEET("Task.Priority.Tall"));
	ButtonTall->setIcon(BUFFER_ICONS("Task.Priority.Tall"));
	ButtonTall->setFont(FONT_APPLICATION_BOLD);
	AddButton(ButtonTall, 3);

	//Установка низкого приоритета задачи по умолчанию
	ButtonLow->setChecked(true);
	SetModificationFlag(true);
}
//-----------------------------------------------------------------------------
ISTaskPriorityEdit::~ISTaskPriorityEdit()
{

}
//-----------------------------------------------------------------------------
