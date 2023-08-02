#include <gtk/gtk.h>
#include <adwaita.h>

#include <theme_selector.h>

struct _ThemeSelector
{
        GtkWidget parent;
        GtkCssProvider *provider;
        GtkWidget *box;
        // GtkToggleButton *dark;
        // GtkToggleButton *light;
        // GtkToggleButton *follow;
        // GtkToggleButton *black;
        // GtkToggleButton *yellow;
        gchar *theme;
        GtkLayoutManager *layout;
};

G_DEFINE_TYPE(ThemeSelector, theme_selector, GTK_TYPE_WIDGET)

enum
{
        PROP_0,
        PROP_THEME,
        LAST_PROP,
};

static GParamSpec *properties[LAST_PROP];

static gchar *theme_selector_get_theme(ThemeSelector *self)
{

        g_return_val_if_fail(THEME_IS_SELECTOR(self), NULL);

        return self->theme;
}

static void theme_selector_set_theme(ThemeSelector *self, const gchar *theme)
{
        g_return_if_fail(THEME_IS_SELECTOR(self));
        if (g_strcmp0(theme, self->theme) != 0)
        {
                g_free(self->theme);
                self->theme = g_strdup(theme);
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_THEME]);
        }
}

void theme_selector_get_property(GObject *object,
                                 guint prop_id,
                                 GValue *value,
                                 GParamSpec *pspec)
{
        ThemeSelector *self = THEME_SELECTOR(object);

        switch (prop_id)
        {
        case PROP_THEME:
                g_value_set_string(value, theme_selector_get_theme(self));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

void theme_selector_set_property(GObject *object,
                                 guint prop_id,
                                 const GValue *value,
                                 GParamSpec *pspec)
{
        ThemeSelector *self = THEME_SELECTOR(object);

        switch (prop_id)
        {
        case PROP_THEME:
                theme_selector_set_theme(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

static void _on_notify_system_supports_color_schemes(AdwStyleManager *style_manager,
                                                     GParamSpec *param,
                                                     ThemeSelector *selector)
{

        gtk_widget_set_visible(GTK_WIDGET(selector->box), adw_style_manager_get_system_supports_color_schemes(style_manager));
}

static void
theme_selector_init(ThemeSelector *selector)
{
        AdwStyleManager *style_manager = adw_style_manager_get_default();
        gtk_widget_init_template(GTK_WIDGET(selector));

        g_signal_connect(style_manager, "notify::system-supports-color-schemes", G_CALLBACK(_on_notify_system_supports_color_schemes), selector);

        if (!selector->provider)
        {
                selector->provider = gtk_css_provider_new();
                gtk_css_provider_load_from_resource(selector->provider, "/org/robertomorrison/wordofgod/theme_selector.css");
                gtk_style_context_add_provider_for_display(
                    gdk_display_get_default(),
                    GTK_STYLE_PROVIDER(selector->provider),
                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }

        _on_notify_system_supports_color_schemes(style_manager, NULL, selector);
}

static void
theme_selector_dispose(GObject *object)
{
        ThemeSelector *selector;

        selector = (ThemeSelector *)object;

        g_clear_pointer (&selector->box, gtk_widget_unparent);
        g_clear_pointer(&selector->theme, g_free);
        g_clear_pointer(&selector->provider, g_free);
        // g_clear_object(G_OBJECT(selector->style_manager));
        G_OBJECT_CLASS(theme_selector_parent_class)->dispose(object);
}

static void
theme_selector_class_init(ThemeSelectorClass *klass)
{
        G_OBJECT_CLASS(klass)->dispose = theme_selector_dispose;
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

        GObjectClass *object_class = G_OBJECT_CLASS(klass);
        object_class->get_property = theme_selector_get_property;
        object_class->set_property = theme_selector_set_property;

        properties[PROP_THEME] = g_param_spec_string(
            "theme", // Name
            "Theme", // Nick
            "Theme", // Blurb
            "follow",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        g_object_class_install_properties(object_class, LAST_PROP, properties);
        gtk_widget_class_set_css_name(widget_class, "themeselector");
        gtk_widget_class_install_property_action(widget_class, "style-variant", "theme");
        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/wordofgod/theme_selector.ui");
        gtk_widget_class_bind_template_child(widget_class, ThemeSelector, box);

        gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
        // gtk_widget_class_bind_template_callback(widget_class, _on_button_clicked);
}

ThemeSelector *
theme_selector_new()
{
        return (ThemeSelector *)g_object_new(THEME_SELECTOR_TYPE, NULL);
}
