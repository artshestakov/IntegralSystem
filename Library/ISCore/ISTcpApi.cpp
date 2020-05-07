#include "ISTcpApi.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpApi::ISTcpApi()
{
	Functions[API_TEST_QUERY] = std::mem_fn(&ISTcpApi::TestQuery);
	Functions[API_SLEEP] = std::mem_fn(&ISTcpApi::Sleep);
}
//-----------------------------------------------------------------------------
ISTcpApi::~ISTcpApi()
{

}
//-----------------------------------------------------------------------------
bool ISTcpApi::Has(const QString &QueryType)
{
	return Functions.count(QueryType) == 1 ? true : false;
}
//-----------------------------------------------------------------------------
void ISTcpApi::Execute(const QString &QueryType, const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
	Functions[QueryType](*this, Parameters, TcpAnswer);
}
//-----------------------------------------------------------------------------
bool ISTcpApi::CheckField(const QVariantMap &Parameters, const ISVectorString &Fields, ISTcpAnswer &TcpAnswer)
{
	for (const QString &FieldName : Fields)
	{
		if (!Parameters.contains(FieldName))
		{
			TcpAnswer.SetError(QString("Not found field \"%1\"").arg(FieldName));
			return false;
		}

		if (!Parameters[FieldName].isValid())
		{
			TcpAnswer.SetError(QString("Field \"%1\" is empty").arg(FieldName));
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpApi::TestQuery(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
	
}
//-----------------------------------------------------------------------------
void ISTcpApi::Sleep(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
	if (!CheckField(Parameters, { "MSec" }, TcpAnswer))
	{
		return;
	}

	bool Result = true;
	int MSec = Parameters["MSec"].toInt(&Result);
	if (Result)
	{
		Result = MSec > 0;
	}

	if (Result)
	{
		ISSleep(MSec);
	}
	else
	{
		TcpAnswer.SetError("Invalid value msec");
	}
}
//-----------------------------------------------------------------------------
