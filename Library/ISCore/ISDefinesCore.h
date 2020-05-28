#pragma once
#ifndef _ISDEFINESCORE_H_INCLUDED
#define _ISDEFINESCORE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
namespace ISDefines
{
	class ISCORE_EXPORT Core
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
        static QString PATH_PID_FILE;
		static QString PATH_LAST_DIRECTORY; //��������� �������������� ���� � ������ ISFileDialog
		static QString SYMBOL_CIRCLE; //������ "����"
		static QString SYMBOL_OBJECT_CHANGED;
		static QString SYMBOL_SPACE_HIDE; //�������� ������ (�� �������� ����������� �������� �������)
		static QString SYMBOL_FIAS_SPLIT;
	};
}
//-----------------------------------------------------------------------------
#endif
