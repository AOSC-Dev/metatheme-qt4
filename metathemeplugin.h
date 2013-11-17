#ifndef METATHEMEPLUGIN_H
#define METATHEMEPLUGIN_H

#include <QStylePlugin>


class MetaThemePlugin : public QStylePlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "MetaThemeQ4.json")
#endif // QT_VERSION >= 0x050000
    
public:
    MetaThemePlugin(QObject *parent = 0);
    virtual QStyle* create(const QString &key);
    virtual QStringList keys() const;
};

#endif // METATHEMEPLUGIN_H
