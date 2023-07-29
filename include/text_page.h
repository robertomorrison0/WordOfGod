#ifndef TEXT_PAGE_H
#define TEXT_PAGE_H

#include <gtk/gtk.h>
#include <adwaita.h>
#include <app.h>
#include <color_selector.h>
#include <scroll_button.h>

#define BIBLE_TEXT_PAGE_TYPE (bible_text_page_get_type())
G_DECLARE_FINAL_TYPE(BibleTextPage, bible_text_page, BIBLE, TEXT_PAGE, GtkBox)

G_BEGIN_DECLS

BibleTextPage *
bible_text_page_new();

PangoContext *
get_pango_context(BibleTextPage *self);

void bible_text_page_set_translation(BibleTextPage *self, const gchar *translation);

void bible_text_page_set_title(BibleTextPage *self, const gchar *title);

void bible_text_page_scroll_up(BibleTextPage *self);

void bible_text_page_scroll_down(BibleTextPage *self);

GtkTextBuffer *bible_text_page_get_buffer(BibleTextPage *self);

GtkLabel *bible_text_page_get_title_label(BibleTextPage *self);

ScrollButton *bible_text_page_get_book_button(BibleTextPage *self);

ScrollButton *bible_text_page_get_translation_button(BibleTextPage *self);

// static void bible_text_page_set_content(BibleTextPage *self, BibleContent *content);
void bible_text_page_set_window(BibleTextPage *self, gpointer window);

// void bible_text_page_set_text(BibleTextPage *self, GtkTextBuffer *buffer);

void bible_text_page_get_property(GObject *object,
                                  guint prop_id,
                                  GValue *value,
                                  GParamSpec *pspec);

void bible_text_page_set_property(GObject *object,
                                  guint prop_id,
                                  const GValue *value,
                                  GParamSpec *pspec);

G_END_DECLS

#endif