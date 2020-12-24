#pragma once
#ifndef _ISTCPQUERY_H_INCLUDED
#define _ISTCPQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISTcpQuery
{
public:
	ISTcpQuery(const QString &query_type);
	~ISTcpQuery();

	QString GetErrorString() const;
	void BindValue(const QString &ParamterName, const QVariant &ParameterValue);
	virtual bool Execute(const QString &query_type);
	virtual bool Execute();
	QVariantMap& GetAnswer();
	QVariantMap TakeAnswer();

protected:
	QVariant GetParamet(const QString &ParameterName) const;

private:
	bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap);

private:
	QString ErrorString;
	QString QueryType;
	QVariantMap Parameters;
	QVariantMap TcpAnswer;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTcpQueryTable : public ISTcpQuery
{
public:
	ISTcpQueryTable();
	virtual ~ISTcpQueryTable();

	bool Execute() override;
	void SetSorting(const QString &SortingField, Qt::SortOrder SortingOrder); //Изменить сортировку
	void AddFilter(const QString &FieldName, const QVariant &Value); //Добавить фильтр

private:
	QVariantMap FilterMap;
	QVariantMap SortingMap;
};
//-----------------------------------------------------------------------------
#endif
