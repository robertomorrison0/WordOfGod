#include <bible_content.h>
#include <json-c/json.h>

#define BASE_URL ""

struct _BibleContent
{
        GObject parent;

        guint current_chapter;
        guint current_book;
        gchar *current_translation;
        gchar *current_language;

        gchar *translations_json;
        gchar *books_json;

        gchar *translation_books;
        gchar *translation_verses;

        GtkTextBuffer *buffer;
        gchar *curl_json_result;
        gchar *current_title;

        BibleTextPage *page;
        GtkStringList *language_list;
        GtkStringList *translation_list;
        GtkStringList *books_list;
        GtkStringList *chapters_list;

        BiblePreferencesWindow *preferences_window;

        guint current_book_chapters;
        guint current_translation_books;
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

static GParamSpec *properties[LAST_PROP];

typedef struct
{
        gchar *memory;
        size_t size;
} Memory;

typedef struct
{
        gchar *url;
        Memory memory;
} AsyncData;

void bible_content_set_current_chapter_next(BibleContent *self)
{
        g_return_if_fail(BIBLE_IS_CONTENT(self));
        if (self->current_chapter < self->current_book_chapters)
        {
                self->current_chapter++;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_CHAPTER]);
        }
        else if (self->current_book < self->current_translation_books)
        {
                self->current_chapter = 1;
                self->current_book++;
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_BOOK]);
                g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_CURRENT_CHAPTER]);
        }
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
                self->current_translation = g_strdup(translation);
                if (self->page)
                        bible_text_page_set_translation(self->page, self->current_translation);
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

void bible_content_set_page(BibleContent *self, BibleTextPage *page)
{
        if (!self->page)
                self->page = page;
}

gboolean
update_progressbar(Memory *data)
{
        // AsyncData *data = (AsyncData *)data_;

        // gtk_progress_bar_pulse(data->progressbar);

        return G_SOURCE_REMOVE;
}

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

void bible_content_get_current_title(BibleContent *self)
{

        struct json_object *parsed_json;
        struct json_object *test;

        parsed_json = json_tokener_parse(self->curl_json_result);

        self->current_translation_books = json_object_array_length(parsed_json);
        if (self->current_book < 1)
        {
                self->current_title = g_strdup("");
                return;
        }

        json_object *title;
        json_object *prev_title;
        json_object *title_text;
        json_object *book_chapters;
        json_object *prev_book_chapters;

        gchar book[100];
        title = json_object_array_get_idx(parsed_json, self->current_book - 1);
        // if (self->current_book > 2)
        // {
        //         prev_title = json_object_array_get_idx(parsed_json, self->current_book - 2);
        //         json_object_object_get_ex(prev_title, "chapters", &prev_book_chapters);
        //         self->previous_book_chapters = json_object_get_uint64(book_chapters);
        // }
        json_object_object_get_ex(title, "name", &title_text);
        json_object_object_get_ex(title, "chapters", &book_chapters);
        sprintf(book, "%s %i", json_object_get_string(title_text), self->current_chapter);

        self->current_title = g_strdup(book);
        self->current_book_chapters = json_object_get_uint64(book_chapters);
}

void bible_content_get_translation_info(BibleContent *self, const gchar *translation)
{

        struct json_object *parsed_json;
        struct json_object *test;

        parsed_json = json_tokener_parse(self->curl_json_result);

        self->current_translation_books = json_object_array_length(parsed_json);
        json_object *title;
        json_object *prev_title;
        json_object *title_text;
        json_object *book_chapters;
        json_object *prev_book_chapters;

        gchar book[100];
        title = json_object_array_get_idx(parsed_json, self->current_book - 1);
        // if (self->current_book > 2)
        // {
        //         prev_title = json_object_array_get_idx(parsed_json, self->current_book - 2);
        //         json_object_object_get_ex(prev_title, "chapters", &prev_book_chapters);
        //         self->previous_book_chapters = json_object_get_uint64(book_chapters);
        // }
        json_object_object_get_ex(title, "name", &title_text);
        json_object_object_get_ex(title, "chapters", &book_chapters);
        sprintf(book, "%s\n%i", json_object_get_string(title_text), self->current_chapter);

        self->current_title = g_strdup(book);
        self->current_book_chapters = json_object_get_uint64(book_chapters);
}

// todo: error check
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

        translation_verses_json = json_tokener_parse(self->translation_verses);
        translation_books_json = json_tokener_parse(self->translation_books);

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
                        gchar *chapter_string = g_strdup_printf("%lu", k + 1);
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
                                json_object_object_get_ex(verse, "verse", &verse_id);

                                json_object *verse_object = json_object_new_object();

                                // todo: error check
                                json_object_object_add(verse_object, "n", verse_id);
                                json_object_object_add(verse_object, "t", verse_text);
                                json_object_array_add(chapter_array, verse_object);
                                main_verse_counter++;
                        }
                }
        }

        // struct json_object_iterator it_export = json_object_iter_begin(export_json);
        // struct json_object_iterator itEnd_export = json_object_iter_end(export_json);
        // while (!json_object_iter_equal(&it_verses, &itEnd_verses))
        // {
        //         // json_object *obj = json_object_iter_peek_value(&it_verses);
        //         // json_object *book = json_object_object_get(obj, "book");
        //         // ;
        //         // printf("%lu %s\n", current_object_cunter, json_object_get_string(book));

        //         current_object_cunter++;
        //         json_object_iter_next(&it_verses);

        gchar file_path[80];
        sprintf(file_path,
                "%s.json",
                self->current_translation);

        json_object_to_file(file_path, export_json);
}

// GtkTextBuffer *
void bible_content_get_current_text(BibleContent *self)
{

        struct json_object *parsed_json;
        struct json_object *test;

        parsed_json = json_tokener_parse(self->curl_json_result);

        size_t n_verses = json_object_array_length(parsed_json);

        json_object *verse;
        json_object *verse_text;
        json_object *verse_number;

        if (!GTK_IS_TEXT_BUFFER(self->buffer))
        {
                self->buffer = gtk_text_buffer_new(NULL);
        }
        else
        {
                GtkTextIter start, end;
                gtk_text_buffer_get_start_iter(self->buffer, &start);
                gtk_text_buffer_get_end_iter(self->buffer, &end);
                gtk_text_buffer_delete(self->buffer, &start, &end);
                // g_object_free(self->buffer);
                self->buffer = gtk_text_buffer_new(NULL);
        }
        if (n_verses < 1)
        {
                self->buffer = NULL;
                return;
        }

        // GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
        GtkTextIter start, end;
        // GtkTextIter iters[n_verses + 1];

        gtk_text_buffer_get_start_iter(self->buffer, &start);
        gtk_text_buffer_get_end_iter(self->buffer, &end);
        gchar chapter[1000];

        gint mark_offsets[n_verses + 1];

        mark_offsets[0] = 0;
        for (size_t i = 0; i < n_verses; i++)
        {
                // memset(chapter, 0, strlen(chapter));

                verse = json_object_array_get_idx(parsed_json, i);
                json_object_object_get_ex(verse, "text", &verse_text);
                json_object_object_get_ex(verse, "verse", &verse_number);
                // sprintf(chapter, "<i><small>%i</small></i> %s ", json_object_get_int(verse_number), json_object_get_string(verse_text));
                sprintf(chapter, "%i %s ", json_object_get_int(verse_number), json_object_get_string(verse_text));

                gtk_text_buffer_insert_markup(self->buffer, &end, chapter, -1);
                gtk_text_buffer_get_end_iter(self->buffer, &end);
                mark_offsets[i + 1] = gtk_text_iter_get_offset(&end);
        }

        for (size_t i = 0; i < n_verses; i++)
        {
                gchar *index_str = g_strdup_printf("Verse%lu", i);

                GtkTextIter iter;
                gtk_text_buffer_get_iter_at_offset(self->buffer, &iter, mark_offsets[i]);

                GtkTextMark *m = gtk_text_buffer_get_mark(self->buffer, index_str);

                if (GTK_IS_TEXT_MARK(m))
                        gtk_text_buffer_delete_mark(self->buffer, m);

                gtk_text_buffer_add_mark(self->buffer, gtk_text_mark_new(index_str, false), &iter);
        }
}

static void
curl_get_async_thread_cb(GTask *task,
                         gpointer source_object,
                         gpointer task_data,
                         GCancellable *cancellable)
{

        AsyncData *data = (AsyncData *)task_data;
        curl_get_(data->url, &data->memory);

        g_task_return_pointer(task, task_data, NULL);
}

static void
parse_translation_thread_cb(GTask *task,
                            gpointer source_object,
                            gpointer task_data,
                            GCancellable *cancellable)
{

        AsyncData *data = (AsyncData *)task_data;
        curl_get_(data->url, &data->memory);

        g_task_return_pointer(task, task_data, NULL);
}

void curl_get_text_finished_cb(GObject *source_object,
                               GAsyncResult *res,
                               gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        AsyncData *res_data = ((AsyncData *)result);
        self->curl_json_result = g_strdup(res_data->memory.memory);
        bible_content_get_current_text(self);

        if (self->buffer != NULL)
                bible_text_page_set_text(self->page, self->buffer);
        // g_free(self->curl_json_result);
}

void curl_get_title_finished_cb(GObject *source_object,
                                GAsyncResult *res,
                                gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        AsyncData *res_data = ((AsyncData *)result);

        self->curl_json_result = g_strdup(res_data->memory.memory);
        bible_content_get_current_title(self);

        // bible_app_window_set_title(self->window, self->current_title);
        if (self->current_title != NULL)
                bible_text_page_set_title(self->page, self->current_title);
        // g_free(self->curl_json_result);
}

void translation_parse_async(BibleContent *self,
                             //     GCancellable *cancellable,
                             GAsyncReadyCallback callback,
                             gpointer user_data, const gchar *url)
{
        GTask *task = NULL; /* owned */
        AsyncData *data = NULL;

        task = g_task_new(self, NULL, callback, user_data);

        g_task_set_source_tag(task, translation_parse_async);

        /* Cancellation should be handled manually using mechanisms specific to
         * some_blocking_function(). */
        g_task_set_return_on_cancel(task, FALSE);

        /* Set up a closure containing the call’s parameters. Copy them to avoid
         * locking issues between the calling thread and the worker thread. */
        data = g_new0(AsyncData, 1);
        data->url = g_strdup(url);
        // data->memory.progressbar = self->progressbar;

        g_task_set_task_data(task, data, g_free);

        /* Run the task in a worker thread and return immediately while that continues
         * in the background. When it’s done it will call @callback in the current
         * thread default main context. */
        g_task_run_in_thread(task, parse_translation_thread_cb);

        g_object_unref(task);
}

void curl_get_translation_verses_finished_cb(GObject *source_object,
                                             GAsyncResult *res,
                                             gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        AsyncData *res_data = ((AsyncData *)result);
        self->translation_verses = g_strdup(res_data->memory.memory);

        // translation_parse_async(self, NULL, NULL, url);
}

void free_data(gpointer _data)
{
        AsyncData *data = (AsyncData *)_data;
        // g_free(data->url);
        // free(&data->memory);
        g_free(data);
}

void curl_get_async(BibleContent *self,
                    //     GCancellable *cancellable,
                    GAsyncReadyCallback callback,
                    gpointer user_data, const gchar url[])
{
        GTask *task = NULL; /* owned */
        AsyncData *data = NULL;

        task = g_task_new(self, NULL, callback, user_data);

        g_task_set_source_tag(task, curl_get_async);

        /* Cancellation should be handled manually using mechanisms specific to
         * some_blocking_function(). */
        g_task_set_return_on_cancel(task, FALSE);

        /* Set up a closure containing the call’s parameters. Copy them to avoid
         * locking issues between the calling thread and the worker thread. */
        data = g_new0(AsyncData, 1);
        data->url = g_strdup(url);
        // data->memory.progressbar = self->progressbar;

        g_task_set_task_data(task, data, g_free);

        /* Run the task in a worker thread and return immediately while that continues
         * in the background. When it’s done it will call @callback in the current
         * thread default main context. */
        g_task_run_in_thread(task, curl_get_async_thread_cb);

        g_object_unref(task);
}

void curl_get_translation_info_finished_cb(GObject *source_object,
                                           GAsyncResult *res,
                                           gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        AsyncData *res_data = ((AsyncData *)result);
        self->translation_books = g_strdup(res_data->memory.memory);

        gchar url[80];
        sprintf(url,
                "https://bolls.life/static/translations/%s.json",
                self->current_translation);

        curl_get_async(self, curl_get_translation_verses_finished_cb, NULL, url);
}

void bible_content_get_translations(BibleContent *self, const gchar *language)
{
        json_object *translations_json = json_tokener_parse(self->translations_json);

        json_object *language_object;
        json_object *translation_language_object;
        json_object *translation_object;
        json_object *translations_language_array;
        json_object *full_name_object;
        json_object *short_name_object;

        size_t n_languages = json_object_array_length(translations_json);

        size_t n_translations;

        if (!self->language_list)
                self->language_list = gtk_string_list_new(NULL);
        for (size_t i = 0; i < n_languages; i++)
        {
                language_object = json_object_array_get_idx(translations_json, i);
                json_object_object_get_ex(language_object, "language", &translation_language_object);

                if (g_strcmp0(language, json_object_get_string(translation_language_object)) == 0)
                        break;

                // gtk_string_list_append(self->language_list, json_object_get_string(translation_language_object));
        }

        json_object_object_get_ex(language_object, "translations", &translations_language_array);
        n_translations = json_object_array_length(translations_language_array);

        if (!self->translation_list)
                self->translation_list = gtk_string_list_new(NULL);

        for (size_t i = 0; i < n_translations; i++)
        {
                translation_object = json_object_array_get_idx(translations_language_array, i);
                json_object_object_get_ex(translation_object, "short_name", &short_name_object);
                gtk_string_list_append(self->translation_list, json_object_to_json_string(translation_object));
        }

        bible_preferences_window_add_translations(self->preferences_window, self->translation_list);

        if (self->translation_list)
        {
                g_object_unref(self->translation_list);
                self->translation_list = NULL;
        }
        if (self->language_list)
        {
                g_object_unref(self->language_list);
                self->language_list = NULL;
        }
}

void parse_languages(BibleContent *self)
{
        json_object *translations_json = json_tokener_parse(self->translations_json);

        json_object *language_object;
        json_object *translation_language_object;
        // json_object *full_name_object;
        // json_object *short_name_object;

        size_t n_languages = json_object_array_length(translations_json);

        if (!self->language_list)
                self->language_list = gtk_string_list_new(NULL);
        for (size_t i = 0; i < n_languages; i++)
        {
                language_object = json_object_array_get_idx(translations_json, i);
                json_object_object_get_ex(language_object, "language", &translation_language_object);
                gtk_string_list_append(self->language_list, json_object_get_string(translation_language_object));
        }
        bible_preferences_window_add_languages(self->preferences_window, self->language_list);
        if (self->translation_list)
        {
                g_object_unref(self->translation_list);
                self->translation_list = NULL;
        }
        if (self->language_list)
        {
                g_object_unref(self->language_list);
                self->language_list = NULL;
        }
}

void parse_books(BibleContent *self)
{
        json_object *books_json = json_tokener_parse(self->books_json);

        json_object *book_object;
        json_object *book_name_object;
        // json_object *full_name_object;
        // json_object *short_name_object;

        size_t n_books = json_object_array_length(books_json);

        if (!self->books_list)
                self->books_list = gtk_string_list_new(NULL);
        for (size_t i = 0; i < n_books; i++)
        {
                book_object = json_object_array_get_idx(books_json, i);
                json_object_object_get_ex(book_object, "name", &book_name_object);
                gtk_string_list_append(self->books_list, json_object_get_string(book_name_object));
        }
        bible_preferences_window_add_books(self->preferences_window, self->books_list, n_books);
        if (self->books_list)
        {
                g_object_unref(self->books_list);
                self->books_list = NULL;
        }
        if (self->language_list)
        {
                g_object_unref(self->language_list);
                self->language_list = NULL;
        }
        if (self->translation_list)
        {
                g_object_unref(self->translation_list);
                self->translation_list = NULL;
        }
}

void bible_content_get_chapters(BibleContent *self, BiblePreferencesWindow *preferences_window, guint book)
{

        if (self->preferences_window == NULL)
                self->preferences_window = preferences_window;

        json_object *books_json = json_tokener_parse(self->books_json);

        json_object *book_object;
        json_object *book_chapters_object;
        json_object *book_name_object;
        // json_object *full_name_object;
        // json_object *short_name_object;

        size_t n_books = json_object_array_length(books_json);

        if (!self->chapters_list)
                self->chapters_list = gtk_string_list_new(NULL);

        book_object = json_object_array_get_idx(books_json, book - 1);
        json_object_object_get_ex(book_object, "chapters", &book_chapters_object);
        guint n_chapters_p = json_object_get_int(book_chapters_object) + 1;

        for (int i = 1; i < n_chapters_p; i++)
        {
                gtk_string_list_append(self->chapters_list, g_strdup_printf("%i", i));
        }
        bible_preferences_window_add_chapters(self->preferences_window, self->chapters_list, n_chapters_p - 1);
        if (self->chapters_list)
        {
                g_object_unref(self->chapters_list);
                self->chapters_list = NULL;
        }
        if (self->books_list)
        {
                g_object_unref(self->books_list);
                self->books_list = NULL;
        }
        if (self->language_list)
        {
                g_object_unref(self->language_list);
                self->language_list = NULL;
        }
        if (self->translation_list)
        {
                g_object_unref(self->translation_list);
                self->translation_list = NULL;
        }
}

void curl_get_languages_finished_cb(GObject *source_object,
                                    GAsyncResult *res,
                                    gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        AsyncData *res_data = ((AsyncData *)result);
        self->translations_json = g_strdup(res_data->memory.memory);
        parse_languages(self);
}

void curl_get_books_finished_cb(GObject *source_object,
                                GAsyncResult *res,
                                gpointer data)
{
        BibleContent *self = ((BibleContent *)source_object);

        gpointer result = g_task_propagate_pointer(G_TASK(res), NULL);
        AsyncData *res_data = ((AsyncData *)result);
        self->books_json = g_strdup(res_data->memory.memory);
        parse_books(self);
}

void bible_content_get_translation(BibleContent *self, BibleTextPage *page)
{
        if (page == NULL)
                return;

        if (self->page == NULL)
                self->page = page;

        gchar url[80];
        sprintf(url,
                "https://bolls.life/get-books/%s/",
                self->current_translation);

        curl_get_async(self, curl_get_translation_info_finished_cb, NULL, url);
}

void bible_content_get_languages(BibleContent *self, BiblePreferencesWindow *preferences_window)
{
        if (self->preferences_window == NULL)
                self->preferences_window = preferences_window;

        gchar url[] = "https://bolls.life/static/bolls/app/views/languages.json";

        curl_get_async(self, curl_get_languages_finished_cb, NULL, url);
}

void bible_content_get_text(BibleContent *self, BibleTextPage *page)
{
        if (page == NULL)
                return;

        if (self->page == NULL)
                self->page = page;

        gchar url[80];
        sprintf(url,
                "https://bolls.life/get-text/%s/%i/%i/",
                self->current_translation,
                self->current_book,
                self->current_chapter);
        curl_get_async(self, curl_get_text_finished_cb, NULL, url);
}

void bible_content_get_books(BibleContent *self, BiblePreferencesWindow *preferences_window, const gchar *translation)
{
        if (self->preferences_window == NULL)
                self->preferences_window = preferences_window;

        gchar url[80];
        sprintf(url,
                "https://bolls.life/get-books/%s/",
                translation);
        curl_get_async(self, curl_get_books_finished_cb, NULL, url);
}

void bible_content_get_title(BibleContent *self, BibleTextPage *page)
{
        if (page == NULL)
                return;

        if (self->page == NULL)
                self->page = page;

        gchar url[80];
        sprintf(url,
                "https://bolls.life/get-books/%s/",
                self->current_translation);
        curl_get_async(self, curl_get_title_finished_cb, NULL, url);
        // g_free(url);
}

static void
bible_content_init(BibleContent *self)
{
}

static void
bible_content_dispose(GObject *object)
{
        BibleContent *self = (BibleContent *)object;

        g_strfreev(&self->current_translation);
        g_strfreev(&self->current_language);
        g_strfreev(&self->translations_json);
        g_strfreev(&self->books_json);
        g_strfreev(&self->translation_books);
        g_strfreev(&self->translation_verses);
        g_strfreev(&self->curl_json_result);
        g_strfreev(&self->current_title);

        g_clear_object(&self->language_list);
        g_clear_object(&self->translation_list);
        g_clear_object(&self->books_list);
        g_clear_object(&self->chapters_list);

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
}

BibleContent *bible_content_new(void)
{

        return (BibleContent *)g_object_new(BIBLE_TYPE_CONTENT, NULL);
}