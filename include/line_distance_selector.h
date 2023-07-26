#ifndef LINE_DISTANCE_SELECTOR_H
#define LINE_DISTANCE_SELECTOR_H

#include <gtk/gtk.h>
#include <adwaita.h>

#define LINE_DISTANCE_TYPE_SELECTOR (line_distance_selector_get_type())
G_DECLARE_FINAL_TYPE(LineDistanceSelector, line_distance_selector, LINE_DISTANCE, SELECTOR, GtkWidget)

LineDistanceSelector *
line_distance_selector_new();

 void
line_distance_selector_get_property(GObject *object,
                                    guint prop_id,
                                    GValue *value,
                                    GParamSpec *pspec);

 void
line_distance_selector_set_property(GObject *object,
                                    guint prop_id,
                                    const GValue *value,
                                    GParamSpec *pspec);

#endif