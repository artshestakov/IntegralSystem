#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISCheckViewParagraph : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCheckViewParagraph(QWidget *parent);
	virtual ~ISCheckViewParagraph();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

private:
	QButtonGroup *ButtonGroup;

};
//-----------------------------------------------------------------------------
