#pragma once
#ifndef _ISUSBSEARCHER_H_INCLUDED
#define _ISUSBSEARCHER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
#include <windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
//-----------------------------------------------------------------------------
bool FindDevice(std::string &ErrorString)
{
	//Получаем объект классов
	HDEVINFO DeviceInfo = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (DeviceInfo == INVALID_HANDLE_VALUE) //Ошибка - выходим
	{
		ErrorString = "Error SetupDiGetClassDevs()";
		return false;
	}

	//Обходим устройства пока не дойдём до конца
	for (unsigned i = 0; ; ++i)
	{
		SP_DEVINFO_DATA DeviceInfoData = { 0 };
		DeviceInfoData.cbSize = sizeof(DeviceInfoData);
		if (!SetupDiEnumDeviceInfo(DeviceInfo, i, &DeviceInfoData)) //Дошли до конца - выходим из цикла
		{
			break;
		}
		
		//Если тип устройства не USB-носитель - идём дальше
		char Buffer[39] = { 0 };
		sprintf(Buffer, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
			DeviceInfoData.ClassGuid.Data1, DeviceInfoData.ClassGuid.Data2, DeviceInfoData.ClassGuid.Data3,
			DeviceInfoData.ClassGuid.Data4[0], DeviceInfoData.ClassGuid.Data4[1], DeviceInfoData.ClassGuid.Data4[2], DeviceInfoData.ClassGuid.Data4[3],
			DeviceInfoData.ClassGuid.Data4[4], DeviceInfoData.ClassGuid.Data4[5], DeviceInfoData.ClassGuid.Data4[6], DeviceInfoData.ClassGuid.Data4[7]);
		if (strcmp(Buffer, "{36FC9E60-C465-11CF-8056-444553540000}") != 0)
		{
			continue;
		}

		char DeviceID[MAX_DEVICE_ID_LEN];
		unsigned int Result = CM_Get_Device_ID(DeviceInfoData.DevInst, DeviceID, MAX_PATH, 0);
		if (Result != CR_SUCCESS)
		{
			ErrorString = "Error CM_Get_Device_ID(): " + std::to_string(Result);
			return false;
		}
		printf("%s", DeviceID); //Печатаем

		//Получаем описание
		unsigned long Size = 0,
			PropertyRegDataType = 0;
		unsigned char Description[1024] = { 0 };
		if (SetupDiGetDeviceRegistryProperty(DeviceInfo, &DeviceInfoData, SPDRP_DEVICEDESC,
			&PropertyRegDataType, Description, sizeof(Description), &Size)) //Все ок - печатаем
		{
			printf(": %s", Description);
		}
		printf("\n"); //Новая строка
	}
	return true;
}
//-----------------------------------------------------------------------------
#endif
