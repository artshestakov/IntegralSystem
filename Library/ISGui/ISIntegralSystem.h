#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISIntegralSystem : public QApplication
{
	Q_OBJECT

public:
	ISIntegralSystem(int &argc, char **argv);
	virtual ~ISIntegralSystem();

	bool notify(QObject *Object, QEvent *e);
};
//-----------------------------------------------------------------------------
