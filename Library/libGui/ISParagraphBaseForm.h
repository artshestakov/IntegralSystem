#pragma once
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
