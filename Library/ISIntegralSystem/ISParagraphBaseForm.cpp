#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
//-----------------------------------------------------------------------------
ISParagraphBaseForm::ISParagraphBaseForm(QWidget *parent) : QWidget(parent)
{
	ParagraphButton = nullptr;
}
//-----------------------------------------------------------------------------
ISParagraphBaseForm::~ISParagraphBaseForm()
{

}
//-----------------------------------------------------------------------------
void ISParagraphBaseForm::SetButtonParagraph(ISParagraphButton *paragraph_button)
{
	ParagraphButton = paragraph_button;
}
//-----------------------------------------------------------------------------
ISParagraphButton* ISParagraphBaseForm::GetButtonParagraph()
{
	return ParagraphButton;
}
//-----------------------------------------------------------------------------
void ISParagraphBaseForm::Invoke()
{

}
//-----------------------------------------------------------------------------
