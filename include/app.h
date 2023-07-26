#ifndef BIBLE_APP_H
#define BIBLE_APP_H

#include <gtk/gtk.h>
#include <adwaita.h>

#define BIBLE_APP_TYPE (bible_app_get_type())
G_DECLARE_FINAL_TYPE(BibleApp, bible_app, BIBLE, APP, AdwApplication)

BibleApp *bible_app_new(void);

void bible_app_startup(GApplication *app);

#endif /* BIBLE_APP_H */
