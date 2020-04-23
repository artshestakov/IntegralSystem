#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTaskTabBase : public QWidget
{
	Q_OBJECT

signals:
	void TabTextChanged(int Count);

public:
	ISTaskTabBase(int task_id, QTabWidget *parent = 0);
	virtual ~ISTaskTabBase();

	virtual void Update() = 0;

protected:
	int GetTaskID() const;
	void SetCount(int count);
	void Change();

private:
	int TaskID;
	int Count;
};
//-----------------------------------------------------------------------------
