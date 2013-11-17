#ifndef METATHEMEHELPER_H
#define METATHEMEHELPER_H

#include <QObject>
#include <QtCore>
#include <QtGui>

class MetaThemeHelper: public QObject
{
   Q_OBJECT

public slots:
   void widgetDestroyedForData(QObject *obj);
   void widgetDestroyedForRef(QObject *obj);

};

#endif // METATHEMEHELPER_H
