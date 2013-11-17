#include "metathemeplugin.h"

#include <qmetathemestyle.h>

MetaThemePlugin::MetaThemePlugin(QObject *parent) :
    QStylePlugin(parent)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MetaThemeQ4, MetaThemePlugin)
#endif // QT_VERSION < 0x050000
QStyle* MetaThemePlugin::create(const QString &key)
{
    return new QMetaThemeStyle();
}

QStringList MetaThemePlugin::keys() const
{
    return QStringList("MetaTheme");
}
