#include "ISFavorites.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_FAVORITES = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectsid "
											"FROM _favorites "
											"WHERE fvts_creationuseroid = currentuseroid()");
//-----------------------------------------------------------------------------
static QString QD_FAVORITE = PREPARE_QUERY("DELETE FROM _favorites "
										   "WHERE fvts_creationuseroid = currentuseroid() "
										   "AND fvts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QS_FAVORITE = PREPARE_QUERY("SELECT COUNT(*) "
										   "FROM _favorites "
										   "WHERE fvts_creationuseroid = currentuseroid() "
										   "AND fvts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QU_FAVORITE = PREPARE_QUERY("UPDATE _favorites SET "
										   "fvts_objectsid = :ObjectsID "
										   "WHERE fvts_creationuseroid = currentuseroid() "
										   "AND fvts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QI_FAVORITE = PREPARE_QUERY("INSERT INTO _favorites(fvts_tablename, fvts_objectsid) "
										   "VALUES(:TableName, :ObjectsID)");
//-----------------------------------------------------------------------------
ISFavorites::ISFavorites()
	: ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISFavorites::~ISFavorites()
{

}
//-----------------------------------------------------------------------------
ISFavorites& ISFavorites::Instance()
{
	static ISFavorites Favorites;
	return Favorites;
}
//-----------------------------------------------------------------------------
QString ISFavorites::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISFavorites::Initialize()
{
	ISQuery qSelectFavorites(QS_FAVORITES);
	bool Result = qSelectFavorites.Execute();
	if (Result)
	{
		while (qSelectFavorites.Next())
		{
			QString TableName = qSelectFavorites.ReadColumn("fvts_tablename").toString();
			QString ObjectsID = qSelectFavorites.ReadColumn("fvts_objectsid").toString();
			ObjectsID.remove(0, 1);
			ObjectsID.chop(1);

			QStringList StringList = ObjectsID.split(SYMBOL_COMMA);
			int StringListSize = StringList.size();
			ISVectorInt VectorInt(StringListSize);
			for (int i = 0; i < StringListSize; ++i)
			{
				VectorInt[i] = StringList[i].toInt();
			}
			Favorites[TableName] = VectorInt;
		}
	}
	else
	{
		ErrorString = qSelectFavorites.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISFavorites::AddFavorite(const QString &TableName, int ObjectID)
{
	Favorites.count(TableName) ? Favorites[TableName].emplace_back(ObjectID) : Favorites[TableName] = ISVectorInt{ ObjectID };
}
//-----------------------------------------------------------------------------
void ISFavorites::DeleteFavorite(const QString &TableName, int ObjectID)
{
	Favorites[TableName].erase(Favorites[TableName].begin() + ISAlgorithm::VectorIndexOf(Favorites[TableName], ObjectID));
}
//-----------------------------------------------------------------------------
void ISFavorites::DeleteAllFavorites()
{
	std::vector<QString> Keys = ISAlgorithm::ConvertMapToKeys(Favorites);
	for (const QString &Key : Keys)
	{
		Favorites[Key].clear();
	}
}
//-----------------------------------------------------------------------------
bool ISFavorites::CheckExistFavoriteObject(const QString &TableName, int ObjectID)
{
	bool Result = Favorites.count(TableName);
	if (Result)
	{
		Result = ISAlgorithm::VectorContains(Favorites[TableName], ObjectID);
	}
	return Result;
}
//-----------------------------------------------------------------------------
ISVectorInt& ISFavorites::GetObjects(const QString &TableName)
{
	return Favorites[TableName];
}
//-----------------------------------------------------------------------------
std::map<QString, ISVectorInt>& ISFavorites::GetObjects()
{
	return Favorites;
}
//-----------------------------------------------------------------------------
bool ISFavorites::Save()
{
	bool Result = true;
	for (const auto &MapItem : Favorites) //Обходим таблицы избранного
	{
		if (MapItem.second.empty()) //Если объектов в избранном по такой таблице нет - удаляем их
		{
			ISQuery qDelete(QD_FAVORITE);
			qDelete.BindValue(":TableName", MapItem.first);
			Result = qDelete.Execute();
			if (!Result)
			{
				ErrorString = qDelete.GetErrorString();
				break;
			}
		}
		else //Избранные объекты по такой таблице есть - сохраняем
		{
			ISQuery qSelect(QS_FAVORITE);
			qSelect.BindValue(":TableName", MapItem.first);
			Result = qSelect.ExecuteFirst();
			if (Result)
			{
				//Формируем массив идентификаторов
				QString ObjectsID = "{";
				for (int ObjectID : MapItem.second)
				{
					ObjectsID += QString::number(ObjectID) + ",";
				}
				ObjectsID.chop(1);
				ObjectsID += "}";

				ISQuery qUpsert(qSelect.ReadColumn("count").toBool() ? QU_FAVORITE : QI_FAVORITE);
				qUpsert.BindValue(":TableName", MapItem.first);
				qUpsert.BindValue(":ObjectsID", ObjectsID);
				Result = qUpsert.Execute();
				if (!Result)
				{
					ErrorString = qUpsert.GetErrorString();
					break;
				}
			}
			else
			{
				ErrorString = qSelect.GetErrorString();
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
