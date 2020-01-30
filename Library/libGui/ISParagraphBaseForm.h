#pragma once
#ifndef _ISPARAGPATHBASEFORM_H_INCLUDED
#define _ISPARAGPATHBASEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMenuBar.h"
//-----------------------------------------------------------------------------
class ISParagraphBaseForm : public QWidget
{
	Q_OBJECT

public:
	ISParagraphBaseForm(QWidget *parent = 0);
	virtual ~ISParagraphBaseForm();

	void SetButtonParagraph(ISParagraphButton *paragraph_button);
	ISParagraphButton *GetButtonParagraph();
	
	virtual void Invoke() = 0;

private:
	ISParagraphButton *ParagraphButton;
};
//-----------------------------------------------------------------------------
#endif
