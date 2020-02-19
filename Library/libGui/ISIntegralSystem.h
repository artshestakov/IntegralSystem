#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISIntegralSystem : public QApplication
{
	Q_OBJECT

public:
	ISIntegralSystem(int &argc, char **argv);
	virtual ~ISIntegralSystem();

	bool notify(QObject *Object, QEvent *e);
};
//-----------------------------------------------------------------------------
