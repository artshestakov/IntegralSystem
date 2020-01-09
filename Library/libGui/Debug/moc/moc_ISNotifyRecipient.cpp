/****************************************************************************
** Meta object code from reading C++ file 'ISNotifyRecipient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISNotifyRecipient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISNotifyRecipient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISNotifyRecipient_t {
    QByteArrayData data[12];
    char stringdata0[153];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISNotifyRecipient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISNotifyRecipient_t qt_meta_stringdata_ISNotifyRecipient = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ISNotifyRecipient"
QT_MOC_LITERAL(1, 18, 11), // "ChatMessage"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 10), // "VariantMap"
QT_MOC_LITERAL(4, 42, 13), // "TermianteUser"
QT_MOC_LITERAL(5, 56, 13), // "UpdateAviable"
QT_MOC_LITERAL(6, 70, 12), // "IncomingCall"
QT_MOC_LITERAL(7, 83, 13), // "CalendarEvent"
QT_MOC_LITERAL(8, 97, 13), // "GetScreenshot"
QT_MOC_LITERAL(9, 111, 17), // "ScreenshotCreated"
QT_MOC_LITERAL(10, 129, 16), // "AlreadyConnected"
QT_MOC_LITERAL(11, 146, 6) // "Notify"

    },
    "ISNotifyRecipient\0ChatMessage\0\0"
    "VariantMap\0TermianteUser\0UpdateAviable\0"
    "IncomingCall\0CalendarEvent\0GetScreenshot\0"
    "ScreenshotCreated\0AlreadyConnected\0"
    "Notify"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISNotifyRecipient[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    1,   67,    2, 0x06 /* Public */,
       5,    1,   70,    2, 0x06 /* Public */,
       6,    1,   73,    2, 0x06 /* Public */,
       7,    1,   76,    2, 0x06 /* Public */,
       8,    1,   79,    2, 0x06 /* Public */,
       9,    1,   82,    2, 0x06 /* Public */,
      10,    1,   85,    2, 0x06 /* Public */,
      11,    1,   88,    2, 0x06 /* Public */,
      11,    0,   91,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void, QMetaType::QVariantMap,    3,
    QMetaType::Void,

       0        // eod
};

void ISNotifyRecipient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISNotifyRecipient *_t = static_cast<ISNotifyRecipient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ChatMessage((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 1: _t->TermianteUser((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 2: _t->UpdateAviable((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 3: _t->IncomingCall((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 4: _t->CalendarEvent((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 5: _t->GetScreenshot((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 6: _t->ScreenshotCreated((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 7: _t->AlreadyConnected((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 8: _t->Notify((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 9: _t->Notify(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::ChatMessage)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::TermianteUser)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::UpdateAviable)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::IncomingCall)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::CalendarEvent)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::GetScreenshot)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::ScreenshotCreated)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::AlreadyConnected)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::Notify)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ISNotifyRecipient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISNotifyRecipient::Notify)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISNotifyRecipient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ISNotifyRecipient.data,
      qt_meta_data_ISNotifyRecipient,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISNotifyRecipient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISNotifyRecipient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISNotifyRecipient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ISNotifyRecipient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ISNotifyRecipient::ChatMessage(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ISNotifyRecipient::TermianteUser(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ISNotifyRecipient::UpdateAviable(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ISNotifyRecipient::IncomingCall(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ISNotifyRecipient::CalendarEvent(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ISNotifyRecipient::GetScreenshot(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ISNotifyRecipient::ScreenshotCreated(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ISNotifyRecipient::AlreadyConnected(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ISNotifyRecipient::Notify(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ISNotifyRecipient::Notify()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
