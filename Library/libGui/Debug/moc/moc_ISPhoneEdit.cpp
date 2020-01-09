/****************************************************************************
** Meta object code from reading C++ file 'ISPhoneEdit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISPhoneEdit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISPhoneEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISPhoneEdit_t {
    QByteArrayData data[5];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISPhoneEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISPhoneEdit_t qt_meta_stringdata_ISPhoneEdit = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ISPhoneEdit"
QT_MOC_LITERAL(1, 12, 6), // "Called"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 8), // "QWidget*"
QT_MOC_LITERAL(4, 29, 6) // "parent"

    },
    "ISPhoneEdit\0Called\0\0QWidget*\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISPhoneEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       1,   23, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // constructors: parameters
    0x80000000 | 2, 0x80000000 | 3,    4,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   20,    2, 0x0e /* Public */,

       0        // eod
};

void ISPhoneEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISPhoneEdit *_r = new ISPhoneEdit((*reinterpret_cast< QWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        ISPhoneEdit *_t = static_cast<ISPhoneEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Called(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISPhoneEdit::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPhoneEdit::Called)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISPhoneEdit::staticMetaObject = {
    { &ISPhoneBaseEdit::staticMetaObject, qt_meta_stringdata_ISPhoneEdit.data,
      qt_meta_data_ISPhoneEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISPhoneEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISPhoneEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISPhoneEdit.stringdata0))
        return static_cast<void*>(this);
    return ISPhoneBaseEdit::qt_metacast(_clname);
}

int ISPhoneEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISPhoneBaseEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ISPhoneEdit::Called()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
