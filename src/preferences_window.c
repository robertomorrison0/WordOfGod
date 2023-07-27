
// #include <app.h>
#include <preferences_window.h>
#include <glib/gi18n.h>
#include <window.h>
#include <bible_content.h>

struct _BiblePreferencesWindow
{
        AdwBin parent;

        GtkCssProvider *text_css_provider;
        GtkCssProvider *provider;
        GdkDisplay *display;
        AdwStyleManager *style_manager;

        ThemeSelector *theme_selector;
        LineDistanceSelector *line_distance_selector;

        GtkButton *font_smaller_button;
        GtkButton *font_bigger_button;
        GtkButton *add_translation_button;

        GtkListBox *font_selection;
        GtkListBox *language_selection;
        GtkListBox *translation_selection;
        GtkListBox *book_selection;
        GtkFlowBox *chapter_selection;

        GSettings *settings;

        BibleAppWindow *window;
        BibleContent *content;
        BibleTextPage *text_page;
        AdwViewStack *stack;

        GtkTextView *example_text_view;

        GtkBox *style_page;
        GtkBox *passage_page;
        AdwClamp *data_page;

        gchar *line_distance;
        guint font_size;
        gchar *font;

        PangoContext *pango_context;

        GtkFileDialog *dialog;

        GtkFileFilter *filter;
        GListStore *filter_store;

        AdwLeaflet *leaflet;
        AdwClamp *overview_page;

        GtkScrolledWindow *passage_scroll;
        AdwCarousel *passage_carousel;

        AdwClamp *language_choose_container;
        AdwClamp *translation_choose_container;
        AdwClamp *book_choose_container;
        AdwClamp *chapter_choose_container;
};

G_DEFINE_TYPE(BiblePreferencesWindow, bible_preferences_window, ADW_TYPE_BIN)

void bible_preferences_window_update_font(BiblePreferencesWindow *self)
{

        if (!self->provider)
        {
                self->provider = gtk_css_provider_new();
        }
        gchar font[200];
        g_snprintf(font, sizeof(font), "textview.chapter-content{\nfont:\t%ipx \"%s\";}\n label.chapter-title{\nfont:\t%ipx \"%s\";\n}", self->font_size, self->font, self->font_size + 3, self->font);

        if (!self->text_css_provider)
        {
                self->text_css_provider = gtk_css_provider_new();
        }
        gtk_css_provider_load_from_data(self->text_css_provider, font, -1);

        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(self->text_css_provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        if (g_strcmp0(self->line_distance, "far") == 0)
        {
                gtk_text_view_set_pixels_inside_wrap(self->example_text_view, self->font_size);
        }
        else if (g_strcmp0(self->line_distance, "normal") == 0)
        {
                gtk_text_view_set_pixels_inside_wrap(self->example_text_view, self->font_size / 2);
        }
        else
        {
                gtk_text_view_set_pixels_inside_wrap(self->example_text_view, self->font_size / 4);
        }
}

static void file_open_finished(GtkFileDialog *self,
                               GAsyncResult *result,
                               gpointer data)
{
        // gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);

        g_assert(GTK_IS_FILE_DIALOG(self));
        GFile *f = gtk_file_dialog_open_finish(self, result, NULL);

        g_print("File: %s\n", g_file_get_path(f));

        gchar *contents;
        g_file_load_contents(f, NULL, &contents, NULL, NULL, NULL);

        g_print("File Content: %s\n", contents);
        // g_print("user-specific application configuration information: %s\n", g_get_user_config_dir());
}

static void _on_add_translation_button_clicked(GtkButton *button,
                                               BiblePreferencesWindow *self)
{
        gtk_file_dialog_open(self->dialog,
                             GTK_WINDOW(self->window),
                             NULL,
                             (GAsyncReadyCallback)file_open_finished,
                             NULL);
}

static void _on_back_button_clicked(GtkButton *button,
                                    BiblePreferencesWindow *self)
{
        adw_leaflet_navigate(self->leaflet, ADW_NAVIGATION_DIRECTION_BACK);
}

void bible_preferences_window_navigate_page_name(BiblePreferencesWindow *self, const gchar *page)
{
        if (g_strcmp0(page, "style") == 0)
        {
                // adw_leaflet_set_visible_child(self->leaflet, GTK_WIDGET(self->style_page));
                adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->style_page), GTK_WIDGET(self->overview_page));
        }
        else if (g_strcmp0(page, "passage") == 0)
        {
                // adw_leaflet_set_visible_child(self->leaflet, GTK_WIDGET(self->passage_page));
                adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->passage_page), GTK_WIDGET(self->overview_page));
        }
        else if (g_strcmp0(page, "data") == 0)
        {
                // adw_leaflet_set_visible_child(self->leaflet, GTK_WIDGET(self->data_page));
                adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->data_page), GTK_WIDGET(self->overview_page));
        }
        adw_leaflet_navigate(self->leaflet, ADW_NAVIGATION_DIRECTION_FORWARD);
}

void bible_preferences_window_navigate_passage_name(BiblePreferencesWindow *self, const gchar *passage)
{
        adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->passage_page), GTK_WIDGET(self->overview_page));
        adw_leaflet_set_visible_child(self->leaflet, GTK_WIDGET(self->overview_page));
        adw_leaflet_navigate(self->leaflet, ADW_NAVIGATION_DIRECTION_FORWARD);

        if (g_strcmp0(passage, "language") == 0)
        {
                adw_carousel_scroll_to(self->passage_carousel, GTK_WIDGET(self->language_choose_container), false);
        }
        else if (g_strcmp0(passage, "translation") == 0)
        {
                adw_carousel_scroll_to(self->passage_carousel, GTK_WIDGET(self->translation_choose_container), false);
        }
        else if (g_strcmp0(passage, "book") == 0)
        {
                adw_carousel_scroll_to(self->passage_carousel, GTK_WIDGET(self->book_choose_container), false);
        }
        else if (g_strcmp0(passage, "chapter") == 0)
        {
                adw_carousel_scroll_to(self->passage_carousel, GTK_WIDGET(self->chapter_choose_container), false);
        }
}

void _on_font_smaller_button_clicked(GtkButton *button,
                                     BiblePreferencesWindow *self)
{

        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(self->text_page), "font-size", &value);
        self->font_size = g_value_get_uint(&value);

        if (self->font_size < 9)
                return;

        self->font_size -= 1;

        GValue font_size_value = G_VALUE_INIT;
        g_value_init(&font_size_value, G_TYPE_UINT);
        g_value_set_uint(&font_size_value, self->font_size);
        g_object_set_property(G_OBJECT(self->text_page), "font-size", &font_size_value);
        g_value_unset(&font_size_value);
        g_value_unset(&value);
}
void _on_font_bigger_button_clicked(GtkButton *button,
                                    BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(self->text_page), "font-size", &value);
        self->font_size = g_value_get_uint(&value);

        if (self->font_size > 39)
                return;

        self->font_size += 1;

        GValue font_size_value = G_VALUE_INIT;
        g_value_init(&font_size_value, G_TYPE_UINT);
        g_value_set_uint(&font_size_value, self->font_size);
        g_object_set_property(G_OBJECT(self->text_page), "font-size", &font_size_value);
        g_value_unset(&font_size_value);
        g_value_unset(&value);
}

static void _on_notify_theme_changed(ThemeSelector *theme_selector,
                                     GParamSpec *param,
                                     BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(theme_selector), param->name, &value);
        const gchar *theme = g_value_get_string(&value);
        if (g_strcmp0(theme, "follow") == 0)
        {
                adw_style_manager_set_color_scheme(self->style_manager, ADW_COLOR_SCHEME_DEFAULT);
                gtk_style_context_remove_provider_for_display(self->display, GTK_STYLE_PROVIDER(self->provider));
        }
        else if (g_strcmp0(theme, "dark") == 0)
        {
                adw_style_manager_set_color_scheme(self->style_manager, ADW_COLOR_SCHEME_FORCE_DARK);
                gtk_style_context_remove_provider_for_display(self->display, GTK_STYLE_PROVIDER(self->provider));
        }
        else if (g_strcmp0(theme, "light") == 0)
        {
                adw_style_manager_set_color_scheme(self->style_manager, ADW_COLOR_SCHEME_FORCE_LIGHT);
                gtk_style_context_remove_provider_for_display(self->display, GTK_STYLE_PROVIDER(self->provider));
        }
        else if (g_strcmp0(theme, "black") == 0)
        {
                gtk_css_provider_load_from_resource(self->provider, "/org/robertomorrison/gtkbible/black.css");
                gtk_style_context_add_provider_for_display(self->display, GTK_STYLE_PROVIDER(self->provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
        else
        {
                gtk_css_provider_load_from_resource(self->provider, "/org/robertomorrison/gtkbible/yellow.css");
                gtk_style_context_add_provider_for_display(self->display, GTK_STYLE_PROVIDER(self->provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
        g_value_unset(&value);
}

static void _on_notify_line_distance_changed(LineDistanceSelector *line_distance_selector,
                                             GParamSpec *param,
                                             BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;

        g_object_get_property(G_OBJECT(line_distance_selector), param->name, &value);
        self->line_distance = g_strdup(g_value_get_string(&value));
        g_settings_set_string(self->settings, param->name, self->line_distance);
        g_value_unset(&value);

        bible_preferences_window_update_font(self);
}

static void
_on_notify_language_changed(BibleContent *content,
                            GParamSpec *param,
                            BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(content), param->name, &value);
        const gchar *language = g_value_get_string(&value);
        g_settings_set_string(self->settings, "current-language", language);

        g_value_unset(&value);
}

static void
_on_notify_translation_changed(BibleContent *content,
                               GParamSpec *param,
                               BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(content), param->name, &value);
        const gchar *translation = g_value_get_string(&value);
        g_settings_set_string(self->settings, "current-translation", "ELB");
        g_value_unset(&value);
        bible_content_get_text(self->content, self->text_page);
        bible_content_get_title(self->content, self->text_page);
        // scroll_button_set_label(self->window->translation_button, translation);
}

static void
_on_notify_book_changed(BibleContent *content,
                        GParamSpec *param,
                        BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(content), param->name, &value);
        guint book = g_value_get_uint(&value);
        g_settings_set_uint(self->settings, "current-book", book);
        g_value_unset(&value);
        bible_content_get_text(self->content, self->text_page);
        bible_content_get_title(self->content, self->text_page);
}

static void
_on_notify_chapter_changed(BibleContent *content,
                           GParamSpec *param,
                           BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(content), param->name, &value);
        guint chapter = g_value_get_uint(&value);
        g_settings_set_uint(self->settings, "current-chapter", chapter);
        g_value_unset(&value);
        bible_content_get_text(self->content, self->text_page);
        bible_content_get_title(self->content, self->text_page);
}

static void _on_notify_font_size_changed(BibleTextPage *page,
                                         GParamSpec *param,
                                         BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(page), param->name, &value);
        self->font_size = g_value_get_uint(&value);
        g_settings_set_uint(self->settings, param->name, self->font_size);
        g_value_unset(&value);

        bible_preferences_window_update_font(self);
}

static void _on_notify_font_changed(BibleTextPage *page,
                                    GParamSpec *param,
                                    BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(page), param->name, &value);
        self->font = g_strdup(g_value_get_string(&value));
        g_settings_set_string(self->settings, param->name, self->font);
        g_value_unset(&value);

        bible_preferences_window_update_font(self);
}

static void _on_notify_highlights_changed(BibleTextPage *page,
                                          GParamSpec *param,
                                          BiblePreferencesWindow *self)
{
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(page), param->name, &value);
        g_settings_set_string(self->settings, param->name, g_strdup(g_value_get_string(&value)));
        g_value_unset(&value);
}

GtkWidget *setup_selection_item(GtkStringObject *item,
                                const gchar **action_name)
{
        GtkWidget *row = adw_action_row_new();
        const gchar *value = gtk_string_object_get_string(item);
        GVariant *var = g_variant_new_string(value);
        gtk_actionable_set_action_name(GTK_ACTIONABLE(row), *action_name);
        gtk_actionable_set_action_target_value(GTK_ACTIONABLE(row), var);
        gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), true);
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), value);

        return row;
}

GtkWidget *setup_chapter_selection_item(GtkStringObject *item,
                                        const gchar **action_name)
{
        const gchar *value = gtk_string_object_get_string(item);
        GtkWidget *button = gtk_button_new_with_label(value);
        GVariant *var = g_variant_new_string(value);
        gtk_actionable_set_action_name(GTK_ACTIONABLE(button), *action_name);
        gtk_actionable_set_action_target_value(GTK_ACTIONABLE(button), var);
        gtk_button_set_has_frame(GTK_BUTTON(button), false);
        gtk_widget_set_margin_top(button, 5);
        gtk_widget_set_margin_bottom(button, 5);
        gtk_widget_set_margin_start(button, 5);
        gtk_widget_set_margin_end(button, 5);
        gtk_widget_add_css_class(button, "chapter-selection");

        return button;
}

void bible_preferences_window_add_languages(BiblePreferencesWindow *self, GtkStringList *list)
{
        gchar *action_name = "language-set";

        gtk_list_box_bind_model(self->book_selection, NULL, NULL, NULL, NULL);
        gtk_list_box_bind_model(self->translation_selection, NULL, NULL, NULL, NULL);
        gtk_list_box_bind_model(self->language_selection, NULL, NULL, NULL, NULL);

        gtk_list_box_bind_model(self->language_selection,
                                G_LIST_MODEL(list),
                                (GtkListBoxCreateWidgetFunc)setup_selection_item,
                                &action_name, NULL);
}

void bible_preferences_window_add_translations(BiblePreferencesWindow *self, GtkStringList *list)
{
        gchar *action_name = "translation-set";

        gtk_list_box_bind_model(self->book_selection, NULL, NULL, NULL, NULL);
        gtk_list_box_bind_model(self->translation_selection, NULL, NULL, NULL, NULL);

        gtk_list_box_bind_model(self->translation_selection,
                                G_LIST_MODEL(list),
                                (GtkListBoxCreateWidgetFunc)setup_selection_item,
                                &action_name, NULL);
}

void bible_preferences_window_add_books(BiblePreferencesWindow *self, GtkStringList *list, guint size)
{
        gchar *action_name = "book-set";

        gtk_list_box_bind_model(self->book_selection, NULL, NULL, NULL, NULL);

        for (int i = 1; i <= size; i++)
        {
                GtkWidget *row = adw_action_row_new();
                GVariant *var = g_variant_new_uint32(i);
                gtk_actionable_set_action_name(GTK_ACTIONABLE(row), action_name);
                gtk_actionable_set_action_target_value(GTK_ACTIONABLE(row), var);
                gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), true);
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), gtk_string_list_get_string(list, i - 1));

                gtk_list_box_append(self->book_selection, GTK_WIDGET(row));
        }
}

void bible_preferences_window_add_chapters(BiblePreferencesWindow *self, GtkStringList *list, guint size)
{

        gchar *action_name = "chapter-set";

        gtk_flow_box_bind_model(self->chapter_selection, NULL, NULL, NULL, NULL);

        gtk_flow_box_bind_model(self->chapter_selection,
                                G_LIST_MODEL(list),
                                (GtkFlowBoxCreateWidgetFunc)setup_chapter_selection_item,
                                &action_name, NULL);
}

static void
add_font_rows(BiblePreferencesWindow *self)
{

        self->pango_context = gtk_text_view_get_rtl_context(self->example_text_view);
        PangoFontMap *font_map = pango_context_get_font_map(self->pango_context);
        PangoFontFamily **fonts;
        gint font_amount;

        pango_font_map_list_families(font_map, &fonts, &font_amount);

        for (int i = 0; i < font_amount; i++)
        {
                PangoFontFamily *font_family = (PangoFontFamily *)g_list_model_get_item(G_LIST_MODEL(font_map), i);

                const gchar *name = pango_font_family_get_name(font_family);
                GVariant *var = g_variant_new_string(name);
                GtkWidget *row = adw_action_row_new();

                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), name);
                gtk_actionable_set_action_name(GTK_ACTIONABLE(row), "font-set");
                gtk_actionable_set_action_target_value(GTK_ACTIONABLE(row), var);

                gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), true);

                gtk_list_box_append(self->font_selection, row);
        }
        g_free(fonts);
}

void action_set_language(GtkWidget *widget,
                         const gchar *action_name,
                         GVariant *parameter)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(widget);
        g_assert(BIBLE_IS_CONTENT(self->content));
        const gchar *language = g_variant_get_string(parameter, NULL);
        bible_content_get_translations(self->content, language);
        bible_preferences_window_navigate_passage_name(self, "translation");

        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, language);
        g_object_set_property(G_OBJECT(self->content), "language", &value);
        g_value_unset(&value);
}

void action_set_translation(GtkWidget *widget,
                            const gchar *action_name,
                            GVariant *parameter)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(widget);
        g_assert(BIBLE_IS_CONTENT(self->content));
        const gchar *translation = g_variant_get_string(parameter, NULL);

        bible_content_get_books(self->content, self, translation);
        bible_preferences_window_navigate_passage_name(self, "book");

        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, translation);
        g_object_set_property(G_OBJECT(self->content), "translation", &value);
        g_value_unset(&value);
}

void action_set_book(GtkWidget *widget,
                     const gchar *action_name,
                     GVariant *parameter)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(widget);
        g_assert(BIBLE_IS_CONTENT(self->content));
        uint32_t book = g_variant_get_uint32(parameter);

        bible_content_get_chapters(self->content, self, book);
        bible_preferences_window_navigate_passage_name(self, "chapter");

        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_UINT);
        g_value_set_uint(&value, book);
        g_object_set_property(G_OBJECT(self->content), "book", &value);
        g_value_unset(&value);
}

void action_set_chapter(GtkWidget *widget,
                        const gchar *action_name,
                        GVariant *parameter)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(widget);
        g_assert(BIBLE_IS_CONTENT(self->content));
        const gchar *chapter = g_variant_get_string(parameter, NULL);

        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_UINT);
        g_value_set_uint(&value, atoi(chapter));
        g_object_set_property(G_OBJECT(self->content), "chapter", &value);
        g_value_unset(&value);
        adw_view_stack_set_visible_child_name(self->stack, "read");
}

void action_set_font(GtkWidget *widget,
                     const gchar *action_name,
                     GVariant *parameter)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(widget);
        // gsize length = g_variant_get_size(parameter);
        const gchar *font = g_variant_get_string(parameter, NULL);

        GValue value = G_VALUE_INIT;
        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, font);

        g_object_set_property(G_OBJECT(self->text_page), "font", &value);

        g_value_unset(&value);
}

void action_open_settings(GtkWidget *widget,
                          const gchar *action_name,
                          GVariant *parameter)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(widget);
        const gchar *page = g_variant_get_string(parameter, NULL);
        if (g_strcmp0(page, "style") == 0)
        {
                adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->style_page), GTK_WIDGET(self->overview_page));
        }
        else if (g_strcmp0(page, "passage") == 0)
        {
                adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->passage_page), GTK_WIDGET(self->overview_page));
        }
        else if (g_strcmp0(page, "data") == 0)
        {
                adw_leaflet_reorder_child_after(self->leaflet, GTK_WIDGET(self->data_page), GTK_WIDGET(self->overview_page));
        }
        adw_leaflet_navigate(self->leaflet, ADW_NAVIGATION_DIRECTION_FORWARD);
}

gboolean leaflet_from_visible_child_to_boolean(GBinding *binding,
                                               const GValue *from_value,
                                               GValue *to_value,
                                               gpointer data)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(data);
        gpointer object = g_value_get_object(from_value);
        if (self->overview_page == object)
        {
                g_value_set_boolean(to_value, false);
                return true;
        }
        g_value_set_boolean(to_value, true);

        return true;
}

gboolean stack_from_visible_child_name_to_boolean(GBinding *binding,
                                                  const GValue *from_value,
                                                  GValue *to_value,
                                                  gpointer data)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(data);

        const gchar *visible = g_value_get_string(from_value);
        if (g_strcmp0(visible, "overview") != 0)
        {
                g_value_set_boolean(to_value, false);
                return true;
        }

        adw_leaflet_set_visible_child(self->leaflet, GTK_WIDGET(self->overview_page));

        // g_value_set_boolean(to_value, false);

        return false;
}

gboolean stack_from_visible_child_name_to_boolean2(GBinding *binding,
                                                   const GValue *from_value,
                                                   GValue *to_value,
                                                   gpointer data)
{
        BiblePreferencesWindow *self = BIBLE_PREFERENCES_WINDOW(data);

        const gchar *visible = g_value_get_string(from_value);
        if (g_strcmp0(visible, "overview") != 0)
        {
                g_value_set_boolean(to_value, true);
                return true;
        }

        g_value_set_boolean(to_value, false);
        // adw_leaflet_set_visible_child(self->leaflet, GTK_WIDGET(self->overview_page));

        // g_value_set_boolean(to_value, false);

        return true;
}

void _on_passage_page_changed(AdwCarousel *widget,
                              guint index,
                              BiblePreferencesWindow *self)
{

        GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(self->passage_scroll);
        gtk_adjustment_set_value(vadjustment, 0.0);
        gtk_scrolled_window_set_vadjustment(self->passage_scroll, vadjustment);
}

void bible_preferences_window_set_window(BiblePreferencesWindow *self, gpointer window)
{
        g_assert(BIBLE_IS_APP_WINDOW(window));
        BibleAppWindow *win = BIBLE_APP_WINDOW(window);
        self->window = win;
        self->text_page = win->text_page;
        self->content = win->bible_content;
        self->stack = win->stack;
        g_assert(BIBLE_IS_TEXT_PAGE(self->text_page));
        g_assert(BIBLE_IS_CONTENT(self->content));
        g_assert(ADW_IS_VIEW_STACK(self->stack));

        // PangoContext *pango_context = get_pango_context(text_page);

        g_signal_connect(self->text_page, "notify::font-size", G_CALLBACK(_on_notify_font_size_changed), self);
        g_signal_connect(self->text_page, "notify::font", G_CALLBACK(_on_notify_font_changed), self);
        g_signal_connect(self->text_page, "notify::highlights", G_CALLBACK(_on_notify_highlights_changed), self);
        g_signal_connect(self->content, "notify::language", G_CALLBACK(_on_notify_language_changed), self);
        g_signal_connect(self->content, "notify::translation", G_CALLBACK(_on_notify_translation_changed), self);
        g_signal_connect(self->content, "notify::book", G_CALLBACK(_on_notify_book_changed), self);
        g_signal_connect(self->content, "notify::chapter", G_CALLBACK(_on_notify_chapter_changed), self);
        g_signal_connect(self->passage_carousel, "page-changed", G_CALLBACK(_on_passage_page_changed), self);

        bible_content_get_languages(self->content, self);

        bible_content_set_page(self->content, self->text_page);

        g_signal_connect(self->add_translation_button, "clicked", G_CALLBACK(_on_add_translation_button_clicked), self);
        g_signal_connect(win->back_button, "clicked", G_CALLBACK(_on_back_button_clicked), self);

        g_settings_bind(self->settings, "font-size",
                        self->text_page, "font-size",
                        G_SETTINGS_BIND_DEFAULT);

        g_settings_bind(self->settings, "highlights",
                        self->text_page, "highlights",
                        G_SETTINGS_BIND_DEFAULT);

        g_settings_bind(self->settings, "font",
                        self->text_page, "font",
                        G_SETTINGS_BIND_DEFAULT);

        g_settings_bind(self->settings, "current-translation",
                        self->content, "translation",
                        G_SETTINGS_BIND_GET);

        g_settings_bind(self->settings, "current-book",
                        self->content, "book",
                        G_SETTINGS_BIND_DEFAULT);

        g_settings_bind(self->settings, "current-chapter",
                        self->content, "chapter",
                        G_SETTINGS_BIND_DEFAULT);

        g_settings_bind(self->settings, "current-language",
                        self->content, "language",
                        G_SETTINGS_BIND_DEFAULT);

        g_object_bind_property(self->line_distance_selector, "line-distance",
                               self->text_page, "line-distance", G_BINDING_BIDIRECTIONAL);

        g_settings_bind(self->settings, "window-width",
                        win, "default-width",
                        G_SETTINGS_BIND_DEFAULT);
        g_settings_bind(self->settings, "window-height",
                        win, "default-height",
                        G_SETTINGS_BIND_DEFAULT);
        g_settings_bind(self->settings, "window-is-maximized",
                        win, "maximized",
                        G_SETTINGS_BIND_DEFAULT);
        g_settings_bind(self->settings, "window-is-fullscreen",
                        win, "fullscreened",
                        G_SETTINGS_BIND_DEFAULT);

        g_object_bind_property_full(self->leaflet, "visible-child",
                                    win->back_button, "visible",
                                    G_BINDING_DEFAULT,
                                    leaflet_from_visible_child_to_boolean, NULL, self, NULL);

        g_object_bind_property_full(win->stack, "visible-child-name",
                                    win->back_button, "visible",
                                    G_BINDING_DEFAULT,
                                    stack_from_visible_child_name_to_boolean, NULL, self, NULL);

        g_object_bind_property_full(win->stack, "visible-child-name",
                                    win->passage_box, "visible",
                                    G_BINDING_DEFAULT,
                                    stack_from_visible_child_name_to_boolean2, NULL, self, NULL);
}

static void
bible_preferences_window_init(BiblePreferencesWindow *self)
{
        g_type_ensure(THEME_SELECTOR_TYPE);
        g_type_ensure(LINE_DISTANCE_TYPE_SELECTOR);
        g_type_ensure(BIBLE_TEXT_PAGE_TYPE);
        gtk_widget_init_template(GTK_WIDGET(self));
        // adw_leaflet_page_set_name(ADW_LEAFLET_PAGE(self->style_page), "style");
        // adw_leaflet_page_set_name(ADW_LEAFLET_PAGE(self->passage_page), "passage");
        // adw_leaflet_page_set_name(ADW_LEAFLET_PAGE(self->data_page), "data");

        self->provider = gtk_css_provider_new();
        self->display = gdk_display_get_default();
        self->style_manager = adw_style_manager_get_default();
        self->dialog = gtk_file_dialog_new();

        self->filter = gtk_file_filter_new();
        gtk_file_filter_set_name(self->filter, "JSON");
        gtk_file_filter_add_mime_type(self->filter, "application/json");

        self->filter_store = g_list_store_new(GTK_TYPE_FILE_FILTER);
        g_list_store_append(self->filter_store, self->filter);

        gtk_file_dialog_set_filters(self->dialog, G_LIST_MODEL(self->filter_store));
        gtk_file_dialog_set_default_filter(self->dialog, self->filter);

        self->settings = g_settings_new("org.robertomorrison.gtkbible");

        g_signal_connect(self->theme_selector, "notify::theme", G_CALLBACK(_on_notify_theme_changed), self);
        g_signal_connect(self->line_distance_selector, "notify::line-distance", G_CALLBACK(_on_notify_line_distance_changed), self);
        g_signal_connect(self->font_smaller_button, "clicked", G_CALLBACK(_on_font_smaller_button_clicked), self);
        g_signal_connect(self->font_bigger_button, "clicked", G_CALLBACK(_on_font_bigger_button_clicked), self);

        g_settings_bind(self->settings, "style-variant",
                        self->theme_selector, "theme",
                        G_SETTINGS_BIND_DEFAULT);

        g_settings_bind(self->settings, "line-distance",
                        self->line_distance_selector, "line-distance",
                        G_SETTINGS_BIND_DEFAULT);

        GtkTextBuffer *example_text_buffer = gtk_text_view_get_buffer(self->example_text_view);
        GtkTextIter start;
        const gchar example_text[] = "So He humbled you, allowed you to hunger, and fed you with manna which you did not know nor did your fathers know, that He might make you know that man shall not live by bread alone; but man lives by every <i>word</i> that proceeds from the mouth of the LORD.";
        gtk_text_buffer_get_start_iter(example_text_buffer, &start);
        gtk_text_buffer_insert_markup(example_text_buffer, &start, example_text, -1);

        gtk_widget_set_size_request(GTK_WIDGET(self->font_selection), 200, 800);
        add_font_rows(self);
}

static void
bible_preferences_window_dispose(GObject *object)
{
        BiblePreferencesWindow *self = (BiblePreferencesWindow *)object;

        g_clear_object(&self->settings);
        g_clear_object(&self->provider);
        g_clear_object(&self->text_css_provider);

        G_OBJECT_CLASS(bible_preferences_window_parent_class)->dispose(object);
}

static void
bible_preferences_window_class_init(BiblePreferencesWindowClass *klass)
{
        GObjectClass *window_class = G_OBJECT_CLASS(klass);
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

        G_OBJECT_CLASS(klass)->dispose = bible_preferences_window_dispose;

        gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);

        gtk_widget_class_set_template_from_resource(widget_class, "/org/robertomorrison/gtkbible/preferences_window.ui");

        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, theme_selector);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, line_distance_selector);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, font_smaller_button);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, font_bigger_button);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, add_translation_button);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, example_text_view);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, style_page);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, passage_page);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, data_page);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, font_selection);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, language_selection);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, translation_selection);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, book_selection);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, chapter_selection);

        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, leaflet);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, overview_page);
        // gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, translation_choose);
        // gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, book_choose);
        // gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, chapter_choose);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, passage_carousel);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, language_choose_container);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, translation_choose_container);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, book_choose_container);
        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, chapter_choose_container);

        gtk_widget_class_bind_template_child(widget_class, BiblePreferencesWindow, passage_scroll);

        gtk_widget_class_install_action(widget_class, "font-set", "s", (GtkWidgetActionActivateFunc)action_set_font);
        gtk_widget_class_install_action(widget_class, "settings-open", "s", (GtkWidgetActionActivateFunc)action_open_settings);
        gtk_widget_class_install_action(widget_class, "language-set", "s", (GtkWidgetActionActivateFunc)action_set_language);
        gtk_widget_class_install_action(widget_class, "translation-set", "s", (GtkWidgetActionActivateFunc)action_set_translation);
        gtk_widget_class_install_action(widget_class, "book-set", "u", (GtkWidgetActionActivateFunc)action_set_book);
        gtk_widget_class_install_action(widget_class, "chapter-set", "s", (GtkWidgetActionActivateFunc)action_set_chapter);
}

BiblePreferencesWindow *
bible_preferences_window_new(void)
{
        return (BiblePreferencesWindow *)g_object_new(BIBLE_PREFERENCES_WINDOW_TYPE, NULL);
}
