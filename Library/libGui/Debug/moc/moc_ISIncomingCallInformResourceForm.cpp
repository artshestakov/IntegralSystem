/****************************************************************************
** Meta object code from reading C++ file 'ISIncomingCallInformResourceForm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISIncomingCallInformResourceForm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISIncomingCallInformResourceForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISIncomingCallInformResourceForm_t {
    QByteArrayData data[5];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISIncomingCallInformResourceForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISIncomingCallInformResourceForm_t qt_meta_stringdata_ISIncomingCallInformResourceForm = {
    {
QT_MOC_LITERAL(0, 0, 32), // "ISIncomingCallInformResourceForm"
QT_MOC_LITERAL(1, 33, 0), // ""
QT_MOC_LITERAL(2, 34, 7), // "payload"
QT_MOC_LITERAL(3, 42, 8), // "QWidget*"
QT_MOC_LITERAL(4, 51, 6) // "parent"

    },
    "ISIncomingCallInformResourceForm\0\0"
    "payload\0QWidget*\0parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISIncomingCallInformResourceForm[] = {

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

void ISIncomingCallInformResourceForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISIncomingCallInformResourceForm *_r = new ISIncomingCallInformResourceForm((*reinterpret_cast< const QVariantMap(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { ISIncomingCallInformResourceForm *_r = new ISIncomingCallInformResourceForm((*reinterpret_cast< const QVariantMap(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISIncomingCallInformResourceForm::staticMetaObject = {
    { &ISIncomingCallBaseForm::staticMetaObject, qt_meta_stringdata_ISIncomingCallInformResourceForm.data,
      qt_meta_data_ISIncomingCallInformResourceForm,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISIncomingCallInformResourceForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISIncomingCallInformResourceForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISIncomingCallInformResourceForm.stringdata0))
        return static_cast<void*>(this);
    return ISIncomingCallBaseForm::qt_metacast(_clname);
}

int ISIncomingCallInformResourceForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISIncomingCallBaseForm::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
