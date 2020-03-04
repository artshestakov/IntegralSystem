#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISNamespace
{
public:
	enum FieldType //Типы данных поля
	{
		FT_Unknown, //Неизвестный
		FT_Int, //Целое число
		FT_String, //Строка
		FT_Text, //Большая строка
		FT_UID, //Уникальный идентификатор
		FT_Bool, //Логический
		FT_Double, //Число с плавающей запятой
		FT_ByteArray, //Массив данных
		FT_Date, //Дата
		FT_Time, //Время
		FT_DateTime, //Дата и время
		FT_Inn, //Индивидуаьный номер налогоплательщика
		FT_Kpp, //Код причины постановки на учёт
		FT_Ogrn, //Основной государственный регистрационный номер
		FT_Okpo, //Общероссийский классификатор предприятий и организаций
		FT_Bik, //Банковский идентификационый номер
		FT_Icq,
		FT_Year, //Год
		FT_PhoneBase, //Номер телефона
		FT_Phone, //Номер телефона
		FT_IPAddress, //IP-адрес
		FT_Password, //Пароль
		FT_Image, //Изображение
		FT_Image64, //Изображение
		FT_Color, //Цвет
		FT_EMail, //Адрес электронной почты
		FT_Passport, //Паспорт
		FT_Volume, //Громкость
		FT_Birthday, //Дата рождения
		FT_Seconds, //Секунды
		FT_CallDetails, //Подробности звонка
		FT_Month, //Месяц
		FT_Login, //Логин пользователя системы
		FT_Percent, //Процент
		FT_Address, //Адрес
		FT_Url, //Ссылка
		FT_File, //Файл
	};

	enum ObjectFormType //Типы формы объекта
	{
		OFT_New, //Новый объект
		OFT_Copy, //Копия объекта
		OFT_Edit, //Изменение объекта
	};

	enum SelectListMode //Режим выбора формы списка
	{
		SLM_Single, //Выбор одной записи
		SLM_Multi, //Выбор нескольких записей
	};

	enum NotificationFormType //Типы формы нотификации
	{
		NFT_Create, //Создание объекта
		NFT_Edit, //Редактирование объекта
		NFT_CreateCopy, //Создание копии объекта
		NFT_Delete, //Удаление объекта
		NFT_Recovery, //Восстановление объекта
	};

	enum SearchOperatorType //Типы операторов поиска
	{
		SOT_Unknown, //Неизвестно
		SOT_Equally, //Равно
		SOT_NotEqually, //Не равно
		SOT_Begins, //Начинается
		SOT_Ends, //Заканчивается
		SOT_Contains, //Содержит
		SOT_Null, //Пусто
		SOT_NotNull, //Не пусто
		SOT_More, //Больше
		SOT_Less, //Меньше
		SOT_MoreOrEqual, //Больше или равно
		SOT_LessOrEqual, //Меньше или равно
		SOT_Range, //Диапазон
	};

	enum ExportType //Типы базового экспорта данных
	{
		ET_Unknown,
		ET_CSV,
		ET_HTML,
		ET_DBF,
		ET_XML,
	};

	enum MessageBoxButton
	{
		MBB_Unknown, //Неизвестно (НЕ ИСПОЛЬЗОВАТЬ ДАННЫЙ ФЛАГ)
	};

	enum DeleteRecoveryObject
	{
		DRO_Delete,
		DRO_Recovery,
	};

	enum ExitFormAction
	{
		EFA_Unknown,
		EFA_Exit,
		EFA_ChangeUser,
		EFA_Lock,
		EFA_Tray,
		EFT_Close,
	};

	enum ActionType
	{
		AT_Create,
		AT_CreateCopy,
		AT_Edit,
		AT_Delete,
		AT_DeleteCascade,
		AT_Update,
		AT_ShowDeleted,
		AT_Search,
		AT_SearchClear,
		AT_Export,
		AT_Print,
		AT_SystemInfo,
		AT_Share,
		AT_AttachTask,
		AT_Favorites,
		AT_Reference,
		AT_NavigationBegin,
		AT_NavigationPrevious,
		AT_NavigationNext,
		AT_NavigationLast,
	};

	enum ActionSpecialType
	{
		AST_SortDefault,
		AST_Note,
		AST_ResizeFromContent,
		AST_ResetWidthColumn,
		AST_CopyRecord,
	};

	enum ObjectActionType
	{
		OAT_Object, //Карточка объекта
		OAT_Escort, //Эскортная форма (список/виджет)
		OAT_Service, //Сервисный эскорт (протокол и т.д.)
		OAT_Other, //Прочее (например, меню)
	};

	enum AttachChatType
	{
		ACT_NoAttach,
		ACT_Image,
		ACT_File,
	};

	enum ReportType
	{
		RT_Unknown,
		RT_Html,
		RT_Word,
	};

	enum QueryModelType
	{
		QMT_Object,
		QMT_List,
	};

	enum FunctionNameFormat //Тип форматирования информации о функции
	{
		FNF_All, //Полный
		FNF_TypeAndFunction, //Тип и функция
		FNF_Type, //Тип
		FNF_Function, //Функция
	};

	enum PhoneIconType //Тип иконки
	{
		PIT_Standart, //Стандартная
		PIT_TRUE, //Корректный ввод
		PIT_FALSE, //Некорректный ввод
	};

	enum OSType //Тип операционной системы
	{
		OST_Unknown,
		OST_Windows,
		OST_Linux,
		OST_MACOS,
	};

	enum ConsoleArgumentType //Тип аргументов консоли
	{
		CAT_Unknown, //Неизвестно
		CAT_Interpreter, //Аргументы отсутствуют
		CAT_OneArgument, //Один аргумент
		CAT_Standart, //Стандартные два аргумента
	};

	enum PeriodType //Тип фильтрации по периоду
	{
		PT_UnknownDate,
		PT_CreationDate,
		PT_UpdationDate,
	};

	enum MoveWidgetDesktop //Тип перемещения виджета
	{
		MWD_LeftUp, //Левый верхний угол
		MWD_LeftBottom, //Левый нижний угол
		MWD_RightUp, //Правый верхний угол
		MWD_RightBottom, //Правый нижний угол
		MWD_Center, //Центр экрана
	};

	enum TelephonyForm
	{
		TF_Journal,
		TF_Summary,
		TF_Pattern,
		TF_Activity,
	};

	enum DebugMessageType //Тип отладочоного сообщения
	{
		DMT_Unknown, //Неизвестное
		DMT_Debug, //Отладочное
		DMT_Info, //Информационное
		DMT_Warning, //Предупреждение
		DMT_Error, //Ошибка
	};

	enum RangeDatePeriod //Тип периода дат
	{
		RDP_Unknown, //Не выбран
		RDP_Today, //Сегодня
		RDP_Yesterday, //Вчера
		RDP_CurrentMonth, //Текущий месяц
		RDP_LastMonth, //Прошлый месяц
		RDP_CurrentYear, //Текущий год
		RDP_LastYear, //Прошлый год
		RDP_Other, //Другой
	};

	enum SMSErrorType //Типы ошибок отправки смс-сообщений
	{
		SET_NoError = 0,
		SET_Paramters = 1,
		SET_Auth = 2,
		SET_FewMoney = 3,
		SET_LockIP = 4,
		SET_InvalidDateFormat = 5,
		SET_MessageBan = 6,
		SET_InvalidPhone = 7,
		SET_SendingCanNot = 8,
		SET_SystemBan = 9,
	};
};
//-----------------------------------------------------------------------------
Q_DECLARE_METATYPE(ISNamespace::FieldType);
Q_DECLARE_METATYPE(ISNamespace::ObjectFormType);
Q_DECLARE_METATYPE(ISNamespace::NotificationFormType);
Q_DECLARE_METATYPE(ISNamespace::SearchOperatorType);
Q_DECLARE_METATYPE(ISNamespace::ExportType);
Q_DECLARE_METATYPE(ISNamespace::MessageBoxButton);
Q_DECLARE_METATYPE(ISNamespace::DeleteRecoveryObject);
Q_DECLARE_METATYPE(ISNamespace::ExitFormAction);
Q_DECLARE_METATYPE(ISNamespace::ActionType);
Q_DECLARE_METATYPE(ISNamespace::ActionSpecialType);
Q_DECLARE_METATYPE(ISNamespace::ObjectActionType);
Q_DECLARE_METATYPE(ISNamespace::AttachChatType);
Q_DECLARE_METATYPE(ISNamespace::ReportType);
Q_DECLARE_METATYPE(ISNamespace::QueryModelType);
Q_DECLARE_METATYPE(ISNamespace::FunctionNameFormat);
Q_DECLARE_METATYPE(ISNamespace::PhoneIconType);
Q_DECLARE_METATYPE(ISNamespace::OSType);
Q_DECLARE_METATYPE(ISNamespace::ConsoleArgumentType);
Q_DECLARE_METATYPE(ISNamespace::PeriodType);
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::TelephonyForm);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::RangeDatePeriod);
Q_DECLARE_METATYPE(ISNamespace::SMSErrorType);
//-----------------------------------------------------------------------------
#endif
