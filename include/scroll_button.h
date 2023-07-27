#ifndef SCROLL_BUTTON_H
#define SCROLL_BUTTON_H

#include <gtk/gtk.h>
#include <adwaita.h>

#define SCROLL_TYPE_BUTTON (scroll_button_get_type())
G_DECLARE_FINAL_TYPE(ScrollButton, scroll_button, SCROLL, BUTTON, GtkWidget)

void scroll_button_get_property(GObject *object,
                                guint prop_id,
                                GValue *value,
                                GParamSpec *pspec);

void scroll_button_set_property(GObject *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec *pspec);

void scroll_button_set_label(ScrollButton *self,
                             const gchar *label);

void scroll_button_set_max_width(ScrollButton *self,
                                 gint max_width);

ScrollButton *scroll_button_new();

#endif /* BIBLE_APP_H */
