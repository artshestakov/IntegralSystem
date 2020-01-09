/****************************************************************************
** Meta object code from reading C++ file 'ISQLineEdit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISQLineEdit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISQLineEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISQLineEdit_t {
    QByteArrayData data[13];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISQLineEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISQLineEdit_t qt_meta_stringdata_ISQLineEdit = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ISQLineEdit"
QT_MOC_LITERAL(1, 12, 13), // "FocusInSignal"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 14), // "FocusOutSignal"
QT_MOC_LITERAL(4, 42, 18), // "MouseDoubleClicked"
QT_MOC_LITERAL(5, 61, 16), // "WheelEventSignal"
QT_MOC_LITERAL(6, 78, 12), // "QWheelEvent*"
QT_MOC_LITERAL(7, 91, 10), // "WheelEvent"
QT_MOC_LITERAL(8, 102, 19), // "KeyPressEventSignal"
QT_MOC_LITERAL(9, 122, 10), // "QKeyEvent*"
QT_MOC_LITERAL(10, 133, 8), // "KeyEvent"
QT_MOC_LITERAL(11, 142, 15), // "MousePressEvent"
QT_MOC_LITERAL(12, 158, 12) // "ClearClicked"

    },
    "ISQLineEdit\0FocusInSignal\0\0FocusOutSignal\0"
    "MouseDoubleClicked\0WheelEventSignal\0"
    "QWheelEvent*\0WheelEvent\0KeyPressEventSignal\0"
    "QKeyEvent*\0KeyEvent\0MousePressEvent\0"
    "ClearClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISQLineEdit[] = {

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
       5,    1,   52,    2, 0x06 /* Public */,
       8,    1,   55,    2, 0x06 /* Public */,
      11,    0,   58,    2, 0x06 /* Public */,
      12,    0,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ISQLineEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISQLineEdit *_t = static_cast<ISQLineEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->FocusInSignal(); break;
        case 1: _t->FocusOutSignal(); break;
        case 2: _t->MouseDoubleClicked(); break;
        case 3: _t->WheelEventSignal((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        case 4: _t->KeyPressEventSignal((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 5: _t->MousePressEvent(); break;
        case 6: _t->ClearClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISQLineEdit::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::FocusInSignal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISQLineEdit::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::FocusOutSignal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISQLineEdit::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::MouseDoubleClicked)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISQLineEdit::*)(QWheelEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::WheelEventSignal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ISQLineEdit::*)(QKeyEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::KeyPressEventSignal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ISQLineEdit::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::MousePressEvent)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ISQLineEdit::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISQLineEdit::ClearClicked)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISQLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_ISQLineEdit.data,
      qt_meta_data_ISQLineEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISQLineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISQLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISQLineEdit.stringdata0))
        return static_cast<void*>(this);
    return QLineEdit::qt_metacast(_clname);
}

int ISQLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
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
void ISQLineEdit::FocusInSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ISQLineEdit::FocusOutSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ISQLineEdit::MouseDoubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ISQLineEdit::WheelEventSignal(QWheelEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ISQLineEdit::KeyPressEventSignal(QKeyEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ISQLineEdit::MousePressEvent()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ISQLineEdit::ClearClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
