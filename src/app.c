
#include <app.h>
#include <glib/gi18n.h>
#include <window.h>

#define PORTAL_BUS_NAME "org.freedesktop.portal.Desktop"
#define PORTAL_OBJECT_PATH "/org/freedesktop/portal/desktop"
#define PORTAL_SETTINGS_INTERFACE "org.freedesktop.portal.Settings"

struct _BibleApp
{
        GtkApplication parent;
};

G_DEFINE_TYPE(BibleApp, bible_app, ADW_TYPE_APPLICATION)

static void
bible_app_init(BibleApp *self)
{
}

static void
bible_app_activate(GApplication *app)
{
        BibleAppWindow *win;
        BibleApp *self = (BibleApp *)app;

        win = bible_app_window_new(BIBLE_APP(app));
        gtk_window_present(GTK_WINDOW(win));

        // const gchar *name = g_strdup_printf("%s/gtkbible", g_get_user_config_dir());
        // g_mkdir(name, 7);

        // g_print("file name: %s\n", name);

        // free(&name);
}

void bible_app_startup(GApplication *app)
{
        //         // g_application_set_application_id(G_APPLICATION(self), APP_ID);
        g_application_set_resource_base_path(app, "/org/robertomorrison/gtkbible/");
        G_APPLICATION_CLASS(bible_app_parent_class)->startup(app);
}

static void
bible_app_class_init(BibleAppClass *klass)
{
        // GObjectClass *object_class = G_OBJECT_CLASS(klass);
        GApplicationClass *application_class = G_APPLICATION_CLASS(klass);

        // G_APPLICATION_CLASS(klass)->startup = bible_app_startup;
        // object_class->constructed = bible_app_constructed;
        application_class->activate = bible_app_activate;
        application_class->startup = bible_app_startup;

        // G_APPLICATION_CLASS(klass)->open = bible_app_open;
}

BibleApp *
bible_app_new(void)
{
        return (BibleApp *)g_object_new(BIBLE_APP_TYPE,
                                        "application-id", "org.robertomorrison.gtkbible",
                                        "flags", G_APPLICATION_HANDLES_OPEN,
                                        NULL);
}
