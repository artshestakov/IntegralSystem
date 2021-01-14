#pragma once
#ifndef _ISEXPORTWORKER_H_INCLUDED
#define _ISEXPORTWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "ISTcpModel.h"
//-----------------------------------------------------------------------------
class ISExportWorker : public QObject
{
	Q_OBJECT

signals:
	void ExportedRow();
	void Message(const QString &Message);

public:
	ISExportWorker(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportWorker();

	virtual bool Prepare() = 0; //Подготовка экспорта
	virtual bool Export() = 0; //Экспорт

	void Cancel(); //Остановка

	void SetFields(const ISVectorUInt &fields);
	void SetHeader(bool header);
	void SetSelectedRows(const ISVectorInt &selected_rows);

	QString GetErrorString() const;

protected:
	QVariant PrepareValue(ISNamespace::FieldType Type, const QVariant &Value) const;

protected:
	QString ErrorString;
	PMetaTable *MetaTable;
	ISTcpModel *TcpModel;
	ISVectorUInt Fields;
	bool Header;
	ISVectorInt SelectedRows;
	bool Canceled;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExportCSV : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportCSV(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportCSV();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileCSV;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExportHTML : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportHTML(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportHTML();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileHTML;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExportJSON : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportJSON(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportJSON();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileJSON;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExportXML : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportXML(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportXML();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileXML;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExportSQLite : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportSQLite(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportSQLite();

	bool Prepare() override;
	bool Export() override;

private:
	QString ConnectionName;
};
//-----------------------------------------------------------------------------
#endif
