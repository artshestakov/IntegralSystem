/****************************************************************************
** Meta object code from reading C++ file 'ISMonitorUserWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ISMonitorUserWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ISMonitorUserWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISMonitorUserWidget_t {
    QByteArrayData data[9];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISMonitorUserWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISMonitorUserWidget_t qt_meta_stringdata_ISMonitorUserWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ISMonitorUserWidget"
QT_MOC_LITERAL(1, 20, 12), // "ShowUserCard"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 12), // "ShowActivity"
QT_MOC_LITERAL(4, 47, 12), // "ShowProtocol"
QT_MOC_LITERAL(5, 60, 11), // "ShowDetails"
QT_MOC_LITERAL(6, 72, 10), // "EndSession"
QT_MOC_LITERAL(7, 83, 13), // "GetScreenshot"
QT_MOC_LITERAL(8, 97, 10) // "SendNotify"

    },
    "ISMonitorUserWidget\0ShowUserCard\0\0"
    "ShowActivity\0ShowProtocol\0ShowDetails\0"
    "EndSession\0GetScreenshot\0SendNotify"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISMonitorUserWidget[] = {

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

void ISMonitorUserWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ISMonitorUserWidget *_t = static_cast<ISMonitorUserWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ShowUserCard(); break;
        case 1: _t->ShowActivity(); break;
        case 2: _t->ShowProtocol(); break;
        case 3: _t->ShowDetails(); break;
        case 4: _t->EndSession(); break;
        case 5: _t->GetScreenshot(); break;
        case 6: _t->SendNotify(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::ShowUserCard)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::ShowActivity)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::ShowProtocol)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::ShowDetails)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::EndSession)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::GetScreenshot)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ISMonitorUserWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ISMonitorUserWidget::SendNotify)) {
                *result = 6;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ISMonitorUserWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_ISMonitorUserWidget.data,
      qt_meta_data_ISMonitorUserWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ISMonitorUserWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISMonitorUserWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISMonitorUserWidget.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int ISMonitorUserWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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
void ISMonitorUserWidget::ShowUserCard()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ISMonitorUserWidget::ShowActivity()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ISMonitorUserWidget::ShowProtocol()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ISMonitorUserWidget::ShowDetails()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ISMonitorUserWidget::EndSession()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ISMonitorUserWidget::GetScreenshot()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ISMonitorUserWidget::SendNotify()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
