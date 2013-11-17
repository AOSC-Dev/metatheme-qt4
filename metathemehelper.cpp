#include "metathemehelper.h"

#include <Qt3Support/Q3PtrDict>

extern "C"{
#include <metathemeInt.h>
}

Q3PtrDict<void *> widgetRefs;
Q3PtrDict<void> widgetData;
Q3PtrDict<void> widgetDataDispose;

void MetaThemeHelper::widgetDestroyedForData(QObject *obj)
{
   void *data = widgetData[obj];
   if (!data) return;

   mt_destroy_func dispose_func = (mt_destroy_func)widgetDataDispose[obj];
   dispose_func(data);
   widgetData.remove(obj);
   widgetDataDispose.remove(obj);
   QObject::disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(widgetDestroyedForData(QObject *)));
}


void MetaThemeHelper::widgetDestroyedForRef(QObject *obj)
{
   Q3PtrDictIterator<void *> it(widgetRefs);
   for( ; it.current(); ++it ) {
      MT_WIDGET *widget = (MT_WIDGET *)it.currentKey();
      if (widget && *widget == obj) {
         widgetRefs.remove(widget);
         break;
      }
   }

   QObject::disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(widgetDestroyedForRef(QObject *)));
}
