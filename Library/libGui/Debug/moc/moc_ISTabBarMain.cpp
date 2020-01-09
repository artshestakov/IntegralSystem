/****************************************************************************
** Meta object code from reading C++ file 'ISTabBarMain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISTabBarMain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISTabBarMain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISTabBarMain_t {
    QByteArrayData data[7];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISTabBarMain_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISTabBarMain_t qt_meta_stringdata_ISTabBarMain = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ISTabBarMain"
QT_MOC_LITERAL(1, 13, 16), // "MidButtonClicked"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 5), // "Index"
QT_MOC_LITERAL(4, 37, 15), // "DuplicateWindow"
QT_MOC_LITERAL(5, 53, 14), // "SeparateWindow"
QT_MOC_LITERAL(6, 68, 8) // "FixedTab"

    },
    "ISTabBarMain\0MidButtonClicked\0\0Index\0"
    "DuplicateWindow\0SeparateWindow\0FixedTab"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISTabBarMain[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       5,    1,   40,    2, 0x06 /* Public */,
       6,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void ISTabBarMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISTabBarMain *_t = static_cast<ISTabBarMain *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->MidButtonClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->DuplicateWindow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->SeparateWindow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->FixedTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISTabBarMain::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISTabBarMain::MidButtonClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISTabBarMain::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISTabBarMain::DuplicateWindow)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISTabBarMain::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISTabBarMain::SeparateWindow)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISTabBarMain::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISTabBarMain::FixedTab)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISTabBarMain::staticMetaObject = {
    { &QTabBar::staticMetaObject, qt_meta_stringdata_ISTabBarMain.data,
      qt_meta_data_ISTabBarMain,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISTabBarMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISTabBarMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISTabBarMain.stringdata0))
        return static_cast<void*>(this);
    return QTabBar::qt_metacast(_clname);
}

int ISTabBarMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ISTabBarMain::MidButtonClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ISTabBarMain::DuplicateWindow(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ISTabBarMain::SeparateWindow(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ISTabBarMain::FixedTab(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
