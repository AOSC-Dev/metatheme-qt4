#ifndef METATHEMEQT_H
#define METATHEMEQT_H

#include <Qt3Support/Q3PtrDict>

#include <metathemehelper.h>
#include "qmetathemestyle.h"

extern Q3PtrDict<void *> widgetRefs;
extern Q3PtrDict<void> widgetData;
extern Q3PtrDict<void> widgetDataDispose;

extern "C" {

typedef struct {
   MT_COLOR col;
} MT_QT_GC;

typedef struct {
   const QMetaThemeStyle *style;
   const QRect &r;
   int flags;
   //const QColorGroup &g;
   const QPalette &g;
   const QString& text;
   int len;
   const QColor *penColor;
} MT_QT_STRING;

/*static bool toolButtonPopup = false;
static bool toggleButton = false;
static bool defaultButton = false;
static bool tabBarButton = false;
static bool drawMask = false;
static int stringType = 0;
static int stringState = 0;*/
static MetaThemeHelper helper;


static MT_GC *_mt_gc_new_with_foreground(MT_COLOR *col)
{
   MT_QT_GC *gc = (MT_QT_GC *)malloc(sizeof(MT_QT_GC));
   gc->col = *col;
   return (MT_GC *)gc;
}


static void _mt_gc_destroy(MT_GC *gc)
{
   free(gc);
}


static void _mt_gc_set_clip_rectangle(MT_RECTANGLE *rect, ...)
{
}


static MT_RECTANGLE *_mt_gc_get_clip_rectangle(MT_GC *gc)
{
   return NULL;
}


static void _mt_gc_set_foreground(MT_GC *gc, MT_COLOR *col)
{
   MT_QT_GC *qtgc = (MT_QT_GC *)gc;
   qtgc->col = *col;
}


static MT_PIXMAP *_mt_pixmap_new_from_xpm(char **xpm_data)
{
   return (MT_PIXMAP *)new QPixmap(xpm_data);
}


static MT_PIXMAP *_mt_pixmap_new_from_image(MT_IMAGE *img)
{
   QImage *i = (QImage *)img;
   //i->setAlphaBuffer(false);
   QPixmap *p = new QPixmap();
   p->convertFromImage(*i);
   //i->setAlphaBuffer(true);
   return (MT_PIXMAP *)p;
}


static void _mt_pixmap_destroy(MT_PIXMAP *pixmap)
{
   delete (QPixmap *)pixmap;
}


static MT_IMAGE *_mt_image_new(int width, int height)
{
   QImage *img = new QImage(width, height,QImage::Format_ARGB32);
   //img->setAlphaBuffer(true);
   return (MT_IMAGE *)img;
}


static MT_IMAGE *_mt_image_new_from_xpm(char **xpm_data)
{
   QImage *img = new QImage(xpm_data);

   /*if (img->depth() != 32) {
      QImage newimg = img->convertDepth(32);
      delete img;

      if (newimg.isNull()) {
         return NULL;
      }

      return (MT_IMAGE *)new QImage(newimg);
   }*/

   return (MT_IMAGE *)img;
}


static MT_IMAGE *_mt_image_copy(MT_IMAGE *img)
{
   QImage *qimg = new QImage(*((const QImage *)img));
   qimg->detach();
   return (MT_IMAGE *)qimg;
}


static void _mt_image_destroy(MT_IMAGE *image)
{
   delete (QImage *)image;
}


static void _mt_image_replace_color(MT_IMAGE *img, int r, int g, int b, MT_COLOR *col)
{
   QImage *qi = (QImage *)img;
   uint i,j,col1,col2;

   col1 = (r << 16) + (g << 8) + b;
   col2 = (col->a << 24) + (col->r << 16) + (col->g << 8) + col->b;

   for (i=0; i<(uint)qi->height(); i++) {
      uint *p = (uint *)qi->scanLine(i);
      for (j=0; j<(uint)qi->width(); j++, p++) {
         if ((*p & 0xFFFFFF) == col1) {
            *p = col2;
         }
      }
   }
}


static void _mt_image_set_pixel(MT_IMAGE *img, int x, int y, MT_COLOR *col)
{
   QImage *i = (QImage *)img;
   i->setPixel(x, y, (col->a << 24) + (col->r << 16) + (col->g << 8) + col->b);
}


static void _mt_image_get_pixel(MT_IMAGE *img, int x, int y, MT_COLOR *col)
{
   QRgb rgba = ((QImage *)img)->pixel(x, y);
   col->a = qAlpha(rgba);
   col->r = qRed(rgba);
   col->g = qGreen(rgba);
   col->b = qBlue(rgba);
}


static void _mt_image_fill(MT_IMAGE *img, MT_COLOR *col)
{
   QImage *i = (QImage *)img;
   i->fill((255 << 24) + (col->r << 16) + (col->g << 8) + col->b);
}


static void _mt_image_get_size(MT_IMAGE *img, int *width, int *height)
{
   QImage *qi = (QImage *)img;
   *width = qi->width();
   *height = qi->height();
}


static void _mt_draw_point(MT_WINDOW *win, MT_GC *gc, int x, int y)
{
   QPainter *p = (QPainter *)win;
   MT_QT_GC *qtgc = (MT_QT_GC *)gc;

   p->setPen(QColor(qtgc->col.r, qtgc->col.g, qtgc->col.b));
   p->drawPoint(x, y);
}


static void _mt_draw_line(MT_WINDOW *win, MT_GC *gc, int x1, int y1, int x2, int y2)
{
   QPainter *p = (QPainter *)win;
   MT_QT_GC *qtgc = (MT_QT_GC *)gc;

   p->setPen(QColor(qtgc->col.r, qtgc->col.g, qtgc->col.b));
   p->drawLine(x1, y1, x2, y2);
}


static void _mt_draw_rectangle(MT_WINDOW *win, MT_GC *gc, int filled, int x1, int y1, int x2, int y2)
{
   QPainter *p = (QPainter *)win;
   MT_QT_GC *qtgc = (MT_QT_GC *)gc;

   p->setPen(QColor(qtgc->col.r, qtgc->col.g, qtgc->col.b));
   if (filled) p->setBrush(QColor(qtgc->col.r, qtgc->col.g, qtgc->col.b));
   p->drawRect(x1, y1, x2-x1+1, y2-y1+1);
}


static void _mt_draw_polygon(MT_WINDOW *win, MT_GC *gc, int filled, MT_POINT *points, int npoints)
{
   QPainter *p = (QPainter *)win;
   MT_QT_GC *qtgc = (MT_QT_GC *)gc;

   QPolygon a(npoints);
   for (int i=0; i<npoints; i++) {
      a.setPoint(i, points[i].x, points[i].y);
   }

   if (filled) {
      p->setPen(Qt::NoPen);
      p->setBrush(QColor(qtgc->col.r, qtgc->col.g, qtgc->col.b));
      //p->drawPolygon(a, false);
      p->drawPolygon(a);
      p->setPen(Qt::SolidLine);
   }
   else {
      p->setPen(QColor(qtgc->col.r, qtgc->col.g, qtgc->col.b));
      p->drawPolyline(a);
   }
}


static void _mt_draw_string(MT_WINDOW *win, MT_STRING *str, int x_offset, int y_offset, MT_COLOR *color)
{
   QPainter *p = (QPainter *)win;
   MT_QT_STRING *s = (MT_QT_STRING *)str;
   QRect r(s->r);
   const QColor *penColor = s->penColor;
   QColor tmpColor;
   QPen oldPen = p->pen();

   r.moveTo(x_offset, y_offset);

   if (color) {
      tmpColor.setRgb(color->r, color->g, color->b);
      penColor = &tmpColor;
   }

   p->setPen(*penColor);
   //s->style->drawItem(p, r, s->flags, s->g, true, NULL, s->text, s->len, penColor);
   s->style->drawItemText(p,r,s->flags,s->g,true,s->text);
   p->setPen(oldPen);
}


static void _mt_draw_pixmap(MT_WINDOW *win, MT_GC *gc, MT_PIXMAP *pixmap, int xsrc, int ysrc, int xdest, int ydest, int width, int height)
{
   QPainter *p = (QPainter *)win;

   p->drawPixmap(xdest, ydest, *((const QPixmap *)pixmap), xsrc, ysrc, width, height);
}


static void _mt_draw_pixmap_tiled(MT_WINDOW *win, MT_GC *gc, MT_PIXMAP *pixmap, int srcwidth, int srcheight, int xdest, int ydest, int width, int height)
{
   QPainter *p = (QPainter *)win;

   p->drawTiledPixmap(xdest, ydest, width, height, *((const QPixmap *)pixmap));
}


static void _mt_draw_image(MT_WINDOW *win, MT_GC *gc, MT_IMAGE *img, int xsrc, int ysrc, int wsrc, int hsrc, int xdest, int ydest, int width, int height)
{
   QPainter *p = (QPainter *)win;
   QImage *i = (QImage *)img;

   p->drawImage(xdest, ydest, i->copy(xsrc, ysrc, wsrc, hsrc).scaled(width, height));
}


static MT_WIDGET *_mt_widget_ref(MT_WIDGET *widget)
{
   MT_WIDGET *result = (MT_WIDGET *)malloc(sizeof(MT_WIDGET));
   *result = *widget;

   // mark the reference mapping:
   widgetRefs.insert(result, result);

   QWidget *w = (QWidget *)*widget;
   QObject::connect(w, SIGNAL(destroyed(QObject *)), &helper, SLOT(widgetDestroyedForRef(QObject *)));

   return result;
}


static void _mt_widget_unref(MT_WIDGET *widget)
{
   MT_WIDGET *handle = widgetRefs[widget];

   // if it is a handle then check if the widget is still available:
   if (handle && *handle == NULL) return;

   widgetRefs.remove(widget);
   QObject::disconnect((QWidget *)*widget, SIGNAL(destroyed(QObject *)), &helper, SLOT(widgetDestroyedForRef(QObject *)));
   free(widget);
}


static MT_WIDGET *_mt_widget_get_parent(MT_WIDGET *widget, MT_WIDGET *result)
{
   MT_WIDGET *handle = widgetRefs[widget];

   // if it is a handle then check if the widget is still available:
   if (handle && *handle == NULL) return NULL;

   if (!result) result = (MT_WIDGET *)malloc(sizeof(MT_WIDGET));

   *result = ((QWidget *)*widget)->parentWidget();
   return result;
}


static void _mt_widget_set_data(MT_WIDGET *widget, void *data, mt_destroy_func data_dispose)
{
   MT_WIDGET *handle = widgetRefs[widget];

   // if it is a handle then check if the widget is still available:
   if (handle && *handle == NULL) return;

   // obtain the real address of QWidget:
   QWidget *w = (QWidget *)*widget;

   // check if it already contains data and free them:
   if (widgetData[w]) {
      mt_destroy_func destroy_func = (mt_destroy_func)widgetDataDispose[w];

      if (destroy_func) {
         destroy_func(widgetData[w]);
         widgetDataDispose.remove(w);
      }

      QObject::disconnect(w, SIGNAL(destroyed(QObject *)), &helper, SLOT(widgetDestroyedForData(QObject *)));
   }

   if (!data) return;

   widgetData.replace(w, data);
   if (data_dispose) {
      widgetDataDispose.replace(w, (void *)data_dispose);
      QObject::connect(w, SIGNAL(destroyed(QObject *)), &helper, SLOT(widgetDestroyedForData(QObject *)));
   }
}


static void *_mt_widget_get_data(MT_WIDGET *widget)
{
   MT_WIDGET *handle = widgetRefs[widget];

   // if it is a handle then check if the widget is still available:
   if (handle && *handle == NULL) return NULL;

   return widgetData[*widget];
}


} /* extern "C" */

#endif // METATHEMEQT_H
