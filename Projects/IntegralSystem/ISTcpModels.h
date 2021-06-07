#pragma once
#ifndef _ISTCPMODEL_H_INCLUDED
#define _ISTCPMODEL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructsGUI.h"
//-----------------------------------------------------------------------------
class ISTcpModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ISTcpModel(QObject *parent);
    virtual ~ISTcpModel();

    void Clear();
    void SetSource(const QVariantList &fields, const QVariantList &records);
    void SetSorting(const QString &sorting_field, Qt::SortOrder sorting_order);

    void RemoveRecordIndex(unsigned int RowIndex); //Удаление записи из модели
    void RemoveRecordID(unsigned int ObjectID); //Удаление записи по её идентификатору
    ISModelRecord& GetRecord(int Index); //Получить строку по индексу
    QVariant GetRecordValue(int Index, const QString &FieldName); //Получить значение строки
    ISVectorUInt GetIDs() const; //Получить список всех индетификаторов записей
    int GetFieldIndex(const QString &FieldName) const;
    ISModelField GetField(const QString &FieldName); //Получить поле по имени
    ISModelField GetField(unsigned int Index); //Получить поле по индексу
    bool GetSum(int ColumnIndex, QString &Sum, QString &Avg); //Получить сумму значений в поле

    QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
    QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &Index) const override;

private:
    std::vector<ISModelField> Fields;
    std::vector<ISModelRecord> Records;
    int SortingColumnIndex;
    Qt::SortOrder SortingOrder;
    QIcon SortingIconUp;
    QIcon SortingIconDown;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISViewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ISViewModel(QObject *parent);
    virtual ~ISViewModel();

    void Clear();
    void SetSource(const QStringList &fields, const QVariantList &records);

    QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
    QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &Index) const override;

private:
    std::vector<QString> Fields;
    std::vector<QStringList> Records;
};
//-----------------------------------------------------------------------------
#endif
