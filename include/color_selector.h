#ifndef COLOR_SELECTOR_H
#define COLOR_SELECTOR_H

#include <gtk/gtk.h>
#include <adwaita.h>
// #include <app.h>

#define COLOR_SELECTOR_TYPE (color_selector_get_type())
G_DECLARE_FINAL_TYPE(ColorSelector, color_selector, COLOR, SELECTOR, GtkWidget)

ColorSelector *
color_selector_new();

 void
color_selector_get_property(GObject *object,
                            guint prop_id,
                            GValue *value,
                            GParamSpec *pspec);

 void
color_selector_set_property(GObject *object,
                            guint prop_id,
                            const GValue *value,
                            GParamSpec *pspec);

#endif