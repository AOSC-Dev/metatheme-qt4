#include "qmetathemestyle.h"

#include "metathemeQt.h"

#include <algorithm>
#include <QtGui>

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

#define SETCOLORROLE(mode,role,g) qpalette.setColor(QPalette::mode,QPalette::role,g.color(QPalette::mode,QPalette::role));
#define SETCOLORGROUP(mode,g) SETCOLORROLE(mode,Background,g) SETCOLORROLE(mode,Foreground,g) SETCOLORROLE(mode,Base,g) \
    SETCOLORROLE(mode,Button,g) SETCOLORROLE(mode,ButtonText,g) SETCOLORROLE(mode,Highlight,g) SETCOLORROLE(mode,HighlightedText,g) \
    SETCOLORROLE(mode,BrightText,g) SETCOLORROLE(mode,Light,g) SETCOLORROLE(mode,Midlight,g) SETCOLORROLE(mode,Dark,g) \
    SETCOLORROLE(mode,Mid,g) SETCOLORROLE(mode,Shadow,g) SETCOLORROLE(mode,Link,g) SETCOLORROLE(mode,LinkVisited,g)

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
   //qpalette.setColor(QPalette::Active,QPalette::NoRole,g.color(QPalette::Active,QPalette::NoRole));
   SETCOLORGROUP(Active,g)
   //qpalette.setInactive(g);
   qpalette.setColor(QPalette::Inactive,QPalette::NoRole,g.color(QPalette::Inactive,QPalette::NoRole));
   SETCOLORGROUP(Inactive,g)

   g.setColor(QPalette::Light, QColor(255, 255, 255));
   i = MT_DISABLED_FOREGROUND; g.setColor(QPalette::Text, QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));
   i = MT_DISABLED_FOREGROUND; g.setColor(QPalette::ButtonText, QColor((*palette)[i].r, (*palette)[i].g, (*palette)[i].b));

   //qpalette.setDisabled(g);
   qpalette.setColor(QPalette::Disabled,QPalette::NoRole,g.color(QPalette::Disabled,QPalette::NoRole));
   SETCOLORGROUP(Disabled,g)

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
#define DRAWSUB(mode,type,ar) mt_engine->draw_widget(mt_engine,p,NULL,type,mode, \
    ar.x(),ar.y(),ar.width(),ar.height(),&data);
#define DRAWBEVELCASE(type) \
if(CHECKMODE(State_Enabled)) \
{ \
    if(CHECKMODE(State_On)) \
    { \
        if(!CHECKMODE(State_MouseOver)) \
        { \
            DRAWBEVEL(MT_ACTIVE,type) \
        } \
        else \
        { \
            DRAWBEVEL(MT_ACTIVE | MT_HOVER,type) \
        } \
    } \
    else if(!CHECKMODE(State_MouseOver)) \
    { \
        DRAWBEVEL(MT_NORMAL,type) \
    } \
    else \
    { \
        qDebug("CheckBox Mouse Active"); \
        DRAWBEVEL(MT_HOVER,type) \
    } \
} \
else \
{ \
    DRAWBEVEL(MT_DISABLED,type) \
}

#define querySubControlMetrics(control, widget, mode, opt) \
    proxy()->subControlRect(control, opt, mode, widget)

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

   case CE_TabBarTab:
       proxy()->drawControl(CE_TabBarTabShape,opt,p,widget);;
       proxy()->drawControl(CE_TabBarTabLabel,opt,p,widget);;
       break;
   case CE_TabBarTabShape:
   {
       const QStyleOptionTab *opttab = (QStyleOptionTab*)opt;
       const QTabBar *tb = (const QTabBar *)widget;
       const int t = tb->currentIndex();
       MT_WIDGET_DATA data;

       data.widget = (MT_WIDGET)widget;

       data.gap_position = (tb->shape() == QTabBar::RoundedNorth || tb->shape() == QTabBar::TriangularNorth)? MT_POSITION_TOP : MT_POSITION_BOTTOM;
       data.gap_x = 0;
       data.gap_width = 0;

       data.flags = 0;
       if (opttab->position == opttab->Beginning || opttab->position == opttab->OnlyOneTab)
           data.flags |= MT_NOTEBOOK_FIRST_VISIBLE_TAB | MT_NOTEBOOK_FIRST_TAB;
       if (opttab->position == opttab->End || opttab->position == opttab->OnlyOneTab)
           data.flags |= MT_NOTEBOOK_LAST_TAB;

       // TODO: special case: Konsole
       /*
      if (tb->parent() && tb->parent()->parent() && tb->parent()->parent()->inherits("Konsole")) {
         data.flags &= ~(MT_NOTEBOOK_FIRST_VISIBLE_TAB | MT_NOTEBOOK_FIRST_TAB);
      }
      */
       QRect r = opt->rect;
       qDebug() << "Tab Rect: " << r << endl;
       int state;
       //state = 0;
       if(CHECKMODE(State_Enabled))
       {
           if(CHECKMODE(State_Selected))
           {
               state = MT_SELECTED;
           }
           else if(!CHECKMODE(State_MouseOver))
           {
               state = MT_NORMAL;
           }
           else
           {
               state = MT_HOVER;
           }
       }
       else
       {
           state = MT_DISABLED;
       }
       p->save();
       p->setClipRect(r);
       mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_NOTEBOOK_TAB, state, r.x(), r.y(), r.width(), r.height(), &data);
       p->restore();
       break;
   }

   case CE_ProgressBarGroove:
   {
      QRect r = opt->rect;
      const QProgressBar *pb = dynamic_cast<const QProgressBar*>(widget);
      if(pb == NULL)break;
      data.flags = 0;
      data.orientation = pb->orientation() == Qt::Horizontal ? MT_HORIZONTAL : MT_VERTICAL;
      mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_PROGRESSBAR, MT_NORMAL, r.x(), r.y(), r.width(), r.height(), &data);
      break;
   }

   case CE_ProgressBarContents:
   {
      QRect r = opt->rect;
      const QProgressBar *pb = dynamic_cast<const QProgressBar*>(widget);
      data.flags = 0;
      data.orientation = pb->orientation() == Qt::Horizontal ? MT_HORIZONTAL : MT_VERTICAL;
      mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_PROGRESSBAR_SLIDER, MT_NORMAL, r.x(), r.y(), (int)((double)r.width() * ((double)pb->value() / (double)pb->maximum())), r.height(), &data);
      break;
   }
      case CE_CheckBox:
      case CE_RadioButton:
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
    mt_color_set(data.background_color, qpalette.background().color().red(), qpalette.background().color().green(), qpalette.background().color().blue());
    int state;
    if(!CHECKMODE(State_Enabled))
    {
        state = MT_DISABLED;
    }
    else
    {
        if(CHECKMODE(State_Active))
            state = MT_ACTIVE;
        else if (CHECKMODE(State_MouseOver))
            state = MT_HOVER;
        else state = MT_NORMAL;
    }
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
    case CC_ComboBox:
    {
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt))
        {
            QRect re;
            int flags = opt->state;
            //if (hoverWidget == widget) state |= MT_HOVER;
            if (!(flags & State_Enabled))
                state = MT_DISABLED;
            else if (cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_Sunken))
                state = MT_ACTIVE;
            else if (cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_MouseOver))
                state = MT_HOVER;
            else
                state = MT_NORMAL;
            p->save();

            if (cmb->subControls & SC_ComboBoxFrame) {
                //re = querySubControlMetrics(control, widget, SC_ComboBoxFrame, opt);
                re = proxy()->subControlRect(CC_ComboBox, opt, SC_ComboBoxFrame, widget);
                p->setClipRect(re);
                mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_CHOICE, state, re.x(), re.y(), re.width(), re.height(), &data);
            }
            if (cmb->subControls & SC_ComboBoxEditField) {
                //re = querySubControlMetrics(control, widget, SC_ComboBoxEditField, opt);
                re = proxy()->subControlRect(CC_ComboBox, opt, SC_ComboBoxEditField, widget);
            }
            if (cmb->subControls & SC_ComboBoxArrow) {
                //re = querySubControlMetrics(control, widget, SC_ComboBoxArrow, opt);
                re = proxy()->subControlRect(CC_ComboBox, opt, SC_ComboBoxArrow, widget);
                p->setClipRect(re);
                mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_CHOICE_BUTTON, state, re.x(), re.y(), re.width(), re.height(), &data);
            }

            p->restore();
            break;
        }
    }
    case CC_ScrollBar:
    {
        const QScrollBar *scrollbar = (const QScrollBar *) widget;
        QRect addline, subline, addpage, subpage, slider, first, last;

        int controls = opt->subControls;
        int active = opt->activeSubControls;
        int flags = opt->state;
        data.flags = 0;
        if (scrollbar->minimum() == scrollbar->maximum()/* || (flags & State_Sunken)*/) {
            data.flags |= MT_SCROLLBAR_UNSCROLLABLE;
        }

        subline = querySubControlMetrics(control, widget, SC_ScrollBarSubLine, opt);
        addline = querySubControlMetrics(control, widget, SC_ScrollBarAddLine, opt);
        subpage = querySubControlMetrics(control, widget, SC_ScrollBarSubPage, opt);
        addpage = querySubControlMetrics(control, widget, SC_ScrollBarAddPage, opt);
        slider  = querySubControlMetrics(control, widget, SC_ScrollBarSlider,  opt);
        first   = querySubControlMetrics(control, widget, SC_ScrollBarFirst,   opt);
        last    = querySubControlMetrics(control, widget, SC_ScrollBarLast,    opt);

        if ((controls & SC_ScrollBarSubLine) && subline.isValid()) {
            if (/*hoverWidget == widget*/CHECKMODE(State_MouseOver) && hoverPart == 2 && !scrollbar->isSliderDown()) state |= MT_HOVER;

            mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL,
                                   (scrollbar->orientation() == Qt::Horizontal)? MT_SCROLLBAR_ARROW_LEFT : MT_SCROLLBAR_ARROW_UP,
                                   state | (active == SC_ScrollBarSubLine? MT_ACTIVE : 0),
                                   subline.x(), subline.y(), subline.width(), subline.height(),
                                   &data);

            state &= ~MT_HOVER;
        }

        if ((controls & SC_ScrollBarAddLine) && addline.isValid()) {
            if (/*hoverWidget == widget*/CHECKMODE(State_MouseOver) && hoverPart == 3 && !scrollbar->isSliderDown()) state = MT_HOVER;
            if(active == SC_ScrollBarAddLine) state = MT_ACTIVE;
            mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL,
                                   (scrollbar->orientation() == Qt::Horizontal)? MT_SCROLLBAR_ARROW_RIGHT : MT_SCROLLBAR_ARROW_DOWN,
                                   state,
                                   addline.x(), addline.y(), addline.width(), addline.height(),
                                   &data);

            state &= ~MT_HOVER;
        }

        if ((controls & SC_ScrollBarSlider) && slider.isValid()) {
            data.orientation = (scrollbar->orientation() == Qt::Horizontal)? MT_HORIZONTAL : MT_VERTICAL;

            if (data.flags & MT_SCROLLBAR_UNSCROLLABLE) {
                QRegion region = slider;
                p->save();
                p->setClipping(true);
                p->setClipRegion(region);
                mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL,
                                       MT_SCROLLBAR,
                                       state | (active == SC_ScrollBarSlider? MT_ACTIVE : 0),
                                       widget->rect().x(), widget->rect().y(), widget->rect().width(), widget->rect().height(),
                                       &data);
                p->restore();
                return;
            }

            if ((CHECKMODE(State_MouseOver) && (active == SC_ScrollBarSlider)) || !scrollbar->isSliderDown()) state |= MT_HOVER;

            if (data.orientation == MT_VERTICAL) {
                data.handle_position = slider.y() - subpage.y();
                data.groove_size = subpage.height() + slider.height() + addpage.height();
            }
            else {
                data.handle_position = slider.x() - subpage.x();
                data.groove_size = subpage.width() + slider.width() + addpage.width();
            }

            mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL,
                                   MT_SCROLLBAR_HANDLE,
                                   state | (active == SC_ScrollBarSlider? MT_ACTIVE : 0),
                                   slider.x(), slider.y(), slider.width(), slider.height(),
                                   &data);

            state &= ~MT_HOVER;
        }

        if (((controls & SC_ScrollBarSubPage) && subpage.isValid()) ||
                ((controls & SC_ScrollBarAddPage) && addpage.isValid())) {
            QRegion region;
            p->save();
            p->setClipping(true);
            if (controls & SC_ScrollBarSubPage) region = region.unite(subpage);
            if (controls & SC_ScrollBarAddPage) region = region.unite(addpage);
            p->setClipRegion(region);

            MT_WIDGET_DATA data;
            data.widget = (MT_WIDGET)widget;
            data.orientation = (scrollbar->orientation() == Qt::Horizontal)? MT_HORIZONTAL : MT_VERTICAL;

            if (active == SC_ScrollBarSubPage) data.flags |= MT_SCROLLBAR_SUBPAGE_ACTIVE;
            if (active == SC_ScrollBarAddPage) data.flags |= MT_SCROLLBAR_ADDPAGE_ACTIVE;

            if (data.flags & (MT_SCROLLBAR_SUBPAGE_ACTIVE | MT_SCROLLBAR_ADDPAGE_ACTIVE)) {
                if (data.orientation == MT_VERTICAL) {
                    data.handle_position = slider.y() - subpage.y();
                    data.groove_size = slider.height();
                }
                else {
                    data.handle_position = slider.x() - subpage.x();
                    data.groove_size = slider.width();
                }
            }

            mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL,
                                   MT_SCROLLBAR,
                                   state,
                                   widget->rect().x(),
                                   widget->rect().y(),
                                   widget->rect().width(),
                                   widget->rect().height(),
                                   &data);
            p->restore();
        }
        break;
    }
    case CC_Slider:
    {
       const QSlider *sl = (const QSlider *)widget;
       QRect groove = querySubControlMetrics(CC_Slider, widget, SC_SliderGroove, opt);
       QRect handle = querySubControlMetrics(CC_Slider, widget, SC_SliderHandle, opt);

       data.orientation = (sl->orientation() == Qt::Horizontal)? MT_HORIZONTAL : MT_VERTICAL;

       if (opt->subControls & SC_SliderGroove) {
          mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_SCALE, state, groove.x(), groove.y(), groove.width(), groove.height(), &data);
       }

       if (opt->subControls & SC_SliderTickmarks) {
          QCommonStyle::drawComplexControl(control,opt,p,widget);
       }

       if (opt->subControls & SC_SliderHandle) {
          mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_SCALE_HANDLE, state, handle.x(), handle.y(), handle.width(), handle.height(), &data);
       }
       break;
    }


        default:
            BaseStyle::drawComplexControl(control,opt,p,widget);
    }
}

void QMetaThemeStyle::drawPrimitive ( PrimitiveElement element, const QStyleOption * opt, QPainter * p, const QWidget * widget) const
{
    MT_WIDGET_DATA data;
    mt_color_set(data.background_color, qpalette.background().color().red(), qpalette.background().color().green(), qpalette.background().color().blue());
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
            //data.flags |= MT_DRAW_MASK;
            qDebug("Checkbox On");
        }
        qDebug("%d %d %d %d",((int)opt->state),
               CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
        if(CHECKMODE(State_Enabled))
        {
            if(CHECKMODE(State_On))
            {
                if(!CHECKMODE(State_MouseOver))
                {
                    DRAWBOXBEVEL(MT_ACTIVE)
                }
                else
                {
                    DRAWBOXBEVEL(MT_ACTIVE | MT_HOVER)
                }
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
        case PE_IndicatorRadioButton:
    {
#define DRAWBOXBEVEL(mode) DRAWBEVEL(mode,MT_RADIO_BOX)
        //QStyleOptionButton *optbtn = (QStyleOptionButton *)opt;
        if(CHECKMODE(State_On))
        {
            //data.flags |= MT_DRAW_MASK;
            qDebug("Checkbox On");
        }
        qDebug("%d %d %d %d",((int)opt->state),
               CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
        if(CHECKMODE(State_Enabled))
        {
            if(CHECKMODE(State_On))
            {
                if(!CHECKMODE(State_MouseOver))
                {
                    DRAWBOXBEVEL(MT_ACTIVE)
                }
                else
                {
                    DRAWBOXBEVEL(MT_ACTIVE | MT_HOVER)
                }
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
    case PE_PanelLineEdit:
    {
#define DRAWBOXBEVEL(mode) DRAWBEVEL(mode,MT_ENTRY_BORDER)
        //QStyleOptionButton *optbtn = (QStyleOptionButton *)opt;
        if(CHECKMODE(State_On))
        {
            //data.flags |= MT_DRAW_MASK;
            qDebug("Checkbox On");
        }
        qDebug("%d %d %d %d",((int)opt->state),
               CHECKMODE(State_Enabled),CHECKMODE(State_Raised),CHECKMODE(State_MouseOver));
        if(CHECKMODE(State_Enabled))
        {
            if(CHECKMODE(State_On))
            {
                if(!CHECKMODE(State_MouseOver))
                {
                    DRAWBOXBEVEL(MT_ACTIVE)
                }
                else
                {
                    DRAWBOXBEVEL(MT_ACTIVE | MT_HOVER)
                }
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
    case PE_IndicatorSpinUp:
    {DRAWBEVELCASE(MT_SPINBUTTON_UP)}break;
    case PE_IndicatorSpinDown:
    {DRAWBEVELCASE(MT_SPINBUTTON_DOWN)}break;
    case PE_IndicatorSpinPlus:
    {DRAWBEVELCASE(MT_SPINBUTTON_UP)}break;
    case PE_IndicatorSpinMinus:
    {DRAWBEVELCASE(MT_SPINBUTTON_DOWN)}break;
        case PE_PanelMenuBar:
        {DRAWBEVELCASE(MT_MENUBAR)}break;
        break;
        default:
            BaseStyle::drawPrimitive(element,opt,p,widget);
    }
}

int QMetaThemeStyle::pixelMetric ( PixelMetric metric, const QStyleOption * opt, const QWidget * w) const
{
    switch(metric)
    {
    case PM_TabBarTabOverlap: // number of pixels the tabs should overlap.
       return mt_engine->metric[MT_NOTEBOOK_TAB_OVERLAP];

    case PM_TabBarTabHSpace: // extra space added to the tab width.
       return 24;

    case PM_TabBarTabVSpace: // extra space added to the tab height.
       return 10;

    case PM_TabBarBaseHeight: // height of the area between the tab bar and the tab pages.
       return 0;

    case PM_TabBarBaseOverlap: // number of pixels the tab bar overlaps the tab bar base.
       return mt_engine->metric[MT_NOTEBOOK_OVERLAP] - 2;

    case PM_TabBarScrollButtonWidth:
       return mt_engine->metric[MT_NOTEBOOK_ARROW_WIDTH];

    case PM_TabBarTabShiftHorizontal: // horizontal pixel shift when a tab is selected.
       return 0;

    case PM_TabBarTabShiftVertical: // vertical pixel shift when a tab is selected.
    {
       if (w) {
          const QTabBar *tb = static_cast<const QTabBar*>(w);
          if (tb->shape() == QTabBar::RoundedSouth || tb->shape() == QTabBar::TriangularSouth) {
             return -mt_engine->metric[MT_NOTEBOOK_TEXT_OFFSET];
          }
       }
       return mt_engine->metric[MT_NOTEBOOK_TEXT_OFFSET];
    }
        default:
        return QWindowsStyle::pixelMetric(metric,opt,w);
    }
}

QPalette QMetaThemeStyle::standardPalette() const
{
    return qpalette;
}

QRect QMetaThemeStyle::subControlRect ( ComplexControl control, const QStyleOptionComplex * opt, SubControl subControl, const QWidget * w) const
{
    switch(control)
    {
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            QRect ret;
            int x = cb->rect.x(),
                y = cb->rect.y(),
                wi = cb->rect.width(),
                he = cb->rect.height();
            int xpos = x;
            int margin = cb->frame ? 3 : 0;
            int bmarg = cb->frame ? 2 : 0;
            xpos += wi - bmarg - 16;


            switch (subControl) {
            case SC_ComboBoxFrame:
                ret = cb->rect;
                break;
            case SC_ComboBoxArrow:
                ret.setRect(xpos, y, std::min(wi/2,mt_engine->metric[MT_CHOICE_BUTTON_WIDTH]), std::max(he,mt_engine->metric[MT_CHOICE_BUTTON_WIDTH]));
                break;
            case SC_ComboBoxEditField:
                ret.setRect(x + margin, y + margin, wi - 2 * margin - 16, he - 2 * margin);
                break;
            case SC_ComboBoxListBoxPopup:
                ret = cb->rect;
                break;
            default:
                break;
            }
            ret = visualRect(cb->direction, cb->rect, ret);
            return ret;
        }

        default:
            return QWindowsStyle::subControlRect(control,opt,subControl,w);
    }
}

void QMetaThemeStyle::polish ( QApplication * application )
{
    QWindowsStyle::polish(application);
    application->setPalette(qpalette);
}

int QMetaThemeStyle::styleHint ( StyleHint hint, const QStyleOption * opt, const QWidget * w, QStyleHintReturn * ret) const
{
    switch(hint)
    {
        default:
            return QWindowsStyle::styleHint(hint,opt,w,ret);
    }
}
