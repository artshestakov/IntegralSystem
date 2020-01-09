/****************************************************************************
** Meta object code from reading C++ file 'ISPopupMessage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISPopupMessage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISPopupMessage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISPopupMessage_t {
    QByteArrayData data[11];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISPopupMessage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISPopupMessage_t qt_meta_stringdata_ISPopupMessage = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ISPopupMessage"
QT_MOC_LITERAL(1, 15, 7), // "Clicked"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "SetTitle"
QT_MOC_LITERAL(4, 33, 4), // "Text"
QT_MOC_LITERAL(5, 38, 10), // "SetMessage"
QT_MOC_LITERAL(6, 49, 7), // "Message"
QT_MOC_LITERAL(7, 57, 4), // "show"
QT_MOC_LITERAL(8, 62, 13), // "HideAnimation"
QT_MOC_LITERAL(9, 76, 4), // "hide"
QT_MOC_LITERAL(10, 81, 12) // "PopupOpacity"

    },
    "ISPopupMessage\0Clicked\0\0SetTitle\0Text\0"
    "SetMessage\0Message\0show\0HideAnimation\0"
    "hide\0PopupOpacity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISPopupMessage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   45,    2, 0x0a /* Public */,
       5,    1,   48,    2, 0x0a /* Public */,
       7,    0,   51,    2, 0x0a /* Public */,
       8,    0,   52,    2, 0x08 /* Private */,
       9,    0,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      10, QMetaType::Float, 0x00095003,

       0        // eod
};

void ISPopupMessage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISPopupMessage *_t = static_cast<ISPopupMessage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Clicked(); break;
        case 1: _t->SetTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->SetMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->show(); break;
        case 4: _t->HideAnimation(); break;
        case 5: _t->hide(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISPopupMessage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISPopupMessage::Clicked)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ISPopupMessage *_t = static_cast<ISPopupMessage *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< float*>(_v) = _t->GetPopupOpacity(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ISPopupMessage *_t = static_cast<ISPopupMessage *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->SetPopupOpacity(*reinterpret_cast< float*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ISPopupMessage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ISPopupMessage.data,
      qt_meta_data_ISPopupMessage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISPopupMessage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISPopupMessage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISPopupMessage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ISPopupMessage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
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
void ISPopupMessage::Clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
