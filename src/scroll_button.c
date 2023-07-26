
// #include <glib/gi18n.h>
#include <scroll_button.h>

struct _ScrollButton
{
        GtkWidget parent;

        GtkButton *button;

        GtkCssProvider *provider;
        int max_width;
        gchar *label;
};

G_DEFINE_TYPE(ScrollButton, scroll_button, GTK_TYPE_WIDGET)

enum
{
        PROP_0,
        PROP_MAX_WIDTH,
        PROP_LABEL,
        LAST_PROP,
};

enum
{
        CLICKED,
        LAST_SIGNAL,
};

static GParamSpec *properties[LAST_PROP];
static guint signals[LAST_SIGNAL];

static void
scroll_button_measure(GtkWidget *widget,
                      GtkOrientation orientation,
                      int for_size,
                      int *minimum,
                      int *natural,
                      int *minimum_baseline,
                      int *natural_baseline)
{
        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_INT);
        g_object_get_property(G_OBJECT(widget), "max-width", &value);

        GtkWidget *child = gtk_widget_get_first_child(widget);

        // for (child = gtk_widget_get_first_child(widget);
        //      child != NULL;
        //      child = gtk_widget_get_next_sibling(child))
        // {
        if (gtk_widget_get_visible(child))
        {
                int child_min = 0;
                int child_nat = 0;
                int child_min_baseline = -1;
                int child_nat_baseline = -1;

                gtk_widget_measure(child, orientation, g_value_get_int(&value),
                                   &child_min, &child_nat,
                                   &child_min_baseline, &child_nat_baseline);

                if (orientation == GTK_ORIENTATION_HORIZONTAL)
                {
                        if (child_min > g_value_get_int(&value))
                        {
                                // gtk_widget_add_css_class(GTK_WIDGET(((ScrollButton *)widget)->button), "scroll");
                                scroll_button_set_css_provider((ScrollButton *)widget, MAX(*minimum, child_min));
                                *minimum = g_value_get_int(&value); // MAX(*minimum, child_min);
                                *natural = g_value_get_int(&value); // MAX(*natural, child_nat);
                        }
                        else
                        {
                                scroll_button_set_css_provider((ScrollButton *)widget, 0);
                                *minimum = MAX(*minimum, child_min);
                                *natural = MAX(*natural, child_nat);
                                // gtk_widget_remove_css_class(GTK_WIDGET(((ScrollButton *)widget)->button), "scroll");
                        }
                }
                else
                {
                        *minimum = MAX(*minimum, child_min);
                        *natural = MAX(*natural, child_nat);
                }
                // if (minimum != NULL)
                //         *minimum = child_min;
                // if (natural != NULL)
                //         *natural = child_nat;

                if (child_min_baseline > -1)
                        *minimum_baseline = MAX(*minimum_baseline, child_min_baseline);
                if (child_nat_baseline > -1)
                        *natural_baseline = MAX(*natural_baseline, child_nat_baseline);
        }
        // }
        g_value_unset(&value);
}

static void
scroll_button_allocate(GtkWidget *widget,
                       int width,
                       int height,
                       int baseline)
{

        GtkWidget *child = gtk_widget_get_first_child(widget);
        if (child && gtk_widget_get_visible(child))
        {
                gtk_widget_allocate(child, width, height, baseline, NULL);
                gtk_widget_set_overflow(child, GTK_OVERFLOW_HIDDEN);
        }
}

static void
_on_button_clicked(GtkButton *button,
                   ScrollButton *scroll_button)
{
        g_signal_emit(scroll_button, signals[CLICKED], 0);
}

void scroll_button_set_css_provider(ScrollButton *self, int width)
{
        if (GTK_IS_CSS_PROVIDER(self->provider) && width > 0)
        {

                // const gchar *css = g_strdup_printf(".scroll label {\ncolor: green;\ntransform: translateX(100px);\nanimation:  slide-animation 8s infinite linear;\n}\n@keyframes slide-animation {\nfrom {\ntransform: translateX(50px);\n}\nto {\ntransform: translateX(-50px);\n}\n}");

                gtk_css_provider_load_from_data(self->provider,
                                                g_strdup_printf(".scroll label {\nanimation:  slide-animation 8s infinite linear alternate;\n}\n@keyframes slide-animation {\nfrom {\ntransform: translateX(%ipx);\n}\nto {\ntransform: translateX(-%ipx);\n}\n}", width - (self->max_width * 2) + 25, width - (self->max_width * 2) + 25),
                                                -1);
                // gtk_css_provider_load_from_resource(self->provider, "/org/robertomorrison/gtkbible/scroll_button.css");
                gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                                           GTK_STYLE_PROVIDER(self->provider),
                                                           GTK_STYLE_PROVIDER_PRIORITY_USER);
                // gtk_widget_add_css_class(GTK_WIDGET(self->button), "scroll");

                g_print("translate: %i \n", width - (self->max_width * 2));
                g_print("width: %i \n", width);
                // g_print("css: %s\n", css);
        }
        else
        {
                gtk_style_context_remove_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(self->provider));
        }
}

static void
scroll_button_init(ScrollButton *self)
{
        self->provider = gtk_css_provider_new();

        gtk_widget_init_template(GTK_WIDGET(self));

        // if (!self->provider)
        // {
        // self->provider = gtk_css_provider_new();
        // gtk_css_provider_load_from_resource(self->provider, "/org/robertomorrison/gtkbible/scroll_button.css");
        // gtk_style_context_add_provider_for_display(
        //     gdk_display_get_default(),
        //     GTK_STYLE_PROVIDER(self->provider),
        //     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        // }

        // scroll_button_add_default_provider(self, strlen(self->label));

        g_signal_connect(self->button, "clicked", G_CALLBACK(_on_button_clicked), self);
}

static void
scroll_button_dispose(GObject *object)
{
        ScrollButton *selector = (ScrollButton *)object;

        g_clear_pointer(&selector->provider, g_free);
        g_clear_pointer(&selector->button, g_free);
        // g_clear_pointer(&selector->color, g_free);
        // g_clear_object(G_OBJECT(selector->provider));
        G_OBJECT_CLASS(scroll_button_parent_class)->dispose(object);
}

int scroll_button_get_max_width(ScrollButton *self)
{
        return self->max_width;
}
void scroll_button_set_max_width(ScrollButton *self,
                                 int max_width)
{
        if (self->max_width != max_width)
        {
                self->max_width = max_width;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_MAX_WIDTH]);
        }
}

gchar *scroll_button_get_label(ScrollButton *self)
{
        return self->label;
}
void scroll_button_set_label(ScrollButton *self,
                             const gchar *label)
{
        if (g_strcmp0(label, self->label) != 0)
        {
                self->label = g_strdup(label);

                gtk_button_set_label(self->button, label);
                gtk_widget_queue_resize(GTK_WIDGET(self));

                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
        }
}

void scroll_button_get_property(GObject *object,
                                guint prop_id,
                                GValue *value,
                                GParamSpec *pspec)
{
        ScrollButton *self = SCROLL_BUTTON(object);

        switch (prop_id)
        {
        case PROP_MAX_WIDTH:
                g_value_set_int(value, scroll_button_get_max_width(self));
                break;

        case PROP_LABEL:
                g_value_set_string(value, scroll_button_get_label(self));
                break;
        }
}

void scroll_button_set_property(GObject *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
        ScrollButton *self = SCROLL_BUTTON(object);

        switch (prop_id)
        {
        case PROP_MAX_WIDTH:
                scroll_button_set_max_width(self, g_value_get_int(value));
                break;
        case PROP_LABEL:
                scroll_button_set_label(self, g_value_get_string(value));
                break;
        }
}

static void
scroll_button_class_init(ScrollButtonClass *klass)
{
        G_OBJECT_CLASS(klass)->dispose = scroll_button_dispose;
        GObjectClass *object_class = G_OBJECT_CLASS(klass);
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

        widget_class->size_allocate = scroll_button_allocate;
        widget_class->measure = scroll_button_measure;

        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/gtkbible/scroll_button.ui");
        // gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
        gtk_widget_class_set_css_name(widget_class, "scroll_button");

        gtk_widget_class_bind_template_child(widget_class, ScrollButton, button);

        object_class->get_property = scroll_button_get_property;
        object_class->set_property = scroll_button_set_property;

        properties[PROP_MAX_WIDTH] = g_param_spec_int(
            "max-width", // Name
            "Chapter",   // Nick
            "Chapter",   // Blurb
            0,
            5000,
            200,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_LABEL] = g_param_spec_string(
            "label", // Name
            "Label", // Nick
            "Label", // Blurb
            "",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        g_object_class_install_properties(object_class, LAST_PROP, properties);

        signals[CLICKED] = g_signal_new(
            "clicked",
            G_TYPE_FROM_CLASS(klass),
            G_SIGNAL_RUN_LAST,
            0, NULL, NULL, NULL,
            G_TYPE_NONE, 0);
}

ScrollButton *
scroll_button_new()
{
        return (ScrollButton *)g_object_new(SCROLL_TYPE_BUTTON, NULL);
}
