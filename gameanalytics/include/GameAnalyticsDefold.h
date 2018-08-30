#pragma once

#include <dmsdk/script/script.h>
#include <vector>
#include <string>

namespace gameanalytics
{
    namespace defold
    {
        enum EGAResourceFlowType
        {
            Source = 1,
            Sink = 2
        };

        enum EGAProgressionStatus
        {
            Start = 1,
            Complete = 2,
            Fail = 3
        };

        enum EGAErrorSeverity
        {
            Debug = 1,
            Info = 2,
            Warning = 3,
            Error = 4,
            Critical = 5
        };

        enum EGAGender
        {
            Male = 1,
            Female = 2
        };

        class GameAnalytics {
        public:
            static void configureAvailableCustomDimensions01(lua_State *L, const std::string& list);
            static void configureAvailableCustomDimensions02(lua_State *L, const std::string& list);
            static void configureAvailableCustomDimensions03(lua_State *L, const std::string& list);

            static void configureAvailableResourceCurrencies(lua_State *L, const std::string& list);
            static void configureAvailableResourceItemTypes(lua_State *L, const std::string& list);

            static void configureBuild(lua_State *L, const char *build);
            static void configureUserId(lua_State *L, const char *userId);
            static void configureSdkGameEngineVersion(lua_State *L, const char *gameEngineSdkVersion);
            static void configureGameEngineVersion(lua_State *L, const char *gameEngineVersion);
            static void initialize(lua_State *L, const char *gameKey, const char *gameSecret);

#if defined(DM_PLATFORM_IOS)
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *fields);
            static void addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields);
#elif defined(DM_PLATFORM_ANDROID)
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *signature, const char *fields);
#endif
            static void addBusinessEvent(lua_State *L, const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields);
            static void addResourceEvent(lua_State *L, EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields);
            static void addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields);
            static void addProgressionEvent(lua_State *L, EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields);
            static void addDesignEvent(lua_State *L, const char *eventId, const char *fields);
            static void addDesignEvent(lua_State *L, const char *eventId, float value, const char *fields);
            static void addErrorEvent(lua_State *L, EGAErrorSeverity severity, const char *message, const char *fields);

            static void setEnabledInfoLog(lua_State *L, bool flag);
            static void setEnabledVerboseLog(lua_State *L, bool flag);
            static void setEnabledManualSessionHandling(lua_State *L, bool flag);
            static void setCustomDimension01(lua_State *L, const char *customDimension);
            static void setCustomDimension02(lua_State *L, const char *customDimension);
            static void setCustomDimension03(lua_State *L, const char *customDimension);
            static void setFacebookId(lua_State *L, const char *facebookId);
            static void setGender(lua_State *L, EGAGender gender);
            static void setBirthYear(lua_State *L, int birthYear);

            static void startSession(lua_State *L);
            static void endSession(lua_State *L);

            static const char* getCommandCenterValueAsString(lua_State *L, const char *key);
            static const char* getCommandCenterValueAsString(lua_State *L, const char *key, const char *defaultValue);
            static bool isCommandCenterReady(lua_State *L);
            static const char* getConfigurationsContentAsString(lua_State *L);

        private:
#if defined(DM_PLATFORM_HTML5)
            static void runHtml5Code(lua_State *L, const std::string& code);
            static const char* runHtml5CodeWithReturnString(lua_State *L, const std::string& code);
            static bool runHtml5CodeWithReturnBool(lua_State *L, const std::string& code);
#elif defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID)
            static std::vector<std::string> split(const std::string& str, char delimiter);
#endif
        };
    }
}
