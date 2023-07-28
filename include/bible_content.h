#ifndef BIBLE_CONTENT_H
#define BIBLE_CONTENT_H

#include <glib-object.h>
#include <curl/curl.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <text_page.h>
#include <preferences_window.h>

#define BIBLE_TYPE_CONTENT (bible_content_get_type())
G_DECLARE_FINAL_TYPE(BibleContent, bible_content, BIBLE, CONTENT, GObject)

G_BEGIN_DECLS

BibleContent *bible_content_new(void);

// GtkTextBuffer *
// bible_content_get_current_text(BibleContent *self);

void bible_content_set_current_chapter_text(BibleContent *self, GtkTextBuffer *buffer);

void bible_content_get_title(BibleContent *self, BibleTextPage *page);

void bible_content_get_languages(BibleContent *self, BiblePreferencesWindow *preferences_window);

void bible_content_get_translations(BibleContent *self, const gchar *language);

void bible_content_get_books(BibleContent *self, BiblePreferencesWindow *preferences_window, const gchar *translation);

void bible_content_get_chapters(BibleContent *self, BiblePreferencesWindow *preferences_window, guint book);

void bible_content_set_current_chapter_next(BibleContent *self);

void bible_content_set_current_chapter_previous(BibleContent *self);

void bible_content_set_page(BibleContent *self, BibleTextPage *page);

void bible_content_get_property(GObject *object,
                                guint prop_id,
                                GValue *value,
                                GParamSpec *pspec);

void bible_content_set_property(GObject *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec *pspec);

G_END_DECLS

#endif