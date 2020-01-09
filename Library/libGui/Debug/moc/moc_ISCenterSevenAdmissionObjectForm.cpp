/****************************************************************************
** Meta object code from reading C++ file 'ISCenterSevenAdmissionObjectForm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISCenterSevenAdmissionObjectForm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISCenterSevenAdmissionObjectForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISCenterSevenAdmissionObjectForm_t {
    QByteArrayData data[9];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISCenterSevenAdmissionObjectForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISCenterSevenAdmissionObjectForm_t qt_meta_stringdata_ISCenterSevenAdmissionObjectForm = {
    {
QT_MOC_LITERAL(0, 0, 32), // "ISCenterSevenAdmissionObjectForm"
QT_MOC_LITERAL(1, 33, 0), // ""
QT_MOC_LITERAL(2, 34, 27), // "ISNamespace::ObjectFormType"
QT_MOC_LITERAL(3, 62, 9), // "form_type"
QT_MOC_LITERAL(4, 72, 16), // "PMetaClassTable*"
QT_MOC_LITERAL(5, 89, 10), // "meta_table"
QT_MOC_LITERAL(6, 100, 8), // "QWidget*"
QT_MOC_LITERAL(7, 109, 6), // "parent"
QT_MOC_LITERAL(8, 116, 9) // "object_id"

    },
    "ISCenterSevenAdmissionObjectForm\0\0"
    "ISNamespace::ObjectFormType\0form_type\0"
    "PMetaClassTable*\0meta_table\0QWidget*\0"
    "parent\0object_id"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISCenterSevenAdmissionObjectForm[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       2,   30, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, 0x80000000 | 2, 0x80000000 | 4, 0x80000000 | 6, QMetaType::Int,    3,    5,    7,    8,
    0x80000000 | 1, 0x80000000 | 2, 0x80000000 | 4, 0x80000000 | 6,    3,    5,    7,

 // constructors: name, argc, parameters, tag, flags
       0,    4,   14,    1, 0x0e /* Public */,
       0,    3,   23,    1, 0x2e /* Public | MethodCloned */,

       0        // eod
};

void ISCenterSevenAdmissionObjectForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISCenterSevenAdmissionObjectForm *_r = new ISCenterSevenAdmissionObjectForm((*reinterpret_cast< ISNamespace::ObjectFormType(*)>(_a[1])),(*reinterpret_cast< PMetaClassTable*(*)>(_a[2])),(*reinterpret_cast< QWidget*(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { ISCenterSevenAdmissionObjectForm *_r = new ISCenterSevenAdmissionObjectForm((*reinterpret_cast< ISNamespace::ObjectFormType(*)>(_a[1])),(*reinterpret_cast< PMetaClassTable*(*)>(_a[2])),(*reinterpret_cast< QWidget*(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISCenterSevenAdmissionObjectForm::staticMetaObject = {
    { &ISObjectFormBase::staticMetaObject, qt_meta_stringdata_ISCenterSevenAdmissionObjectForm.data,
      qt_meta_data_ISCenterSevenAdmissionObjectForm,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISCenterSevenAdmissionObjectForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISCenterSevenAdmissionObjectForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISCenterSevenAdmissionObjectForm.stringdata0))
        return static_cast<void*>(this);
    return ISObjectFormBase::qt_metacast(_clname);
}

int ISCenterSevenAdmissionObjectForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISObjectFormBase::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
