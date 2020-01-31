#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMetaParagraph.h"
//-----------------------------------------------------------------------------
class ISParagraphEntity
{
public:
	ISParagraphEntity(const ISParagraphEntity &) = delete;
	ISParagraphEntity(ISParagraphEntity &&) = delete;
	ISParagraphEntity &operator=(const ISParagraphEntity &) = delete;
	ISParagraphEntity &operator=(ISParagraphEntity &&) = delete;
	~ISParagraphEntity();

	static ISParagraphEntity& GetInstance();
	
	ISUuid GetDefaultParagraph() const;
	ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //�������� ��������
	QVector<ISMetaParagraph*> GetParagraphs(); //�������� ������ ����������

private:
	ISParagraphEntity();

	QVector<ISMetaParagraph*> Paragraphs;
	ISUuid DefaultParagraph;
};
//-----------------------------------------------------------------------------
