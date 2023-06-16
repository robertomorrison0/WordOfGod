#ifndef TRANSLATION_HPP
#define TRANSLATION_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <static_functions.hpp>
#include <filesystem>

const std::string SETTINGS_BASE_PATH = ".settings/";
const std::string LANGUAGES_FILE_PATH = SETTINGS_BASE_PATH + "languages.json";
const std::string BOOKS_FILE_PATH = SETTINGS_BASE_PATH + "books.json";
const std::string TMP_FOLDER = "/tmp/biblebolls/";

namespace BibleBolls
{
        class ContentInstance
        {
        public:
                ContentInstance(std::string translation = "CJB")
                    : current_translation(translation)
                {
                        init();
                        get_chapter_by_translation("ELB", 1, 10);
                }

                std::string get_next_chapter()
                {
                        int current_book_chapters = translation_book_info[current_translation].at(current_book - 1).chapters;
                        if (current_book_chapters == current_chapter)
                        {
                                current_book++;
                                current_chapter = 1;
                        }
                        else if (current_book_chapters > current_chapter)
                        {
                                current_chapter++;
                        }

                        return get_current_data();
                }

                std::string get_prev_chapter()
                {

                        if (current_chapter > 1)
                        {
                                current_chapter--;
                        }
                        else if (current_book > 1)
                        {
                                // int before_book_chapters =
                                current_book--;
                                current_chapter = translation_book_info[current_translation].at(current_book - 2).chapters;
                        }

                        return get_current_data();
                }

                void set_current_translation(std::string _translation)
                {
                        current_translation = _translation;
                }

                void set_current_book(int _book)
                {
                        current_book = _book;
                }

                void set_current_chapter(int _chapter)
                {
                        current_chapter = _chapter;
                }

                std::string get_current_data()
                {
                        return get_chapter_by_translation(current_translation, current_book, current_chapter);
                }

                std::string get_chapter_by_translation(std::string short_name, int _book, int _chapter)
                {
                        if (!std::filesystem::exists(TMP_FOLDER))
                                std::filesystem::create_directory(TMP_FOLDER);
                        std::string _chapter_file_name = TMP_FOLDER + short_name + "_" + std::to_string(_book) + "_" + std::to_string(_chapter) + ".json";
                        if (!std::filesystem::exists(_chapter_file_name))
                        {
                                std::string chapter = curl_get_data_from_url(std::string("https://bolls.life/get-chapter/" + short_name + "/" + std::to_string(_book) + "/" + std::to_string(_chapter) + "/"));

                                write_string_to_file(
                                    chapter,
                                    _chapter_file_name.c_str());
                        }

                        Json::Value parsed_json2;
                        if (!json_get_contents(_chapter_file_name.c_str(), &parsed_json2))
                                return "error";

                        std::stringstream ss;
                        ss << "<markup>"
                           << "<span weight='bold' font='"
                           << std::to_string(font_size * 1.5)
                           << "'> "
                           << translation_book_info[current_translation][current_book - 1].name // todo: bookname
                           << " "
                           << std::to_string(_chapter)
                           << " </span>\n\n";
                        for (auto element : parsed_json2)
                        {

                                ss << "<span style='italic'  font='"
                                   << std::to_string(font_size * 0.5)
                                   << "'> "
                                   << element["verse"].asString()
                                   << "    </span>"
                                   << "<span style='normal'  font='"
                                   << std::to_string(font_size)
                                   << "'>"
                                   << element["text"].asString()
                                   << "</span>";
                        }

                        ss << "\n\n"
                           << "</markup>";

                        return ss.str();
                }

                std::vector<TranslationInfo> &get_names()
                {
                        return names;
                }

                std::map<std::string, std::vector<BookInfo>> &get_bookinfo()
                {
                        return translation_book_info;
                }

                std::vector<BookInfo> &get_translation_bookinfo()
                {
                        return translation_book_info[current_translation];
                }

                void add_name(std::string _short_name, std::string _full_name)
                {
                        names.push_back(TranslationInfo{
                            .short_name = _short_name,
                            .full_name = _full_name,
                        });
                }

                void add_bookinfo(std::string _translation, int _id, int _chapters, std::string _name)
                {
                        translation_book_info[_translation].push_back(BookInfo{
                            .id = _id,
                            .chapters = _chapters,
                            .name = _name,
                        });
                }

        private:
                std::vector<TranslationInfo> names;
                std::map<std::string, std::vector<BookInfo>> translation_book_info;
                int font_size{16};

                std::string current_translation{"ELB"};
                int current_book{1};
                int current_chapter{1};

                void init()
                {
                        std::string languages = curl_get_data_from_url("https://bolls.life/static/bolls/app/views/languages.json");
                        std::string books = curl_get_data_from_url("https://bolls.life/static/bolls/app/views/translations_books.json");

                        if (!std::filesystem::exists(SETTINGS_BASE_PATH))
                        {
                                std::filesystem::create_directory(SETTINGS_BASE_PATH);
                        }

                        write_string_to_file(
                            languages,
                            LANGUAGES_FILE_PATH.c_str());

                        write_string_to_file(
                            books,
                            BOOKS_FILE_PATH.c_str());

                        Json::Value parsed_json2;

                        parsed_json2.clear();

                        if (!json_get_contents(LANGUAGES_FILE_PATH.c_str(), &parsed_json2))
                                return;

                        for (int i = 0; i < parsed_json2.size(); i++)
                        {
                                // std::cout << parsed_json2[i]["language"].asString() << std::endl;
                                for (int k = 0; k < parsed_json2[i]["translations"].size(); k++)
                                {
                                        add_name(parsed_json2[i]["translations"][k]["short_name"].asString(),
                                                 parsed_json2[i]["translations"][k]["full_name"].asString());
                                }
                        }

                        if (!json_get_contents(BOOKS_FILE_PATH.c_str(), &parsed_json2))
                                return;

                        for (auto name : get_names())
                        {
                                Json::Value books_content = parsed_json2[name.short_name];
                                for (int i = 0; i < books_content.size(); i++)
                                {
                                        add_bookinfo(
                                            name.short_name,
                                            books_content[i]["bookid"].asInt(),
                                            books_content[i]["chapters"].asInt(),
                                            books_content[i]["name"].asString());
                                }
                        }

                        // for (auto &names : content->get_names())
                        // {
                        // std::string name = get_names().at(5).short_name;
                        // std::vector<BookInfo> bookinfos = get_bookinfo()[name];

                        // for (auto &bookinfo : bookinfos)
                        //         std::cout << "id:" << bookinfo.id << "chapters:" << bookinfo.chapters << "name:" << bookinfo.name << std::endl;
                }
        };

};
#endif