/****************************************************************************
** Meta object code from reading C++ file 'ISListObjectForm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISListObjectForm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISListObjectForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISListObjectForm_t {
    QByteArrayData data[6];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISListObjectForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISListObjectForm_t qt_meta_stringdata_ISListObjectForm = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ISListObjectForm"
QT_MOC_LITERAL(1, 17, 0), // ""
QT_MOC_LITERAL(2, 18, 9), // "TableName"
QT_MOC_LITERAL(3, 28, 14), // "ParentObjectID"
QT_MOC_LITERAL(4, 43, 8), // "QWidget*"
QT_MOC_LITERAL(5, 52, 6) // "parent"

    },
    "ISListObjectForm\0\0TableName\0ParentObjectID\0"
    "QWidget*\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISListObjectForm[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       2,   26, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, QMetaType::QString, QMetaType::Int, 0x80000000 | 4,    2,    3,    5,
    0x80000000 | 1, QMetaType::QString, QMetaType::Int,    2,    3,

 // constructors: name, argc, parameters, tag, flags
       0,    3,   14,    1, 0x0e /* Public */,
       0,    2,   21,    1, 0x2e /* Public | MethodCloned */,

       0        // eod
};

void ISListObjectForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISListObjectForm *_r = new ISListObjectForm((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QWidget*(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { ISListObjectForm *_r = new ISListObjectForm((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISListObjectForm::staticMetaObject = {
    { &ISListBaseForm::staticMetaObject, qt_meta_stringdata_ISListObjectForm.data,
      qt_meta_data_ISListObjectForm,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISListObjectForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISListObjectForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISListObjectForm.stringdata0))
        return static_cast<void*>(this);
    return ISListBaseForm::qt_metacast(_clname);
}

int ISListObjectForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISListBaseForm::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
