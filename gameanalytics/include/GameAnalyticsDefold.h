#pragma once

#include <dmsdk/script/script.h>
#include <vector>
#include "CharArray.h"

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

        class GameAnalytics {
        public:
            static void configureAvailableCustomDimensions01(const char* list);
            static void configureAvailableCustomDimensions02(const char* list);
            static void configureAvailableCustomDimensions03(const char* list);

            static void configureAvailableResourceCurrencies(const char* list);
            static void configureAvailableResourceItemTypes(const char* list);

            static void configureBuild(const char *build);
            static void configureAutoDetectAppVersion(bool flag);
            static void configureUserId(const char *userId);
            static void configureSdkGameEngineVersion(const char *gameEngineSdkVersion);
            static void configureGameEngineVersion(const char *gameEngineVersion);
            static void configureWritablePath(const char *writablePath);
            static void initialize(const char *gameKey, const char *gameSecret, bool use_imei_android);

#if defined(DM_PLATFORM_IOS)
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *fields);
            static void addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields);
#elif defined(DM_PLATFORM_ANDROID)
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *signature, const char *fields);
#endif
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields);
            static void addResourceEvent(EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields);
            static void addDesignEvent(const char *eventId, const char *fields);
            static void addDesignEvent(const char *eventId, float value, const char *fields);
            static void addErrorEvent(EGAErrorSeverity severity, const char *message, const char *fields);

            static void setEnabledInfoLog(bool flag);
            static void setEnabledVerboseLog(bool flag);
            static void setEnabledManualSessionHandling(bool flag);
            static void setEnabledEventSubmission(bool flag);
            static void setEnabledErrorReporting(bool flag);
            static void setCustomDimension01(const char *customDimension);
            static void setCustomDimension02(const char *customDimension);
            static void setCustomDimension03(const char *customDimension);

            static void startSession();
            static void endSession();
            static void onQuit();

            static std::vector<char> getRemoteConfigsValueAsString(const char *key);
            static std::vector<char> getRemoteConfigsValueAsString(const char *key, const char *defaultValue);
            static bool isRemoteConfigsReady();
            static void setRemoteConfigsListener(dmScript::LuaCallbackInfo* listener);
            static std::vector<char> getRemoteConfigsContentAsString();

        private:
#if defined(DM_PLATFORM_HTML5)
            static void runHtml5Code(const char* code);
            static std::vector<char> runHtml5CodeWithReturnString(const char* code);
            static bool runHtml5CodeWithReturnBool(const char* code);
#endif
            static std::vector<CharArray> split(char* str, const char* delimiter);
        };
    }
}
