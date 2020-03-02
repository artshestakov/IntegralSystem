#pragma once
#ifndef _ISDEFINESCORE_H_INCLUDED
#define _ISDEFINESCORE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
namespace ISDefines
{
	class LIBCORE_EXPORT Core
	{
	public:
		static void Init(bool IsGui); //�������������

		static bool IS_GUI; //���� ������� ������������ ����������
		static QString APPLICATION_NAME;
		static QString PATH_APPLICATION_DIR;
		static QString PATH_APPLICATION_FILE;
		static QString PATH_LOGS_DIR;
		static QString PATH_TEMP_DIR;
		static QString PATH_CRASH_DIR;
		static QString PATH_TRANSLATIONS_DIR;
		static QString PATH_CONFIG_FILE;
		static QString SYMBOL_CIRCLE; //������ "����"
		static QString SYMBOL_MULTIPLY; //������ "���������"
		static QString SYMBOL_DIVIDE; //������ "�������"
		static QString SYMBOL_OBJECT_CHANGED;
		static QString SYMBOL_SPACE_HIDE; //�������� ������ (�� �������� ����������� �������� �������)
		static QString SYMBOL_FIAS_SPLIT;
	};
}
//-----------------------------------------------------------------------------
#endif
