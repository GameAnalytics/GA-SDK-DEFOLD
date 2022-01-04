#pragma once

#import <vector>
#include "CharArray.h"
#include <dmsdk/script/script.h>

class GameAnalyticsCpp {
public:
    typedef gameanalytics::defold::CharArray CharArray;
    static void configureAvailableCustomDimensions01(const std::vector<CharArray>& list);
    static void configureAvailableCustomDimensions02(const std::vector<CharArray>& list);
    static void configureAvailableCustomDimensions03(const std::vector<CharArray>& list);

    static void configureAvailableResourceCurrencies(const std::vector<CharArray>& list);
    static void configureAvailableResourceItemTypes(const std::vector<CharArray>& list);

    static void configureBuild(const char *build);
    static void configureAutoDetectAppVersion(bool flag);
    static void configureUserId(const char *userId);
    static void configureSdkGameEngineVersion(const char *gameEngineSdkVersion);
    static void configureGameEngineVersion(const char *gameEngineVersion);
    static void initialize(const char *gameKey, const char *gameSecret);

    static void addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *fields, bool mergeFields);
    static void addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields, bool mergeFields);
    static void addResourceEvent(int flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields, bool mergeFields);
    static void addProgressionEvent(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields, bool mergeFields);
    static void addProgressionEventWithScore(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields, bool mergeFields);
    static void addDesignEvent(const char *eventId, const char *fields, bool mergeFields);
    static void addDesignEventWithValue(const char *eventId, float value, const char *fields, bool mergeFields);
    static void addErrorEvent(int severity, const char *message, const char *fields, bool mergeFields);
    static void addAdEvent(int adAction, int adType, const char *adSdkName, const char *adPlacement, const char *fields, bool mergeFields);
    static void addAdEventWithDuration(int adAction, int adType, const char *adSdkName, const char *adPlacement, int duration, const char *fields, bool mergeFields);
    static void addAdEventWithNoAdReason(int adAction, int adType, const char *adSdkName, const char *adPlacement, int noAdReason, const char *fields, bool mergeFields);

    static void setEnabledInfoLog(bool flag);
    static void setEnabledVerboseLog(bool flag);
    static void setEnabledManualSessionHandling(bool flag);
    static void setEnabledEventSubmission(bool flag);
    static void setEnabledErrorReporting(bool flag);
    static void setCustomDimension01(const char *customDimension);
    static void setCustomDimension02(const char *customDimension);
    static void setCustomDimension03(const char *customDimension);

    static void setGlobalCustomEventFields(const char *customFields);

    static void startSession();
    static void endSession();

    static std::vector<char> getRemoteConfigsValueAsString(const char *key);
    static std::vector<char> getRemoteConfigsValueAsString(const char *key, const char *defaultValue);
    static bool isRemoteConfigsReady();
    static void setRemoteConfigsListener(dmScript::LuaCallbackInfo* listener);
    static std::vector<char> getRemoteConfigsContentAsString();
};
