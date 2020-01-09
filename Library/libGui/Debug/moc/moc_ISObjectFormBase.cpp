/****************************************************************************
** Meta object code from reading C++ file 'ISObjectFormBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISObjectFormBase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISObjectFormBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISObjectFormBase_t {
    QByteArrayData data[13];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISObjectFormBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISObjectFormBase_t qt_meta_stringdata_ISObjectFormBase = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ISObjectFormBase"
QT_MOC_LITERAL(1, 17, 11), // "SavedObject"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 8), // "ObjectID"
QT_MOC_LITERAL(4, 39, 10), // "UpdateList"
QT_MOC_LITERAL(5, 50, 16), // "CurrentObjectTab"
QT_MOC_LITERAL(6, 67, 5), // "Close"
QT_MOC_LITERAL(7, 73, 8), // "CloseTab"
QT_MOC_LITERAL(8, 82, 5), // "Index"
QT_MOC_LITERAL(9, 88, 13), // "SaveAndCreate"
QT_MOC_LITERAL(10, 102, 16), // "PMetaClassTable*"
QT_MOC_LITERAL(11, 119, 9), // "MetaTable"
QT_MOC_LITERAL(12, 129, 16) // "ModificationFlag"

    },
    "ISObjectFormBase\0SavedObject\0\0ObjectID\0"
    "UpdateList\0CurrentObjectTab\0Close\0"
    "CloseTab\0Index\0SaveAndCreate\0"
    "PMetaClassTable*\0MetaTable\0ModificationFlag"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISObjectFormBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   56, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    0,   48,    2, 0x06 /* Public */,
       6,    0,   49,    2, 0x06 /* Public */,
       7,    1,   50,    2, 0x06 /* Public */,
       9,    1,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, 0x80000000 | 10,   11,

 // properties: name, type, flags
      12, QMetaType::Bool, 0x00095003,

       0        // eod
};

void ISObjectFormBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISObjectFormBase *_t = static_cast<ISObjectFormBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SavedObject((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->UpdateList(); break;
        case 2: _t->CurrentObjectTab(); break;
        case 3: _t->Close(); break;
        case 4: _t->CloseTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->SaveAndCreate((*reinterpret_cast< PMetaClassTable*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PMetaClassTable* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISObjectFormBase::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISObjectFormBase::SavedObject)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISObjectFormBase::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISObjectFormBase::UpdateList)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISObjectFormBase::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISObjectFormBase::CurrentObjectTab)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISObjectFormBase::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISObjectFormBase::Close)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ISObjectFormBase::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISObjectFormBase::CloseTab)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ISObjectFormBase::*)(PMetaClassTable * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISObjectFormBase::SaveAndCreate)) {
                *result = 5;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ISObjectFormBase *_t = static_cast<ISObjectFormBase *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->GetModificationFlag(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ISObjectFormBase *_t = static_cast<ISObjectFormBase *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->SetModificationFlag(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ISObjectFormBase::staticMetaObject = {
    { &ISInterfaceForm::staticMetaObject, qt_meta_stringdata_ISObjectFormBase.data,
      qt_meta_data_ISObjectFormBase,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISObjectFormBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISObjectFormBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISObjectFormBase.stringdata0))
        return static_cast<void*>(this);
    return ISInterfaceForm::qt_metacast(_clname);
}

int ISObjectFormBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ISInterfaceForm::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ISObjectFormBase::SavedObject(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ISObjectFormBase::UpdateList()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ISObjectFormBase::CurrentObjectTab()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ISObjectFormBase::Close()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ISObjectFormBase::CloseTab(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ISObjectFormBase::SaveAndCreate(PMetaClassTable * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
