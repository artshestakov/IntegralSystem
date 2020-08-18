#pragma once
#ifndef _ISFILEEDIT_H_INCLUDED
#define _ISFILEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISFileEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFileEdit(QWidget *parent = 0);
	virtual ~ISFileEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

private:
	void SelectFile();
	void Save();
	void Rename();

private:
	QVariantMap VariantMap;

	ISPushButton *ButtonFile;
	QMenu *Menu;
	QAction *ActionSave;
	QAction *ActionRename;
};
//-----------------------------------------------------------------------------
#endif
