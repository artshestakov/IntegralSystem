/****************************************************************************
** Meta object code from reading C++ file 'ISHighwayIncomingCallForm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISHighwayIncomingCallForm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISHighwayIncomingCallForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISHighwayIncomingCallForm_t {
    QByteArrayData data[5];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISHighwayIncomingCallForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISHighwayIncomingCallForm_t qt_meta_stringdata_ISHighwayIncomingCallForm = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ISHighwayIncomingCallForm"
QT_MOC_LITERAL(1, 26, 0), // ""
QT_MOC_LITERAL(2, 27, 7), // "payload"
QT_MOC_LITERAL(3, 35, 8), // "QWidget*"
QT_MOC_LITERAL(4, 44, 6) // "parent"

    },
    "ISHighwayIncomingCallForm\0\0payload\0"
    "QWidget*\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISHighwayIncomingCallForm[] = {

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
    0x80000000 | 1, QMetaType::QVariantMap, 0x80000000 | 3,    2,    4,
    0x80000000 | 1, QMetaType::QVariantMap,    2,

 // constructors: name, argc, parameters, tag, flags
       0,    2,   14,    1, 0x0e /* Public */,
       0,    1,   19,    1, 0x2e /* Public | MethodCloned */,

       0        // eod
};

void ISHighwayIncomingCallForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISHighwayIncomingCallForm *_r = new ISHighwayIncomingCallForm((*reinterpret_cast< const QVariantMap(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { ISHighwayIncomingCallForm *_r = new ISHighwayIncomingCallForm((*reinterpret_cast< const QVariantMap(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISHighwayIncomingCallForm::staticMetaObject = {
    { &ISIncomingCallBaseForm::staticMetaObject, qt_meta_stringdata_ISHighwayIncomingCallForm.data,
      qt_meta_data_ISHighwayIncomingCallForm,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISHighwayIncomingCallForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISHighwayIncomingCallForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISHighwayIncomingCallForm.stringdata0))
        return static_cast<void*>(this);
    return ISIncomingCallBaseForm::qt_metacast(_clname);
}

int ISHighwayIncomingCallForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISIncomingCallBaseForm::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
