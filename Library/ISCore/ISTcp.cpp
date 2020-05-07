#include "ISTcp.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
bool ISTcp::IsValidQuery(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString)
{
	VariantMap = ISSystem::JsonStringToVariantMap(ByteArray, ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty())
	{
		return false;
	}

	if (!VariantMap.contains("Type"))
	{
		ErrorString = "Not found field \"Type\"";
		return false;
	}

	if (!VariantMap.contains("Parameters"))
	{
		ErrorString = "Not found field \"Parameters\"";
		return false;
	}
	
	if (!VariantMap["Type"].isValid())
	{
		ErrorString = "Empty field \"Type\"";
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcp::IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString)
{
	VariantMap = ISSystem::JsonStringToVariantMap(ByteArray, ErrorString);
	if (VariantMap.isEmpty() && !ErrorString.isEmpty())
	{
		return false;
	}

	if (!VariantMap.contains("IsError"))
	{
		ErrorString = "Not found field \"IsError\"";
		return false;
	}

	if (!VariantMap.contains("ErrorDescription"))
	{
		ErrorString = "Not found field \"ErrorDescription\"";
		return false;
	}

	if (!VariantMap["IsError"].isValid())
	{
		ErrorString = "Empty field \"IsError\"";
		return false;
	}

	if (!VariantMap["ErrorDescription"].isValid())
	{
		ErrorString = "Empty field \"ErrorDescription\"";
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
