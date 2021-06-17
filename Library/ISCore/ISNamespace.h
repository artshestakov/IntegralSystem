#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum class ConfigurationType //Типы конфигураций
    {
        Unknown,
        Empty,
        OilSphere
    };

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

    enum class LogMessageType //Типы сообщений логгера
    {
        Unknown, //Неизвестное сообщение
        Assert, //Ассерт
        Critical, //Критическая ошибка
        Error, //Ошибка
        Warning, //Предупреждение
        Info, //Информация
        Debug, //Отладка
        Trace //Трассировка
    };

    enum class SortingOrder //Направление сортировки
    {
        Ascending,
        Descending
    };

    enum class DirFileSorting //Тип сортировки файлов
    {
        DoNotSort, //Не сортировать
        EditDate, //По дате изменения
        Size, //По размеру
        Name //По имени
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

    enum class SelectListMode //Режим выбора формы списка
    {
        Single, //Выбор одной записи
        Multi, //Выбор нескольких записей
    };
};
//-----------------------------------------------------------------------------
#endif
