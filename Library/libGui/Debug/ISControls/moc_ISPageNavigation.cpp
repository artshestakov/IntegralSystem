/****************************************************************************
** Meta object code from reading C++ file 'ISPageNavigation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../ISControls/ISPageNavigation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISPageNavigation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISPageNavigation_t {
    QByteArrayData data[9];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISPageNavigation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISPageNavigation_t qt_meta_stringdata_ISPageNavigation = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ISPageNavigation"
QT_MOC_LITERAL(1, 17, 5), // "Begin"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "Previous"
QT_MOC_LITERAL(4, 33, 4), // "Next"
QT_MOC_LITERAL(5, 38, 3), // "End"
QT_MOC_LITERAL(6, 42, 12), // "OffsetSignal"
QT_MOC_LITERAL(7, 55, 6), // "offset"
QT_MOC_LITERAL(8, 62, 6) // "Update"

    },
    "ISPageNavigation\0Begin\0\0Previous\0Next\0"
    "End\0OffsetSignal\0offset\0Update"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISPageNavigation[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    0,   46,    2, 0x06 /* Public */,
       5,    0,   47,    2, 0x06 /* Public */,
       6,    1,   48,    2, 0x06 /* Public */,
       8,    0,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,

       0        // eod
};

void ISPageNavigation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISPageNavigation *_t = static_cast<ISPageNavigation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Begin(); break;
        case 1: _t->Previous(); break;
        case 2: _t->Next(); break;
        case 3: _t->End(); break;
        case 4: _t->OffsetSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->Update(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISPageNavigation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPageNavigation::Begin)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISPageNavigation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPageNavigation::Previous)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISPageNavigation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPageNavigation::Next)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISPageNavigation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPageNavigation::End)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ISPageNavigation::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPageNavigation::OffsetSignal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ISPageNavigation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPageNavigation::Update)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISPageNavigation::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ISPageNavigation.data,
      qt_meta_data_ISPageNavigation,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISPageNavigation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISPageNavigation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISPageNavigation.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ISPageNavigation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ISPageNavigation::Begin()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ISPageNavigation::Previous()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ISPageNavigation::Next()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ISPageNavigation::End()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ISPageNavigation::OffsetSignal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ISPageNavigation::Update()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
