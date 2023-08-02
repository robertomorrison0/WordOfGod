#include <gtk/gtk.h>
#include <adwaita.h>

#include <window.h>

G_DEFINE_TYPE(BibleAppWindow, bible_app_window, ADW_TYPE_APPLICATION_WINDOW)

#define BIBLE_MINIMUM_FONT_SIZE 8
#define BIBLE_MAXIMUM_FONT_SIZE 40

static void close_window(GtkButton *self, GtkWindow *window)
{
        gtk_window_close(window);
}

static void
bible_app_window_init(BibleAppWindow *self)
{
        g_type_ensure(SCROLL_TYPE_BUTTON);
        g_type_ensure(BIBLE_TEXT_PAGE_TYPE);
        g_type_ensure(BIBLE_PREFERENCES_WINDOW_TYPE);

        gtk_widget_init_template(GTK_WIDGET(self));

        // self->settings = g_settings_new("org.robertomorrison.wordofgod");
        // self->preferences_window = bible_preferences_window_new();
        self->bible_content = bible_content_new();
        bible_preferences_window_set_window(self->preferences_window, self);
        bible_text_page_set_window(self->text_page, self);

        self->provider = gtk_css_provider_new();
        gtk_css_provider_load_from_resource(self->provider, "/org/robertomorrison/wordofgod/window.css");
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(self->provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        // add_font_rows(self);
        // gtk_window_set_resizable(GTK_WINDOW(self), true);
}

void bible_app_window_set_text(BibleAppWindow *self, GtkTextBuffer *buffer)
{
        bible_text_page_set_text(self->text_page, buffer);
        gtk_widget_set_visible(GTK_WIDGET(self->progressbar), false);
}

void bible_app_window_set_title(BibleAppWindow *self, const gchar *title)
{
        bible_text_page_set_title(self->text_page, title);
        gtk_widget_set_visible(GTK_WIDGET(self->progressbar), false);
}

static void
bible_app_window_dispose(GObject *object)
{
        BibleAppWindow *self = BIBLE_APP_WINDOW(object);

        g_clear_object(&self->provider);

        G_OBJECT_CLASS(bible_app_window_parent_class)->dispose(object);
}

static void
bible_app_window_class_init(BibleAppWindowClass *klass)
{
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
        G_OBJECT_CLASS(klass)->dispose = bible_app_window_dispose;

        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/wordofgod/window.ui");

        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, text_page);
        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, progressbar);
        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, preferences_window);
        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, back_button);
        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, stack);

        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, passage_box);
        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, book_button);
        gtk_widget_class_bind_template_child(widget_class, BibleAppWindow, translation_button);

        GtkIconTheme *theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
        gtk_icon_theme_add_resource_path(theme, "/org/robertomorrison/wordofgod/icons");
}

BibleAppWindow *
bible_app_window_new(BibleApp *app)
{
        return (BibleAppWindow *)g_object_new(BIBLE_APP_WINDOW_TYPE, "application", app, NULL);
}
