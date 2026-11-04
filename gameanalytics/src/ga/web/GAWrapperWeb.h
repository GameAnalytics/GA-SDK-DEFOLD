// Vendored copy of code/web/GAWrapperWeb.h from the shared GameAnalytics C++ wrapper.
// Keep it in sync with the source, do not change it here.

#pragma once

#include <functional>

#include "GAWrapper.h"

namespace gameanalytics
{
    // =====================================================================
    // ENGINE HOOK: javascript evaluation
    //
    // On the web the native code does not talk to GameAnalytics directly, it
    // drives the javascript SDK (GameAnalytics.js) that the page loaded. Every
    // call below is therefore a small snippet of javascript that has to be run
    // in the page, and each engine exposes a different way of doing that:
    //
    //   Godot       Engine::get_singleton()->get_singleton("JavaScriptBridge")
    //                   -> call("eval", code, true)
    //   Unity       Application.ExternalEval / a .jslib plugin
    //   Emscripten  emscripten_run_script_string (used by default, see below)
    //   GameMaker   the HTML5 runner's extension functions
    //
    // Pick ONE of the two ways to plug the engine in:
    //
    //   1. Injection (preferred, no subclassing). Call SetJavaScriptEvaluator()
    //      once during start-up with a callable that runs a snippet and gives
    //      back whatever it evaluated to, as a string.
    //
    //   2. Subclass GAWrapperWeb and override Eval().
    //
    // When the SDK is compiled with Emscripten and nothing was injected, the
    // default uses emscripten_run_script_string, so an Emscripten build works
    // on its own. On any other toolchain the default logs once and does
    // nothing, which keeps a half-finished port quiet instead of crashing.
    // =====================================================================

    // Runs a snippet of javascript in the page and returns its result as a
    // string ("" when the snippet produced nothing).
    using JavaScriptEvaluator = std::function<std::string(std::string const& code)>;

    namespace web
    {
        void SetJavaScriptEvaluator(JavaScriptEvaluator evaluator);
    }

    class GAWrapperWeb:
        public GAWrapper
    {
        RemoteConfigListener _remoteConfigListener;

        protected:

        // ---- ENGINE HOOK, override this to use the engine's own bridge ----
        virtual std::string Eval(std::string const& code);

        public:

            GAWrapperWeb();

            // ENGINE HOOK: the javascript SDK reports remote config updates to
            // the page, not to native code, so the engine's javascript bridge
            // has to call this when that happens. RegisterRemoteConfigListener
            // installs the javascript side that is meant to trigger it; wiring
            // that call back into native is the engine-specific half.
            void NotifyRemoteConfigsUpdated();

            virtual void SetAvailableCustomDimensions01(std::vector<std::string> const& list) override;
            virtual void SetAvailableCustomDimensions02(std::vector<std::string> const& list) override;
            virtual void SetAvailableCustomDimensions03(std::vector<std::string> const& list) override;

            virtual void SetAvailableResourceCurrencies(std::vector<std::string> const& list) override;
            virtual void SetAvailableResourceItemTypes(std::vector<std::string> const& list) override;

            virtual void SetSDKVersion(std::string const& version) override;

            virtual void SetGameEngineVersion(std::string const& version) override;

            virtual void SetBuild(std::string const& build) override;
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
            virtual void SetEnabledManualSessionHandling(bool enabled) override;
            virtual void SetEnabledEventSubmission(bool enable, bool offlineCaching = true) override;
            virtual void SetEnabledErrorReporting(bool flag) override;

            virtual void StartSession() override;
            virtual void EndSession() override;

            virtual void AddBusinessEvent(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const&cartType, std::string const& receipt, std::string const&  fields, bool mergeFields) override;
            virtual void AddBusinessEventAndAutoFetchReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const&  fields, bool mergeFields) override;
            virtual void AddBusinessEventWithReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& receipt, std::string const& store, std::string const& signature, std::string const&  fields, bool mergeFields) override;
            virtual void AddBusinessEventWithValidation(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& store, std::string const& productId, std::string const& purchaseToken, std::string const& fields, bool mergeFields) override;

            virtual void AddResourceEvent(EGAResourceFlowType flowType, std::string const& currency, float amount, std::string const& itemType, std::string const& itemId, std::string const&  fields, bool mergeFields) override;

            virtual void AddProgressionEvent(EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, std::string const&  fields, bool mergeFields) override;
            virtual void AddProgressionEventWithScore(EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, int score, std::string const& fields, bool mergeFields) override;

            virtual void AddDesignEvent(std::string const& eventID, std::string const& fields, bool mergeFields) override;
            virtual void AddDesignEventWithValue(std::string const& eventId, float value, std::string const& fields, bool mergeFields) override;

            virtual void AddErrorEvent(EGAErrorSeverity severity, std::string const& message, std::string const&  fields, bool mergeFields) override;

            virtual void AddAdEventWithDuration(EGAAdAction adAction, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, int64_t duration, std::string const& fields, bool mergeFields) override;
            virtual void AddAdEventWithReason(EGAAdAction adAction, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, EGAAdError noAdReason, std::string const& fields, bool mergeFields) override;
            virtual void AddAdEvent(EGAAdAction adAction, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, std::string const& fields, bool mergeFields) override;

            virtual void SetInfoLog(bool enabled) override;
            virtual void SetVerboseLog(bool enabled) override;


            // ----------------------- REMOTE CONFIGS ---------------------- //

            virtual std::string GetRemoteConfigsValueAsString(std::string const& key, std::string const& defaultValue) override;

            virtual std::string GetRemoteConfigsValueAsJson(std::string const& key) override;

            virtual double GetRemoteConfigsValueAsNumber(std::string const& key, double defaultValue) override;

            virtual bool IsRemoteConfigsReady() override;

            virtual std::string GetRemoteConfigsContentAsString() override;

            virtual void RegisterRemoteConfigListener(RemoteConfigListener listener) override;

            virtual std::string GetABTestingId() override;

            virtual std::string GetABTestingVariantId() override;

            ///// HEALTH

            virtual void EnableSDKInitEvent(bool flag) override;

            virtual void EnableFpsHistogram(FPSTracker tracker, bool flag) override;

            virtual void EnableMemoryHistogram(bool flag) override;

            virtual void EnableHealthHardwareInfo(bool flag) override;

            /////

            virtual void EnableAdvertisingId(bool value) override;

            virtual void OnQuit() override;

            virtual void SetWritablePath(std::string const& path) override;

            ///

            virtual int64_t GetElapsedSessionTime() override;

            virtual int64_t GetElapsedTimeForPreviousSession() override;

            virtual int64_t GetElapsedTimeFromAllSessions() override;
    };
}
