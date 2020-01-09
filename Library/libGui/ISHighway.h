#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISHighway : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighway(QObject *parent);
	virtual ~ISHighway();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
