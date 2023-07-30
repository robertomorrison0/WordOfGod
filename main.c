#include <gtk/gtk.h>

#include <glib/gi18n.h>
#include <config.h>

#include <app.h>

#define _(x) gettext(x)
#define N_(x) x
#define C_(ctx, x) pgettext(ctx, x)

int main(int argc, gchar *argv[])
{
        /* Since this example is running uninstalled,
         * we have to help it find its schema. This
         * is *not* necessary in properly installed
         * application.
         */
        g_setenv("GSETTINGS_SCHEMA_DIR", DATA_DIR, FALSE);

        bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
        bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
        textdomain(GETTEXT_PACKAGE);

        BibleApp *app;
        gint status;

        app = bible_app_new();
        status = g_application_run(G_APPLICATION(app), argc, argv);
        // g_object_unref(theme);
        g_clear_object(&app);

        return status;
}
