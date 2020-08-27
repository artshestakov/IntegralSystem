#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISNamespace
{
public:
    enum FieldType //Типы данных поля
    {
        FT_Unknown, //Неизвестный
        FT_Int, //Целое число
		FT_IntArray, //Массив целых чисел
        FT_BigInt, //Большое целое число
        FT_String, //Строка
        FT_Text, //Большая строка
        FT_UID, //Уникальный идентификатор
        FT_Bool, //Логический
        FT_Double, //Число с плавающей запятой
		FT_OID, //Идентификатор объекта
        FT_ByteArray, //Массив данных
        FT_Date, //Дата
        FT_Time, //Время
        FT_DateTime, //Дата и время
        FT_Inn, //Индивидуаьный номер налогоплательщика
        FT_Kpp, //Код причины постановки на учёт
        FT_Ogrn, //Основной государственный регистрационный номер
        FT_Okpo, //Общероссийский классификатор предприятий и организаций
        FT_Bik, //Банковский идентификационый номер
        FT_Vin, //Идентификационный номер транспортного средства
        FT_Year, //Год
        FT_Phone, //Номер телефона
        FT_Password, //Пароль
        FT_Image, //Изображение
        FT_Color, //Цвет
        FT_EMail, //Адрес электронной почты
        FT_Volume, //Громкость
        FT_Birthday, //Дата рождения
        FT_Seconds, //Секунды
        FT_CallDetails, //Подробности звонка
        FT_Month, //Месяц
        FT_Url, //Ссылка
        FT_File, //Файл
		FT_Sex, //Пол
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

    enum ActionType
    {
        AT_Create,
        AT_CreateCopy,
        AT_Edit,
        AT_Delete,
        AT_DeleteCascade,
        AT_Update,
		AT_ShowActual,
        AT_ShowDeleted,
        AT_Search,
        AT_SearchClear,
        AT_Export,
        AT_Print,
        AT_SystemInfo,
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
    };

    enum ReportType
    {
        RT_Unknown,
        RT_Html,
    };

    enum QueryModelType
    {
        QMT_Object,
        QMT_List,
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

};
//-----------------------------------------------------------------------------
Q_DECLARE_METATYPE(ISNamespace::FieldType);
Q_DECLARE_METATYPE(ISNamespace::ObjectFormType);
Q_DECLARE_METATYPE(ISNamespace::SearchOperatorType);
Q_DECLARE_METATYPE(ISNamespace::ExportType);
Q_DECLARE_METATYPE(ISNamespace::ActionType);
Q_DECLARE_METATYPE(ISNamespace::ActionSpecialType);
Q_DECLARE_METATYPE(ISNamespace::ReportType);
Q_DECLARE_METATYPE(ISNamespace::QueryModelType);
Q_DECLARE_METATYPE(ISNamespace::PeriodType);
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::TelephonyForm);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::RangeDatePeriod);
//-----------------------------------------------------------------------------
#endif
