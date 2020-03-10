#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGIndex
{
public:
	static bool CreateIndex(PMetaClassIndex *Index, QString &ErrorString); //�������� �������
	static bool UpdateIndex(PMetaClassIndex *Index, QString &ErrorString); //���������� �������
	static bool CheckExistIndex(PMetaClassIndex *Index, bool &Exist, QString &ErrorString); //�������� ������������� �������
	static bool CheckIndexForeign(PMetaClassIndex *Index); //��������� ��������� �� �� ����� ������ ������� ����
	static bool ReindexIndex(PMetaClassIndex *Index, QString &ErrorString); //����������� ������

private:
	static QString GetIndexName(PMetaClassIndex *Index); //��������� ����� �������
};
//-----------------------------------------------------------------------------
