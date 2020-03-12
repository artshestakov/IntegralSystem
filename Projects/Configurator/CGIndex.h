#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGIndex
{
public:
	static bool CreateIndex(PMetaIndex *Index, QString &ErrorString); //�������� �������
	static bool UpdateIndex(PMetaIndex *Index, QString &ErrorString); //���������� �������
	static bool CheckExistIndex(PMetaIndex *Index, bool &Exist, QString &ErrorString); //�������� ������������� �������
	static bool CheckIndexForeign(PMetaIndex *Index); //��������� ��������� �� �� ����� ������ ������� ����
	static bool ReindexIndex(PMetaIndex *Index, QString &ErrorString); //����������� ������

private:
	static QString GetIndexName(PMetaIndex *Index); //��������� ����� �������
};
//-----------------------------------------------------------------------------
