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

    enum class LogMessageType //Типы сообщений логгера
    {
		Unknown, //Неизвестное сообщение
		Debug, //Отладка
		Info, //Информация
		Warning, //Предупреждение
		Error, //Ошибка
		Critical, //Критическая ошибка
		Trace, //Трассировка
		Assert, //Ассерт
		QT //Сообщения от Qt
    };

	enum class VariantType //Типы данных
	{
		Invalid, //Невалидное значение
		Bool, //Логический тип
		Short, //Числовой тип (16 бит)
		Int, //Числовой тип (32 бита)
		Int64, //Числовой тип (64 бита)
		UInt, //Беззнаковый числовой тип
		Double, //Дробный тип
		Float, //Дробный тип
		Char, //Символьный тип
		String //Строковый тип
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
Q_DECLARE_METATYPE(ISNamespace::LogMessageType);
Q_DECLARE_METATYPE(ISNamespace::VariantType);
//-----------------------------------------------------------------------------
#endif
