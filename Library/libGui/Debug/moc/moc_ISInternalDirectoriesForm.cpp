/****************************************************************************
** Meta object code from reading C++ file 'ISInternalDirectoriesForm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISInternalDirectoriesForm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISInternalDirectoriesForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISInternalDirectoriesForm_t {
    QByteArrayData data[4];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISInternalDirectoriesForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISInternalDirectoriesForm_t qt_meta_stringdata_ISInternalDirectoriesForm = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ISInternalDirectoriesForm"
QT_MOC_LITERAL(1, 26, 0), // ""
QT_MOC_LITERAL(2, 27, 8), // "QWidget*"
QT_MOC_LITERAL(3, 36, 6) // "parent"

    },
    "ISInternalDirectoriesForm\0\0QWidget*\0"
    "parent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISInternalDirectoriesForm[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       2,   18, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, 0x80000000 | 2,    3,
    0x80000000 | 1,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   14,    1, 0x0e /* Public */,
       0,    0,   17,    1, 0x2e /* Public | MethodCloned */,

       0        // eod
};

void ISInternalDirectoriesForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { ISInternalDirectoriesForm *_r = new ISInternalDirectoriesForm((*reinterpret_cast< QWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { ISInternalDirectoriesForm *_r = new ISInternalDirectoriesForm();
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

QT_INIT_METAOBJECT const QMetaObject ISInternalDirectoriesForm::staticMetaObject = {
    { &ISInterfaceMetaForm::staticMetaObject, qt_meta_stringdata_ISInternalDirectoriesForm.data,
      qt_meta_data_ISInternalDirectoriesForm,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISInternalDirectoriesForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISInternalDirectoriesForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISInternalDirectoriesForm.stringdata0))
        return static_cast<void*>(this);
    return ISInterfaceMetaForm::qt_metacast(_clname);
}

int ISInternalDirectoriesForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISInterfaceMetaForm::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
