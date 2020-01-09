/****************************************************************************
** Meta object code from reading C++ file 'ISDoubleEdit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISDoubleEdit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISDoubleEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISDoubleEdit_t {
    QByteArrayData data[6];
    char stringdata0[52];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISDoubleEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISDoubleEdit_t qt_meta_stringdata_ISDoubleEdit = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ISDoubleEdit"
QT_MOC_LITERAL(1, 13, 11), // "SetReadOnly"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9), // "read_only"
QT_MOC_LITERAL(4, 36, 8), // "QWidget*"
QT_MOC_LITERAL(5, 45, 6) // "parent"

    },
    "ISDoubleEdit\0SetReadOnly\0\0read_only\0"
    "QWidget*\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISDoubleEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       1,   25, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,

 // constructors: parameters
    0x80000000 | 2, 0x80000000 | 4,    5,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   22,    2, 0x0e /* Public */,

       0        // eod
};

void ISDoubleEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISDoubleEdit *_r = new ISDoubleEdit((*reinterpret_cast< QWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        ISDoubleEdit *_t = static_cast<ISDoubleEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SetReadOnly((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISDoubleEdit::staticMetaObject = {
    { &ISFieldEditBase::staticMetaObject, qt_meta_stringdata_ISDoubleEdit.data,
      qt_meta_data_ISDoubleEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISDoubleEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISDoubleEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISDoubleEdit.stringdata0))
        return static_cast<void*>(this);
    return ISFieldEditBase::qt_metacast(_clname);
}

int ISDoubleEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISFieldEditBase::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
