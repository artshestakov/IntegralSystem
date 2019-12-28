#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMetaUserData.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaUser : public QObject
{
	Q_OBJECT

public:
	ISMetaUser(const ISMetaUser &) = delete;
	ISMetaUser(ISMetaUser &&) = delete;
	ISMetaUser &operator=(const ISMetaUser &) = delete;
	ISMetaUser &operator=(ISMetaUser &&) = delete;
	~ISMetaUser();

	static ISMetaUser& GetInstance();

	void Initialize(const QString &login, const QString &password);

	ISMetaUserData* GetData();
	bool CheckPassword(const QString &EnteredPassword);
	QString GetCurrentStatus(int UserID) const; //Получить статус пользователя

private:
	ISMetaUser();

	ISMetaUserData *UserData;
};
//-----------------------------------------------------------------------------
#define CURRENT_USER_ID ISMetaUser::GetInstance().GetData()->ID
#define CURRENT_USER_LOGIN ISMetaUser::GetInstance().GetData()->Login
#define CURRENT_USER_PASSWORD ISMetaUser::GetInstance().GetData()->Password
#define CURRENT_USER_FULL_NAME ISMetaUser::GetInstance().GetData()->FullName
#define CURRENT_USER_SYSTEM ISMetaUser::GetInstance().GetData()->System
//-----------------------------------------------------------------------------
