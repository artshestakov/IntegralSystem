#pragma once
#ifndef _PMETACLASSFOREIGN_H_INCLUDED
#define _PMETACLASSFOREIGN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassForeign : public PMetaClass
{
	PMetaClassForeign() : PMetaClass("Foreign") { }

	QString Field; //����, �� ������� ��������������� ������� ����
	QString ForeignClass; //�� ����� ������� ���������� ������� ����
	QString ForeignField; //�� ����� ���� ���������� ������� ����
	QString ForeignViewNameField; //����� ���� (����) ���������� � ������� �� �������
	QString OrderField; //���� �� �������� ����� ����������� ����������

	QString TableName; //�������, ���������� ����, �� ������� ��������������� ������� ����
	QString SqlQuery; //������ �� ������ �� �������� �����
};
//-----------------------------------------------------------------------------
#endif
