#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCenterSeven : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSeven(QObject *parent);
	virtual ~ISCenterSeven();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
