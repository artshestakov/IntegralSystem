#pragma once
#ifndef _ISSETTINGFIELDWIDGETS_H_INCLUDED
#define _ISSETTINGFIELDWIDGETS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISComboEdit.h"
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboSettingEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSettingEdit(QWidget *parent);
	virtual ~ISComboSettingEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboCalendarNoteType : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboCalendarNoteType(QWidget *parent);
	virtual ~ISComboCalendarNoteType();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboIncomingFormPosition : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboIncomingFormPosition(QWidget *parent);
	virtual ~ISComboIncomingFormPosition();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboStartedParagraph : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboStartedParagraph(QWidget *parent);
	virtual ~ISComboStartedParagraph();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTabBarSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTabBarSelectionBehavior(QWidget *parent);
	virtual ~ISComboTabBarSelectionBehavior();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTableDateFormat : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDateFormat(QWidget *parent);
	virtual ~ISComboTableDateFormat();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTableDoubleClickEvent : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDoubleClickEvent(QWidget *parent);
	virtual ~ISComboTableDoubleClickEvent();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTableSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableSelectionBehavior(QWidget *parent);
	virtual ~ISComboTableSelectionBehavior();
};
//-----------------------------------------------------------------------------
#endif
