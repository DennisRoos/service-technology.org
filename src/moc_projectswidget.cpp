/****************************************************************************
** Meta object code from reading C++ file 'projectswidget.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "projectswidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'projectswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProjectsWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   15,   15,   15, 0x0a,
      58,   15,   15,   15, 0x2a,
      71,   15,   15,   15, 0x0a,
      92,   15,   15,   15, 0x2a,
     111,   15,  106,   15, 0x0a,
     126,   15,   15,   15, 0x0a,
     145,   15,   15,   15, 0x0a,
     160,   15,   15,   15, 0x0a,
     170,   15,   15,   15, 0x0a,
     180,   15,   15,   15, 0x0a,
     195,  193,   15,   15, 0x0a,
     223,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ProjectsWidget[] = {
    "ProjectsWidget\0\0showDockWidget(Tool*)\0"
    "newProject(QString)\0newProject()\0"
    "openProject(QString)\0openProject()\0"
    "bool\0closeProject()\0closeAllProjects()\0"
    "makeMakefile()\0addNode()\0addLink()\0"
    "revertLink()\0,\0createItem(QString,QString)\0"
    "dockWidget(Tool*)\0"
};

const QMetaObject ProjectsWidget::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_ProjectsWidget,
      qt_meta_data_ProjectsWidget, 0 }
};

const QMetaObject *ProjectsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *ProjectsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectsWidget))
        return static_cast<void*>(const_cast< ProjectsWidget*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int ProjectsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showDockWidget((*reinterpret_cast< Tool*(*)>(_a[1]))); break;
        case 1: newProject((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: newProject(); break;
        case 3: openProject((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: openProject(); break;
        case 5: { bool _r = closeProject();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: closeAllProjects(); break;
        case 7: makeMakefile(); break;
        case 8: addNode(); break;
        case 9: addLink(); break;
        case 10: revertLink(); break;
        case 11: createItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: dockWidget((*reinterpret_cast< Tool*(*)>(_a[1]))); break;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ProjectsWidget::showDockWidget(Tool * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
