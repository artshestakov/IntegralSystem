#pragma once
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPatriotHelper
{
public:
	static bool IsVisit(int SubscriptionID);
	static void OpenVisit(int SubscriptionID);
	static void CloseVisit(int SubscriptionID);
	static void SetNowExist(int SubscriptionID, bool NowExist);
};
//-----------------------------------------------------------------------------
