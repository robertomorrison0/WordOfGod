#include <gtk/gtk.h>

#include <app.h>

int main(int argc, gchar *argv[])
{
        /* Since this example is running uninstalled,
         * we have to help it find its schema. This
         * is *not* necessary in properly installed
         * application.
         */
        g_setenv("GSETTINGS_SCHEMA_DIR", "./data", FALSE);
        BibleApp *app;
        gint status;

        app = bible_app_new();
        status = g_application_run(G_APPLICATION(app), argc, argv);
        // g_object_unref(theme);
        g_clear_object(&app);

        return status;
}
