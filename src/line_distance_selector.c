#include <gtk/gtk.h>
#include <adwaita.h>

#include <line_distance_selector.h>

struct _LineDistanceSelector
{
        GtkWidget parent;
        GtkCssProvider *provider;
        gchar *line_distance;
        GtkButton *line_distance_button;
        GtkBox *box;
};

G_DEFINE_TYPE(LineDistanceSelector, line_distance_selector, GTK_TYPE_WIDGET)

enum
{
        PROP_0,
        PROP_LINE_DISTANCE,
        LAST_PROP,
};

static GParamSpec *properties[LAST_PROP];

static gchar *line_distance_selector_get_line_distance(LineDistanceSelector *self)
{

        g_return_val_if_fail(LINE_DISTANCE_IS_SELECTOR(self), NULL);

        return self->line_distance;
}

static void line_distance_selector_set_line_distance(LineDistanceSelector *self, const gchar *line_distance)
{
        g_return_if_fail(LINE_DISTANCE_IS_SELECTOR(self));
        if (g_strcmp0(line_distance, self->line_distance) != 0)
        {
                g_free(self->line_distance);
                self->line_distance = g_strdup(line_distance);

                gchar icon_name[strlen(self->line_distance) + strlen("text-distance-") + 2];
                snprintf(icon_name, sizeof(icon_name), "%s%s", "text-distance-", self->line_distance);
                gtk_button_set_icon_name(self->line_distance_button, icon_name);
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LINE_DISTANCE]);

                // g_value_set_object()

                // g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LINE_DISTANCE]);
        }
}

void line_distance_selector_get_property(GObject *object,
                                         guint prop_id,
                                         GValue *value,
                                         GParamSpec *pspec)
{
        LineDistanceSelector *self = LINE_DISTANCE_SELECTOR(object);

        switch (prop_id)
        {
        case PROP_LINE_DISTANCE:
                g_value_set_string(value, line_distance_selector_get_line_distance(self));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

void line_distance_selector_set_property(GObject *object,
                                         guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec)
{
        LineDistanceSelector *self = LINE_DISTANCE_SELECTOR(object);

        switch (prop_id)
        {
        case PROP_LINE_DISTANCE:
                line_distance_selector_set_line_distance(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

static void
line_distance_selector_init(LineDistanceSelector *selector)
{
        gtk_widget_init_template(GTK_WIDGET(selector));

        if (!selector->provider)
        {
                selector->provider = gtk_css_provider_new();
                gtk_css_provider_load_from_resource(selector->provider, "/org/robertomorrison/wordofgod/line_distance_selector.css");
                gtk_style_context_add_provider_for_display(
                    gdk_display_get_default(),
                    GTK_STYLE_PROVIDER(selector->provider),
                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }
}

static void
line_distance_selector_dispose(GObject *object)
{
        LineDistanceSelector *self = (LineDistanceSelector *)object;

        g_clear_pointer (&self->box, gtk_widget_unparent);
        g_clear_pointer (&self->line_distance_button, gtk_widget_unparent);
        g_clear_pointer (&self->line_distance, g_free);
        g_clear_pointer(&self->provider, g_free);

        G_OBJECT_CLASS(line_distance_selector_parent_class)->dispose(object);
}

void line_distance_action(GtkWidget *widget,
                          const gchar *action_name,
                          GVariant *parameter)
{
        g_assert(LINE_DISTANCE_IS_SELECTOR(widget));
        LineDistanceSelector *self = LINE_DISTANCE_SELECTOR(widget);
        GObject *object = G_OBJECT(widget);

        GValue get_value = G_VALUE_INIT;
        GValue set_value = G_VALUE_INIT;
        g_value_init(&set_value, G_TYPE_STRING);
        g_value_init(&get_value, G_TYPE_STRING);

        g_object_get_property(object, "line-distance", &get_value);
        const gchar *line_distance = g_value_get_string(&get_value);

        if (g_strcmp0(line_distance, "normal") == 0)
        {
                g_value_set_string(&set_value, "far");
        }
        else if (g_strcmp0(line_distance, "far") == 0)
        {
                g_value_set_string(&set_value, "close");
        }
        else
        {
                g_value_set_string(&set_value, "normal");
        }

        g_object_set_property(object, "line-distance", &set_value);
        g_value_unset(&set_value);
        g_value_unset(&get_value);
}

static void
line_distance_selector_class_init(LineDistanceSelectorClass *klass)
{
        G_OBJECT_CLASS(klass)->dispose = line_distance_selector_dispose;
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

        GObjectClass *object_class = G_OBJECT_CLASS(klass);
        object_class->get_property = line_distance_selector_get_property;
        object_class->set_property = line_distance_selector_set_property;

        properties[PROP_LINE_DISTANCE] = g_param_spec_string(
            "line-distance", // Name
            "line-distance", // Nick
            "Line Distance", // Blurb
            NULL,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        g_object_class_install_properties(object_class, LAST_PROP, properties);
        gtk_widget_class_set_css_name(widget_class, "line_distance_selector");
        // gtk_widget_class_install_action(widget_class, "style-variant", "s", style_variant_to_color_scheme);
        // gtk_widget_class_install_property_action(widget_class, "line-distance", "line-distance");
        gtk_widget_class_install_action(widget_class, "line-distance", NULL, (GtkWidgetActionActivateFunc)line_distance_action);
        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/wordofgod/line_distance_selector.ui");
        gtk_widget_class_bind_template_child(widget_class, LineDistanceSelector, line_distance_button);
        gtk_widget_class_bind_template_child(widget_class, LineDistanceSelector, box);

        gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
        // gtk_widget_class_bind_template_callback(widget_class, _on_SELECTOR_clicked);
}

LineDistanceSelector *
line_distance_selector_new()
{
        return (LineDistanceSelector *)g_object_new(LINE_DISTANCE_TYPE_SELECTOR, NULL);
}
