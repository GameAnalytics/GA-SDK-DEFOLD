#pragma once
#if defined(DM_PLATFORM_ANDROID)

#include <vector>
#include <jni.h>
#include "CharArray.h"
#include <dmsdk/script/script.h>

namespace gameanalytics {
#ifdef __cplusplus
    extern "C"
    {
#endif
        typedef gameanalytics::defold::CharArray CharArray;
        extern void jni_configureAvailableCustomDimensions01(const std::vector<CharArray>& list);
        extern void jni_configureAvailableCustomDimensions02(const std::vector<CharArray>& list);
        extern void jni_configureAvailableCustomDimensions03(const std::vector<CharArray>& list);

        extern void jni_configureAvailableResourceCurrencies(const std::vector<CharArray>& list);
        extern void jni_configureAvailableResourceItemTypes(const std::vector<CharArray>& list);

        extern void jni_configureBuild(const char *build);
        extern void jni_configureSdkGameEngineVersion(const char *gameEngineSdkVersion);
        extern void jni_configureGameEngineVersion(const char *gameEngineVersion);
        extern void jni_configureAutoDetectAppVersion(bool flag);
        extern void jni_configureUserId(const char *userId);
        extern void jni_initialize(const char *gameKey, const char *gameSecret);

        extern void jni_addBusinessEventWithReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *store, const char *signature, const char *fields, bool mergeFields);
        extern void jni_addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields, bool mergeFields);
        extern void jni_addResourceEvent(int flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields, bool mergeFields);
        extern void jni_addProgressionEvent(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields, bool mergeFields);
        extern void jni_addProgressionEventWithScore(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields, bool mergeFields);
        extern void jni_addDesignEvent(const char *eventId, const char *fields, bool mergeFields);
        extern void jni_addDesignEventWithValue(const char *eventId, float value, const char *fields, bool mergeFields);
        extern void jni_addErrorEvent(int severity, const char *message, const char *fields, bool mergeFields);
        extern void jni_addAdEvent(int adAction, int adType, const char *adSdkName, const char *adPlacement, const char *fields, bool mergeFields);
        extern void jni_addAdEventWithDuration(int adAction, int adType, const char *adSdkName, const char *adPlacement, int duration, const char *fields, bool mergeFields);
        extern void jni_addAdEventWithNoAdReason(int adAction, int adType, const char *adSdkName, const char *adPlacement, int noAdReason, const char *fields, bool mergeFields);

        extern void jni_setEnabledInfoLog(bool flag);
        extern void jni_setEnabledVerboseLog(bool flag);
        extern void jni_setEnabledManualSessionHandling(bool flag);
        extern void jni_setEnabledEventSubmission(bool flag);
        extern void jni_setEnabledErrorReporting(bool flag);
        extern void jni_setCustomDimension01(const char *customDimension);
        extern void jni_setCustomDimension02(const char *customDimension);
        extern void jni_setCustomDimension03(const char *customDimension);

        extern void jni_setGlobalCustomEventFields(const char *customFields);

        extern void jni_startSession();
        extern void jni_endSession();

        extern std::vector<char> jni_getRemoteConfigsValueAsString(const char *key);
        extern std::vector<char> jni_getRemoteConfigsValueAsStringWithDefaultValue(const char *key, const char *defaultValue);
        extern bool jni_isRemoteConfigsReady();
        extern void jni_setRemoteConfigsListener(dmScript::LuaCallbackInfo* listener);
        extern JNIEXPORT void JNICALL Java_com_gameanalytics_sdk_GAJNI_onRemoteConfigsUpdatedNative(JNIEnv* env, jobject);
        extern std::vector<char> jni_getRemoteConfigsContentAsString();

        extern void ga_initialize();
        extern void ga_finalize();
#ifdef __cplusplus
    }
#endif
}

#endif
