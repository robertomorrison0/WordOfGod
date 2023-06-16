#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <adwaita.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Translation.hpp>

struct InterfaceOptions
{
        gint size = 16;
};

struct NavigationData
{
        BibleBolls::ContentInstance content;
        GObject *text_buffer;
        GObject *text_view;
        GObject *scrolled_window;
        GObject *leaflet;
        GObject *book_row;
};

enum BibleColorMode
{
        OLED_DARK,
        WHITE,
        GREY,
        OLD,
};

static void
quit(GtkWindow *window)
{
        gtk_window_close(window);
}

static void
open_style_popover(GtkWidget *widget, gpointer data)
{
        GtkPopover *pop = GTK_POPOVER(widget);

        gtk_popover_popup(pop);
        g_print("Hello World\n");
}

static void show_navigation(bool _visibility)
{
        g_print((_visibility) ? "visible\n" : "invisible\n");
}

static void
edge_reached(GtkScrolledWindow *self,
             GtkPositionType pos,
             gpointer data)
{

        // if (pos == GTK_POS_BOTTOM)
        // {
        //         show_navigation(true);
        //         gtk_widget_set_visible(GTK_WIDGET(data), true);
        //         g_timeout_add_seconds(7, (GSourceFunc)edge_reached, data);
        //         return;
        // }
        // show_navigation(false);
        // gtk_widget_set_visible(GTK_WIDGET(data), false);
}

template <typename T>
static void
set_buffer_style(GtkTextBuffer *buffer, const char *tag_name, const char *first_property_name, T second_property_name)
{
        GtkTextTag *tag;
        GtkTextIter start, end;
        tag = gtk_text_buffer_create_tag(buffer, tag_name,
                                         first_property_name, second_property_name,
                                         NULL);
        gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
        gtk_text_buffer_get_iter_at_offset(buffer, &end, -1);

        gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
}
static void
get_buffer_style(GtkTextBuffer *buffer, const char *tag_name)
{
        GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);

        GtkTextTag *tag = gtk_text_tag_table_lookup(table, tag_name);
}

static void set_color_mode(BibleColorMode mode)
{
        GdkDisplay *display = gdk_display_get_default();
        GtkCssProvider *provider = gtk_css_provider_new();
        // const char *css_path = (_dark) ? "../styles/dark.css" : "../styles/light.css";
        // gtk_css_provider_load_from_path(provider, css_path);
        switch (mode)
        {
        case BibleColorMode::OLD:
                gtk_css_provider_load_from_resource(provider, (std::string("org/robertomorrison/biblebolls/") + "style.css").c_str());
                break;
        case BibleColorMode::OLED_DARK:
                gtk_css_provider_load_from_resource(provider, (std::string("org/robertomorrison/biblebolls/") + "style-dark.css").c_str());
                break;
        default:
                // gtk_css_provider_load_from_resource(provider, (std::string("org/robertomorrison/biblebolls/") + "style.css").c_str());
                break;
        }
        gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void
_on_dark_mode_button_clicked(GtkWidget *widget, gpointer data)
{
        std::cout << gtk_button_get_icon_name(GTK_BUTTON(widget)) << std::endl;
        std::string current = std::string(gtk_button_get_icon_name(GTK_BUTTON(widget)));
        AdwStyleManager *style = adw_style_manager_get_default();

        // std::cout << adw_style_manager_get_high_contrast(style) << std::endl;

        if (current == "display-brightness-symbolic")
        {
                set_color_mode(BibleColorMode::OLD);
                gtk_button_set_icon_name(GTK_BUTTON(widget), "weather-clear-night-symbolic");
                // set_dark_mode(false);
                // gtk_set_prefer
                // adw_style_manager_set_color_scheme(style, ADW_COLOR_SCHEME_FORCE_LIGHT);
        }
        else if (current == "weather-clear-night-symbolic")
        {
                set_color_mode(BibleColorMode::OLED_DARK);
                gtk_button_set_icon_name(GTK_BUTTON(widget), "display-brightness-symbolic");
                // adw_style_manager_set_color_scheme(style, ADW_COLOR_SCHEME_FORCE_DARK);
                // set_dark_mode();
        }
}
static void
_on_next_chapter_button_clicked(NavigationData *nav, gpointer data)
{
        std::string chapter = nav->content.get_next_chapter();
        GtkTextIter start;
        GtkTextIter end;

        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(nav->text_buffer), &start, 0);
        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(nav->text_buffer), &end, -1);
        gtk_text_buffer_delete(GTK_TEXT_BUFFER(nav->text_buffer), &start, &end);
        gtk_text_buffer_insert_markup(GTK_TEXT_BUFFER(nav->text_buffer), &start, chapter.c_str(), -1);
        g_usleep(50000);
        g_signal_emit_by_name(nav->scrolled_window, "scroll-child", GTK_SCROLL_START, false, NULL);
}

static void
_on_previous_chapter_button_clicked(NavigationData *nav, gpointer data)
{
        std::string chapter = nav->content.get_prev_chapter();
        GtkTextIter start;
        GtkTextIter end;
        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(nav->text_buffer), &start, 0);
        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(nav->text_buffer), &end, -1);
        gtk_text_buffer_delete(GTK_TEXT_BUFFER(nav->text_buffer), &start, &end);
        gtk_text_buffer_insert_markup(GTK_TEXT_BUFFER(nav->text_buffer), &start, chapter.c_str(), -1);
        g_usleep(50000);
        g_signal_emit_by_name(nav->scrolled_window, "scroll-child", GTK_SCROLL_START, false, NULL);
}

static void
_on_chapter_button_clicked(NavigationData *nav, gpointer data)
{
        GtkWidget *row = gtk_widget_get_parent(GTK_WIDGET(data));

        g_print("book: %s; chapter: %s", adw_expander_row_get_subtitle(ADW_EXPANDER_ROW(row)),
                gtk_widget_get_name(GTK_WIDGET(row)));
        adw_leaflet_navigate(ADW_LEAFLET(nav->leaflet), ADW_NAVIGATION_DIRECTION_FORWARD);
}

static void
_on_close(GtkApplication *self, gpointer *data)
{
        // delete ((BibleBolls::Translation *)data);
        // std::cout << "deleted" << std::endl;
}

static void
activate(GtkApplication *app, NavigationData *nav)
{
        GtkBuilder *builder = gtk_builder_new_from_resource("/org/robertomorrison/biblebolls/builder.ui");
        gtk_builder_add_from_file(builder, "builder.ui", NULL);

        GObject *window = gtk_builder_get_object(builder, "window");
        gtk_window_set_application(GTK_WINDOW(window), app);
        gtk_window_set_default_size(GTK_WINDOW(window), 350, 650);

        GObject *text_buffer = gtk_builder_get_object(builder, "text");
        GObject *text_view = gtk_builder_get_object(builder, "view");

        // gtk_window_set_title(GTK_WINDOW(window), translation->get_title().c_str());

        // InterfaceOptions int_options{};
        nav->text_buffer = text_buffer;

        GObject *navigation_overlay = gtk_builder_get_object(builder, "navigation_overlay");
        GtkWidget *previous_chapter_button = gtk_button_new();
        GtkWidget *next_chapter_button = gtk_button_new();
        g_signal_connect_swapped(previous_chapter_button, "clicked", G_CALLBACK(_on_previous_chapter_button_clicked), nav);
        g_signal_connect_swapped(next_chapter_button, "clicked", G_CALLBACK(_on_next_chapter_button_clicked), nav);

        gtk_button_set_has_frame(GTK_BUTTON(previous_chapter_button), false);
        gtk_widget_set_name(previous_chapter_button, "navigation_arrow");
        gtk_button_set_icon_name(GTK_BUTTON(previous_chapter_button), "go-previous-symbolic");
        gtk_widget_set_halign(previous_chapter_button, GTK_ALIGN_START);
        gtk_widget_set_valign(previous_chapter_button, GTK_ALIGN_CENTER);

        gtk_button_set_has_frame(GTK_BUTTON(next_chapter_button), false);
        gtk_widget_set_name(next_chapter_button, "navigation_arrow");
        gtk_button_set_icon_name(GTK_BUTTON(next_chapter_button), "go-next-symbolic");
        gtk_widget_set_halign(next_chapter_button, GTK_ALIGN_END);
        gtk_widget_set_valign(next_chapter_button, GTK_ALIGN_CENTER);

        gtk_overlay_add_overlay(GTK_OVERLAY(navigation_overlay), previous_chapter_button);
        gtk_overlay_add_overlay(GTK_OVERLAY(navigation_overlay), next_chapter_button);

        GObject *style_popover = gtk_builder_get_object(builder, "style_popover");
        GObject *style_button = gtk_builder_get_object(builder, "style_button");
        g_signal_connect_swapped(style_button, "clicked", G_CALLBACK(open_style_popover), style_popover);

        GObject *dark_mode_button = gtk_builder_get_object(builder, "dark_mode_button");
        g_signal_connect_swapped(dark_mode_button, "clicked", G_CALLBACK(_on_dark_mode_button_clicked), dark_mode_button);

        GObject *navigation_container = gtk_builder_get_object(builder, "navigation_containter");
        GObject *main_window = gtk_builder_get_object(builder, "main_window");
        GObject *leaflet = gtk_builder_get_object(builder, "leaflet");

        nav->leaflet = leaflet;
        nav->text_view = text_view;
        nav->scrolled_window = main_window;

        g_signal_connect(main_window, "edge-reached", G_CALLBACK(edge_reached), navigation_container);

        GObject *overview_box = gtk_builder_get_object(builder, "overview_box");

        // for (auto &book : nav->content.get_translation_bookinfo())
        // {
        // GtkWidget *expander = gtk_expander_new("book");
        // GtkSelectionModel *model;
        // GtkListItemFactory *factory;
        // GtkWidget *grid = gtk_grid_view_new(gtkselectionmodel, factory);

        // gtk_expander_set_child(GTK_EXPANDER(expander), GTK_WIDGET(grid));

        // GtkWidget *expander = adw_expander_row_new();
        // gtk_widget_set_hexpand(expander, true);
        // gtk_widget_set_name(expander, std::to_string(book.id).c_str());
        // adw_expander_row_set_subtitle(ADW_EXPANDER_ROW(expander), book.name.c_str());

        // gtk_list_box_append(GTK_LIST_BOX(overview_box), expander);

        // for (int i = 1; i <= book.chapters; i++)
        // {
        //         std::stringstream chapter_number;
        //         chapter_number << i;
        //         GtkWidget *button = gtk_button_new_with_label(chapter_number.str().c_str());
        //         gtk_button_set_has_frame(GTK_BUTTON(button), false);
        //         adw_expander_row_add_row(ADW_EXPANDER_ROW(expander), button);
        //         g_signal_connect_swapped(button, "clicked", G_CALLBACK(_on_chapter_button_clicked), nav);
        // }
        // }

        // for (int i = 0; i < 10; i++)
        // {
        //         std::stringstream chapter_number;
        //         chapter_number << i;
        //         GtkWidget *button = gtk_button_new_with_label(chapter_number.str().c_str());
        //         // gtk_button_set_(GTK_BUTTON(button), false);
        //         adw_expander_row_add_row(ADW_EXPANDER_ROW(expander), button);
        // }

        GtkTextIter start;
        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(text_buffer), &start, 0);

        std::string chapter = nav->content.get_current_data();

        // std::string te = ((Translation *)data)->content.at(0).at(0).at(30);
        // gtk_text_buffer_set_text(GTK_TEXT_BUFFER(text_buffer), chapter.c_str(), chapter.length());
        gtk_text_buffer_insert_markup(GTK_TEXT_BUFFER(text_buffer), &start, chapter.c_str(), -1);

        g_signal_connect(G_OBJECT(app), "query-end", G_CALLBACK(_on_close), (nav));

        gtk_window_present(GTK_WINDOW(window));
}

int main(int argc,
         char *argv[])
{

        AdwApplication *app = adw_application_new("org/robertomorrison/biblebolls", G_APPLICATION_DEFAULT_FLAGS);

        // g_print("%s\n", argv[1]);

        NavigationData navData{
            .content = BibleBolls::ContentInstance((argc > 1) ? std::string(argv[1]) : std::string("ELB")),
        };

        // GResource *styles_resource = g_resource_load("../resources/style.gresource", NULL);
        // g_resources_register(styles_resource);

        g_signal_connect(app, "activate", G_CALLBACK(activate), &navData);
        g_signal_connect(G_OBJECT(app), "query-end", G_CALLBACK(_on_close), &navData);

        return g_application_run(G_APPLICATION(app), 1, &argv[0]);
}
