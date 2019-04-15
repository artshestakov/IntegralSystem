#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISListEdit.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISExecutorEdit : public ISListEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISExecutorEdit(QWidget *parent = 0);
	virtual ~ISExecutorEdit();

	void SetReadOnly(bool read_only) override;

protected:
	void DesignateMe();

private:
	ISPushButton *ButtonDesignateMe;
};
//-----------------------------------------------------------------------------
