#pragma once
#ifndef _ISSERVERINFOSUBSYSTEM_H_INCLUDED
#define _ISSERVERINFOSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
class ISServerInfoSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISServerInfoSubSystem(QWidget *parent);
	virtual ~ISServerInfoSubSystem();

	void LoadData() override;
};
//-----------------------------------------------------------------------------
#endif
