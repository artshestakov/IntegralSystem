#include "ISTcpApi.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpApi::ISTcpApi()
{
	Functions[API_AUTH] = std::mem_fn(&ISTcpApi::Auth);
}
//-----------------------------------------------------------------------------
ISTcpApi::~ISTcpApi()
{

}
//-----------------------------------------------------------------------------
bool ISTcpApi::Has(const QString &QueryType)
{
	return Functions.count(QueryType) > 0 ? true : false;
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
void ISTcpApi::Auth(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer)
{
	if (!CheckField(Parameters, { "Login", "Password" }, TcpAnswer))
	{
		return;
	}
	
	if (Parameters["Login"].toString() != "postgres" || Parameters["Password"].toString() != "adm777")
	{
		TcpAnswer.SetError("Invalid login or password");
	}
}
//-----------------------------------------------------------------------------
