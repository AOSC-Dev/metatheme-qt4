#include "qmetathemestyle.h"

#include "metathemeQt.h"


MT_TOOLKIT qt = {
   _mt_gc_new_with_foreground,
   _mt_gc_destroy,
   _mt_gc_set_clip_rectangle,
   _mt_gc_get_clip_rectangle,
   _mt_gc_set_foreground,

   _mt_pixmap_new_from_xpm,
   _mt_pixmap_new_from_image,
   _mt_pixmap_destroy,

   _mt_image_new,
   _mt_image_new_from_xpm,
   _mt_image_copy,
   _mt_image_destroy,
   _mt_image_replace_color,
   _mt_image_set_pixel,
   _mt_image_get_pixel,
   _mt_image_fill,
   _mt_image_get_size,

   _mt_draw_point,
   _mt_draw_line,
   _mt_draw_rectangle,
   _mt_draw_polygon,
   _mt_draw_string,
   _mt_draw_pixmap,
   _mt_draw_pixmap_tiled,
   _mt_draw_image,

   NULL,
   NULL,

   _mt_widget_ref,
   _mt_widget_unref,
   _mt_widget_get_parent,
   _mt_widget_set_data,
   _mt_widget_get_data
};

QMetaThemeStyle::QMetaThemeStyle()
{
    metatheme_toolkit_init(&qt);

    MT_LIBRARY *mt_library = metatheme_library_init(&qt);
    if (!mt_library) {
       exit(1);
    }

    mt_engine = metatheme_load_engine(mt_library, NULL);
    if (!mt_engine) {
       metatheme_library_exit(mt_library);
       exit(1);
    }

    metatheme_load_default_settings(mt_engine);
    mt_engine->init(mt_engine);
    mt_engine->realize(mt_engine);

    setColorPalette(&(mt_engine->palette));
    setFont();

    hoverWidget = 0;
    hoverPart = 0;
    //toolButtonPopup = false;
    toolButtonDropDownActiveWidget = NULL;
}

void QMetaThemeStyle::setColorPalette(MT_COLOR_PALETTE *palette)
{
   QPalette g;
   int i;

   i = MT_BACKGROUND;          g.setColor(QPalette::Background,      QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_FOREGROUND;          g.setColor(QPalette::Foreground,      QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_WINDOW_BACKGROUND;   g.setColor(QPalette::Base,            QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_WINDOW_FOREGROUND;   g.setColor(QPalette::Text,            QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_BUTTON_BACKGROUND;   g.setColor(QPalette::Button,          QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_BUTTON_FOREGROUND;   g.setColor(QPalette::ButtonText,      QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_SELECTED_BACKGROUND; g.setColor(QPalette::Highlight,       QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_SELECTED_FOREGROUND; g.setColor(QPalette::HighlightedText, QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));

   i = MT_WINDOW_FOREGROUND;   g.setColor(QPalette::BrightText,      QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));

   i = MT_BUTTON_LIGHT;        g.setColor(QPalette::Light,           QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_BUTTON_MIDLIGHT;     g.setColor(QPalette::Midlight,        QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_BUTTON_DARK;         g.setColor(QPalette::Dark,            QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_BUTTON_MID;          g.setColor(QPalette::Mid,             QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_BUTTON_SHADOW;       g.setColor(QPalette::Shadow,          QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));

   g.setColor(QPalette::Link, Qt::blue);
   g.setColor(QPalette::LinkVisited, Qt::magenta);

   //qpalette.setActive(g);
   qpalette.setColor(QPalette::Active,QPalette::NoRole,g.color(QPalette::Active,QPalette::NoRole));
   //qpalette.setInactive(g);
   qpalette.setColor(QPalette::Inactive,QPalette::NoRole,g.color(QPalette::Inactive,QPalette::NoRole));

   g.setColor(QPalette::Light, QColor(255, 255, 255));
   i = MT_DISABLED_FOREGROUND; g.setColor(QPalette::Text, QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_DISABLED_FOREGROUND; g.setColor(QPalette::ButtonText, QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));

   //qpalette.setDisabled(g);
   qpalette.setColor(QPalette::Disabled,QPalette::NoRole,g.color(QPalette::Disabled,QPalette::NoRole));
}


void QMetaThemeStyle::setFont()
{
   qfont.setFamily(mt_engine->font);
   qfont.setPointSize(mt_engine->font_size);
   qfont.setWeight(QFont::Normal);
   qfont.setItalic(false);

   QApplication::setFont(qfont);
   QApplication::setFont(qfont, "QMenuBar");
   QApplication::setFont(qfont, "QPopupMenu");
   QApplication::setFont(qfont, "KPopupTitle");
}

#define CHECKMODE(mode) ((((int)opt->state) | mode) == ((int)opt->state))
#define DRAWBEVEL(mode,type) mt_engine->draw_widget(mt_engine,p,NULL,type,mode, \
    opt->rect.x(),opt->rect.y(),opt->rect.width(),opt->rect.height(),&data);

void QMetaThemeStyle::drawControl(ControlElement element,const QStyleOption *opt,
                                  QPainter *p,
                                  const QWidget *widget) const
{
    MT_WIDGET_DATA data;
    //data.background_color =
    mt_color_set(data.background_color, qpalette.background().color().red(), qpalette.background().color().green(), qpalette.background().color().blue());
   switch (element) {
      case CE_PushButtonBevel:
   {
#define DRAWBUTTONBEVEL(mode) DRAWBEVEL(mode,MT_BUTTON)
         const QStyleOptionButton *optbtn = (const QStyleOptionButton*)opt;
         qDebug("%d %d %d %d",((int)optbtn->state),
                CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
         if(CHECKMODE(State_Enabled))
         {
             if(!CHECKMODE(State_Raised))
             {
                 DRAWBUTTONBEVEL(MT_ACTIVE)
             }
             else if(!CHECKMODE(State_MouseOver))
             {
                 DRAWBUTTONBEVEL(MT_NORMAL)
             }
             else
             {
                 qDebug("PushButton Mouse Active");
                 DRAWBUTTONBEVEL(MT_HOVER)
             }
         }
         else
         {
             DRAWBUTTONBEVEL(MT_DISABLED)
         }
   }
         break;
#undef DRAWBUTTONBEVEL
         case CE_ScrollBarSlider:
{
#define DRAWSCROLLBARBEVEL(mode) DRAWBEVEL(mode,MT_SCROLLBAR_HANDLE)
      if(CHECKMODE(State_Enabled))
      {
          if(CHECKMODE(State_Raised))
          {
              DRAWSCROLLBARBEVEL(MT_ACTIVE)
          }
          else if(!CHECKMODE(State_MouseOver))
          {
              DRAWSCROLLBARBEVEL(MT_NORMAL)
          }
          else
          {
              qDebug("Slider Mouse Active");
              DRAWSCROLLBARBEVEL(MT_HOVER)
          }
      }
      else
      {
          DRAWSCROLLBARBEVEL(MT_DISABLED)
      }
}
#undef DRAWSCROLLBARBEVEL
      break;
   case CE_ScrollBarAddLine:
       if(widget != NULL && widget->size().height() > widget->size().width())
       {
#define DRAWSCROLLBARBEVEL(mode) DRAWBEVEL(mode,MT_SCROLLBAR_ARROW_DOWN)
           if(CHECKMODE(State_Enabled))
           {
               if(CHECKMODE(State_Raised))
               {
                   DRAWSCROLLBARBEVEL(MT_ACTIVE)
               }
               else if(!CHECKMODE(State_MouseOver))
               {
                   DRAWSCROLLBARBEVEL(MT_NORMAL)
               }
               else
               {
                   qDebug("SubLine Mouse Active");
                   DRAWSCROLLBARBEVEL(MT_HOVER)
               }
           }
           else
           {
               DRAWSCROLLBARBEVEL(MT_DISABLED)
           }
#undef DRAWSCROLLBARBEVEL
        }
       else
       {
#define DRAWSCROLLBARBEVEL(mode) DRAWBEVEL(mode,MT_SCROLLBAR_ARROW_RIGHT)
           if(CHECKMODE(State_Enabled))
           {
               if(CHECKMODE(State_Raised))
               {
                   DRAWSCROLLBARBEVEL(MT_ACTIVE)
               }
               else if(!CHECKMODE(State_MouseOver))
               {
                   DRAWSCROLLBARBEVEL(MT_NORMAL)
               }
               else
               {
                   qDebug("SubLine Mouse Active");
                   DRAWSCROLLBARBEVEL(MT_HOVER)
               }
           }
           else
           {
               DRAWSCROLLBARBEVEL(MT_DISABLED)
           }
#undef DRAWSCROLLBARBEVEL
        }
        break;
   case CE_ScrollBarSubLine:
       if(widget != NULL && widget->size().height() > widget->size().width())
       {
#define DRAWSCROLLBARBEVEL(mode) DRAWBEVEL(mode,MT_SCROLLBAR_ARROW_UP)
           if(CHECKMODE(State_Enabled))
           {
               if(CHECKMODE(State_Raised))
               {
                   DRAWSCROLLBARBEVEL(MT_ACTIVE)
               }
               else if(!CHECKMODE(State_MouseOver))
               {
                   DRAWSCROLLBARBEVEL(MT_NORMAL)
               }
               else
               {
                   qDebug("SubLine Mouse Active");
                   DRAWSCROLLBARBEVEL(MT_HOVER)
               }
           }
           else
           {
               DRAWSCROLLBARBEVEL(MT_DISABLED)
           }
#undef DRAWSCROLLBARBEVEL
        }
       else
       {
#define DRAWSCROLLBARBEVEL(mode) DRAWBEVEL(mode,MT_SCROLLBAR_ARROW_LEFT)
           if(CHECKMODE(State_Enabled))
           {
               if(CHECKMODE(State_Raised))
               {
                   DRAWSCROLLBARBEVEL(MT_ACTIVE)
               }
               else if(!CHECKMODE(State_MouseOver))
               {
                   DRAWSCROLLBARBEVEL(MT_NORMAL)
               }
               else
               {
                   qDebug("SubLine Mouse Active");
                   DRAWSCROLLBARBEVEL(MT_HOVER)
               }
           }
           else
           {
               DRAWSCROLLBARBEVEL(MT_DISABLED)
           }
#undef DRAWSCROLLBARBEVEL
        }
        break;
      case CE_CheckBox:
         QWindowsStyle::drawControl(element,opt,p,widget);
         break;
      default:
         BaseStyle::drawControl(element, opt, p, widget);
         break;
   }
}

QSize QMetaThemeStyle::sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget) const
{
    switch(type)
    {
        case CT_PushButton:
    {   const QPushButton *button = (const QPushButton *)widget;
        const QStyleOptionButton *optbtn = (const QStyleOptionButton *)option;
        int w = optbtn->rect.size().width(), h = optbtn->rect.size().height();
        int fw = pixelMetric(PM_DefaultFrameWidth,option, widget) * 2;
        //int m = button->isToggleButton()? MT_BUTTON_TOGGLE_BORDER : MT_BUTTON_BORDER;
        int m = MT_BUTTON_BORDER;

        w += mt_engine->metric_size[m].x*2 + fw;
        h += mt_engine->metric_size[m].y*2 + fw;

        if (w < 80 && !button->icon().isNull()) w = 80;
        if (h < 23) h = 23;

        return QSize(w, h);}
            break;
        default:
            return QWindowsStyle::sizeFromContents(type,option,contentsSize,widget);
    }
}

void QMetaThemeStyle::drawComplexControl ( ComplexControl control, const QStyleOptionComplex * opt, QPainter * p, const QWidget * widget) const
{
    MT_WIDGET_DATA data;
    switch(control)
    {
#define DRAWTOOLBUTTONBEVEL(mode) DRAWBEVEL(mode,MT_TOOLBAR_ITEM)
        case CC_ToolButton:
            {
        const QStyleOptionToolButton *optbtn = (const QStyleOptionToolButton*)opt;
               qDebug("%d %d %d %d",((int)optbtn->state),
                      CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
               if(CHECKMODE(State_Enabled))
               {
                   if(!CHECKMODE(State_Raised))
                   {
                       DRAWTOOLBUTTONBEVEL(MT_ACTIVE)
                   }
                   else if(!CHECKMODE(State_MouseOver))
                   {
                       DRAWTOOLBUTTONBEVEL(MT_NORMAL)
                   }
                   else
                   {
                       qDebug("Mouse Active");
                       DRAWTOOLBUTTONBEVEL(MT_HOVER)
                   }
               }
               else
               {
                   DRAWTOOLBUTTONBEVEL(MT_DISABLED)
               }
               drawControl(CE_ToolButtonLabel,opt,p,widget);
    }
            break;
        case CC_ScrollBar:
            QWindowsStyle::drawComplexControl(control,opt,p,widget);
        default:
            BaseStyle::drawComplexControl(control,opt,p,widget);
    }
}

void QMetaThemeStyle::drawPrimitive ( PrimitiveElement element, const QStyleOption * opt, QPainter * p, const QWidget * widget) const
{
    MT_WIDGET_DATA data;
    data.flags = 0;
    switch(element)
    {
        case PE_PanelButtonCommand:
    {
#define DRAWBUTTONBEVEL(mode) DRAWBEVEL(mode,MT_BUTTON)
        const QStyleOptionButton *optbtn = (const QStyleOptionButton*)opt;
        qDebug("%d %d %d %d",((int)optbtn->state),
               CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
        if(CHECKMODE(State_Enabled))
        {
            if(CHECKMODE(State_Raised))
            {
                DRAWBUTTONBEVEL(MT_ACTIVE)
            }
            else if(!CHECKMODE(State_MouseOver))
            {
                DRAWBUTTONBEVEL(MT_NORMAL)
            }
            else
            {
                qDebug("PushButton Mouse Active");
                DRAWBUTTONBEVEL(MT_HOVER)
            }
        }
        else
        {
            DRAWBUTTONBEVEL(MT_DISABLED)
        }
        break;
#undef DRAWBUTTONBEVEL
    }
    case PE_IndicatorCheckBox:
    {
#define DRAWBOXBEVEL(mode) DRAWBEVEL(mode,MT_CHECK_BOX)
        //QStyleOptionButton *optbtn = (QStyleOptionButton *)opt;
        if(CHECKMODE(State_On))
        {
            data.flags |= MT_DRAW_MASK;
            qDebug("Checkbox On");
        }
        qDebug("%d %d %d %d",((int)opt->state),
               CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
        if(CHECKMODE(State_Enabled))
        {
            if(CHECKMODE(State_Raised))
            {
                DRAWBOXBEVEL(MT_ACTIVE)
            }
            else if(!CHECKMODE(State_MouseOver))
            {
                DRAWBOXBEVEL(MT_NORMAL)
            }
            else
            {
                qDebug("CheckBox Mouse Active");
                DRAWBOXBEVEL(MT_HOVER)
            }
        }
        else
        {
            DRAWBOXBEVEL(MT_DISABLED)
        }
        break;
#undef DRAWBOXBEVEL
    }
        default:
            BaseStyle::drawPrimitive(element,opt,p,widget);
    }
}
