#include "ISFavorites.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_FAVORITES = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectid "
											"FROM _favorites "
											"WHERE fvts_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QI_FAVORITE = PREPARE_QUERY("INSERT INTO _favorites(fvts_user, fvts_tablename, fvts_tablelocalname, fvts_objectname, fvts_objectid) "
										   "VALUES(:User, :TableName, :TableLocalName, :ObjectName, :ObjectID)");
//-----------------------------------------------------------------------------
static QString QD_FAVORITE = PREPARE_QUERY("DELETE FROM _favorites "
										   "WHERE fvts_tablename = :TableName "
										   "AND fvts_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QD_ALL_FAVORITES = PREPARE_QUERY("DELETE FROM _favorites "
												"WHERE fvts_user = currentuserid()");
//-----------------------------------------------------------------------------
ISFavorites::ISFavorites()
{

}
//-----------------------------------------------------------------------------
ISFavorites::~ISFavorites()
{

}
//-----------------------------------------------------------------------------
ISFavorites& ISFavorites::GetInstance()
{
	static ISFavorites Favorites;
	return Favorites;
}
//-----------------------------------------------------------------------------
void ISFavorites::Initialize()
{
	ISQuery qSelectFavorites(QS_FAVORITES);
	if (qSelectFavorites.Execute())
	{
		while (qSelectFavorites.Next())
		{
			QString TableName = qSelectFavorites.ReadColumn("fvts_tablename").toString();
			int ObjectID = qSelectFavorites.ReadColumn("fvts_objectid").toInt();

			if (Favorites.contains(TableName))
			{
				Favorites[TableName].emplace_back(ObjectID);
			}
			else
			{
				Favorites.insert(TableName, ISVectorInt{ ObjectID });
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISFavorites::AddFavorite(const QString &TableName, const QString &TableLocalName, const QString &ObjectName, int ObjectID)
{
	ISQuery qInsertFavorite(QI_FAVORITE);
	qInsertFavorite.BindValue(":User", CURRENT_USER_ID);
	qInsertFavorite.BindValue(":TableName", TableName);
	qInsertFavorite.BindValue(":TableLocalName", TableLocalName);
	qInsertFavorite.BindValue(":ObjectName", ObjectName);
	qInsertFavorite.BindValue(":ObjectID", ObjectID);
	if (qInsertFavorite.Execute())
	{
		Favorites.contains(TableName) ? Favorites[TableName].emplace_back(ObjectID) : Favorites.insert(TableName, ISVectorInt{ ObjectID });
	}
}
//-----------------------------------------------------------------------------
bool ISFavorites::DeleteFavorite(const QString &TableName, int ObjectID)
{
	ISQuery qDeleteFavorite(QD_FAVORITE);
	qDeleteFavorite.BindValue(":TableName", TableName);
	qDeleteFavorite.BindValue(":ObjectID", ObjectID);
	bool Executed = qDeleteFavorite.Execute();
	if (Executed)
	{
		Favorites[TableName].erase(Favorites[TableName].begin() + ISAlgorithm::VectorIndexOf(Favorites[TableName], ObjectID));
		if (Favorites[TableName].empty())
		{
			Favorites.take(TableName);
		}
	}
	return Executed;
}
//-----------------------------------------------------------------------------
void ISFavorites::DeleteAllFavorites()
{
	ISQuery qDeleteFavorites(QD_ALL_FAVORITES);
	if (qDeleteFavorites.Execute())
	{
		while (Favorites.count())
		{
			Favorites.take(Favorites.begin().key()).clear();
		}
	}
}
//-----------------------------------------------------------------------------
bool ISFavorites::CheckExistFavoriteObject(const QString &TableName, int ObjectID)
{
	bool Result = Favorites.contains(TableName);
	if (Result)
	{
		Result = ISAlgorithm::VectorContains(Favorites[TableName], ObjectID);
	}
	return Result;
}
//-----------------------------------------------------------------------------