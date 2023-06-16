#ifndef STATIC_FUNCTIONS_HPP
#define STATIC_FUNCTIONS_HPP

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/value.h>
#include <json/json.h>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <filesystem>

namespace BibleBolls
{
        static void
        write_string_to_file(std::string &_content, const char *_file_path)
        {
                std::ofstream outfile(_file_path);

                outfile << _content << std::endl;

                outfile.close();
        }

        typedef struct Memory
        {
                std::stringstream data;
        } Memory;

        typedef struct TranslationInfo
        {
                std::string short_name;
                std::string full_name;

        } TranslationInfo;

        typedef struct BookInfo
        {
                int id;
                int chapters;
                std::string name;

        } BookInfo;

        typedef struct VerseInfo
        {
                int verse;
                std::string text;

        } VerseInfo;

        typedef struct ChapterInfo
        {
                int id;
                std::vector<VerseInfo> verses;

        } ChapterInfo;

        static size_t write_callback(char *contents, size_t size, size_t nmemb, void *userp)
        {
                size_t real_size = size * nmemb;
                Memory *data = (Memory *)userp;

                data->data << std::string(contents);
                // char *ptr = (char *)realloc(data->data, data->size + real_size + 1);
                // if (ptr == nullptr)
                //         return 0;

                // data->data = ptr;
                // memcpy(&data->data[data->size], contents, realsize);
                // data->size += real_size;
                // data->data[data->size] = 0;
                return real_size;
        }

        static bool json_get_contents(const char *_path, Json::Value *_value, bool _collect_comments = false)
        {
                Json::CharReaderBuilder rbuilder;
                rbuilder.settings_["emitUTF8"] = false;
                rbuilder["collectComments"] = _collect_comments;
                JSONCPP_STRING errs;

                std::fstream f;
                f.open(_path, std::ios::in);
                if (!f.is_open())
                {
                        std::cout << "Open json file error!(" << _path << ")" << std::endl;
                }

                bool parse_ok = Json::parseFromStream(rbuilder, f, _value, &errs);

                f.close();
                return parse_ok;
        };

        static std::string curl_get_data_from_url(std::string _url)
        {
                CURL *curl;
                CURLcode return_code;
                curl_global_init(CURL_GLOBAL_ALL);

                Memory data = Memory{.data = std::stringstream("")};

                curl = curl_easy_init();

                if (!curl)
                {
                        printf("%s", "Curl init failed");
                        return "";
                }

                curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

                return_code = curl_easy_perform(curl);

                if (return_code != CURLE_OK)
                {
                        fprintf(stderr, "CURL_ERROR: %s\n", curl_easy_strerror(return_code));
                        curl_easy_cleanup(curl);
                }

                // std::cout << data.data.str() << std::endl;

                curl_global_cleanup();

                return data.data.str();
        }
};

#endif