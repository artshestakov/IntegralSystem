#pragma once
#ifndef _CGCONFIGURATORUPDATE_H_INCLUDED
#define _CGCONFIGURATORUPDATE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorUpdate : public CGConfiguratorBase
{
public:
    CGConfiguratorUpdate();
    virtual ~CGConfiguratorUpdate();

    bool database(); //���������� ���� ������
    bool functions(); //���������� �������
    bool tables(); //���������� ������
    bool comment(); //��������������� ������
    bool indexes(); //���������� ��������
    bool foreigns(); //���������� ������� ������
    bool resources(); //���������� ��������
    bool databasesettings(); //���������� �������� ��
    //bool protocol(); //������� ���������
};
//-----------------------------------------------------------------------------
#endif
