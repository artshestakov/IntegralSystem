/****************************************************************************
** Meta object code from reading C++ file 'ISClientsSqlModel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISClientsSqlModel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISClientsSqlModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISClientsSqlModel_t {
    QByteArrayData data[5];
    char stringdata0[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISClientsSqlModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISClientsSqlModel_t qt_meta_stringdata_ISClientsSqlModel = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ISClientsSqlModel"
QT_MOC_LITERAL(1, 18, 0), // ""
QT_MOC_LITERAL(2, 19, 16), // "PMetaClassTable*"
QT_MOC_LITERAL(3, 36, 10), // "meta_table"
QT_MOC_LITERAL(4, 47, 6) // "parent"

    },
    "ISClientsSqlModel\0\0PMetaClassTable*\0"
    "meta_table\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISClientsSqlModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       2,   22, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, 0x80000000 | 2, QMetaType::QObjectStar,    3,    4,
    0x80000000 | 1, 0x80000000 | 2,    3,

 // constructors: name, argc, parameters, tag, flags
       0,    2,   14,    1, 0x0e /* Public */,
       0,    1,   19,    1, 0x2e /* Public | MethodCloned */,

       0        // eod
};

void ISClientsSqlModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISClientsSqlModel *_r = new ISClientsSqlModel((*reinterpret_cast< PMetaClassTable*(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { ISClientsSqlModel *_r = new ISClientsSqlModel((*reinterpret_cast< PMetaClassTable*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISClientsSqlModel::staticMetaObject = {
    { &ISSqlModelCore::staticMetaObject, qt_meta_stringdata_ISClientsSqlModel.data,
      qt_meta_data_ISClientsSqlModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISClientsSqlModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISClientsSqlModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISClientsSqlModel.stringdata0))
        return static_cast<void*>(this);
    return ISSqlModelCore::qt_metacast(_clname);
}

int ISClientsSqlModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISSqlModelCore::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
