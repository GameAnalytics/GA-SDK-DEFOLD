#include "GameAnalyticsDefold.h"
#if defined(DM_PLATFORM_IOS)
#include "ios/GameAnalyticsCpp.h"
#elif defined(DM_PLATFORM_ANDROID)
#include "android/GameAnalyticsJNI.h"
#elif defined(DM_PLATFORM_HTML5)
#include "html5/GameAnalytics.h"
#include <emscripten.h>
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
#if defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
        dmScript::LuaCallbackInfo* _remote_configs_listener;
#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
        class DefoldOnRemoteConfigsListener : public IRemoteConfigsListener
        {
        public:
            DefoldOnRemoteConfigsListener() {}
            void onRemoteConfigsUpdated();
        };

        void DefoldOnRemoteConfigsListener::onRemoteConfigsUpdated()
        {
            if(!_remote_configs_listener)
            {
                dmLogWarning("Received remote configs update but no listener was set!");
                return;
            }

            lua_State* L = dmScript::GetCallbackLuaContext(_remote_configs_listener);
            DM_LUA_STACK_CHECK(L, 0);

            if (!dmScript::SetupCallback(_remote_configs_listener))
            {
                dmLogWarning("SetupCallback failed for remote configs");
                return;
            }

            int ret = dmScript::PCall(L, 1, 0);
            dmScript::TeardownCallback(_remote_configs_listener);
        }
#endif
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

        void GameAnalytics::configureAvailableCustomDimensions01(const char* list)
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
            js_configureAvailableCustomDimensions01(arrayString);
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

        void GameAnalytics::configureAvailableCustomDimensions02(const char* list)
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
            js_configureAvailableCustomDimensions02(arrayString);
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

        void GameAnalytics::configureAvailableCustomDimensions03(const char* list)
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
            js_configureAvailableCustomDimensions03(arrayString);
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

        void GameAnalytics::configureAvailableResourceCurrencies(const char* list)
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
            js_configureAvailableResourceCurrencies(arrayString);
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

        void GameAnalytics::configureAvailableResourceItemTypes(const char* list)
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
            js_configureAvailableResourceItemTypes(arrayString);
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

        void GameAnalytics::configureBuild(const char *build)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureBuild(build);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureBuild(build);
#elif defined(DM_PLATFORM_HTML5)
            js_configureBuild(build);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureBuild(build);
#endif
        }

        void GameAnalytics::configureAutoDetectAppVersion(bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAutoDetectAppVersion(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAutoDetectAppVersion(flag);
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
#endif
        }

        void GameAnalytics::configureUserId(const char *userId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureUserId(userId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureUserId(userId);
#elif defined(DM_PLATFORM_HTML5)
            js_configureUserId(userId);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureUserId(userId);
#endif
        }

        void GameAnalytics::configureSdkGameEngineVersion(const char *gameEngineSdkVersion)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureSdkGameEngineVersion(gameEngineSdkVersion);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureSdkGameEngineVersion(gameEngineSdkVersion);
#elif defined(DM_PLATFORM_HTML5)
            js_configureSdkGameEngineVersion(gameEngineSdkVersion);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureSdkGameEngineVersion(gameEngineSdkVersion);
#endif
        }

        void GameAnalytics::configureGameEngineVersion(const char *gameEngineVersion)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureGameEngineVersion(gameEngineVersion);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureGameEngineVersion(gameEngineVersion);
#elif defined(DM_PLATFORM_HTML5)
            js_configureGameEngineVersion(gameEngineVersion);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureGameEngineVersion(gameEngineVersion);
#endif
        }

        void GameAnalytics::configureWritablePath(const char *writablePath)
        {
#if defined(DM_PLATFORM_IOS)
#elif defined(DM_PLATFORM_ANDROID)
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::configureWritablePath(writablePath);
#endif
        }

        void GameAnalytics::initialize(const char *gameKey, const char *gameSecret, bool use_imei_android)
        {

#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_ANDROID)
            jni_initialize(gameKey, gameSecret, use_imei_android);
#elif defined(DM_PLATFORM_HTML5)
            js_initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            auto listener = std::make_shared<DefoldOnRemoteConfigsListener>();
            gameanalytics::GameAnalytics::addRemoteConfigsListener(listener);
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

        void GameAnalytics::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addBusinessEvent(currency, amount, itemType, itemId, cartType, "", "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addBusinessEvent(currency, amount, itemType, itemId, cartType, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addBusinessEvent(currency, amount, itemType, itemId, cartType, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType);
#endif
        }

        void GameAnalytics::addResourceEvent(EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addResourceEvent((int)flowType, currency, amount, itemType, itemId, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addResourceEvent((int)flowType, currency, amount, itemType, itemId, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addResourceEvent((int)flowType, currency, amount, itemType, itemId, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addResourceEvent((gameanalytics::EGAResourceFlowType)((int)flowType), currency, amount, itemType, itemId);
#endif
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEvent((int)progressionStatus, progression01, progression02, progression03, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEvent((int)progressionStatus, progression01, progression02, progression03, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addProgressionEvent((int)progressionStatus, progression01, progression02, progression03, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03);
#endif
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03, score);
#endif
        }

        void GameAnalytics::addDesignEvent(const char *eventId, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEvent(eventId, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEvent(eventId, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addDesignEvent(eventId, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addDesignEvent(eventId);
#endif
        }

        void GameAnalytics::addDesignEvent(const char *eventId, float value, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEventWithValue(eventId, value, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEventWithValue(eventId, value, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addDesignEventWithValue(eventId, value, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addDesignEvent(eventId, value);
#endif
        }

        void GameAnalytics::addErrorEvent(EGAErrorSeverity severity, const char *message, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addErrorEvent((int)severity, message, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addErrorEvent((int)severity, message, "");
#elif defined(DM_PLATFORM_HTML5)
            js_addErrorEvent((int)severity, message, "");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::addErrorEvent((gameanalytics::EGAErrorSeverity)((int)severity), message);
#endif
        }

        void GameAnalytics::addAdEvent(EGAAdAction adAction, EGAAdType adType, const char *adSdkName, const char *adPlacement, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addAdEvent((int)adAction, (int)adType, adSdkName, adPlacement, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addAdEvent((int)adAction, (int)adType, adSdkName, adPlacement, "");
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
#endif
        }

        void GameAnalytics::addAdEventWithDuration(EGAAdAction adAction, EGAAdType adType, const char *adSdkName, const char *adPlacement, int duration, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addAdEventWithDuration((int)adAction, (int)adType, adSdkName, adPlacement, duration, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addAdEventWithDuration((int)adAction, (int)adType, adSdkName, adPlacement, duration, "");
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
#endif
        }

        void GameAnalytics::addAdEventWithNoAdReason(EGAAdAction adAction, EGAAdType adType, const char *adSdkName, const char *adPlacement, EGAAdError noAdReason, const char *fields)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addAdEventWithNoAdReason((int)adAction, (int)adType, adSdkName, adPlacement, (int)noAdReason, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addAdEventWithNoAdReason((int)adAction, (int)adType, adSdkName, adPlacement, (int)noAdReason, "");
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
#endif
        }

        void GameAnalytics::setEnabledInfoLog(bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledInfoLog(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledInfoLog(flag);
#elif defined(DM_PLATFORM_HTML5)
            js_setEnabledInfoLog(flag);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledInfoLog(flag);
#endif
        }

        void GameAnalytics::setEnabledVerboseLog(bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledVerboseLog(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledVerboseLog(flag);
#elif defined(DM_PLATFORM_HTML5)
            js_setEnabledVerboseLog(flag);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledVerboseLog(flag);
#endif
        }

        void GameAnalytics::setEnabledManualSessionHandling(bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledManualSessionHandling(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledManualSessionHandling(flag);
#elif defined(DM_PLATFORM_HTML5)
            js_setManualSessionHandling(flag);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledManualSessionHandling(flag);
#endif
        }

        void GameAnalytics::setEnabledEventSubmission(bool flag)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setEnabledEventSubmission(flag);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setEnabledEventSubmission(flag);
#elif defined(DM_PLATFORM_HTML5)
            js_setEventSubmission(flag);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setEnabledEventSubmission(flag);
#endif
        }

        void GameAnalytics::setEnabledErrorReporting(bool flag)
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

        void GameAnalytics::setCustomDimension01(const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            js_setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setCustomDimension01(customDimension);
#endif
        }

        void GameAnalytics::setCustomDimension02(const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension02(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension02(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            js_setCustomDimension02(customDimension);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setCustomDimension02(customDimension);
#endif
        }

        void GameAnalytics::setCustomDimension03(const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension03(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension03(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            js_setCustomDimension03(customDimension);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::setCustomDimension03(customDimension);
#endif
        }

        void GameAnalytics::startSession()
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::startSession();
#elif defined(DM_PLATFORM_ANDROID)
            jni_startSession();
#elif defined(DM_PLATFORM_HTML5)
            js_startSession();
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::startSession();
#endif
        }

        void GameAnalytics::endSession()
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::endSession();
#elif defined(DM_PLATFORM_ANDROID)
            jni_endSession();
#elif defined(DM_PLATFORM_HTML5)
            js_endSession();
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::endSession();
#endif
        }

        void GameAnalytics::onQuit()
        {
#if defined(DM_PLATFORM_IOS)
#elif defined(DM_PLATFORM_ANDROID)
#elif defined(DM_PLATFORM_HTML5)
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            gameanalytics::GameAnalytics::onQuit();
#endif
        }

        std::vector<char> GameAnalytics::getRemoteConfigsValueAsString(const char *key, const char *defaultValue)
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::getRemoteConfigsValueAsString(key, defaultValue);
#elif defined(DM_PLATFORM_ANDROID)
            return jni_getRemoteConfigsValueAsStringWithDefaultValue(key, defaultValue);
#elif defined(DM_PLATFORM_HTML5)
            std::vector<char> result;
            const char* returnValue = js_getRemoteConfigsValueAsStringWithDefaultValue(key, defaultValue);
            size_t s = strlen(returnValue);
            for(size_t i = 0; i < s; ++i)
            {
                result.push_back(returnValue[i]);
            }
            return result;
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            return gameanalytics::GameAnalytics::getRemoteConfigsValueAsString(key, defaultValue);
#endif
        }

        std::vector<char> GameAnalytics::getRemoteConfigsValueAsString(const char *key)
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::getRemoteConfigsValueAsString(key);
#elif defined(DM_PLATFORM_ANDROID)
            return jni_getRemoteConfigsValueAsString(key);
#elif defined(DM_PLATFORM_HTML5)
            std::vector<char> result;
            const char* returnValue = js_getRemoteConfigsValueAsString(key);
            size_t s = strlen(returnValue);
            for(size_t i = 0; i < s; ++i)
            {
                result.push_back(returnValue[i]);
            }
            return result;
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            return gameanalytics::GameAnalytics::getRemoteConfigsValueAsString(key);
#endif
        }

        bool GameAnalytics::isRemoteConfigsReady()
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::isRemoteConfigsReady();
#elif defined(DM_PLATFORM_ANDROID)
            return jni_isRemoteConfigsReady();
#elif defined(DM_PLATFORM_HTML5)
            return js_isRemoteConfigsReady();
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            return gameanalytics::GameAnalytics::isRemoteConfigsReady();
#endif
        }

        void GameAnalytics::setRemoteConfigsListener(dmScript::LuaCallbackInfo* listener)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setRemoteConfigsListener(listener);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setRemoteConfigsListener(listener);
#elif defined(DM_PLATFORM_HTML5)
            _remote_configs_listener = listener;
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)
            _remote_configs_listener = listener;
#endif
        }

        std::vector<char> GameAnalytics::getRemoteConfigsContentAsString()
        {
#if defined(DM_PLATFORM_IOS)
            return GameAnalyticsCpp::getRemoteConfigsContentAsString();
#elif defined(DM_PLATFORM_ANDROID)
            return jni_getRemoteConfigsContentAsString();
#elif defined(DM_PLATFORM_HTML5)
            std::vector<char> result;
            const char* returnValue = js_getRemoteConfigsContentAsString();
            size_t s = strlen(returnValue);
            for(size_t i = 0; i < s; ++i)
            {
                result.push_back(returnValue[i]);
            }
            return result;
#elif defined(DM_PLATFORM_LINUX) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            return gameanalytics::GameAnalytics::getRemoteConfigsContentAsString();
#endif
        }
    }
}

#if defined(DM_PLATFORM_HTML5)
#ifdef __cplusplus
extern "C"
{
#endif
    void EMSCRIPTEN_KEEPALIVE js_remote_configs_callback()
    {
        if(!gameanalytics::defold::_remote_configs_listener)
        {
            dmLogWarning("Received remote configs update but no listener was set!");
            return;
        }

        lua_State* L = dmScript::GetCallbackLuaContext(gameanalytics::defold::_remote_configs_listener);
        DM_LUA_STACK_CHECK(L, 0);

        if (!dmScript::SetupCallback(gameanalytics::defold::_remote_configs_listener))
        {
            dmLogWarning("SetupCallback failed for remote configs");
            return;
        }

        int ret = dmScript::PCall(L, 1, 0);
        dmScript::TeardownCallback(gameanalytics::defold::_remote_configs_listener);
    }
#ifdef __cplusplus
}
#endif
#endif
