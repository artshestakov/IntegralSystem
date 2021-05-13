#pragma once
#ifndef _ISEXPORTWORKER_H_INCLUDED
#define _ISEXPORTWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpModels.h"
//-----------------------------------------------------------------------------
class ISExportWorker : public QObject
{
    Q_OBJECT

signals:
    void ExportedRow();
    void Message(const QString &Message);

public:
    ISExportWorker(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent = 0);
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
    QString TableName;
    QString TableLocalListName;
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
    ISExportCSV(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent = 0);
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
    ISExportHTML(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent = 0);
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
    ISExportJSON(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent = 0);
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
    ISExportXML(const QString &table_name, const QString &table_local_list_name, ISTcpModel *tcp_model, QObject *parent = 0);
    virtual ~ISExportXML();

    bool Prepare() override;
    bool Export() override;

private:
    QFile *FileXML;
};
//-----------------------------------------------------------------------------
#endif
