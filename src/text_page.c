#include <gtk/gtk.h>
#include <adwaita.h>

#include <text_page.h>
#include <bible_content.h>
#include <window.h>
#include <json-c/json.h>

typedef struct HighlightData
{
        guint hightlight_verse;
        guint chapter;
        guint book;
        gchar *color;
        guint start;
        guint end;

} HighlightData;

struct _BibleTextPage
{
        GtkBox parent;

        GtkLabel *text_title;
        GtkTextView *text_view;
        GtkTextBuffer *text_buffer;

        gchar *line_distance;
        guint font_size;
        gchar *font;
        gchar *highlights_string;
        GtkCssProvider *provider;
        ColorSelector *color_selector;

        GtkScrolledWindow *scrolled;
        GtkOverlay *overlay_box;

        HighlightData hightlight_data;
        GtkTextIter hightlight_start_iter;
        GtkTextIter hightlight_end_iter;

        BibleContent *bible_content;

        GtkButton *next_chapter;
        GtkButton *previous_chapter;

        GtkEventController *event_controller;

        ScrollButton *book_button;
        ScrollButton *translation_button;

        GtkLabel *book_label;
        GtkLabel *translation_label;

        GtkBox *navigation_box;
        GtkBox *navigation_box_hidden;

        double last_scroll_value;
};

G_DEFINE_TYPE(BibleTextPage, bible_text_page, GTK_TYPE_BOX)

enum
{
        PROP_0,
        PROP_LINE_DISTANCE,
        PROP_FONT_SIZE,
        PROP_FONT,
        PROP_HIGHLIGHTS,
        LAST_PROP,
};

static GParamSpec *properties[LAST_PROP];

GtkTextBuffer *bible_text_page_get_buffer(BibleTextPage *self)
{
        if (GTK_IS_TEXT_BUFFER(self->text_buffer))
                return self->text_buffer;

        self->text_buffer = gtk_text_buffer_new(NULL);
        gtk_text_view_set_buffer(self->text_view, self->text_buffer);
        return self->text_buffer;
}

GtkLabel *bible_text_page_get_title_label(BibleTextPage *self)
{
        if (GTK_IS_TEXT_BUFFER(self->text_buffer))
                return self->text_title;

        self->text_title = (GtkLabel *)gtk_label_new("");
        return self->text_title;
}

ScrollButton *bible_text_page_get_book_button(BibleTextPage *self)
{
        if (SCROLL_IS_BUTTON(self->translation_button))
                return self->translation_button;

        return NULL;
}

ScrollButton *bible_text_page_get_translation_button(BibleTextPage *self)
{
        if (SCROLL_IS_BUTTON(self->book_button))
                return self->book_button;

        return NULL;
}

void bible_text_page_set_title(BibleTextPage *self, const gchar *title)
{
        gtk_label_set_markup(self->text_title, title);
        // gtk_label_set_use_markup(GTK_LABEL(gtk_button_get_child(self->book)), true);
        // gtk_button_set_label(self->book_button, title);
        scroll_button_set_label(self->book_button, title);
        gtk_label_set_label(self->book_label, title);
}

void bible_text_page_set_translation(BibleTextPage *self, const gchar *translation)
{
        if (SCROLL_IS_BUTTON(self->translation_button))
                scroll_button_set_label(self->translation_button, g_strdup(translation));
        gtk_label_set_label(self->translation_label, translation);
}

PangoContext *
get_pango_context(BibleTextPage *self)
{
        return gtk_text_view_get_rtl_context(self->text_view);
}

void _on_previous_chapter_button_clicked(BibleTextPage *self)
{
        // gtk_progress_bar_pulse(self->progressbar);
        // bible_content_set_current_chapter_previous(self->bible_content);
        bible_content_set_current_chapter_previous(self->bible_content); //, self->text_buffer);
}

void _on_next_chapter_button_clicked(BibleTextPage *self)
{
        // gtk_progress_bar_pulse(self->progressbar);
        bible_content_set_current_chapter_next(self->bible_content);
}

static void
highlight_in_page(BibleTextPage *self)
{
        json_object *export_json = json_tokener_parse(self->highlights_string);
        json_object *highlight_object;

        const gchar color_key[] = "color";
        const gchar book_key[] = "book";
        const gchar chapter_key[] = "chapter";
        // const gchar verse_key[] = "verse";
        const gchar start_key[] = "start";
        const gchar end_key[] = "end";

        json_object *color_object;
        json_object *book_object;
        json_object *chapter_object;
        // json_object *verse_object;
        json_object *start_object;
        json_object *end_object;

        guint start_offset;
        guint end_offset;
        GtkTextIter start_iter;
        GtkTextIter end_iter;

        size_t n_highlights = json_object_array_length(export_json);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(self->text_view);
        gtk_text_buffer_get_start_iter(buffer, &start_iter);
        gtk_text_buffer_get_end_iter(buffer, &end_iter);

        for (size_t i = 0; i < n_highlights; i++)
        {
                highlight_object = json_object_array_get_idx(export_json, i);
                json_object_object_get_ex(highlight_object, book_key, &book_object);
                json_object_object_get_ex(highlight_object, chapter_key, &chapter_object);

                if (json_object_get_int(chapter_object) != self->hightlight_data.chapter ||
                    json_object_get_int(book_object) != self->hightlight_data.book)
                        continue;

                json_object_object_get_ex(highlight_object, color_key, &color_object);
                json_object_object_get_ex(highlight_object, start_key, &start_object);
                json_object_object_get_ex(highlight_object, end_key, &end_object);

                g_assert(color_object != NULL);
                g_assert(book_object != NULL);
                g_assert(chapter_object != NULL);
                g_assert(start_object != NULL);
                g_assert(end_object != NULL);

                start_offset = json_object_get_int(start_object);
                end_offset = json_object_get_int(end_object);

                gtk_text_iter_set_offset(&start_iter, start_offset);
                gtk_text_iter_set_offset(&end_iter, end_offset);

                gtk_text_buffer_apply_tag_by_name(buffer,
                                                  json_object_get_string(color_object),
                                                  &start_iter,
                                                  &end_iter);
        }
}

void _on_text_changed(GtkTextBuffer *buffer, BibleTextPage *self)
{

        GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(self->scrolled);
        gtk_adjustment_set_value(vadjustment, 0.0);
        gtk_scrolled_window_set_vadjustment(self->scrolled, vadjustment);
        highlight_in_page(self);
}

void bible_text_page_scroll_up(BibleTextPage *self)
{
        GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(self->scrolled);
        gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_value(vadjustment) - 5.0);

        gtk_scrolled_window_set_vadjustment(self->scrolled, vadjustment);
}

void bible_text_page_scroll_down(BibleTextPage *self)
{
        GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(self->scrolled);
        gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_value(vadjustment) + 5.0);

        gtk_scrolled_window_set_vadjustment(self->scrolled, vadjustment);
}

static gchar *
bible_text_page_get_line_distance(BibleTextPage *self)
{

        g_return_val_if_fail(BIBLE_IS_TEXT_PAGE(self), NULL);

        return self->line_distance;
}

static void
bible_text_page_reset_line_distance(BibleTextPage *self)
{
        g_return_if_fail(BIBLE_IS_TEXT_PAGE(self));
        if (g_strcmp0(self->line_distance, "far") == 0)
        {
                gtk_text_view_set_pixels_inside_wrap(self->text_view, self->font_size);
        }
        else if (g_strcmp0(self->line_distance, "normal") == 0)
        {
                gtk_text_view_set_pixels_inside_wrap(self->text_view, self->font_size / 2);
        }
        else
        {
                gtk_text_view_set_pixels_inside_wrap(self->text_view, self->font_size / 4);
        }
}

static void
bible_text_page_set_line_distance(BibleTextPage *self, const gchar *line_distance)
{
        g_return_if_fail(BIBLE_IS_TEXT_PAGE(self));
        if (g_strcmp0(line_distance, self->line_distance) != 0)
        {
                g_free(self->line_distance);
                self->line_distance = g_strdup(line_distance);

                bible_text_page_reset_line_distance(self);

                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LINE_DISTANCE]);
        }
}

static guint
bible_text_page_get_font_size(BibleTextPage *self)
{
        g_assert(BIBLE_IS_TEXT_PAGE(self));

        return self->font_size;
}

static void
bible_text_page_activate_font(BibleTextPage *self, const gchar *font_name, guint font_size)
{
        if (!self->provider)
        {
                self->provider = gtk_css_provider_new();
        }
        gchar font[200];
        g_snprintf(font, sizeof(font), "textview.chapter-content{\nfont:\t%ipx \"%s\";}\n label.chapter-title{\nfont:\t%ipx \"%s\";\n}", font_size, font_name, font_size + 3, font_name);

        gtk_css_provider_load_from_data(self->provider, font, -1);
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(self->provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void
bible_text_page_set_font_size(BibleTextPage *self, guint font_size)
{
        g_return_if_fail(BIBLE_IS_TEXT_PAGE(self));
        self->font_size = font_size;

        bible_text_page_activate_font(self, self->font, font_size);
        bible_text_page_reset_line_distance(self);

        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_FONT_SIZE]);
}

static gchar *
bible_text_page_get_font(BibleTextPage *self)
{

        g_return_val_if_fail(BIBLE_IS_TEXT_PAGE(self), NULL);

        return self->font;
}

static void
bible_text_page_set_font(BibleTextPage *self, const gchar *font)
{
        g_return_if_fail(BIBLE_IS_TEXT_PAGE(self));
        g_free(self->font);
        self->font = g_strdup(font);
        bible_text_page_activate_font(self, self->font, self->font_size);
        bible_text_page_reset_line_distance(self);

        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_FONT]);
}

static gchar *
bible_text_page_get_highlights(BibleTextPage *self)
{

        g_return_val_if_fail(BIBLE_IS_TEXT_PAGE(self), NULL);

        return self->highlights_string;
}

static void
bible_text_page_set_highlights(BibleTextPage *self, const gchar *highlights)
{
        g_return_if_fail(BIBLE_IS_TEXT_PAGE(self));
        g_free(self->highlights_string);
        self->highlights_string = g_strdup(highlights);

        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_HIGHLIGHTS]);
}

void bible_text_page_get_property(GObject *object,
                                  guint prop_id,
                                  GValue *value,
                                  GParamSpec *pspec)
{
        BibleTextPage *self = BIBLE_TEXT_PAGE(object);

        switch (prop_id)
        {
        case PROP_LINE_DISTANCE:
                g_value_set_string(value, bible_text_page_get_line_distance(self));
                break;

        case PROP_FONT_SIZE:
                g_value_set_uint(value, bible_text_page_get_font_size(self));
                break;

        case PROP_FONT:
                g_value_set_string(value, bible_text_page_get_font(self));
                break;

        case PROP_HIGHLIGHTS:
                g_value_set_string(value, bible_text_page_get_highlights(self));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

void bible_text_page_set_property(GObject *object,
                                  guint prop_id,
                                  const GValue *value,
                                  GParamSpec *pspec)
{
        BibleTextPage *self = BIBLE_TEXT_PAGE(object);

        switch (prop_id)
        {
        case PROP_LINE_DISTANCE:
                bible_text_page_set_line_distance(self, g_value_get_string(value));
                break;
        case PROP_FONT_SIZE:
                bible_text_page_set_font_size(self, g_value_get_uint(value));
                break;

        case PROP_FONT:
                bible_text_page_set_font(self, g_value_get_string(value));
                break;

        case PROP_HIGHLIGHTS:
                bible_text_page_set_highlights(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
                g_print("\n::%s\n\n", g_value_get_string(value));
        }
}

static void
_on_text_long_pressed(GtkGestureLongPress *self,
                      BibleTextPage *page)
{
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(page->text_view);
        GtkTextIter position;
        gint pos;
        gchar index_str[16];
        gint index = 0;

        // g_signal_emit_by_name(buffer, "move-cursor");
        g_object_get(buffer, "cursor-position", &pos, NULL);
        gtk_text_buffer_get_iter_at_offset(buffer, &position, pos);

        gtk_text_buffer_get_start_iter(buffer, &page->hightlight_start_iter);
        gtk_text_buffer_get_end_iter(buffer, &page->hightlight_end_iter);
        // GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);

        // gtk_text_tag_table_lookup()

        // gtk_text_buffer_remove_tag_by_name(buffer,
        //                                    "highlighted",
        //                                    &page->hightlight_start_iter,
        //                                    &page->hightlight_end_iter);
        while (true)
        {
                sprintf(index_str, "Verse%i", index);
                GtkTextMark *mark = gtk_text_buffer_get_mark(buffer, index_str);

                if (!GTK_IS_TEXT_MARK(mark))
                {
                        break;
                }
                GtkTextIter iter;
                gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark);
                gint result = gtk_text_iter_compare(&position, &iter);

                if (result < 0)
                {
                        // lhs is less than rhs
                        gtk_text_iter_assign(&page->hightlight_end_iter, &iter);
                        page->hightlight_data.hightlight_verse = index;
                        break;
                }
                else if (result > 0)
                {
                        // lhs is greater than rhs
                        gtk_text_iter_assign(&page->hightlight_start_iter, &iter);
                }
                else
                {
                        // lhs is less than rhs
                        gtk_text_iter_assign(&page->hightlight_end_iter, &iter);
                        // g_print("index: %i, mark: %s\n", index, index_str);
                        page->hightlight_data.hightlight_verse = index;

                        break;
                }
                memset(index_str, 0, strlen(index_str));
                index++;
        }

        gtk_text_buffer_apply_tag_by_name(buffer,
                                          "highlighted",
                                          &page->hightlight_start_iter,
                                          &page->hightlight_end_iter);

        GdkRectangle loc;
        gtk_text_view_get_iter_location(page->text_view, &position, &loc);

        gint x, y;
        gtk_text_view_buffer_to_window_coords(page->text_view, GTK_TEXT_WINDOW_TEXT, loc.x, loc.y, &x, &y);

        gtk_widget_set_visible(GTK_WIDGET(page->color_selector), true);
        gtk_text_view_move_overlay(page->text_view, GTK_WIDGET(page->color_selector), 0, y);
}

static void save_highlight(BibleTextPage *self)
{
        json_object *export_json = json_tokener_parse(self->highlights_string);

        json_object *highlight_object = json_object_new_object();
        json_object *color_object = json_object_new_string(self->hightlight_data.color);
        json_object *book_object = json_object_new_int(self->hightlight_data.book);
        json_object *chapter_object = json_object_new_int(self->hightlight_data.chapter);
        json_object *verse_object = json_object_new_int(self->hightlight_data.hightlight_verse);
        json_object *start_object = json_object_new_int(self->hightlight_data.start);
        json_object *end_object = json_object_new_int(self->hightlight_data.end);

        g_assert(export_json != NULL);
        g_assert(highlight_object != NULL);
        g_assert(color_object != NULL);
        g_assert(book_object != NULL);
        g_assert(chapter_object != NULL);
        g_assert(verse_object != NULL);
        g_assert(start_object != NULL);
        g_assert(end_object != NULL);

        const gchar color_key[] = "color";
        const gchar book_key[] = "book";
        const gchar chapter_key[] = "chapter";
        const gchar verse_key[] = "verse";
        const gchar start_key[] = "start";
        const gchar end_key[] = "end";

        if (g_strcmp0(self->hightlight_data.color, "none") == 0)
        {
                size_t n_highlights = json_object_array_length(export_json);
                for (size_t i = 0; i < n_highlights; i++)
                {
                        highlight_object = json_object_array_get_idx(export_json, i);
                        json_object_object_get_ex(highlight_object, book_key, &book_object);
                        json_object_object_get_ex(highlight_object, chapter_key, &chapter_object);

                        if (json_object_get_int(chapter_object) != self->hightlight_data.chapter ||
                            json_object_get_int(book_object) != self->hightlight_data.book ||
                            json_object_get_int(verse_object) != self->hightlight_data.hightlight_verse)
                                continue;

                        json_object_array_del_idx(export_json, i, 1);
                }

                bible_text_page_set_highlights(self, json_object_get_string(export_json));
                return;
        }

        gchar *array_key = g_strdup_printf("%i%i%i", self->hightlight_data.book,
                                           self->hightlight_data.chapter,
                                           self->hightlight_data.hightlight_verse);

        json_object_object_add(highlight_object, color_key, color_object);
        json_object_object_add(highlight_object, book_key, book_object);
        json_object_object_add(highlight_object, chapter_key, chapter_object);
        json_object_object_add(highlight_object, verse_key, verse_object);
        json_object_object_add(highlight_object, start_key, start_object);
        json_object_object_add(highlight_object, end_key, end_object);

        json_object_array_add(export_json, highlight_object);
        bible_text_page_set_highlights(self, json_object_get_string(export_json));

        g_free(array_key);
}

static void
color_selected(ColorSelector *self,
               const gchar *color,
               BibleTextPage *page)
{
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(page->text_view);
        gtk_text_buffer_remove_all_tags(buffer,
                                        &page->hightlight_start_iter,
                                        &page->hightlight_end_iter);

        if (g_strcmp0(color, "none") != 0)
        {
                // gtk_widget_set_visible(GTK_WIDGET(self), false);
                // gtk_text_buffer_remove_tag_by_name(buffer, "highlighted", &page->hightlight_start_iter, &page->hightlight_end_iter);
                gtk_text_buffer_apply_tag_by_name(buffer,
                                                  color,
                                                  &page->hightlight_start_iter,
                                                  &page->hightlight_end_iter);
        }
        page->hightlight_data.color = g_strdup(color);

        page->hightlight_data.start = gtk_text_iter_get_offset(&page->hightlight_start_iter);
        page->hightlight_data.end = gtk_text_iter_get_offset(&page->hightlight_end_iter);
        save_highlight(page);
        gtk_widget_set_visible(GTK_WIDGET(self), false);
}

static gboolean
event_key_pressed_cb(GtkEventControllerKey *self,
                     guint keyval,
                     guint keycode,
                     GdkModifierType *state,
                     BibleTextPage *page)
{
        g_assert(BIBLE_IS_TEXT_PAGE(page));

        // if ((*state) & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_ALT_MASK))
        //         return FALSE;

        if (keyval == GDK_KEY_Right)
        {
                gtk_widget_activate_action_variant(GTK_WIDGET(page->next_chapter), "chapter-choose", g_variant_new_string("next"));
        }
        else if (keyval == GDK_KEY_Left)
        {
                gtk_widget_activate_action_variant(GTK_WIDGET(page->previous_chapter), "chapter-choose", g_variant_new_string("previous"));
        }

        if (keyval == GDK_KEY_Up)
        {
                // gtk_widget_activate_action_variant(GTK_WIDGET(win->next_chapter), "chapter-choose", g_variant_new_string("next"));

                bible_text_page_scroll_up(page);
        }
        else if (keyval == GDK_KEY_Down)
        {
                // gtk_widget_activate_action_variant(GTK_WIDGET(win->previous_chapter), "chapter-choose", g_variant_new_string("previous"));

                bible_text_page_scroll_down(page);
        }

        return TRUE;
}

static gboolean
event_key_released_cb(GtkEventControllerKey *self,
                      guint keyval,
                      guint keycode,
                      GdkModifierType *state,
                      BibleTextPage *win)
{

        return FALSE;
}

static void
_on_notify_chapter_changed(BibleContent *bible_content,
                           GParamSpec *param,
                           BibleTextPage *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(bible_content), param->name, &value);
        self->hightlight_data.chapter = g_value_get_uint(&value);
        g_value_unset(&value);
}

static void
_on_notify_book_changed(BibleContent *bible_content,
                        GParamSpec *param,
                        BibleTextPage *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(bible_content), param->name, &value);
        self->hightlight_data.book = g_value_get_uint(&value);
        g_value_unset(&value);
}

void _on_translation_button_clicked(GtkButton *button, BibleAppWindow *window)
{
        adw_view_stack_set_visible_child_name(window->stack, "overview");
        // bible_preferences_window_navigate_page_name(window->preferences_window, "passage");
        bible_preferences_window_navigate_passage_name(window->preferences_window, "translation");
}

void _on_book_button_clicked(GtkButton *button, BibleAppWindow *window)
{
        adw_view_stack_set_visible_child_name(window->stack, "overview");
        // bible_preferences_window_navigate_page_name(window->preferences_window, "passage");
        bible_preferences_window_navigate_passage_name(window->preferences_window, "book");
}

void bible_text_page_set_window(BibleTextPage *self, gpointer window)
{
        g_assert(BIBLE_IS_APP_WINDOW(window));
        BibleAppWindow *win = BIBLE_APP_WINDOW(window);
        self->bible_content = win->bible_content;
        g_assert(BIBLE_IS_CONTENT(self->bible_content));

        self->translation_button = win->translation_button;
        self->book_button = win->book_button;

        g_signal_connect(self->bible_content, "notify::chapter", G_CALLBACK(_on_notify_chapter_changed), self);
        g_signal_connect(self->bible_content, "notify::book", G_CALLBACK(_on_notify_book_changed), self);
        g_signal_connect(self->translation_button, "clicked", G_CALLBACK(_on_translation_button_clicked), window);
        g_signal_connect(self->book_button, "clicked", G_CALLBACK(_on_book_button_clicked), window);

        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(self->bible_content), "chapter", &value);
        self->hightlight_data.chapter = g_value_get_uint(&value);
        g_value_unset(&value);

        g_object_get_property(G_OBJECT(self->bible_content), "book", &value);
        self->hightlight_data.book = g_value_get_uint(&value);
        g_value_unset(&value);

        // self->bible_content = content;
}
void hide_navigation_box_timeout(GtkWidget *nav)
{
        gtk_widget_set_visible(nav, false);
}

void _on_text_scrolled(GtkAdjustment *adjustment,
                       BibleTextPage *self)
{
        double value = gtk_adjustment_get_value(adjustment);

        if (self->last_scroll_value > value || value >= gtk_adjustment_get_upper(adjustment) * 0.8)
        {

                gtk_widget_add_css_class(GTK_WIDGET(self->navigation_box), "visible");
                gtk_widget_set_visible(GTK_WIDGET(self->navigation_box), true);

                self->last_scroll_value = value;
                return;
        }
        gtk_widget_remove_css_class(GTK_WIDGET(self->navigation_box), "visible");
        g_timeout_add_once(400, (GSourceOnceFunc)hide_navigation_box_timeout, self->navigation_box);
        self->last_scroll_value = value;
}

static void
bible_text_page_init(BibleTextPage *self)
{
        gtk_widget_init_template(GTK_WIDGET(self));

        self->color_selector = color_selector_new();
        self->event_controller = gtk_event_controller_key_new();
        self->hightlight_data = (HighlightData){.hightlight_verse = 1};

        gtk_text_view_add_overlay(self->text_view, GTK_WIDGET(self->color_selector), 0, 0);
        gtk_widget_set_visible(GTK_WIDGET(self->color_selector), false);

        GtkGesture *gesture = gtk_gesture_long_press_new();
        g_signal_connect(gesture, "pressed", G_CALLBACK(_on_text_long_pressed), self);
        g_signal_connect(self->color_selector, "color-selected", G_CALLBACK(color_selected), self);
        g_signal_connect(gtk_scrolled_window_get_vadjustment(self->scrolled), "value-changed", G_CALLBACK(_on_text_scrolled), self);

        gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(gesture), GTK_PHASE_CAPTURE);
        gtk_gesture_single_set_touch_only(GTK_GESTURE_SINGLE(gesture), false);
        gtk_widget_add_controller(GTK_WIDGET(self->text_view), GTK_EVENT_CONTROLLER(gesture));

        gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(self->event_controller));
        g_signal_connect(self->event_controller, "key-pressed", G_CALLBACK(event_key_pressed_cb), self);
        g_signal_connect(self->event_controller, "key-released", G_CALLBACK(event_key_released_cb), self);

        self->navigation_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        self->previous_chapter = GTK_BUTTON(gtk_button_new());
        self->next_chapter = GTK_BUTTON(gtk_button_new());

        gtk_widget_set_valign(GTK_WIDGET(self->navigation_box), GTK_ALIGN_CENTER);

        gtk_button_set_icon_name(self->previous_chapter, "go-previous-symbolic");
        gtk_button_set_icon_name(self->next_chapter, "go-next-symbolic");

        // gtk_button_set_has_frame(self->previous_chapter, false);
        // gtk_button_set_has_frame(self->next_chapter, false);

        gtk_widget_add_css_class(GTK_WIDGET(self->previous_chapter), "osd");
        gtk_widget_add_css_class(GTK_WIDGET(self->next_chapter), "osd");

        gtk_widget_set_halign(GTK_WIDGET(self->previous_chapter), GTK_ALIGN_START);
        gtk_widget_set_halign(GTK_WIDGET(self->next_chapter), GTK_ALIGN_END);

        gtk_widget_set_hexpand(GTK_WIDGET(self->previous_chapter), true);
        gtk_widget_set_hexpand(GTK_WIDGET(self->next_chapter), true);

        gtk_actionable_set_action_name(GTK_ACTIONABLE(self->previous_chapter), "chapter-choose");
        gtk_actionable_set_action_name(GTK_ACTIONABLE(self->next_chapter), "chapter-choose");

        GVariant *previous = g_variant_new_string("previous");
        GVariant *next = g_variant_new_string("next");
        gtk_actionable_set_action_target_value(GTK_ACTIONABLE(self->previous_chapter), previous);
        gtk_actionable_set_action_target_value(GTK_ACTIONABLE(self->next_chapter), next);
        // g_variant_unref(previous);
        // g_variant_unref(next);

        gtk_widget_add_css_class(GTK_WIDGET(self->navigation_box), "navbox");
        gtk_widget_add_css_class(GTK_WIDGET(self->navigation_box), "visible");

        gtk_widget_set_margin_start(GTK_WIDGET(self->navigation_box), 10);
        gtk_widget_set_margin_end(GTK_WIDGET(self->navigation_box), 10);

        gtk_box_append(self->navigation_box, GTK_WIDGET(self->previous_chapter));
        gtk_box_append(self->navigation_box, GTK_WIDGET(self->next_chapter));

        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_INT);
        g_value_set_int(&value, 50);

        g_object_set_property(G_OBJECT(self->book_button), "width-request", &value);

        gtk_overlay_add_overlay(self->overlay_box, GTK_WIDGET(self->navigation_box));

        // gtk_text_view_set_buffer(self->text_view, NULL);

        // GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
        // if (!GTK_IS_TEXT_TAG(gtk_text_tag_table_lookup(table, "highlighted")))
        gtk_text_buffer_create_tag(
            self->text_buffer, "highlighted",
            "underline", PANGO_UNDERLINE_ERROR_LINE
            //     ,
            //     "background", "#05F230"
            //     ,
            //     "underline-rgba-set", TRUE,
            //     "underline-rgba", color
        );

        // if (!GTK_IS_TEXT_TAG(gtk_text_tag_table_lookup(table, "green")))
        gtk_text_buffer_create_tag(
            self->text_buffer, "green",
            "background", "#31793e");

        // if (!GTK_IS_TEXT_TAG(gtk_text_tag_table_lookup(table, "red")))
        gtk_text_buffer_create_tag(
            self->text_buffer, "red",
            "background", "#FF4608");

        // if (!GTK_IS_TEXT_TAG(gtk_text_tag_table_lookup(table, "blue")))
        gtk_text_buffer_create_tag(
            self->text_buffer, "blue",
            "background", "#0767DB");

        // if (!GTK_IS_TEXT_TAG(gtk_text_tag_table_lookup(table, "yellow")))
        gtk_text_buffer_create_tag(
            self->text_buffer, "yellow",
            "background", "#DBBC07");

        // if (!GTK_IS_TEXT_TAG(gtk_text_tag_table_lookup(table, "pink")))
        gtk_text_buffer_create_tag(
            self->text_buffer, "pink",
            "background", "#DB00FA");
        // gtk_text_view_set_buffer(self->text_view, buffer);

        g_signal_connect(self->text_buffer, "changed", G_CALLBACK(_on_text_changed), self);
}

static void
bible_text_page_dispose(GObject *object)
{
        // BibleTextPage *page;

        // page = BIBLE_TEXT_PAGE(object);

        G_OBJECT_CLASS(bible_text_page_parent_class)->dispose(object);
}

void action_choose_chapter(GtkWidget *widget,
                           const gchar *action_name,
                           GVariant *parameter)
{
        BibleTextPage *self = BIBLE_TEXT_PAGE(widget);
        // gsize length = g_variant_get_size(parameter);
        const gchar *direction = g_variant_get_string(parameter, NULL);
        if (g_strcmp0(direction, "previous") == 0)
                _on_previous_chapter_button_clicked(self);

        else if (g_strcmp0(direction, "next") == 0)
                _on_next_chapter_button_clicked(self);
}

static void
bible_text_page_class_init(BibleTextPageClass *klass)
{
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
        GObjectClass *object_class = G_OBJECT_CLASS(klass);

        object_class->dispose = bible_text_page_dispose;

        object_class->get_property = bible_text_page_get_property;
        object_class->set_property = bible_text_page_set_property;

        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/gtkbible/text_page.ui");

        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, text_view);
        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, text_buffer);
        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, text_title);
        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, scrolled);
        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, overlay_box);

        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, book_label);
        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, translation_label);

        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, overlay_box);
        gtk_widget_class_bind_template_child(widget_class, BibleTextPage, navigation_box_hidden);

        properties[PROP_LINE_DISTANCE] = g_param_spec_string(
            "line-distance", // Name
            "line-distance", // Nick
            "Line Distance", // Blurb
            NULL,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_FONT_SIZE] = g_param_spec_uint(
            "font-size",
            "Font Size",
            "The font size of the TextView",
            8, 40, 12,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_FONT] = g_param_spec_string(
            "font",
            "Font",
            "The font the TextView",
            NULL,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_HIGHLIGHTS] = g_param_spec_string(
            "highlights",
            "Highlights",
            "The highlighted verses in json string format",
            "",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        g_object_class_install_properties(object_class, LAST_PROP, properties);
        gtk_widget_class_install_action(widget_class, "chapter-choose", "s", (GtkWidgetActionActivateFunc)action_choose_chapter);

        // gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(klass), BibleTextPage, overlay);

        // gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(klass), _on_button_clicked);
}

BibleTextPage *
bible_text_page_new()
{
        return (BibleTextPage *)g_object_new(BIBLE_TEXT_PAGE_TYPE, NULL);
}
