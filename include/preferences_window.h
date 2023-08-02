#ifndef BIBLE_PREFERENCES_WINDOW_H
#define BIBLE_PREFERENCES_WINDOW_H

#include <gtk/gtk.h>
#include <adwaita.h>
#include <theme_selector.h>
#include <line_distance_selector.h>
#include <text_page.h>

#define BIBLE_PREFERENCES_WINDOW_TYPE (bible_preferences_window_get_type())
G_DECLARE_FINAL_TYPE(BiblePreferencesWindow, bible_preferences_window, BIBLE, PREFERENCES_WINDOW, AdwBin)

BiblePreferencesWindow *bible_preferences_window_new();

// void bible_preferences_window_set_content(BiblePreferencesWindow *self, BibleContent *content);

void bible_preferences_window_navigate_page_name(BiblePreferencesWindow *self, const gchar *page);

void bible_preferences_window_navigate_passage_name(BiblePreferencesWindow *self, const gchar *passage);

void bible_preferences_window_add_languages(BiblePreferencesWindow *self, GtkStringList *list);

void bible_preferences_window_add_translations(BiblePreferencesWindow *self, GtkStringList *list);

void bible_preferences_window_add_books(BiblePreferencesWindow *self, GtkStringList *list, guint size);

void bible_preferences_window_add_chapters(BiblePreferencesWindow *self, GtkStringList *list, guint size);

void bible_preferences_window_set_window(BiblePreferencesWindow *self, gpointer window);

#endif /* __EXAMPLEAPP_H */
