#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum class FieldType //Типы данных поля
    {
        Unknown, //Неизвестный
		ID, //Идентификатор записи
        Int, //Целое число
		IntArray, //Массив целых чисел
        BigInt, //Большое целое число
        String, //Строка
        Text, //Большая строка
        UID, //Уникальный идентификатор
        Bool, //Логический
        Double, //Число с плавающей запятой
		Money, //Деньги
        ByteArray, //Массив данных
        Date, //Дата
        Time, //Время
        DateTime, //Дата и время
        Inn, //Индивидуаьный номер налогоплательщика
        Kpp, //Код причины постановки на учёт
        Ogrn, //Основной государственный регистрационный номер
        Okpo, //Общероссийский классификатор предприятий и организаций
        Bik, //Банковский идентификационый номер
        Vin, //Идентификационный номер транспортного средства
        Year, //Год
        Phone, //Номер телефона
        Password, //Пароль
        Image, //Изображение
        Color, //Цвет
        EMail, //Адрес электронной почты
        Volume, //Громкость
        Birthday, //Дата рождения
        Seconds, //Секунды
        Month, //Месяц
        Url, //Ссылка
        File, //Файл
		TaskImportant, //Важность задачи
		ProtocolDT, //Дата события в протоколе
    };

    enum class ObjectFormType //Типы формы объекта
    {
        New, //Новый объект
        Copy, //Копия объекта
        Edit, //Изменение объекта
    };

    enum class SelectListMode //Режим выбора формы списка
    {
        Single, //Выбор одной записи
        Multi, //Выбор нескольких записей
    };

    enum class SearchType //Типы операторов поиска
    {
        Unknown, //Неизвестно
        Equally, //Равно
        NotEqually, //Не равно
        Begins, //Начинается
        Ends, //Заканчивается
        Contains, //Содержит
        More, //Больше
        Less, //Меньше
        MoreOrEqual, //Больше или равно
        LessOrEqual, //Меньше или равно
    };

    enum class ExportType //Типы базового экспорта данных
    {
        Unknown,
        CSV,
        HTML,
        XML,
		JSON,
    };

    enum class ActionType
    {
        Create,
        CreateCopy,
        Edit,
        Delete,
        Update,
        Search,
        SearchClear,
        Export,
        Favorites,
        Reference,
    };

    enum class ActionSpecialType
    {
		RecordInfo,
		Favorite,
        Note,
    };

    enum class MoveWidgetDesktop //Тип перемещения виджета
    {
        LeftUp, //Левый верхний угол
        LeftBottom, //Левый нижний угол
        RightUp, //Правый верхний угол
        RightBottom, //Правый нижний угол
        Center, //Центр экрана
    };

    enum class DebugMessageType //Тип отладочоного сообщения
    {
        Unknown, //Неизвестное
        Debug, //Отладочное
        Info, //Информационное
        Warning, //Предупреждение
        Error, //Ошибка
    };

    enum class ApiMessageType //Тип API-сообщения
	{
		Unknown, //Неизвестное
		Auth, //Авторизация
		Sleep, //Ожидание
		GetMetaData, //Получить мета-данные
		GetLastClient, //Получить последнюю версию клиента
		UserPasswordExist, //Проверить существование пароля
		UserPasswordCreate, //Создание пароля пользователю
		UserPasswordEdit, //Изменение пароля пользователя
		UserPasswordReset, //Сброс пароля парользователя
		UserSettingsReset, //Сброс пользовательских настроек
		UserDeviceAdd, //Привязка устройка к учётной записи пользователя
		UserDeviceDelete, //Отвязка устройка от учётной записи пользователя
		GetRecordCall, //Получить запись разговора
		GetClients, //Получить список подключенных клиентов
		RecordAdd, //Добавление записи
		RecordEdit, //Изменение записи
		RecordDelete, //Удаление записи
		RecordGet, //Получить запись
		RecordGetInfo, //Получить информацию о записи
		DiscussionAdd, //Добавить обсуждение
		DiscussionEdit, //Изменить обсуждение
		DiscussionCopy, //Скопировать обсуждение
		GetTableData, //Получить данные таблицы
		GetTableQuery, //Получить данные по запросу
		NoteRecordGet, //Получить примечание записи
		NoteRecordSet, //Изменить примечание записи
		FileStorageAdd, //Добавить файл в хранилище
		FileStorageCopy, //Скопировать файл в хранилище
		FileStorageGet, //Получить файл из хранилища
		SearchTaskText, //Поиск задач по тексту
		SearchTaskID, //Поиск задач по номеру
		SearchFullText, //Полнотекстовый поиск
		GetCalendarEvents, //Получить события календаря
		CalendarDelete, //Удаление события календаря
		GetInternalLists, //Получить список внутренних справочников
		SaveMetaData, //Сохранение мета-данных
		GetGroupRights, //Получить права группы
		GroupRightSubSystemAdd, //Добавить группе право на подсистему
		GroupRightSubSystemDelete, //Удалить у группы право на подсистему
		GroupRightTableAdd, //Добавить группе право на таблицу
		GroupRightTableDelete, //Удалить у группы право на таблицу
		GroupRightSpecialAdd, //Добавить спец. право группе
		GroupRightSpecialDelete, //Удалить спец. право у группы
		GetRecordValue, //Получить значение поля записи
		RecordFavoriteAdd, //Добавить запись в избранное
		RecordFavoriteDelete, //Удалить запись из избранного
		GetFavoriteNames, //Получить имена избранных записей
		FavoritesDelete, //Удалить все избранные записи пользователя
		LogGetStructure, //Получить структуру лог-файлов
		LogGetContent, //Получить содержимое лог-файла
		CalendarClose, //Закрытие события календаря
		GetHistoryList, //Получить список открытых записей
		TaskCommentAdd, //Добавить комментарий к задаче
		GetForeignList, //Получить список значений по внешнему ключу
		GetServerInfo, //Получить информацию о сервере
		OrganizationFromINN, //Поиск организации по ИНН
		GetStatement, //Получить статистику запросов
		PeriodContains, //Проверка наличия текущего периода
		GetStockList, //Получить список складов
		StatementAdd, //Добавить выгрузку в ведомости азс
		GetGasStation, //Заполнить на основании
		GetDebtImplementation, //Получить загрузки и выгрузки
		GetDebtCounterparty, //Получить долги контргаента
		GetUserConsumption, //Получить расходы пользователей
	};

    enum class LogMessageType //Типы сообщений логгера
    {
		Unknown, //Неизвестное сообщение
		Debug, //Отладка
		Info, //Информация
		Warning, //Предупреждение
		Error, //Ошибка
		Critical, //Критическая ошибка
		Trace, //Трассировка
		Assert //Ассерт
    };
};
//-----------------------------------------------------------------------------
Q_DECLARE_METATYPE(ISNamespace::FieldType);
Q_DECLARE_METATYPE(ISNamespace::ObjectFormType);
Q_DECLARE_METATYPE(ISNamespace::SearchType);
Q_DECLARE_METATYPE(ISNamespace::ExportType);
Q_DECLARE_METATYPE(ISNamespace::ActionType);
Q_DECLARE_METATYPE(ISNamespace::ActionSpecialType);
Q_DECLARE_METATYPE(ISNamespace::MoveWidgetDesktop);
Q_DECLARE_METATYPE(ISNamespace::DebugMessageType);
Q_DECLARE_METATYPE(ISNamespace::ApiMessageType);
Q_DECLARE_METATYPE(ISNamespace::LogMessageType);
//-----------------------------------------------------------------------------
#endif
