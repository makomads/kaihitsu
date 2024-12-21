/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TabBar_t {
    QByteArrayData data[1];
    char stringdata0[7];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TabBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TabBar_t qt_meta_stringdata_TabBar = {
    {
QT_MOC_LITERAL(0, 0, 6) // "TabBar"

    },
    "TabBar"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TabBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void TabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject TabBar::staticMetaObject = { {
    &QTabBar::staticMetaObject,
    qt_meta_stringdata_TabBar.data,
    qt_meta_data_TabBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabBar.stringdata0))
        return static_cast<void*>(this);
    return QTabBar::qt_metacast(_clname);
}

int TabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata0[321];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 13), // "onTriggerMenu"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 8), // "QAction*"
QT_MOC_LITERAL(4, 35, 3), // "act"
QT_MOC_LITERAL(5, 39, 16), // "onCurrentChanged"
QT_MOC_LITERAL(6, 56, 5), // "index"
QT_MOC_LITERAL(7, 62, 19), // "onEditorViewEntered"
QT_MOC_LITERAL(8, 82, 7), // "QEvent*"
QT_MOC_LITERAL(9, 90, 1), // "e"
QT_MOC_LITERAL(10, 92, 19), // "onEditorViewDropped"
QT_MOC_LITERAL(11, 112, 11), // "QDropEvent*"
QT_MOC_LITERAL(12, 124, 7), // "fileNew"
QT_MOC_LITERAL(13, 132, 8), // "openFile"
QT_MOC_LITERAL(14, 141, 5), // "fpath"
QT_MOC_LITERAL(15, 147, 4), // "save"
QT_MOC_LITERAL(16, 152, 6), // "saveAs"
QT_MOC_LITERAL(17, 159, 7), // "saveAll"
QT_MOC_LITERAL(18, 167, 5), // "close"
QT_MOC_LITERAL(19, 173, 14), // "showFindDialog"
QT_MOC_LITERAL(20, 188, 8), // "findNext"
QT_MOC_LITERAL(21, 197, 12), // "findPrevious"
QT_MOC_LITERAL(22, 210, 17), // "showReplaceDialog"
QT_MOC_LITERAL(23, 228, 7), // "replace"
QT_MOC_LITERAL(24, 236, 10), // "replaceAll"
QT_MOC_LITERAL(25, 247, 22), // "on_btntabclose_clicked"
QT_MOC_LITERAL(26, 270, 37), // "on_editorview_mouseRightButto..."
QT_MOC_LITERAL(27, 308, 12) // "QMouseEvent*"

    },
    "MainWindow\0onTriggerMenu\0\0QAction*\0"
    "act\0onCurrentChanged\0index\0"
    "onEditorViewEntered\0QEvent*\0e\0"
    "onEditorViewDropped\0QDropEvent*\0fileNew\0"
    "openFile\0fpath\0save\0saveAs\0saveAll\0"
    "close\0showFindDialog\0findNext\0"
    "findPrevious\0showReplaceDialog\0replace\0"
    "replaceAll\0on_btntabclose_clicked\0"
    "on_editorview_mouseRightButtonPressed\0"
    "QMouseEvent*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  109,    2, 0x0a /* Public */,
       5,    1,  112,    2, 0x0a /* Public */,
       7,    1,  115,    2, 0x0a /* Public */,
      10,    1,  118,    2, 0x0a /* Public */,
      12,    0,  121,    2, 0x0a /* Public */,
      13,    0,  122,    2, 0x0a /* Public */,
      13,    1,  123,    2, 0x0a /* Public */,
      15,    0,  126,    2, 0x0a /* Public */,
      16,    0,  127,    2, 0x0a /* Public */,
      17,    0,  128,    2, 0x0a /* Public */,
      18,    0,  129,    2, 0x0a /* Public */,
      19,    0,  130,    2, 0x0a /* Public */,
      20,    0,  131,    2, 0x0a /* Public */,
      21,    0,  132,    2, 0x0a /* Public */,
      22,    0,  133,    2, 0x0a /* Public */,
      23,    0,  134,    2, 0x0a /* Public */,
      24,    0,  135,    2, 0x0a /* Public */,
      25,    0,  136,    2, 0x0a /* Public */,
      26,    1,  137,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 27,    9,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onTriggerMenu((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 1: _t->onCurrentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onEditorViewEntered((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 3: _t->onEditorViewDropped((*reinterpret_cast< QDropEvent*(*)>(_a[1]))); break;
        case 4: _t->fileNew(); break;
        case 5: _t->openFile(); break;
        case 6: _t->openFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->save(); break;
        case 8: _t->saveAs(); break;
        case 9: _t->saveAll(); break;
        case 10: _t->close(); break;
        case 11: _t->showFindDialog(); break;
        case 12: _t->findNext(); break;
        case 13: _t->findPrevious(); break;
        case 14: _t->showReplaceDialog(); break;
        case 15: _t->replace(); break;
        case 16: _t->replaceAll(); break;
        case 17: _t->on_btntabclose_clicked(); break;
        case 18: _t->on_editorview_mouseRightButtonPressed((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
