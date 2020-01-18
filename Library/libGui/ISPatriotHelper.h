#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPatriotHelper
{
public:
	static bool IsVisit(int SubscriptionID);
	static void OpenVisit(int SubscriptionID);
	static void CloseVisit(int SubscriptionID);
	static void SetNowExist(int SubscriptionID, bool NowExist);
};
//-----------------------------------------------------------------------------
