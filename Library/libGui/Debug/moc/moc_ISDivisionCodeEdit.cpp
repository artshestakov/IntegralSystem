/****************************************************************************
** Meta object code from reading C++ file 'ISDivisionCodeEdit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISDivisionCodeEdit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISDivisionCodeEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISDivisionCodeEdit_t {
    QByteArrayData data[4];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISDivisionCodeEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISDivisionCodeEdit_t qt_meta_stringdata_ISDivisionCodeEdit = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ISDivisionCodeEdit"
QT_MOC_LITERAL(1, 19, 0), // ""
QT_MOC_LITERAL(2, 20, 8), // "QWidget*"
QT_MOC_LITERAL(3, 29, 6) // "parent"

    },
    "ISDivisionCodeEdit\0\0QWidget*\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISDivisionCodeEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       1,   17, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, 0x80000000 | 2,    3,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   14,    1, 0x0e /* Public */,

       0        // eod
};

void ISDivisionCodeEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISDivisionCodeEdit *_r = new ISDivisionCodeEdit((*reinterpret_cast< QWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISDivisionCodeEdit::staticMetaObject = {
    { &ISLineEdit::staticMetaObject, qt_meta_stringdata_ISDivisionCodeEdit.data,
      qt_meta_data_ISDivisionCodeEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISDivisionCodeEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISDivisionCodeEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISDivisionCodeEdit.stringdata0))
        return static_cast<void*>(this);
    return ISLineEdit::qt_metacast(_clname);
}

int ISDivisionCodeEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISLineEdit::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
