#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
    PMetaBase(const std::string &type_object)
        : TypeObject(type_object.empty() ? "Unknown" : type_object) { }

    std::string TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
    PMetaField()
        : PMetaBase("Field")
    {

    }
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
    PMetaTable()
        : PMetaBase("Table")
    {

    }

    std::string TableName;
    std::string UID;
    std::string Alias;
    std::string LocalName;
    std::string LocalListName;

    std::vector<PMetaField*> Fields;
};
//-----------------------------------------------------------------------------
#endif
