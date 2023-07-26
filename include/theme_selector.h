#ifndef THEME_SELECTOR_H
#define THEME_SELECTOR_H

#include <gtk/gtk.h>
#include <adwaita.h>
// #include <app.h>

#define THEME_SELECTOR_TYPE (theme_selector_get_type())
G_DECLARE_FINAL_TYPE(ThemeSelector, theme_selector, THEME, SELECTOR, GtkWidget)

ThemeSelector *
theme_selector_new();

 void
theme_selector_get_property(GObject *object,
                            guint prop_id,
                            GValue *value,
                            GParamSpec *pspec);

 void
theme_selector_set_property(GObject *object,
                            guint prop_id,
                            const GValue *value,
                            GParamSpec *pspec);

#endif