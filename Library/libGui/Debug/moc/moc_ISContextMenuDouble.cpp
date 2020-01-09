/****************************************************************************
** Meta object code from reading C++ file 'ISContextMenuDouble.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISContextMenuDouble.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISContextMenuDouble.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISContextMenuDouble_t {
    QByteArrayData data[4];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISContextMenuDouble_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISContextMenuDouble_t qt_meta_stringdata_ISContextMenuDouble = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ISContextMenuDouble"
QT_MOC_LITERAL(1, 20, 6), // "StepUp"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 8) // "StepDown"

    },
    "ISContextMenuDouble\0StepUp\0\0StepDown"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISContextMenuDouble[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ISContextMenuDouble::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISContextMenuDouble *_t = static_cast<ISContextMenuDouble *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->StepUp(); break;
        case 1: _t->StepDown(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISContextMenuDouble::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuDouble::StepUp)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISContextMenuDouble::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuDouble::StepDown)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ISContextMenuDouble::staticMetaObject = {
    { &ISContextMenuBase::staticMetaObject, qt_meta_stringdata_ISContextMenuDouble.data,
      qt_meta_data_ISContextMenuDouble,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISContextMenuDouble::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISContextMenuDouble::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISContextMenuDouble.stringdata0))
        return static_cast<void*>(this);
    return ISContextMenuBase::qt_metacast(_clname);
}

int ISContextMenuDouble::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISContextMenuBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ISContextMenuDouble::StepUp()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ISContextMenuDouble::StepDown()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
