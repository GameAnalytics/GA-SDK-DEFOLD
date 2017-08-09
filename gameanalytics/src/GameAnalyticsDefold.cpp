#include "GameAnalyticsDefold.h"
#if defined(DM_PLATFORM_IOS)
#include "ios/GameAnalyticsCpp.h"
#elif defined(DM_PLATFORM_ANDROID)
#include "android/GameAnalyticsJNI.h"
#elif defined(DM_PLATFORM_HTML5)
#include "html5/GameAnalytics.h"
#include <regex>
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
#include "cpp/GameAnalytics.h"
#endif

#include <iostream>
#include <sstream>

namespace gameanalytics
{
    namespace defold
    {
#if defined(DM_PLATFORM_HTML5)
        static void runHtml5Code(lua_State *L, const std::string code)
        {
            lua_getglobal(L, "html5");                              // push 'html5' onto stack
            lua_getfield(L, -1, "run");                             // push desired function
            lua_pushstring(L, code.c_str());                        // push argument
            lua_call(L, 1, 0);                                      // call function with 1 arg, 0 return value
            lua_pop(L, 1);                                          // pop 'html5'
        }
#else
        static std::vector<std::string> split(std::string str, char delimiter)
        {
            std::vector<std::string> internal;
            std::stringstream ss(str); // Turn the string into a stream.
            std::string tok;

            int i = 0;
            while(std::getline(ss, tok, delimiter))
            {
                internal.push_back(tok);
                ++i;
            }

            return internal;
        }
#endif

        void GameAnalytics::configureAvailableCustomDimensions01(lua_State *L, const std::string& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableCustomDimensions01(split(list, ','));
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableCustomDimensions01(split(list, ','));
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            if(list.length() > 0)
            {
                std::string replaceString = std::regex_replace(list, std::regex(","), "\",\"");
                arrayString = "[\"" + replaceString + "\"]";
            }
            else
            {
                arrayString = "[]";
            }
            runHtml5Code(L, "gameanalytics.GameAnalytics.configureAvailableCustomDimensions01(JSON.parse('" + arrayString + "'))");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions01(split(list, ','));
#endif
        }

        void GameAnalytics::configureAvailableCustomDimensions02(lua_State *L, const std::string& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableCustomDimensions02(split(list, ','));
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableCustomDimensions02(split(list, ','));
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            if(list.length() > 0)
            {
                std::string replaceString = std::regex_replace(list, std::regex(","), "\",\"");
                arrayString = "[\"" + replaceString + "\"]";
            }
            else
            {
                arrayString = "[]";
            }
            runHtml5Code(L, "gameanalytics.GameAnalytics.configureAvailableCustomDimensions02(JSON.parse('" + arrayString + "'))");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions02(split(list, ','));
#endif
        }

        void GameAnalytics::configureAvailableCustomDimensions03(lua_State *L, const std::string& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableCustomDimensions03(split(list, ','));
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableCustomDimensions03(split(list, ','));
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            if(list.length() > 0)
            {
                std::string replaceString = std::regex_replace(list, std::regex(","), "\",\"");
                arrayString = "[\"" + replaceString + "\"]";
            }
            else
            {
                arrayString = "[]";
            }
            runHtml5Code(L, "gameanalytics.GameAnalytics.configureAvailableCustomDimensions03(JSON.parse('" + arrayString + "'))");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions03(split(list, ','));
#endif
        }

        void GameAnalytics::configureAvailableResourceCurrencies(lua_State *L, const std::string& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableResourceCurrencies(split(list, ','));
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableResourceCurrencies(split(list, ','));
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            if(list.length() > 0)
            {
                std::string replaceString = std::regex_replace(list, std::regex(","), "\",\"");
                arrayString = "[\"" + replaceString + "\"]";
            }
            else
            {
                arrayString = "[]";
            }
            runHtml5Code(L, "gameanalytics.GameAnalytics.configureAvailableResourceCurrencies(JSON.parse('" + arrayString + "'))");
            // js_configureAvailableResourceCurrencies(arrayString.c_str());
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureAvailableResourceCurrencies(split(list, ','));
#endif
        }

        void GameAnalytics::configureAvailableResourceItemTypes(lua_State *L, const std::string& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableResourceItemTypes(split(list, ','));
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableResourceItemTypes(split(list, ','));
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            if(list.length() > 0)
            {
                std::string replaceString = std::regex_replace(list, std::regex(","), "\",\"");
                arrayString = "[\"" + replaceString + "\"]";
            }
            else
            {
                arrayString = "[]";
            }
            runHtml5Code(L, "gameanalytics.GameAnalytics.configureAvailableResourceItemTypes(JSON.parse('" + arrayString + "'))");
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureAvailableResourceItemTypes(split(list, ','));
#endif
        }

        void GameAnalytics::configureBuild(lua_State *L, const char *build)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureBuild(build);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureBuild(build);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.configureBuild('" << build << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureBuild(build);
#endif
        }

        void GameAnalytics::configureUserId(lua_State *L, const char *userId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureUserId(userId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureUserId(userId);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.configureUserId('" << userId << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.configureSdkGameEngineVersion('" << gameEngineSdkVersion << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.configureGameEngineVersion('" << gameEngineVersion << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::configureGameEngineVersion(gameEngineVersion);
#endif
        }

        void GameAnalytics::initialize(lua_State *L, const char *gameKey, const char *gameSecret)
        {

#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_ANDROID)
            jni_initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.initialize('" << gameKey << "', '" << gameSecret << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::initialize(gameKey, gameSecret);
#endif
        }

#if defined(DM_PLATFORM_IOS)
        void GameAnalytics::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt)
        {
            GameAnalyticsCpp::addBusinessEvent(currency, amount, itemType, itemId, cartType, receipt);
        }

        void GameAnalytics::addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType)
        {
            GameAnalyticsCpp::addBusinessEventAndAutoFetchReceipt(currency, amount, itemType, itemId, cartType);
        }
#elif defined(DM_PLATFORM_ANDROID)
        void GameAnalytics::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *signature)
        {
            jni_addBusinessEventWithReceipt(currency, amount, itemType, itemId, cartType, receipt, "google_play", signature);
        }
#endif

        void GameAnalytics::addBusinessEvent(lua_State *L, const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addBusinessEvent(currency, amount, itemType, itemId, cartType, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addBusinessEvent(currency, amount, itemType, itemId, cartType);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addBusinessEvent('" << currency << "', " << amount << ", '" << itemType << "', '" << itemId << "', '" << cartType << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType);
#endif
        }

        void GameAnalytics::addResourceEvent(lua_State *L, EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addResourceEvent((int)flowType, currency, amount, itemType, itemId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addResourceEvent((int)flowType, currency, amount, itemType, itemId);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addResourceEvent(" << (int)flowType << ", '" << currency << "', " << amount << ", '" << itemType << "', '" << itemId << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::addResourceEvent((gameanalytics::EGAResourceFlowType)((int)flowType), currency, amount, itemType, itemId);
#endif
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01)
        {
            addProgressionEvent(L, progressionStatus, progression01, "", "");
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, int score)
        {
            addProgressionEvent(L, progressionStatus, progression01, "", "", score);
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02)
        {
            addProgressionEvent(L, progressionStatus, progression01, progression02, "");
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, int score)
        {
            addProgressionEvent(L, progressionStatus, progression01, progression02, "", score);
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEvent((int)progressionStatus, progression01, progression02, progression03);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEvent((int)progressionStatus, progression01, progression02, progression03);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addProgressionEvent(" << (int)progressionStatus << ", '" << progression01 << "', '" << progression02 << "', '" << progression03 << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03);
#endif
        }

        void GameAnalytics::addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addProgressionEvent(" << (int)progressionStatus << ", '" << progression01 << "', '" << progression02 << "', '" << progression03 << "', " << score << ")";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03, score);
#endif
        }

        void GameAnalytics::addDesignEvent(lua_State *L, const char *eventId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEvent(eventId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEvent(eventId);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addDesignEvent('" << eventId << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::addDesignEvent(eventId);
#endif
        }

        void GameAnalytics::addDesignEvent(lua_State *L, const char *eventId, float value)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEventWithValue(eventId, value);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEventWithValue(eventId, value);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addDesignEvent('" << eventId << "', " << value << ")";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::addDesignEvent(eventId, value);
#endif
        }

        void GameAnalytics::addErrorEvent(lua_State *L, EGAErrorSeverity severity, const char *message)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addErrorEvent((int)severity, message);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addErrorEvent((int)severity, message);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.addErrorEvent(" << (int)severity << ", '" << message << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setEnabledInfoLog(" << (flag ? "true" : "false") << ")";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setEnabledVerboseLog(" << (flag ? "true" : "false") << ")";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setEnabledManualSessionHandling(" << (flag ? "true" : "false") << ")";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::setEnabledManualSessionHandling(flag);
#endif
        }

        void GameAnalytics::setCustomDimension01(lua_State *L, const char *customDimension)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setCustomDimension01(customDimension);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setCustomDimension01('" << customDimension << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setCustomDimension02('" << customDimension << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setCustomDimension03('" << customDimension << "')";
            std::string code = ss.str();
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::setCustomDimension03(customDimension);
#endif
        }

        void GameAnalytics::setFacebookId(lua_State *L, const char *facebookId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setFacebookId(facebookId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setFacebookId(facebookId);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setFacebookId('" << facebookId << "')";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::setFacebookId(facebookId);
#endif
        }

        void GameAnalytics::setGender(lua_State *L, EGAGender gender)
        {
            switch(gender)
            {
                case Male:
                {
#if defined(DM_PLATFORM_IOS)
                    GameAnalyticsCpp::setGender("male");
#elif defined(DM_PLATFORM_ANDROID)
                    jni_setGender((int)gender);
#elif defined(DM_PLATFORM_HTML5)
                    std::ostringstream ss;
                    ss << "gameanalytics.GameAnalytics.setGender(" << (int)gender << ")";
                    std::string code = ss.str();
                    runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
                    gameanalytics::GameAnalytics::setGender((gameanalytics::EGAGender)((int)gender));
#endif
                }
                break;

                case Female:
                {
#if defined(DM_PLATFORM_IOS)
                    GameAnalyticsCpp::setGender("female");
#elif defined(DM_PLATFORM_ANDROID)
                    jni_setGender((int)gender);
#elif defined(DM_PLATFORM_HTML5)
                    std::ostringstream ss;
                    ss << "gameanalytics.GameAnalytics.setGender(" << (int)gender << ")";
                    std::string code = ss.str();
                    runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
                    gameanalytics::GameAnalytics::setGender((gameanalytics::EGAGender)((int)gender));
#endif
                }
                break;
            }
        }

        void GameAnalytics::setBirthYear(lua_State *L, int birthYear)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setBirthYear(birthYear);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setBirthYear(birthYear);
#elif defined(DM_PLATFORM_HTML5)
            std::ostringstream ss;
            ss << "gameanalytics.GameAnalytics.setBirthYear(" << birthYear << ")";
            std::string code = ss.str();
            runHtml5Code(L, code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::setBirthYear(birthYear);
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
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
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
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS)
            gameanalytics::GameAnalytics::endSession();
#endif
        }
    }
}
