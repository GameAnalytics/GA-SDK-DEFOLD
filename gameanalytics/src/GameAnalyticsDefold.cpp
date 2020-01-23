#include "GameAnalyticsDefold.h"
#if defined(DM_PLATFORM_IOS)
#include "ios/GameAnalyticsCpp.h"
#elif defined(DM_PLATFORM_ANDROID)
#include "android/GameAnalyticsJNI.h"
#elif defined(DM_PLATFORM_HTML5)
#include "html5/GameAnalytics.h"
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
#include "cpp/GameAnalytics.h"
#endif
#include <string.h>
#include <stdio.h>
#include <dmsdk/sdk.h>
#include <dmsdk/dlib/dstrings.h>

namespace gameanalytics
{
    namespace defold
    {
#if defined(DM_PLATFORM_HTML5)
        void GameAnalytics::runHtml5Code(lua_State *L, const char* code)
        {
            lua_getglobal(L, "html5");                              // push 'html5' onto stack
            lua_getfield(L, -1, "run");                             // push desired function
            lua_pushstring(L, code);                                // push argument
            lua_call(L, 1, 0);                                      // call function with 1 arg, 0 return value
            lua_pop(L, 1);                                          // pop 'html5'
        }

        std::vector<char> GameAnalytics::runHtml5CodeWithReturnString(lua_State *L, const char* code)
        {
            std::vector<char> result;
            lua_getglobal(L, "html5");                              // push 'html5' onto stack
            lua_getfield(L, -1, "run");                             // push desired function
            lua_pushstring(L, code);                                // push argument
            lua_call(L, 1, 1);                                      // call function with 1 arg, 1 return value
            const char* returnValue = lua_tostring(L, 1);           // get the result
            size_t s = strlen(returnValue);
            for(size_t i = 0; i < s; ++i)
            {
                result.push_back(returnValue[i]);
            }
            result.push_back('\0');
            lua_pop(L, 1);                                          // pop 'html5'
            return result;
        }

        bool GameAnalytics::runHtml5CodeWithReturnBool(lua_State *L, const char* code)
        {
            bool result = false;
            lua_getglobal(L, "html5");                              // push 'html5' onto stack
            lua_getfield(L, -1, "run");                             // push desired function
            lua_pushstring(L, code);                                // push argument
            lua_call(L, 1, 1);                                      // call function with 1 arg, 1 return value
            result = lua_toboolean(L, 1);                            // get the result
            lua_pop(L, 1);                                          // pop 'html5'

            return result;
        }
#endif
        std::vector<CharArray> GameAnalytics::split(char* str, const char* delimiter)
        {
            std::vector<CharArray> result;

            char * token , *last;
            token = dmStrTok(str, delimiter, &last);
            while (token != NULL)
            {
                CharArray a;
                dmSnPrintf(a.array, sizeof(a.array), "%s", token);
                result.push_back(a);
                token = dmStrTok(NULL, delimiter, &last);
            }

            return result;
        }

        void GameAnalytics::configureAvailableCustomDimensions01(lua_State *L, const char* list)
        {
#if defined(DM_PLATFORM_IOS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            GameAnalyticsCpp::configureAvailableCustomDimensions01(split(listArray, ","));
#elif defined(DM_PLATFORM_ANDROID)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            jni_configureAvailableCustomDimensions01(split(listArray, ","));
#elif defined(DM_PLATFORM_HTML5)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(list) + strlen(list) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            char code[strlen(arrayString) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureAvailableCustomDimensions01(JSON.parse('%s'))", arrayString);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(listArray) + strlen(listArray) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions01(arrayString);
#endif
        }

        void GameAnalytics::configureAvailableCustomDimensions02(lua_State *L, const char* list)
        {
#if defined(DM_PLATFORM_IOS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            GameAnalyticsCpp::configureAvailableCustomDimensions02(split(listArray, ","));
#elif defined(DM_PLATFORM_ANDROID)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            jni_configureAvailableCustomDimensions02(split(listArray, ","));
#elif defined(DM_PLATFORM_HTML5)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(list) + strlen(list) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            char code[strlen(arrayString) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureAvailableCustomDimensions02(JSON.parse('%s'))", arrayString);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(listArray) + strlen(listArray) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions02(arrayString);
#endif
        }

        void GameAnalytics::configureAvailableCustomDimensions03(lua_State *L, const char* list)
        {
#if defined(DM_PLATFORM_IOS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            GameAnalyticsCpp::configureAvailableCustomDimensions03(split(listArray, ","));
#elif defined(DM_PLATFORM_ANDROID)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            jni_configureAvailableCustomDimensions03(split(listArray, ","));
#elif defined(DM_PLATFORM_HTML5)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(list) + strlen(list) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            char code[strlen(arrayString) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureAvailableCustomDimensions03(JSON.parse('%s'))", arrayString);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(listArray) + strlen(listArray) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions03(arrayString);
#endif
        }

        void GameAnalytics::configureAvailableResourceCurrencies(lua_State *L, const char* list)
        {
#if defined(DM_PLATFORM_IOS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            GameAnalyticsCpp::configureAvailableResourceCurrencies(split(listArray, ","));
#elif defined(DM_PLATFORM_ANDROID)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            jni_configureAvailableResourceCurrencies(split(listArray, ","));
#elif defined(DM_PLATFORM_HTML5)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(list) + strlen(list) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            char code[strlen(arrayString) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureAvailableResourceCurrencies(JSON.parse('%s'))", arrayString);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(listArray) + strlen(listArray) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            gameanalytics::GameAnalytics::configureAvailableResourceCurrencies(arrayString);
#endif
        }

        void GameAnalytics::configureAvailableResourceItemTypes(lua_State *L, const char* list)
        {
#if defined(DM_PLATFORM_IOS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            GameAnalyticsCpp::configureAvailableResourceItemTypes(split(listArray, ","));
#elif defined(DM_PLATFORM_ANDROID)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            jni_configureAvailableResourceItemTypes(split(listArray, ","));
#elif defined(DM_PLATFORM_HTML5)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(list) + strlen(list) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            char code[strlen(arrayString) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureAvailableResourceItemTypes(JSON.parse('%s'))", arrayString);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            char listArray[strlen(list) + 1];
            dmSnPrintf(listArray, sizeof(listArray), "%s", list);
            size_t totalSize = strlen(listArray) + strlen(listArray) * 2 + 3;
            char arrayString[totalSize];
            if(strlen(list) > 0)
            {
                std::vector<CharArray> array = split(listArray, ",");
                dmStrlCpy(arrayString, "[\"", sizeof(arrayString));

                for(size_t i = 0; i < array.size(); ++i)
                {
                    if(i > 0)
                    {
                        dmStrlCat(arrayString, "\",\"", sizeof(arrayString));
                    }
                    dmStrlCat(arrayString, array[i].array, sizeof(arrayString));
                }
                dmStrlCat(arrayString, "\"]", sizeof(arrayString));
            }
            else
            {
                dmSnPrintf(arrayString, sizeof(arrayString), "[]");
            }
            gameanalytics::GameAnalytics::configureAvailableResourceItemTypes(arrayString);
#endif
        }

        void GameAnalytics::configureBuild(lua_State *L, const char *build)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureBuild(build);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureBuild(build);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(build) + 48];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureBuild('%s')", build);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureBuild(build);
#endif
        }

        void GameAnalytics::configureAutoDetectAppVersion(lua_State *L, bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAutoDetectAppVersion(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAutoDetectAppVersion(flag);
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
#endif
        }

        void GameAnalytics::configureUserId(lua_State *L, const char *userId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureUserId(userId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureUserId(userId);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(userId) + 49];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureUserId('%s')", userId);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureUserId(userId);
#endif
        }

        void GameAnalytics::configureSdkGameEngineVersion(lua_State *L, const char *gameEngineSdkVersion)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureSdkGameEngineVersion(gameEngineSdkVersion);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureSdkGameEngineVersion(gameEngineSdkVersion);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(gameEngineSdkVersion) + 63];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureSdkGameEngineVersion('%s')", gameEngineSdkVersion);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureSdkGameEngineVersion(gameEngineSdkVersion);
#endif
        }

        void GameAnalytics::configureGameEngineVersion(lua_State *L, const char *gameEngineVersion)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureGameEngineVersion(gameEngineVersion);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureGameEngineVersion(gameEngineVersion);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(gameEngineVersion) + 60];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.configureGameEngineVersion('%s')", gameEngineVersion);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureGameEngineVersion(gameEngineVersion);
#endif
        }

        void GameAnalytics::configureWritablePath(lua_State *L, const char *writablePath)
        {
#if defined(DM_PLATFORM_IOS)
#elif defined(DM_PLATFORM_ANDROID)
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureWritablePath(writablePath);
#endif
        }

        void GameAnalytics::initialize(lua_State *L, const char *gameKey, const char *gameSecret, bool use_imei_android)
        {

#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_ANDROID)
            jni_initialize(gameKey, gameSecret, use_imei_android);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(gameKey) + strlen(gameSecret) + 50];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.initialize('%s', '%s')", gameKey, gameSecret);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::initialize(gameKey, gameSecret);
#endif
        }

#if defined(DM_PLATFORM_IOS)
        void GameAnalytics::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *fields)
        {
            GameAnalyticsCpp::addBusinessEvent(currency, amount, itemType, itemId, cartType, receipt, "");
        }

        void GameAnalytics::addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields)
        {
            GameAnalyticsCpp::addBusinessEventAndAutoFetchReceipt(currency, amount, itemType, itemId, cartType, "");
        }
#elif defined(DM_PLATFORM_ANDROID)
        void GameAnalytics::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *signature, const char *fields)
        {
            jni_addBusinessEventWithReceipt(currency, amount, itemType, itemId, cartType, receipt, "google_play", signature, "");
        }
#endif

        void GameAnalytics::addBusinessEvent(lua_State *L, const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addBusinessEvent(currency, amount, itemType, itemId, cartType, "", "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addBusinessEvent(currency, amount, itemType, itemId, cartType, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(currency) + strlen(itemType) + strlen(itemId) + strlen(cartType) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addBusinessEvent('%s', %d, '%s', '%s', '%s')", currency, amount, itemType, itemId, cartType);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType);
#endif
        }

        void GameAnalytics::addResourceEvent(lua_State *L, EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addResourceEvent((int)flowType, currency, amount, itemType, itemId, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addResourceEvent((int)flowType, currency, amount, itemType, itemId, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(currency) + strlen(itemType) + strlen(itemId) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addResourceEvent(%d, '%s', %f, '%s', '%s')", (int)flowType, currency, amount, itemType, itemId);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addResourceEvent((gameanalytics::EGAResourceFlowType)((int)flowType), currency, amount, itemType, itemId);
#endif
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEvent((int)progressionStatus, progression01, progression02, progression03, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEvent((int)progressionStatus, progression01, progression02, progression03, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(progression01) + strlen(progression02) + strlen(progression03) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addProgressionEvent(%d, '%s', '%s', '%s')", (int)progressionStatus, progression01, progression02, progression03);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03);
#endif
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(progression01) + strlen(progression02) + strlen(progression03) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addProgressionEvent(%d, '%s', '%s', '%s', %d)", (int)progressionStatus, progression01, progression02, progression03, score);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03, score);
#endif
        }

        void GameAnalytics::addDesignEvent(lua_State *L, const char *eventId, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEvent(eventId, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEvent(eventId, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(eventId) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addDesignEvent('%s')", eventId);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addDesignEvent(eventId);
#endif
        }

        void GameAnalytics::addDesignEvent(lua_State *L, const char *eventId, float value, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEventWithValue(eventId, value, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEventWithValue(eventId, value, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(eventId) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addDesignEvent('%s', %f)", eventId, value);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addDesignEvent(eventId, value);
#endif
        }

        void GameAnalytics::addErrorEvent(lua_State *L, EGAErrorSeverity severity, const char *message, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addErrorEvent((int)severity, message, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addErrorEvent((int)severity, message, "");
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(message) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.addErrorEvent(%d, '%s')", (int)severity, message);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addErrorEvent((gameanalytics::EGAErrorSeverity)((int)severity), message);
#endif
        }

        void GameAnalytics::setEnabledInfoLog(lua_State *L, bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledInfoLog(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledInfoLog(flag);
#elif defined(DM_PLATFORM_HTML5)
            char code[100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setEnabledInfoLog(%s)", (flag ? "true" : "false"));
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledInfoLog(flag);
#endif
        }

        void GameAnalytics::setEnabledVerboseLog(lua_State *L, bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledVerboseLog(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledVerboseLog(flag);
#elif defined(DM_PLATFORM_HTML5)
            char code[100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setEnabledVerboseLog(%s)", (flag ? "true" : "false"));
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledVerboseLog(flag);
#endif
        }

        void GameAnalytics::setEnabledManualSessionHandling(lua_State *L, bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledManualSessionHandling(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledManualSessionHandling(flag);
#elif defined(DM_PLATFORM_HTML5)
            char code[100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setEnabledManualSessionHandling(%s)", (flag ? "true" : "false"));
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledManualSessionHandling(flag);
#endif
        }

        void GameAnalytics::setEnabledEventSubmission(lua_State *L, bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledEventSubmission(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledEventSubmission(flag);
#elif defined(DM_PLATFORM_HTML5)
            char code[100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setEnabledEventSubmission(%s)", (flag ? "true" : "false"));
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledEventSubmission(flag);
#endif
        }

        void GameAnalytics::setEnabledErrorReporting(lua_State *L, bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledErrorReporting(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledErrorReporting(flag);
#elif defined(DM_PLATFORM_HTML5)
            // ;
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledErrorReporting(flag);
#endif
        }

        void GameAnalytics::setCustomDimension01(lua_State *L, const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(customDimension) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setCustomDimension01('%s')", customDimension);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setCustomDimension01(customDimension);
#endif
        }

        void GameAnalytics::setCustomDimension02(lua_State *L, const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension02(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension02(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(customDimension) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setCustomDimension02('%s')", customDimension);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setCustomDimension02(customDimension);
#endif
        }

        void GameAnalytics::setCustomDimension03(lua_State *L, const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension03(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension03(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            char code[strlen(customDimension) + 100];
            dmSnPrintf(code, sizeof(code), "gameanalytics.GameAnalytics.setCustomDimension03('%s')", customDimension);
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setCustomDimension03(customDimension);
#endif
        }

        void GameAnalytics::startSession(lua_State *L)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::startSession();
#elif defined(DM_PLATFORM_ANDROID)
            jni_startSession();
#elif defined(DM_PLATFORM_HTML5)
            runHtml5Code(L, "gameanalytics.GameAnalytics.startSession()");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::startSession();
#endif
        }

        void GameAnalytics::endSession(lua_State *L)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::endSession();
#elif defined(DM_PLATFORM_ANDROID)
            jni_endSession();
#elif defined(DM_PLATFORM_HTML5)
            runHtml5Code(L, "gameanalytics.GameAnalytics.endSession()");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::endSession();
#endif
        }

        void GameAnalytics::onQuit(lua_State *L)
        {
#if defined(DM_PLATFORM_IOS)
#elif defined(DM_PLATFORM_ANDROID)
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::onQuit();
#endif
        }

        std::vector<char> GameAnalytics::getRemoteConfigsValueAsString(lua_State *L, const char *key, const char *defaultValue)
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::getRemoteConfigsValueAsString(key, defaultValue);
#elif defined(DM_PLATFORM_ANDROID)
            return jni_getRemoteConfigsValueAsStringWithDefaultValue(key, defaultValue);
#elif defined(DM_PLATFORM_HTML5)
            size_t s = strlen(key) + strlen(defaultValue) + 70;
            char code[s];
            dmSnPrintf(code, s, "gameanalytics.GameAnalytics.getRemoteConfigsValueAsString('%s', '%s')", key, defaultValue);
            return runHtml5CodeWithReturnString(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            return gameanalytics::GameAnalytics::getRemoteConfigsValueAsString(key, defaultValue);
#endif
        }

        std::vector<char> GameAnalytics::getRemoteConfigsValueAsString(lua_State *L, const char *key)
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::getRemoteConfigsValueAsString(key);
#elif defined(DM_PLATFORM_ANDROID)
            return jni_getRemoteConfigsValueAsString(key);
#elif defined(DM_PLATFORM_HTML5)
            size_t s = strlen(key) + 63;
            char code[s];
            dmSnPrintf(code, s, "gameanalytics.GameAnalytics.getRemoteConfigsValueAsString('%s)", key);
            return runHtml5CodeWithReturnString(L, code);
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            return gameanalytics::GameAnalytics::getRemoteConfigsValueAsString(key);
#endif
        }

        bool GameAnalytics::isRemoteConfigsReady(lua_State *L)
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::isRemoteConfigsReady();
#elif defined(DM_PLATFORM_ANDROID)
            return jni_isRemoteConfigsReady();
#elif defined(DM_PLATFORM_HTML5)
            return runHtml5CodeWithReturnBool(L, "gameanalytics.GameAnalytics.isRemoteConfigsReady()");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            return gameanalytics::GameAnalytics::isRemoteConfigsReady();
#endif
        }

        std::vector<char> GameAnalytics::getRemoteConfigsContentAsString(lua_State *L)
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::getRemoteConfigsContentAsString();
#elif defined(DM_PLATFORM_ANDROID)
            return jni_getRemoteConfigsContentAsString();
#elif defined(DM_PLATFORM_HTML5)
            return runHtml5CodeWithReturnString(L, "gameanalytics.GameAnalytics.getRemoteConfigsContentAsString()");
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            return gameanalytics::GameAnalytics::getRemoteConfigsContentAsString();
#endif
        }
    }
}
