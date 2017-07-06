#include "GameAnalyticsDefold.h"
#if defined(DM_PLATFORM_IOS)
#include "ios/GameAnalyticsCpp.h"
#elif defined(DM_PLATFORM_ANDROID)
#include "android/GameAnalyticsJNI.h"
#elif defined(DM_PLATFORM_HTML5)
#include "html5/GameAnalytics.h"
#include <iostream>
#include <sstream>
#elif defined(DM_PLATFORM_OSX)
#include "cpp/GameAnalytics.h"
#endif

namespace gameanalytics
{
    namespace defold
    {
        void GameAnalytics::configureAvailableCustomDimensions01(const std::vector<std::string>& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableCustomDimensions01(list);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableCustomDimensions01(list);
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            std::ostringstream ss;
            ss << "[";
            for (int i = 0; list.size(); ++i)
            {
                ss << "\"" << list[i] << "\"";
                if(i < list.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << "]";
            arrayString = ss.str();
            js_configureAvailableCustomDimensions01(arrayString.c_str());
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions01(list);
#endif
        }

        void GameAnalytics::configureAvailableCustomDimensions02(const std::vector<std::string>& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableCustomDimensions02(list);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableCustomDimensions02(list);
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            std::ostringstream ss;
            ss << "[";
            for (int i = 0; list.size(); ++i)
            {
                ss << "\"" << list[i] << "\"";
                if(i < list.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << "]";
            arrayString = ss.str();
            js_configureAvailableCustomDimensions02(arrayString.c_str());
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions02(list);
#endif
        }

        void GameAnalytics::configureAvailableCustomDimensions03(const std::vector<std::string>& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableCustomDimensions03(list);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableCustomDimensions03(list);
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            std::ostringstream ss;
            ss << "[";
            for (int i = 0; list.size(); ++i)
            {
                ss << "\"" << list[i] << "\"";
                if(i < list.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << "]";
            arrayString = ss.str();
            js_configureAvailableCustomDimensions03(arrayString.c_str());
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureAvailableCustomDimensions03(list);
#endif
        }

        void GameAnalytics::configureAvailableResourceCurrencies(const std::vector<std::string>& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableResourceCurrencies(list);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableResourceCurrencies(list);
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            std::ostringstream ss;
            ss << "[";
            for (int i = 0; list.size(); ++i)
            {
                ss << "\"" << list[i] << "\"";
                if(i < list.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << "]";
            arrayString = ss.str();
            js_configureAvailableResourceCurrencies(arrayString.c_str());
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureAvailableResourceCurrencies(list);
#endif
        }

        void GameAnalytics::configureAvailableResourceItemTypes(const std::vector<std::string>& list)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::configureAvailableResourceItemTypes(list);
#elif defined(DM_PLATFORM_ANDROID)
            jni_configureAvailableResourceItemTypes(list);
#elif defined(DM_PLATFORM_HTML5)
            std::string arrayString;
            std::ostringstream ss;
            ss << "[";
            for (int i = 0; list.size(); ++i)
            {
                ss << "\"" << list[i] << "\"";
                if(i < list.size() - 1)
                {
                    ss << ",";
                }
            }
            ss << "]";
            arrayString = ss.str();
            js_configureAvailableResourceItemTypes(arrayString.c_str());
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureAvailableResourceItemTypes(list);
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
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureBuild(build);
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::configureGameEngineVersion(gameEngineVersion);
#endif
        }

        void GameAnalytics::initialize(const char *gameKey, const char *gameSecret)
        {

#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_ANDROID)
            jni_initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_HTML5)
            js_initialize(gameKey, gameSecret);
#elif defined(DM_PLATFORM_OSX)
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

        void GameAnalytics::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addBusinessEvent(currency, amount, itemType, itemId, cartType, "");
#elif defined(DM_PLATFORM_ANDROID)
            jni_addBusinessEvent(currency, amount, itemType, itemId, cartType);
#elif defined(DM_PLATFORM_HTML5)
            js_addBusinessEvent(currency, amount, itemType, itemId, cartType);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType);
#endif
        }

        void GameAnalytics::addResourceEvent(EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addResourceEvent((int)flowType, currency, amount, itemType, itemId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addResourceEvent((int)flowType, currency, amount, itemType, itemId);
#elif defined(DM_PLATFORM_HTML5)
            js_addResourceEvent((int)flowType, currency, amount, itemType, itemId);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addResourceEvent((gameanalytics::EGAResourceFlowType)((int)flowType), currency, amount, itemType, itemId);
#endif
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01)
        {
            addProgressionEvent(progressionStatus, progression01, "", "");
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, int score)
        {
            addProgressionEvent(progressionStatus, progression01, "", "", score);
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02)
        {
            addProgressionEvent(progressionStatus, progression01, progression02, "");
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, int score)
        {
            addProgressionEvent(progressionStatus, progression01, progression02, "", score);
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEvent((int)progressionStatus, progression01, progression02, progression03);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEvent((int)progressionStatus, progression01, progression02, progression03);
#elif defined(DM_PLATFORM_HTML5)
            js_addProgressionEvent((int)progressionStatus, progression01, progression02, progression03);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03);
#endif
        }

        void GameAnalytics::addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score);
#elif defined(DM_PLATFORM_HTML5)
            js_addProgressionEventWithScore((int)progressionStatus, progression01, progression02, progression03, score);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addProgressionEvent((gameanalytics::EGAProgressionStatus)((int)progressionStatus), progression01, progression02, progression03, score);
#endif
        }

        void GameAnalytics::addDesignEvent(const char *eventId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEvent(eventId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEvent(eventId);
#elif defined(DM_PLATFORM_HTML5)
            js_addDesignEvent(eventId);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addDesignEvent(eventId);
#endif
        }

        void GameAnalytics::addDesignEvent(const char *eventId, float value)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addDesignEventWithValue(eventId, value);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addDesignEventWithValue(eventId, value);
#elif defined(DM_PLATFORM_HTML5)
            js_addDesignEventWithValue(eventId, value);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addDesignEvent(eventId, value);
#endif
        }

        void GameAnalytics::addErrorEvent(EGAErrorSeverity severity, const char *message)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::addErrorEvent((int)severity, message);
#elif defined(DM_PLATFORM_ANDROID)
            jni_addErrorEvent((int)severity, message);
#elif defined(DM_PLATFORM_HTML5)
            js_addErrorEvent((int)severity, message);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::addErrorEvent((gameanalytics::EGAErrorSeverity)((int)severity), message);
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::setEnabledManualSessionHandling(flag);
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::setCustomDimension03(customDimension);
#endif
        }

        void GameAnalytics::setFacebookId(const char *facebookId)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setFacebookId(facebookId);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setFacebookId(facebookId);
#elif defined(DM_PLATFORM_HTML5)
            js_setFacebookId(facebookId);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::setFacebookId(facebookId);
#endif
        }

        void GameAnalytics::setGender(EGAGender gender)
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
                    js_setGender((int)gender);
#elif defined(DM_PLATFORM_OSX)
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
                    js_setGender((int)gender);
#elif defined(DM_PLATFORM_OSX)
                    gameanalytics::GameAnalytics::setGender((gameanalytics::EGAGender)((int)gender));
#endif
                }
                break;
            }
        }

        void GameAnalytics::setBirthYear(int birthYear)
        {
#if defined(DM_PLATFORM_IOS)
            GameAnalyticsCpp::setBirthYear(birthYear);
#elif defined(DM_PLATFORM_ANDROID)
            jni_setBirthYear(birthYear);
#elif defined(DM_PLATFORM_HTML5)
            js_setBirthYear(birthYear);
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::setBirthYear(birthYear);
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
#elif defined(DM_PLATFORM_OSX)
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
#elif defined(DM_PLATFORM_OSX)
            gameanalytics::GameAnalytics::endSession();
#endif
        }
    }
}
