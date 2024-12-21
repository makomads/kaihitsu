/****************************************************************************
** Meta object code from reading C++ file 'configdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../configdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'configdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConfigDialog_t {
    QByteArrayData data[20];
    char stringdata0[397];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConfigDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConfigDialog_t qt_meta_stringdata_ConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ConfigDialog"
QT_MOC_LITERAL(1, 13, 16), // "on_btnok_clicked"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 20), // "on_btncancel_clicked"
QT_MOC_LITERAL(4, 52, 19), // "on_btnapply_clicked"
QT_MOC_LITERAL(5, 72, 30), // "on_lsttypes_currentItemChanged"
QT_MOC_LITERAL(6, 103, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 120, 7), // "current"
QT_MOC_LITERAL(8, 128, 8), // "previous"
QT_MOC_LITERAL(9, 137, 28), // "on_btnmoveupfiletype_clicked"
QT_MOC_LITERAL(10, 166, 30), // "on_btnmovedownfiletype_clicked"
QT_MOC_LITERAL(11, 197, 25), // "on_btnaddfiletype_clicked"
QT_MOC_LITERAL(12, 223, 25), // "on_btndelfiletype_clicked"
QT_MOC_LITERAL(13, 249, 28), // "on_lstattributes_itemChanged"
QT_MOC_LITERAL(14, 278, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(15, 296, 4), // "item"
QT_MOC_LITERAL(16, 301, 21), // "on_btnaddattr_clicked"
QT_MOC_LITERAL(17, 323, 21), // "on_btndelattr_clicked"
QT_MOC_LITERAL(18, 345, 24), // "on_btnmoveupattr_clicked"
QT_MOC_LITERAL(19, 370, 26) // "on_btnmovedownattr_clicked"

    },
    "ConfigDialog\0on_btnok_clicked\0\0"
    "on_btncancel_clicked\0on_btnapply_clicked\0"
    "on_lsttypes_currentItemChanged\0"
    "QListWidgetItem*\0current\0previous\0"
    "on_btnmoveupfiletype_clicked\0"
    "on_btnmovedownfiletype_clicked\0"
    "on_btnaddfiletype_clicked\0"
    "on_btndelfiletype_clicked\0"
    "on_lstattributes_itemChanged\0"
    "QTableWidgetItem*\0item\0on_btnaddattr_clicked\0"
    "on_btndelattr_clicked\0on_btnmoveupattr_clicked\0"
    "on_btnmovedownattr_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConfigDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    2,   82,    2, 0x08 /* Private */,
       9,    0,   87,    2, 0x08 /* Private */,
      10,    0,   88,    2, 0x08 /* Private */,
      11,    0,   89,    2, 0x08 /* Private */,
      12,    0,   90,    2, 0x08 /* Private */,
      13,    1,   91,    2, 0x08 /* Private */,
      16,    0,   94,    2, 0x08 /* Private */,
      17,    0,   95,    2, 0x08 /* Private */,
      18,    0,   96,    2, 0x08 /* Private */,
      19,    0,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6,    7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConfigDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_btnok_clicked(); break;
        case 1: _t->on_btncancel_clicked(); break;
        case 2: _t->on_btnapply_clicked(); break;
        case 3: _t->on_lsttypes_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 4: _t->on_btnmoveupfiletype_clicked(); break;
        case 5: _t->on_btnmovedownfiletype_clicked(); break;
        case 6: _t->on_btnaddfiletype_clicked(); break;
        case 7: _t->on_btndelfiletype_clicked(); break;
        case 8: _t->on_lstattributes_itemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 9: _t->on_btnaddattr_clicked(); break;
        case 10: _t->on_btndelattr_clicked(); break;
        case 11: _t->on_btnmoveupattr_clicked(); break;
        case 12: _t->on_btnmovedownattr_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ConfigDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_ConfigDialog.data,
    qt_meta_data_ConfigDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConfigDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
