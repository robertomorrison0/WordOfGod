#include <bible_content.h>
#include <json-c/json.h>
#include <window.h>

struct _BibleContent
{
        GtkApplicationWindow parent;
        GtkButton *style_preferences_button;
        GtkPopover *style_popover;
        ThemeSelector *theme_selector;
        GtkCssProvider *provider;
        GdkDisplay *display;
        AdwStyleManager *style_manager;

        gchar *current_language;
        gchar *current_translation;
        gint current_chapter;
        gint current_book;

        // guint current_book_chapters;
        // guint current_translation_books;
        // gint current_translation;
        // gint current_chapter;

        gchar *config_folder;

        gchar *current_translation_content_json;
        json_object *current_translation_json_object;
        gchar *verses;
        gchar *books;
};

G_DEFINE_TYPE(BibleContent, bible_content, G_TYPE_OBJECT)

enum
{
        PROP_0,
        PROP_CURRENT_CHAPTER,
        PROP_CURRENT_BOOK,
        PROP_CURRENT_TRANSLATION,
        PROP_CURRENT_LANGUAGE,
        LAST_PROP,
};

enum
{
        SIG_TRANSLATION_LOADED,
        SIG_LAST
};

static GParamSpec *properties[LAST_PROP];

static guint signals[SIG_LAST];

typedef struct
{
        gchar *memory;
        size_t size;
} Memory;

typedef struct
{
        gchar *url;
        Memory memory;
} CurlAsyncData;

typedef struct
{
        gint chapter, book;
        GtkTextBuffer *buffer;
        GtkLabel *title_label;
        ScrollButton *book_button;
        ScrollButton *tranlation_button;

} GetChapterData;

typedef struct
{
        gpointer       userdata;
        gchar*          content;
        void (*callback)(gpointer, gchar*);
} ContentCallback;


// #define BIBLE_MINIMUM_FONT_SIZE 8
// #define BIBLE_MAXIMUM_FONT_SIZE 40

static size_t
curl_callback(char *contents, size_t size, size_t nmemb, void *userp)
{
        size_t real_size = size * nmemb;
        Memory *data = (Memory *)userp;

        char *ptr = (char *)realloc(data->memory, data->size + real_size + 1);
        if (ptr == NULL)
                return 0;

        data->memory = ptr;
        memcpy(&data->memory[data->size], contents, real_size);
        data->size += real_size;
        data->memory[data->size] = 0;

        // g_main_context_invoke(NULL, (GSourceFunc)update_progressbar, data);
        g_usleep(500);
        return real_size;
}

static void curl_get_(const gchar *_url, Memory *chunk)
{
        CURL *curl;
        CURLcode return_code;

        chunk->memory = NULL;
        chunk->size = 0;

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        if (!curl)
        {
                // printf("%s", "Curl init failed");
                return;
        }
        curl_easy_setopt(curl, CURLOPT_URL, _url);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, chunk);

        return_code = curl_easy_perform(curl);

        if (return_code != CURLE_OK)
        {
                const gchar *error = curl_easy_strerror(return_code);
                fprintf(stderr, "CURL_ERROR: %s\n", error);
                chunk->size = strlen(error);

                chunk->memory = strdup(error);

                curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
}

static void
curl_get_async_thread_cb(GTask *task,
                         gpointer source_object,
                         gpointer task_data,
                         GCancellable *cancellable)
{

        CurlAsyncData *data = (CurlAsyncData *)task_data;
        curl_get_(data->url, &data->memory);

        g_task_return_pointer(task, task_data, NULL);
}

void curl_get_async(BibleContent *self,
                    //     GCancellable *cancellable,
                    GAsyncReadyCallback callback,
                    gpointer user_data, const gchar url[])
{
        GTask *task = NULL; /* owned */
        CurlAsyncData *data = NULL;

        task = g_task_new(self, NULL, callback, user_data);

        g_task_set_source_tag(task, curl_get_async);

        g_task_set_return_on_cancel(task, FALSE);

        data = g_new0(CurlAsyncData, 1);
        data->url = g_strdup(url);

        g_task_set_task_data(task, data, g_free);
        g_task_run_in_thread(task, curl_get_async_thread_cb);

        g_object_unref(task);
}

void 
emit_translation_loaded(BibleContent*       self,        gchar*          content)
{
        self->current_translation_content_json = g_strdup(content);
        g_signal_emit(self, signals[SIG_TRANSLATION_LOADED], 0);
}

static void
open_translation_file_complete(GObject *source_object,
                               GAsyncResult *result,
                               ContentCallback* obj_call)
{
        GFile *file = G_FILE(source_object);

        GError *error = NULL;
        gchar *contents = NULL;
        gsize length = 0;

        g_file_load_contents_finish(file,
                                    result,
                                    &contents,
                                    &length,
                                    NULL,
                                    &error);

        if (error != NULL)
        {
                g_printerr("Unable to open “%s”: %s\n",
                           g_file_peek_path(file),
                           error->message);
        }


        // g_print(BIBLE_CONTENT(obj_call.userdata)->current_translation_content_json);

        realloc(obj_call, sizeof(obj_call) + sizeof(contents));

        (*obj_call->callback)(obj_call->userdata, contents);
        g_free(obj_call);
        // obj_call.callback(obj_call.object);
}

static void
open_translation_file(BibleContent *self, GFile *file)
{
        ContentCallback *call = malloc(sizeof(self)+sizeof(emit_translation_loaded));
        call->userdata=self, 
        call->callback=emit_translation_loaded;

        // Start the asynchronous operation to save the data into the file
        g_file_load_contents_async(file,
                                   NULL,
                                   (GAsyncReadyCallback)open_translation_file_complete,
                                   call);
}

static void
save_translation_file_complete(GObject *source_object,
                               GAsyncResult *result,
                               BibleContent *self)
{
        GFile *file = G_FILE(source_object);


        GError *error = NULL;
        g_file_replace_contents_finish(file, result, NULL, &error);

        // Query the display name for the file
        gchar *display_name = NULL;
        GFileInfo *info =
            g_file_query_info(file,
                              "standard::display-name",
                              G_FILE_QUERY_INFO_NONE,
                              NULL,
                              NULL);
        if (info != NULL)
        {
                display_name =
                    g_strdup(g_file_info_get_attribute_string(info, "standard::display-name"));
        }
        else
        {
                display_name = g_file_get_basename(file);
        }

        if (error != NULL)
        {
                g_printerr("Unable to save “%s”: %s\n",
                           display_name,
                           error->message);
        }

        // gchar *file_path = g_strdup_printf("%s%s.json", self->config_folder, self->current_translation);
        // file = g_file_new_for_path(file_path);

        g_print("open file: %s\n", g_file_info_get_display_name(info));
        open_translation_file(self, file);
}

static void
save_translation_file(BibleContent *self, GFile *file, gchar *content)
{
        if (content == NULL)
                return;

        GBytes *bytes = g_bytes_new_take(content, strlen(content));

        // Start the asynchronous operation to save the data into the file
        g_file_replace_contents_bytes_async(file,
                                            bytes,
                                            NULL,
                                            FALSE,
                                            G_FILE_CREATE_NONE,
                                            NULL,
                                            (GAsyncReadyCallback)save_translation_file_complete,
                                            self);
}

void bible_content_parse_translation(BibleContent *self)
{
        json_object *export_json;
        json_object *translation_verses_json;
        json_object *translation_books_json;
        json_object *book;
        json_object *book_id;
        json_object *book_name;
        json_object *book_chapters;
        json_object *verse;
        json_object *verse_id;
        json_object *verse_text;
        json_object *verse_book;
        json_object *verse_chapter;

        translation_verses_json = json_tokener_parse(self->verses);
        translation_books_json = json_tokener_parse(self->books);

        size_t n_verses = json_object_array_length(translation_verses_json);
        size_t n_books = json_object_array_length(translation_books_json);

        // printf("Download: %.2f MB\n", (strlen(self->translation_verses) + strlen(self->translation_books)) / 1024.f / 1024.f);
        // printf("Books: %i; size: %.2f MB\n", n_books, strlen(self->translation_books) / 1024.f / 1024.f);

        export_json = json_object_new_object();
        g_assert(export_json != NULL);

        guint main_verse_counter = 0;

        for (size_t i = 0; i < n_books; i++)
        {
                book = json_object_array_get_idx(translation_books_json, i);

                json_object_object_get_ex(book, "bookid", &book_id);
                json_object_object_get_ex(book, "name", &book_name);
                json_object_object_get_ex(book, "chapters", &book_chapters);

                json_object *book_object = json_object_new_object();
                const gchar *key = json_object_get_string(book_name);
                g_assert(book_object != NULL);
                if (json_object_object_add(export_json, key, book_object))
                {
                        fprintf(stderr, "FAILED to add object (%s)\n", key);
                        abort();
                }
                gint n_chapters = json_object_get_int(book_chapters);

                for (size_t k = 0; k < n_chapters; k++)
                {
                        gchar chapter_string[4];
                        sprintf(chapter_string, "%lu", k + 1);
                        json_object *chapter_array = json_object_new_array();

                        if (json_object_object_add(book_object, chapter_string, json_object_get(chapter_array)))
                        {
                                fprintf(stderr, "FAILED to add object (%s)\n", key);
                                abort();
                        }

                        while (true)
                        {
                                verse = json_object_array_get_idx(translation_verses_json, main_verse_counter);
                                json_object_object_get_ex(verse, "chapter", &verse_chapter);
                                json_object_object_get_ex(verse, "book", &verse_book);
                                if (json_object_get_int(verse_chapter) != (k + 1) || json_object_get_int(verse_book) != (i + 1))
                                        break;

                                json_object_object_get_ex(verse, "text", &verse_text);
                                // json_object_object_get_ex(verse, "verse", &verse_id);

                                json_object *verse_object = json_object_new_object();

                                // todo: error check
                                // json_object_object_add(verse_object, "n", verse_id);
                                // json_object_object_add(verse_object, "t", verse_text);
                                json_object_array_add(chapter_array, verse_text);
                                main_verse_counter++;
                        }
                }
        }

        gchar *file_path = g_strdup_printf("%s%s.json", self->config_folder, self->current_translation);

        g_print("save path: %s\n", file_path);
        GFile *file = g_file_new_for_path(file_path);
        GError *error = NULL;

        if (!g_file_test(self->config_folder, G_FILE_TEST_IS_DIR))
                g_file_make_directory_with_parents(g_file_new_for_path(self->config_folder), NULL, &error);
        if (error != NULL)
        {
                g_printerr("Unable to save “%s”: %s(%i)\n",
                           self->config_folder,
                           error->message,
                           error->code);
        }

        if (!g_file_test(file_path, G_FILE_TEST_EXISTS))
        {
                save_translation_file(self, file, g_strdup(json_object_to_json_string(export_json)));
        }
        else
        {
                g_print("File already exists (%s.json)\nIf there is a problem opening the file, you may need to delete it in: %s\n",
                        self->current_translation, file_path);
        }
}

static void
create_translation_async_thread_cb(GTask *task,
                                   gpointer source_object,
                                   gpointer task_data,
                                   GCancellable *cancellable)
{
        BibleContent *self = BIBLE_CONTENT(source_object);
        bible_content_parse_translation(self);
        g_task_return_pointer(task, task_data, NULL);
}

void create_translation_async(BibleContent *self,
                              //     GCancellable *cancellable,
                              GAsyncReadyCallback callback,
                              gpointer user_data)
{
        GTask *task = NULL; /* owned */
        CurlAsyncData *data = NULL;

        task = g_task_new(self, NULL, callback, user_data);

        g_task_set_source_tag(task, curl_get_async);

        g_task_set_return_on_cancel(task, FALSE);

        g_task_run_in_thread(task, create_translation_async_thread_cb);

        g_object_unref(task);
}

void curl_get_translation_books_finished_cb(GObject *source_object,
                                            GAsyncResult *res,
                                            gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        CurlAsyncData *res_data = ((CurlAsyncData *)result);
        self->books = g_strdup(res_data->memory.memory);

        create_translation_async(self, NULL, NULL);
}

void curl_get_translation_verses_finished_cb(GObject *source_object,
                                             GAsyncResult *res,
                                             gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        CurlAsyncData *res_data = ((CurlAsyncData *)result);
        self->verses = g_strdup(res_data->memory.memory);
        gchar *url = g_strdup_printf("https://bolls.life/get-books/%s/", self->current_translation);
        curl_get_async(self, curl_get_translation_books_finished_cb, NULL, url);

        // g_strfreev(&url);
}

void get_translation(BibleContent *self)
{
        if (self->current_translation_content_json && g_strcmp0(self->current_translation_content_json, "") > 0)
        {
                g_signal_emit(self, signals[SIG_TRANSLATION_LOADED], 0);
                return;
        }

        gchar *file_path = g_strdup_printf("%s%s.json", self->config_folder, self->current_translation);

        if (!g_file_test(file_path, G_FILE_TEST_EXISTS))
        {
                gchar *url = g_strdup_printf("https://bolls.life/static/translations/%s.json", self->current_translation);
                g_print("downloading (from: \"%s\") and creating file: %s.json\n",
                        url,
                        self->current_translation);
                curl_get_async(self, curl_get_translation_verses_finished_cb, NULL, url);
        }
        else
        {
                gchar *file_path = g_strdup_printf("%s%s.json", self->config_folder, self->current_translation);
                GFile *file = g_file_new_for_path(file_path);
                open_translation_file(self, file);
        }
}

static void
get_chapter_from_translation(BibleContent *self, GetChapterData *data)
{

        // g_print("%100.100s", self->current_translation_content_json);
        if (!data)
                return;

        if (!self->current_translation)
                return;

        if (data->book <= 0 || data->chapter <= 0)
                return;

        if (!self->current_translation_json_object)
                self->current_translation_json_object = json_tokener_parse(self->current_translation_content_json);

        json_object *book_object;
        json_object *chapter_object;
        json_object *verse_containter_object;
        json_object *verse_object;

        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(data->buffer, &start);
        gtk_text_buffer_get_end_iter(data->buffer, &end);
        gtk_text_buffer_delete(data->buffer, &start, &end);

        struct json_object_iterator it_export = json_object_iter_begin(self->current_translation_json_object);
        struct json_object_iterator itEnd_export = json_object_iter_end(self->current_translation_json_object);

        guint current_book_counter = 1;
        while (!json_object_iter_equal(&it_export, &itEnd_export))
        {
                if (data->book != current_book_counter)
                {
                        current_book_counter++;
                        json_object_iter_next(&it_export);
                        continue;
                }
                else
                {
                        book_object = json_object_iter_peek_value(&it_export);
                        if (!json_object_object_get_ex(book_object, g_strdup_printf("%i", data->chapter), &chapter_object))
                                break;

                        gtk_label_set_label(data->title_label,
                                            g_strdup_printf("%s %i",
                                                            json_object_iter_peek_name(&it_export),
                                                            self->current_chapter));

                        // scroll_button_set_label(data->book_button, json_object_iter_peek_name(&it_export));
                        // scroll_button_set_label(data->tranlation_button, self->current_translation);

                        size_t chapter_length = json_object_array_length(chapter_object);
                        for (size_t i = 1; i < chapter_length; i++)
                        {
                                verse_containter_object = json_object_array_get_idx(chapter_object, i - 1);
                                // g_print("verse (%lu): %10.10s \n", i, json_object_get_string(verse_containter_object));
                                ;
                                gtk_text_buffer_get_end_iter(data->buffer, &end);
                                gtk_text_buffer_insert_markup(data->buffer,
                                                              &end,
                                                              g_strdup_printf(" <i>%lu</i> %s",
                                                                              i,
                                                                              json_object_get_string(verse_containter_object)),
                                                              -1);
                        }
                        break;
                }
        }
        g_signal_handlers_disconnect_by_func(self, get_chapter_from_translation, data);

        free(data);
}

void bible_content_get_chapter(BibleContent *self,
                               GtkTextBuffer *buffer,
                               GtkLabel *title,
                               ScrollButton *book_button,
                               ScrollButton *translation_button)
{

        // if (!SCROLL_IS_BUTTON(book_button) || !SCROLL_IS_BUTTON(translation_button))
        //         return;

        GetChapterData *data;
        data = (GetChapterData *)malloc(sizeof(guint) * 2); //+ sizeof(book_button) + sizeof(translation_button));

        data->book = self->current_book;
        data->chapter = self->current_chapter;

        g_assert(GTK_IS_TEXT_BUFFER(buffer));
        g_assert(GTK_IS_LABEL(title));
        // g_assert(SCROLL_IS_BUTTON(book_button));
        // g_assert(SCROLL_IS_BUTTON(translation_button));

        data->title_label = title;
        data->buffer = buffer;
        // data->book_button = book_button;
        // data->tranlation_button = translation_button;

        g_signal_connect(self, "translation-loaded", G_CALLBACK(get_chapter_from_translation), data);
        get_translation(self);
}

void bible_content_set_current_chapter_text(BibleContent *self,
                                            GtkTextBuffer *buffer,
                                            GtkLabel *title,
                                            ScrollButton *book_button,
                                            ScrollButton *translation_button)
{
        bible_content_get_chapter(self, buffer, title, book_button, translation_button);
}

void bible_content_set_current_chapter_next(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        // if (self->current_chapter < self->current_book_chapters)
        // {
        self->current_chapter++;
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_CHAPTER]);
        // }
        // else if (self->current_book < self->current_translation_books)
        // {
        //         self->current_chapter = 1;
        //         self->current_book++;
        //         g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_BOOK]);
        //         g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_CHAPTER]);
        // }
}

void bible_content_set_current_chapter_previous(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        if (self->current_chapter > 1)
        {
                self->current_chapter--;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_CHAPTER]);
        }
        else if (self->current_book > 1)
        {
                self->current_book--;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_BOOK]);
        }
}

static void
bible_content_set_current_chapter(BibleContent *self, guint chapter)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        if (chapter != self->current_chapter)
        {
                self->current_chapter = chapter;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_CHAPTER]);
        }
}

static guint
bible_content_get_current_chapter(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        return self->current_chapter;
}

static void
bible_content_set_current_book(BibleContent *self, guint book)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        if (book != self->current_book)
        {
                self->current_book = book;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_BOOK]);
        }
}

static const guint
bible_content_get_current_book(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        return self->current_book;
}

static void
bible_content_set_current_translation(BibleContent *self, const gchar *translation)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        if (g_strcmp0(translation, self->current_translation) != 0)
        {
                g_free(self->current_translation);
                // self->current_translation = g_strdup("CJB");
                self->current_translation = g_strdup(translation);

                // g_print("translation: %s\n", translation);
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_TRANSLATION]);
        }
}
static const gchar *
bible_content_get_current_translation(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        return self->current_translation;
}

static void
bible_content_set_current_language(BibleContent *self, const gchar *language)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        if (g_strcmp0(language, self->current_language) != 0)
        {
                g_free(self->current_language);
                self->current_language = g_strdup(language);
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_LANGUAGE]);
        }
}
static const gchar *
bible_content_get_current_language(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        return self->current_language;
}

void bible_content_get_property(GObject *object,
                                guint prop_id,
                                GValue *value,
                                GParamSpec *pspec)
{
        BibleContent *self = BIBLE_CONTENT(object);

        switch (prop_id)
        {
        case PROP_CURRENT_CHAPTER:
                g_value_set_uint(value, bible_content_get_current_chapter(self));
                break;

        case PROP_CURRENT_BOOK:
                g_value_set_uint(value, bible_content_get_current_book(self));
                break;

        case PROP_CURRENT_TRANSLATION:
                g_value_set_string(value, bible_content_get_current_translation(self));
                break;
        case PROP_CURRENT_LANGUAGE:
                g_value_set_string(value, bible_content_get_current_translation(self));
                break;
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

void bible_content_set_property(GObject *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
        BibleContent *self = BIBLE_CONTENT(object);

        if (!value)
                return;

        switch (prop_id)
        {
        case PROP_CURRENT_CHAPTER:
                bible_content_set_current_chapter(self, g_value_get_uint(value));
                break;
        case PROP_CURRENT_BOOK:
                bible_content_set_current_book(self, g_value_get_uint(value));
                break;
        case PROP_CURRENT_TRANSLATION:
                bible_content_set_current_translation(self, g_value_get_string(value));
                break;
        case PROP_CURRENT_LANGUAGE:
                bible_content_set_current_language(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        }
}

static void
bible_content_init(BibleContent *self)
{
        // translation_parse_async(self, NULL, NULL);
        self->config_folder = g_strdup_printf("%s/wordofgod/", g_get_user_config_dir());
}

static void
bible_content_dispose(GObject *object)
{
        BibleContent *self = (BibleContent *)object;
        G_OBJECT_CLASS(bible_content_parent_class)->dispose(object);
}

static void
bible_content_class_init(BibleContentClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS(klass);

        object_class->get_property = bible_content_get_property;
        object_class->set_property = bible_content_set_property;
        object_class->dispose = bible_content_dispose;

        properties[PROP_CURRENT_CHAPTER] = g_param_spec_uint(
            "chapter", // Name
            "Chapter", // Nick
            "Chapter", // Blurb
            1,
            200,
            1,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_CURRENT_BOOK] = g_param_spec_uint(
            "book", // Name
            "Book", // Nick
            "Book", // Blurb
            1,
            100,
            1,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_CURRENT_TRANSLATION] = g_param_spec_string(
            "translation", // Name
            "Translation", // Nick
            "Translation", // Blurb
            "NKJV",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

        properties[PROP_CURRENT_LANGUAGE] = g_param_spec_string(
            "language", // Name
            "Language", // Nick
            "Language", // Blurb
            "English",
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));
        g_object_class_install_properties(object_class, LAST_PROP, properties);

        signals[SIG_TRANSLATION_LOADED] = g_signal_new(
            "translation-loaded",
            G_TYPE_FROM_CLASS(klass),
            G_SIGNAL_RUN_FIRST,
            0, NULL, NULL, NULL,
            G_TYPE_NONE, 0);
}

BibleContent *bible_content_new()
{

        // return (BibleContent *)g_object_new(BIBLE_TYPE_CONTENT, "window", window);
        return (BibleContent *)g_object_new(BIBLE_TYPE_CONTENT, NULL);
}