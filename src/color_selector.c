#include <gtk/gtk.h>
#include <adwaita.h>

#include <color_selector.h>

struct _ColorSelector
{
        GtkWidget parent;
        GtkCssProvider *provider;
        GtkWidget *box;
        gchar *color;
};

G_DEFINE_TYPE(ColorSelector, color_selector, GTK_TYPE_WIDGET)

enum
{
        PROP_0,
        PROP_COLOR,
        LAST_PROP,
};

enum
{
        COLOR_SELECTED,
        LAST_SIGNAL,
};

static GParamSpec *properties[LAST_PROP];
static guint signals[LAST_SIGNAL];

static void
color_selector_color_selected(ColorSelector *color_selector,
                              GParamSpec *param)
{
        g_signal_emit(color_selector, signals[COLOR_SELECTED], 0, color_selector->color);
}

static gchar *
color_selector_get_color(ColorSelector *self)
{

        g_return_val_if_fail(COLOR_IS_SELECTOR(self), NULL);

        return self->color;
}

static void
color_selector_set_color(ColorSelector *self, const gchar *color)
{
        g_return_if_fail(COLOR_IS_SELECTOR(self));
        // if (g_strcmp0(color, self->color) != 0)
        // {
        g_free(self->color);
        self->color = g_strdup(color);
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_COLOR]);
        // }
}

void color_selector_get_property(GObject *object,
                                 guint prop_id,
                                 GValue *value,
                                 GParamSpec *pspec)
{
        ColorSelector *self = COLOR_SELECTOR(object);

        switch (prop_id)
        {
        case PROP_COLOR:
                g_value_set_string(value, color_selector_get_color(self));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

void color_selector_set_property(GObject *object,
                                 guint prop_id,
                                 const GValue *value,
                                 GParamSpec *pspec)
{
        ColorSelector *self = COLOR_SELECTOR(object);

        switch (prop_id)
        {
        case PROP_COLOR:
                color_selector_set_color(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

static void
color_selector_init(ColorSelector *self)
{
        AdwStyleManager *style_manager = adw_style_manager_get_default();
        gtk_widget_init_template(GTK_WIDGET(self));
        // g_signal_connect(self->green, "toggled", G_CALLBACK(color_selector_color_selected), "green");
        g_signal_connect(self, "notify::color", G_CALLBACK(color_selector_color_selected), NULL);

        self->provider = gtk_css_provider_new();
        gtk_css_provider_load_from_resource(self->provider, "/org/robertomorrison/gtkbible/color_selector.css");
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(self->provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void
color_selector_dispose(GObject *object)
{
        ColorSelector *self = (ColorSelector *)object;

        g_clear_pointer(&self->box, g_free);
        g_strfreev(&self->color);
        g_clear_object(&self->provider);
        G_OBJECT_CLASS(color_selector_parent_class)->dispose(object);
}

static void
color_selector_class_init(ColorSelectorClass *klass)
{
        G_OBJECT_CLASS(klass)->dispose = color_selector_dispose;
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

        GObjectClass *object_class = G_OBJECT_CLASS(klass);
        object_class->get_property = color_selector_get_property;
        object_class->set_property = color_selector_set_property;

        properties[PROP_COLOR] = g_param_spec_string(
            "color", // Name
            "Color", // Nick
            "Color", // Blurb
            "green",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        signals[COLOR_SELECTED] = g_signal_new(
            "color-selected",
            G_TYPE_FROM_CLASS(klass),
            G_SIGNAL_RUN_LAST,
            0, NULL, NULL, NULL,
            G_TYPE_NONE, 1, G_TYPE_STRING);

        g_object_class_install_properties(object_class, LAST_PROP, properties);
        gtk_widget_class_set_css_name(widget_class, "colorselector");
        gtk_widget_class_install_property_action(widget_class, "color-variant", "color");
        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/gtkbible/color_selector.ui");
        gtk_widget_class_bind_template_child(widget_class, ColorSelector, box);

        gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

ColorSelector *
color_selector_new()
{
        return (ColorSelector *)g_object_new(COLOR_SELECTOR_TYPE, NULL);
}
