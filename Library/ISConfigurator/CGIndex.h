#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassIndex.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGIndex : public QObject
{
	Q_OBJECT

public:
	CGIndex();
	virtual ~CGIndex();

	static bool CreateIndex(PMetaClassIndex *Index, QString &ErrorString); //�������� �������
	static bool UpdateIndex(PMetaClassIndex *Index, QString &ErrorString); //���������� �������
	static bool CheckExistIndex(PMetaClassIndex *Index); //�������� ������������� �������
	static bool CheckIndexForeign(PMetaClassIndex *Index); //��������� ��������� �� �� ����� ������ ������� ����
	static bool ReindexIndex(PMetaClassIndex *Index, QString &ErrorString); //����������� ������

private:
	static QString GetIndexName(PMetaClassIndex *Index); //��������� ����� �������
};
//-----------------------------------------------------------------------------
