#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISPatriotHelper : public QObject
{
	Q_OBJECT

public:
	ISPatriotHelper();
	virtual ~ISPatriotHelper();

	static bool IsVisit(int SubscriptionID);
	static void OpenVisit(int SubscriptionID);
	static void CloseVisit(int SubscriptionID);
	static void SetNowExist(int SubscriptionID, bool NowExist);
};
//-----------------------------------------------------------------------------
