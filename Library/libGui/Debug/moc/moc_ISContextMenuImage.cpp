/****************************************************************************
** Meta object code from reading C++ file 'ISContextMenuImage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISContextMenuImage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISContextMenuImage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISContextMenuImage_t {
    QByteArrayData data[9];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISContextMenuImage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISContextMenuImage_t qt_meta_stringdata_ISContextMenuImage = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ISContextMenuImage"
QT_MOC_LITERAL(1, 19, 6), // "Select"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 3), // "Cut"
QT_MOC_LITERAL(4, 31, 4), // "Copy"
QT_MOC_LITERAL(5, 36, 5), // "Paste"
QT_MOC_LITERAL(6, 42, 13), // "PasteFromLink"
QT_MOC_LITERAL(7, 56, 4), // "Save"
QT_MOC_LITERAL(8, 61, 8) // "OpenView"

    },
    "ISContextMenuImage\0Select\0\0Cut\0Copy\0"
    "Paste\0PasteFromLink\0Save\0OpenView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISContextMenuImage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,
       6,    0,   53,    2, 0x06 /* Public */,
       7,    0,   54,    2, 0x06 /* Public */,
       8,    0,   55,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ISContextMenuImage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISContextMenuImage *_t = static_cast<ISContextMenuImage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Select(); break;
        case 1: _t->Cut(); break;
        case 2: _t->Copy(); break;
        case 3: _t->Paste(); break;
        case 4: _t->PasteFromLink(); break;
        case 5: _t->Save(); break;
        case 6: _t->OpenView(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::Select)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::Cut)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::Copy)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::Paste)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::PasteFromLink)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::Save)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ISContextMenuImage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISContextMenuImage::OpenView)) {
                *result = 6;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ISContextMenuImage::staticMetaObject = {
    { &QMenu::staticMetaObject, qt_meta_stringdata_ISContextMenuImage.data,
      qt_meta_data_ISContextMenuImage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISContextMenuImage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISContextMenuImage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISContextMenuImage.stringdata0))
        return static_cast<void*>(this);
    return QMenu::qt_metacast(_clname);
}

int ISContextMenuImage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ISContextMenuImage::Select()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ISContextMenuImage::Cut()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ISContextMenuImage::Copy()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ISContextMenuImage::Paste()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ISContextMenuImage::PasteFromLink()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ISContextMenuImage::Save()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ISContextMenuImage::OpenView()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
