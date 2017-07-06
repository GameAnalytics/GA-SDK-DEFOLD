#pragma once

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
            static void configureAvailableCustomDimensions01(const std::vector<std::string>& list);
            static void configureAvailableCustomDimensions02(const std::vector<std::string>& list);
            static void configureAvailableCustomDimensions03(const std::vector<std::string>& list);

            static void configureAvailableResourceCurrencies(const std::vector<std::string>& list);
            static void configureAvailableResourceItemTypes(const std::vector<std::string>& list);

            static void configureBuild(const char *build);
            static void configureUserId(const char *userId);
            static void configureSdkGameEngineVersion(const char *gameEngineSdkVersion);
            static void configureGameEngineVersion(const char *gameEngineVersion);
            static void initialize(const char *gameKey, const char *gameSecret);

    #if defined(DM_PLATFORM_IOS)
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt);
            static void addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType);
    #elif defined(DM_PLATFORM_ANDROID)
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *signature);
    #endif
            static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType);
            static void addResourceEvent(EGAResourceFlowType flowType, const char *currency, float amount, const char *itemType, const char *itemId);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, int score);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, int score);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03);
            static void addProgressionEvent(EGAProgressionStatus progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score);
            static void addDesignEvent(const char *eventId);
            static void addDesignEvent(const char *eventId, float value);
            static void addErrorEvent(EGAErrorSeverity severity, const char *message);

            static void setEnabledInfoLog(bool flag);
            static void setEnabledVerboseLog(bool flag);
            static void setEnabledManualSessionHandling(bool flag);
            static void setCustomDimension01(const char *customDimension);
            static void setCustomDimension02(const char *customDimension);
            static void setCustomDimension03(const char *customDimension);
            static void setFacebookId(const char *facebookId);
            static void setGender(EGAGender gender);
            static void setBirthYear(int birthYear);

            static void startSession();
            static void endSession();
        };
    }
}
