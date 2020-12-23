#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
namespace ISNamespace
{
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
		FT_Money, //Деньги
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
        FT_Month, //Месяц
        FT_Url, //Ссылка
        FT_File, //Файл
		FT_TaskImportant, //Важность задачи
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
        AT_Update,
        AT_Search,
        AT_SearchClear,
        AT_Export,
        AT_Print,
        AT_Favorites,
        AT_Reference,
        AT_NavigationBegin,
        AT_NavigationPrevious,
        AT_NavigationNext,
        AT_NavigationLast,
    };

    enum ActionSpecialType
    {
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

    enum MoveWidgetDesktop //Тип перемещения виджета
    {
        MWD_LeftUp, //Левый верхний угол
        MWD_LeftBottom, //Левый нижний угол
        MWD_RightUp, //Правый верхний угол
        MWD_RightBottom, //Правый нижний угол
        MWD_Center, //Центр экрана
    };

    enum DebugMessageType //Тип отладочоного сообщения
    {
        DMT_Unknown, //Неизвестное
        DMT_Debug, //Отладочное
        DMT_Info, //Информационное
        DMT_Warning, //Предупреждение
        DMT_Error, //Ошибка
    };

	enum ApiMessageType //Тип API-сообщения
	{
		AMT_Unknown, //Неизвестное
		AMT_Auth, //Авторизация
		AMT_Sleep, //Ожидание
		AMT_GetMetaData, //Получить мета-данные
		AMT_GetLastClient, //Получить последнюю версию клиента
		AMT_UserPasswordExist, //Проверить существование пароля
		AMT_UserPasswordCreate, //Создание пароля пользователю
		AMT_UserPasswordEdit, //Изменение пароля пользователя
		AMT_UserPasswordReset, //Сброс пароля парользователя
		AMT_UserSettingsReset, //Сброс пользовательских настроек
		AMT_GetRecordCall, //Получить запись разговора
		AMT_GetClients, //Получить список подключенных клиентов
		AMT_RecordDelete, //Удаление записи
		AMT_DiscussionAdd, //Добавить обсуждение
		AMT_DiscussionEdit, //Изменить обсуждение
		AMT_DiscussionCopy, //Скопировать обсуждение
		AMT_GetTableData, //Получить данные таблицы
		AMT_NoteRecordGet, //Получить примечание записи
		AMT_NoteRecordSet, //Изменить примечание записи
		AMT_FileStorageAdd, //Добавить файл в хранилище
		AMT_FileStorageCopy, //Скопировать файл в хранилище
		AMT_FileStorageGet, //Получить файл из хранилища
		AMT_SearchTaskText, //Поиск задач по тексту
		AMT_SearchTaskID, //Поиск задач по номеру
		AMT_SearchFullText, //Полнотекстовый поиск
		AMT_GetCalendarEvents, //Получить события календаря
		AMT_CalendarDelete, //Удаление события календаря
		AMT_GetInternalLists, //Получить список внутренних справочников
		AMT_SaveMetaData, //Сохранение мета-данных
		AMT_GetGroupRights, //Получить права группы
		AMT_GroupRightSubSystemAdd, //Добавить группе право на подсистему
		AMT_GroupRightSubSystemDelete, //Удалить у группы право на подсистему
		AMT_GroupRightTableAdd, //Добавить группе право на таблицу
		AMT_GroupRightTableDelete, //Удалить у группы право на таблицу
		AMT_GroupRightSpecialAdd, //Добавить спец. право группе
		AMT_GroupRightSpecialDelete, //Удалить спец. право у группы
		AMT_GetRecordValue, //Получить значение поля записи
		AMT_RecordFavoriteAdd, //Добавить запись в избранное
		AMT_RecordFavoriteDelete, //Удалить запись из избранного
		AMT_GetFavoriteNames, //Получить имена избранных записей
		AMT_FavoritesDelete, //Удалить все избранные записи пользователя
		AMT_CalendarClose, //Закрытие события календаря
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
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::ApiMessageType);
//-----------------------------------------------------------------------------
#endif
