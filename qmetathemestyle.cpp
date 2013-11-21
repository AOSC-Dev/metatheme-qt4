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
#define DRAWSUB(mode,type,ar) mt_engine->draw_widget(mt_engine,p,NULL,type,mode, \
    ar.x(),ar.y(),ar.width(),ar.height(),&data);

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
#if 0
   case CE_TabBarTabLabel:
   {
       //if (opt.isDefault()) break;

       const QStyleOptionTab *opttab = (QStyleOptionTab*)opt;
       //if(opttab->)
       const QTabBar *tb = (const QTabBar *)widget;

       const int t = tb->currentIndex();
       QRect tr = opt->rect;
       if (t == tb->currentIndex()) {
           if (tb->shape() == QTabBar::RoundedNorth || tb->shape() == QTabBar::TriangularNorth) {
               tr.adjust(0, mt_engine->metric[MT_NOTEBOOK_TEXT_OFFSET],0,0);
           }
           else {
               tr.adjust(0, -mt_engine->metric[MT_NOTEBOOK_TEXT_OFFSET],0,0);
           }
       }

       qDebug() << "Text Rect: " << tr << endl;

       int alignment = Qt::AlignCenter/* | ShowPrefix;
#if QT_VERSION >= 0x030300
       if (!styleHint(SH_UnderlineAccelerator, widget, QStyleOption::Default, 0)) {
           alignment |= NoAccel;
       }
#endif*/;

       //drawItem(p, tr, alignment, qpalette, opt->state & State_Enabled, 0, t->text());
       drawItemText(p,tr,alignment,qpalette,true,opttab->text);

       int state;
       state = 0;
       QRect r = opt->rect;
       if ((opt->state & State_HasFocus) && !tb->tabText(t).isEmpty()) {
           mt_engine->draw_widget(mt_engine, (MT_WINDOW *)p, NULL, MT_FOCUS_TAB, state, r.x(), r.y(), r.width(), r.height(), &data);
       }
       break;
   }
#endif
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
#if 0
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt))
        {
            State flags = cmb->state;
            SubControls sub = cmb->subControls;
            if (sub & SC_ComboBoxEditField) {
                if (cmb->frame) {
                    int stateId;
                    if (!(flags & State_Enabled))
                        stateId = MT_DISABLED;
                    /*else if (flags & State_HasFocus)
                        stateId = MT_FOCUSED;*/
                    else
                        stateId = MT_NORMAL;
                    //XPThemeData theme(widget, p, QLatin1String("EDIT"), partId, stateId, r);
                    //d->drawBackground(theme);
                    DRAWBEVEL(stateId,MT_ENTRY)
                } else {
                    QBrush editBrush = cmb->palette.brush(QPalette::Base);
                    p->fillRect(opt->rect, editBrush);
                }
                if (!cmb->editable) {
                    QRect re = proxy()->subControlRect(CC_ComboBox, opt, SC_ComboBoxEditField, widget);
                    if (opt->state & State_HasFocus) {
                        p->fillRect(re, opt->palette.highlight());
                        p->setPen(opt->palette.highlightedText().color());
                        p->setBackground(opt->palette.highlight());
                    } else {
                        p->fillRect(re, opt->palette.base());
                        p->setPen(opt->palette.text().color());
                        p->setBackground(opt->palette.base());
                    }
                }
            }
            if (sub & SC_ComboBoxArrow) {
                int stateId;
                /*XPThemeData theme(widget, p, QLatin1String("COMBOBOX"));
                theme.rect = proxy()->subControlRect(CC_ComboBox, option, SC_ComboBoxArrow, widget);
                partId = CP_DROPDOWNBUTTON;
                if (!(flags & State_Enabled))
                    stateId = CBXS_DISABLED;
                else if (cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_Sunken))
                    stateId = CBXS_PRESSED;
                else if (cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_MouseOver))
                    stateId = CBXS_HOT;
                else
                    stateId = CBXS_NORMAL;
                theme.partId = partId;
                theme.stateId = stateId;
                d->drawBackground(theme);*/
                QRect subrect = proxy()->subControlRect(CC_ComboBox, opt, SC_ComboBoxArrow, widget);
                if (!(flags & State_Enabled))
                    stateId = MT_DISABLED;
                else if (cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_Sunken))
                    stateId = MT_ACTIVE;
                else if (cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_MouseOver))
                    stateId = MT_HOVER;
                else
                    stateId = MT_NORMAL;
                DRAWSUB(stateId,MT_CHOICE_BUTTON,subrect)
            }
        }
        break;
#endif
    case CC_ComboBox:
    {
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt))
        {
            QRect re;
            int state;
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
#if 0
    case CC_ScrollBar:
            QWindowsStyle::drawComplexControl(control,opt,p,widget);
            break;
#endif
    case CC_ScrollBar:
    {
        const QScrollBar *scrollbar = (const QScrollBar *) widget;
        QRect addline, subline, addpage, subpage, slider, first, last;

        int controls = opt->subControls;
        int active = opt->activeSubControls;
        int flags = opt->state;
        int state = MT_NORMAL;
        data.flags = 0;
        if (scrollbar->minimum() == scrollbar->maximum() || (flags & State_Sunken)) {
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

            if ((hoverWidget == widget && hoverPart == 1) || scrollbar->isSliderDown()) state |= MT_HOVER;

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
