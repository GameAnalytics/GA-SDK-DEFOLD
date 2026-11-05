// Vendored copy of code/android/GAWrapperAndroid.h from the shared GameAnalytics C++ wrapper.
// Keep it in sync with the source, do not change it here.

#pragma once

#include <jni.h>

#include "GAWrapper.h"

namespace gameanalytics
{
    // =====================================================================
    // ENGINE HOOK: JNI context
    //
    // Every call below needs a JNIEnv, the application's ClassLoader (to find
    // the GameAnalytics java classes from any thread) and the game Activity.
    // Game engines already own all three and each exposes them differently:
    //
    //   Unreal    FAndroidApplication::GetJavaEnv() / FindJavaClass() / GetGameActivityThis()
    //   Unity     AndroidJavaClass("com.unity3d.player.UnityPlayer") -> "currentActivity"
    //   Godot     the godot-cpp JNI singleton
    //   GameMaker the runner's JNIEnv and activity
    //
    // Pick ONE of the two ways to plug the engine in:
    //
    //   1. Injection (preferred, no subclassing). Call SetJavaVM() and
    //      SetGameActivity() once during engine start-up. If the native
    //      library is loaded by the java side, JNI_OnLoad already captures
    //      the JavaVM and the ClassLoader, so only SetGameActivity() is
    //      strictly needed.
    //
    //   2. Subclass GAWrapperAndroid and override GetJavaEnv(),
    //      FindJavaClass() and GetGameActivity() to forward to the engine.
    //
    // The defaults implemented in GAWrapperAndroid.cpp work on their own on
    // plain Android (no engine), so an SDK that does nothing still links and
    // runs; it just will not see the engine's Activity until one is provided.
    // =====================================================================
    namespace android
    {
        // Hand the JavaVM over when the native library is NOT loaded through
        // java (JNI_OnLoad is not called). Also caches the ClassLoader.
        void SetJavaVM(JavaVM* vm);

        // Hand over the current game Activity. A global reference is taken,
        // so the caller keeps ownership of the reference it passes in.
        void SetGameActivity(JNIEnv* env, jobject activity);

        // The ClassLoader used by FindJavaClass. Set it explicitly when the
        // engine loads the GameAnalytics classes through its own loader.
        void SetClassLoader(JNIEnv* env, jobject classLoader);
    }

    class GAWrapperAndroid:
        public GAWrapper
    {
        std::string CallStringMethod(std::string const& name);
        int64_t CallLongMethod(const char* methodName);
        std::string ConvertJString(jstring str);

        protected:

        // ---- ENGINE HOOKS, override these to use the engine's own JNI ----

        // The JNIEnv for the calling thread. The default attaches the thread
        // to the JavaVM on first use and detaches it when the thread exits.
        virtual JNIEnv* GetJavaEnv();

        // Resolves a java class by its JNI name ("com/gameanalytics/sdk/...").
        // The default goes through the cached ClassLoader so it also works on
        // threads that java did not create; it falls back to FindClass.
        // Returns a local reference, the caller deletes it.
        virtual jclass FindJavaClass(const char* className);

        // The current game Activity, needed by Initialize. The default
        // returns whatever was passed to android::SetGameActivity(), or
        // nullptr when nothing was provided.
        virtual jobject GetGameActivity();

        // ------------------------------------------------------------------

        virtual jclass GetGameAnalyticsClass();

        public:

        GAWrapperAndroid();
        virtual ~GAWrapperAndroid();

        virtual void SetAvailableCustomDimensions01(std::vector<std::string> const& list) override;
        virtual void SetAvailableCustomDimensions02(std::vector<std::string> const& list) override;
        virtual void SetAvailableCustomDimensions03(std::vector<std::string> const& list) override;

        virtual void SetAvailableResourceCurrencies(std::vector<std::string> const& list) override;
        virtual void SetAvailableResourceItemTypes(std::vector<std::string> const& list) override;

        virtual void SetSDKVersion(std::string const& version) override;
        virtual void SetGameEngineVersion(std::string const& version) override;

        virtual void SetBuild(std::string const& build)   override;
        virtual void SetCustomUserId(std::string const& userId) override;
        virtual void SetExternalUserId(std::string const& extUserId) override;

        virtual std::string GetUserId() override;
        virtual std::string GetExternalUserId() override;

        virtual void SetAutoDetectAppVersion(bool flag) override;

        virtual void Initialize(std::string const& gamekey, std::string const& gamesecret) override;

        virtual void SetCustomDimension01(std::string const& customDimension) override;
        virtual void SetCustomDimension02(std::string const& customDimension) override;
        virtual void SetCustomDimension03(std::string const& customDimension) override;

        virtual void SetGlobalCustomEventFields(std::string const& customFields) override;
        virtual void SetEnabledManualSessionHandling (bool enabled) override;
        virtual void SetEnabledEventSubmission(bool enabled, bool doCacheLocally = false) override;

        virtual void StartSession() override;
        virtual void EndSession() override;
        
        virtual void AddBusinessEvent(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const&cartType, std::string const& receipt, std::string const& fields, bool mergeFields) override;
        virtual void AddBusinessEventAndAutoFetchReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& fields, bool mergeFields) override;
        virtual void AddBusinessEventWithReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& receipt, std::string const& store, std::string const& signature, std::string const& fields, bool mergeFields) override;
        virtual void AddBusinessEventWithValidation(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& store, std::string const& productId, std::string const& purchaseToken, std::string const& fields, bool mergeFields) override;


        virtual void AddResourceEvent(::EGAResourceFlowType flowType, std::string const& currency, float amount, std::string const& itemType, std::string const& itemId, std::string const&  fields, bool mergeFields) override;

        virtual void AddProgressionEvent(::EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, std::string const&  fields, bool mergeFields) override;
        virtual void AddProgressionEventWithScore(::EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, int score, std::string const& fields, bool mergeFields) override;
        
        virtual void AddDesignEvent(std::string const& eventID, std::string const&  fields, bool mergeFields) override;
        virtual void AddDesignEventWithValue(std::string const& eventId, float value, std::string const& fields, bool mergeFields) override;

        virtual void AddErrorEvent(EGAErrorSeverity severity, std::string const& message, std::string const&  fields, bool mergeFields) override;

        virtual void AddAdEventWithDuration(::EGAAdAction adAction, ::EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, int64_t duration, std::string const& fields, bool mergeFields) override;
        virtual void AddAdEventWithReason(::EGAAdAction adAction, ::EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, ::EGAAdError noAdReason, std::string const& fields, bool mergeFields) override;
        virtual void AddAdEvent(::EGAAdAction adAction, ::EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, std::string const& fields, bool mergeFields) override;

        virtual void SetInfoLog(bool enabled) override;

        virtual void SetVerboseLog(bool enabled) override;

        virtual void SetEnabledErrorReporting(bool flag) override;

        // ----------------------- REMOTE CONFIGS ---------------------- //
        virtual std::string GetRemoteConfigsValueAsString(std::string const& key, std::string const& defaultValue) override;

        virtual double GetRemoteConfigsValueAsNumber(std::string const& key, double defaultValue) override;

        virtual std::string GetRemoteConfigsValueAsJson(std::string const& key) override;

        virtual void RegisterRemoteConfigListener(RemoteConfigListener listener) override;

        virtual bool IsRemoteConfigsReady() override;

        virtual std::string GetRemoteConfigsContentAsString() override;

        virtual std::string GetABTestingId() override;

        virtual std::string GetABTestingVariantId() override;

        ///// HEALTH

        virtual void EnableSDKInitEvent(bool flag) override;

        virtual void EnableFpsHistogram(FPSTracker tracker, bool flag) override;

        virtual void EnableMemoryHistogram(bool flag) override;

        virtual void EnableHealthHardwareInfo(bool flag) override;

        ////////////////////////////////////////////////////////

        virtual int64_t GetElapsedSessionTime() override;

        virtual int64_t GetElapsedTimeForPreviousSession() override;
        
        virtual int64_t GetElapsedTimeFromAllSessions() override;

        ////////////////////////////////////////////////////////

        virtual void EnableAdvertisingId(bool value) override;

        virtual void OnQuit() override;

        virtual void SetWritablePath(std::string const& path) override;
    };
}
