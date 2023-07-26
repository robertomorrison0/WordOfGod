#ifndef APP_WINDOW_H
#define APP_WINDOW_H

// #include <gtk/gtk.h>
// #include <adwaita.h>
#include <app.h>
#include <adw-carousel.h>
#include <bible_content.h>
#include <text_page.h>
#include <theme_selector.h>
#include <preferences_window.h>
#include <line_distance_selector.h>
#include <scroll_button.h>

#define BIBLE_APP_WINDOW_TYPE (bible_app_window_get_type())
G_DECLARE_FINAL_TYPE(BibleAppWindow, bible_app_window, BIBLE, APP_WINDOW, AdwApplicationWindow)

struct _BibleAppWindow
{
        GtkApplicationWindow parent;
        BibleTextPage *text_page;

        // GSettings *settings;
        GtkCssProvider *provider;

        GtkProgressBar *progressbar;

        BiblePreferencesWindow *preferences_window;

        BibleContent *bible_content;

        GtkButton *back_button;
        GtkBox *passage_box;
        ScrollButton *book_button;
        GtkButton *translation_button;

        AdwViewStack *stack;
};

G_BEGIN_DECLS

BibleAppWindow *bible_app_window_new(BibleApp *app);

void bible_app_window_set_text(BibleAppWindow *self, GtkTextBuffer *buffer);

void bible_app_window_set_title(BibleAppWindow *self, const gchar *title);

void bible_app_window_open(BibleAppWindow *win,
                           GFile *file);

G_END_DECLS

#endif