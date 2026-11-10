// Vendored copy of code/web/GAWrapperWeb.cpp from the shared GameAnalytics C++ wrapper.
// Keep it in sync with the source, do not change it here.
//
// Defold compiles every source under src/ for every platform, so the whole
// file is wrapped in a platform guard. That is the only change made to it.

#if defined(DM_PLATFORM_HTML5)

#include "GAWrapperWeb.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
#endif

namespace gameanalytics
{
    namespace
    {
        JavaScriptEvaluator jsEvaluator;

        // The javascript SDK namespace as it is exposed on the page.
        constexpr const char* GA = "gameanalytics.GameAnalytics";

        const char* BoolToJS(bool v)
        {
            return v ? "true" : "false";
        }

        std::string NumberToJS(double v)
        {
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "%g", v);
            return buffer;
        }

        std::string NumberToJS(int64_t v)
        {
            return std::to_string(v);
        }

        // Escapes a value so it can be embedded in a single quoted javascript
        // string. Without this a quote or a backslash inside an event id or a
        // custom field breaks the snippet, and could inject javascript.
        std::string QuoteJS(std::string const& value)
        {
            std::string out;
            out.reserve(value.size() + 2);
            out += '\'';

            for(char c : value)
            {
                switch(c)
                {
                    case '\'': out += "\\'";  break;
                    case '\"': out += "\\\""; break;
                    case '\\': out += "\\\\"; break;
                    case '\n': out += "\\n";  break;
                    case '\r': out += "\\r";  break;
                    case '\t': out += "\\t";  break;
                    default:   out += c;      break;
                }
            }

            out += '\'';
            return out;
        }

        std::string MakeJSArray(std::vector<std::string> const& list)
        {
            std::string out = "[";

            for(size_t i = 0; i < list.size(); ++i)
            {
                if(i > 0)
                {
                    out += ",";
                }

                out += QuoteJS(list[i]);
            }

            out += "]";
            return out;
        }

        // Custom event fields travel as a JSON string through the wrapper but
        // the javascript SDK expects a real object, so parse them in the page.
        std::string MakeJSFields(std::string const& fields)
        {
            if(fields.empty())
            {
                return "{}";
            }

            return std::string("JSON.parse(") + QuoteJS(fields) + ")";
        }

        std::string Call(std::string const& method, std::string const& args = "")
        {
            return std::string(GA) + "." + method + "(" + args + ")";
        }

        std::string Join(std::initializer_list<std::string> parts)
        {
            std::string out;

            for(auto const& part : parts)
            {
                if(!out.empty())
                {
                    out += ",";
                }

                out += part;
            }

            return out;
        }

        bool IsTruthy(std::string const& result)
        {
            return result == "true" || result == "1";
        }
    }

    namespace web
    {
        void SetJavaScriptEvaluator(JavaScriptEvaluator evaluator)
        {
            jsEvaluator = evaluator;
        }
    }

    std::string GAWrapperWeb::Eval(std::string const& code)
    {
        if(jsEvaluator)
        {
            return jsEvaluator(code);
        }

#if defined(__EMSCRIPTEN__)
        char* result = emscripten_run_script_string(code.c_str());
        return result ? std::string(result) : std::string();
#else
        static bool warned = false;
        if(!warned)
        {
            warned = true;
            std::fprintf(stderr,
                "[GameAnalytics] no javascript evaluator set, the web wrapper will do nothing. "
                "Call gameanalytics::web::SetJavaScriptEvaluator() or override GAWrapperWeb::Eval().\n");
        }

        (void)code;
        return std::string();
#endif
    }

    GAWrapperWeb::GAWrapperWeb()
    {
    }

    void GAWrapperWeb::SetAvailableCustomDimensions01(const std::vector<std::string>& list)
    {
        Eval(Call("configureAvailableCustomDimensions01", MakeJSArray(list)));
    }

    void GAWrapperWeb::SetAvailableCustomDimensions02(const std::vector<std::string>& list)
    {
        Eval(Call("configureAvailableCustomDimensions02", MakeJSArray(list)));
    }

    void GAWrapperWeb::SetAvailableCustomDimensions03(const std::vector<std::string>& list)
    {
        Eval(Call("configureAvailableCustomDimensions03", MakeJSArray(list)));
    }

    void GAWrapperWeb::SetAvailableResourceCurrencies(const std::vector<std::string>& list)
    {
        Eval(Call("configureAvailableResourceCurrencies", MakeJSArray(list)));
    }

    void GAWrapperWeb::SetAvailableResourceItemTypes(const std::vector<std::string>& list)
    {
        Eval(Call("configureAvailableResourceItemTypes", MakeJSArray(list)));
    }

    void GAWrapperWeb::SetBuild(std::string const& build) {
        Eval(Call("configureBuild", QuoteJS(build)));
    }

    void GAWrapperWeb::SetAutoDetectAppVersion(bool flag) {
        (void)flag; // there is no app version to detect on the web
    }

    void GAWrapperWeb::SetCustomUserId(std::string const& userId) {
        Eval(Call("configureUserId", QuoteJS(userId)));
    }

    void GAWrapperWeb::SetSDKVersion(std::string const& gameEngineSdkVersion) {
        Eval(Call("configureSdkGameEngineVersion", QuoteJS(gameEngineSdkVersion)));
    }

    void GAWrapperWeb::SetGameEngineVersion(std::string const& gameEngineVersion) {
        Eval(Call("configureGameEngineVersion", QuoteJS(gameEngineVersion)));
    }

    void GAWrapperWeb::Initialize(std::string const& gameKey, std::string const& gameSecret) {
        Eval(Call("initialize", Join({QuoteJS(gameKey), QuoteJS(gameSecret)})));
    }

    void GAWrapperWeb::AddBusinessEvent(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& receipt, std::string const& fields, bool mergeFields)
    {
        (void)receipt; // there is no store receipt to validate on the web
        Eval(Call("addBusinessEvent", Join({
            QuoteJS(currency),
            NumberToJS(static_cast<int64_t>(amount)),
            QuoteJS(itemType),
            QuoteJS(itemId),
            QuoteJS(cartType),
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddBusinessEventWithReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& receipt, std::string const& store, std::string const& signature, std::string const& fields, bool mergeFields)
    {
        (void)store;
        (void)signature;
        return AddBusinessEvent(currency, amount, itemType, itemId, cartType, receipt, fields, mergeFields);
    }

    void GAWrapperWeb::AddBusinessEventWithValidation(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& store, std::string const& productId, std::string const& purchaseToken, std::string const& fields, bool mergeFields)
    {
        // receipt validation is a store feature, there is nothing to validate on the web
        (void)store;
        (void)productId;
        (void)purchaseToken;
        return AddBusinessEvent(currency, amount, itemType, itemId, cartType, "", fields, mergeFields);
    }

    void GAWrapperWeb::AddBusinessEventAndAutoFetchReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& fields, bool mergeFields) {
        return AddBusinessEvent(currency, amount, itemType, itemId, cartType, "", fields, mergeFields);
    }

    void GAWrapperWeb::AddResourceEvent(EGAResourceFlowType flowType, std::string const& currency, float amount, std::string const& itemType, std::string const& itemId, std::string const& fields, bool mergeFields)
    {
        Eval(Call("addResourceEvent", Join({
            NumberToJS(static_cast<int64_t>(flowType)),
            QuoteJS(currency),
            NumberToJS(static_cast<double>(amount)),
            QuoteJS(itemType),
            QuoteJS(itemId),
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddProgressionEvent(EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, std::string const& fields, bool mergeFields) {
        // the javascript SDK skips the score when it is not a number
        Eval(Call("addProgressionEvent", Join({
            NumberToJS(static_cast<int64_t>(progressionStatus)),
            QuoteJS(progression01),
            QuoteJS(progression02),
            QuoteJS(progression03),
            "null",
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddProgressionEventWithScore(EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, int score, std::string const& fields, bool mergeFields) {
        Eval(Call("addProgressionEvent", Join({
            NumberToJS(static_cast<int64_t>(progressionStatus)),
            QuoteJS(progression01),
            QuoteJS(progression02),
            QuoteJS(progression03),
            NumberToJS(static_cast<int64_t>(score)),
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddDesignEvent(std::string const& eventId, std::string const& fields, bool mergeFields) {
        // the javascript SDK skips the value when it is not a number
        Eval(Call("addDesignEvent", Join({
            QuoteJS(eventId),
            "null",
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddDesignEventWithValue(std::string const& eventId, float value, std::string const& fields, bool mergeFields) {
        Eval(Call("addDesignEvent", Join({
            QuoteJS(eventId),
            NumberToJS(static_cast<double>(value)),
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddErrorEvent(EGAErrorSeverity severity, std::string const& message, std::string const& fields, bool mergeFields) {
        Eval(Call("addErrorEvent", Join({
            NumberToJS(static_cast<int64_t>(severity)),
            QuoteJS(message),
            MakeJSFields(fields),
            BoolToJS(mergeFields)})));
    }

    void GAWrapperWeb::AddAdEvent(EGAAdAction action, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, std::string const& fields, bool mergeFields) {
        return; // ad events are not supported
    }

    void GAWrapperWeb::AddAdEventWithDuration(EGAAdAction action, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, int64_t duration, std::string const& fields, bool mergeFields) {
        return; // ad events are not supported
    }

    void GAWrapperWeb::AddAdEventWithReason(EGAAdAction adAction, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, EGAAdError noAdReason, std::string const& fields, bool mergeFields) {
        return; // ad events are not supported
    }

    void GAWrapperWeb::SetInfoLog(bool flag) {
        Eval(Call("setEnabledInfoLog", BoolToJS(flag)));
    }

    void GAWrapperWeb::SetVerboseLog(bool flag) {
        Eval(Call("setEnabledVerboseLog", BoolToJS(flag)));
    }

    void GAWrapperWeb::SetEnabledManualSessionHandling(bool flag) {
        Eval(Call("setEnabledManualSessionHandling", BoolToJS(flag)));
    }

    void GAWrapperWeb::SetEnabledErrorReporting(bool flag) {
        (void)flag; // not exposed by the javascript SDK
    }

    void GAWrapperWeb::SetEnabledEventSubmission(bool flag, bool doCache) {
        (void)flag; // not exposed by the javascript SDK
        (void)doCache;
    }

    void GAWrapperWeb::SetCustomDimension01(std::string const& customDimension) {
        Eval(Call("setCustomDimension01", QuoteJS(customDimension)));
    }

    void GAWrapperWeb::SetCustomDimension02(std::string const& customDimension) {
        Eval(Call("setCustomDimension02", QuoteJS(customDimension)));
    }

    void GAWrapperWeb::SetCustomDimension03(std::string const& customDimension) {
        Eval(Call("setCustomDimension03", QuoteJS(customDimension)));
    }

    void GAWrapperWeb::StartSession() {
        Eval(Call("startSession"));
    }

    void GAWrapperWeb::EndSession() {
        Eval(Call("endSession"));
    }

    std::string GAWrapperWeb::GetRemoteConfigsValueAsString(std::string const& key, std::string const& defaultValue) {
        return Eval(Call("getRemoteConfigsValueAsString", Join({QuoteJS(key), QuoteJS(defaultValue)})));
    }

    double GAWrapperWeb::GetRemoteConfigsValueAsNumber(std::string const& key, double defaultValue) {
        // the javascript SDK only returns strings, so convert here
        const std::string value = GetRemoteConfigsValueAsString(key, "");
        if(value.empty())
        {
            return defaultValue;
        }

        char* end = nullptr;
        const double parsed = std::strtod(value.c_str(), &end);

        return (end && *end == '\0') ? parsed : defaultValue;
    }

    std::string GAWrapperWeb::GetRemoteConfigsValueAsJson(std::string const& key) {
        // the javascript SDK has no typed getter, read the value back as a
        // string and let the caller parse it
        return GetRemoteConfigsValueAsString(key, "");
    }

    bool GAWrapperWeb::IsRemoteConfigsReady() {
        return IsTruthy(Eval(Call("isRemoteConfigsReady")));
    }

    std::string GAWrapperWeb::GetRemoteConfigsContentAsString() {
        return Eval(Call("getRemoteConfigsContentAsString"));
    }

    void GAWrapperWeb::RegisterRemoteConfigListener(RemoteConfigListener listener) {
        _remoteConfigListener = listener;

        // Install the javascript half. The SDK calls this back in the page, so
        // the engine's javascript bridge has to turn that into a call to
        // NotifyRemoteConfigsUpdated() on this object. Under Emscripten that
        // is a Module.ccall on an exported function; other engines have their
        // own mechanism, which is why this is left to the engine wrapper.
        Eval(Call("addRemoteConfigsListener",
                  "function(){ if (window.GameAnalyticsOnRemoteConfigsUpdated) "
                  "{ window.GameAnalyticsOnRemoteConfigsUpdated(); } }"));
    }

    void GAWrapperWeb::NotifyRemoteConfigsUpdated() {
        if(_remoteConfigListener)
        {
            _remoteConfigListener(GetRemoteConfigsContentAsString());
        }
    }

    std::string GAWrapperWeb::GetABTestingId() {
        return Eval(Call("getABTestingId"));
    }

    std::string GAWrapperWeb::GetABTestingVariantId() {
        return Eval(Call("getABTestingVariantId"));
    }

    void GAWrapperWeb::EnableAdvertisingId(bool value) {
        (void)value; // there is no advertising id on the web
    }

    // The javascript SDK has one switch for all the health events and
    // measures the frame rate itself.
    void GAWrapperWeb::EnableSDKInitEvent(bool value) {
        Eval(Call("enableHealthEvent", BoolToJS(value)));
    }

    void GAWrapperWeb::EnableFpsHistogram(FPSTracker tracker, bool value) {
        (void)tracker;
        Eval(Call("enableHealthEvent", BoolToJS(value)));
    }

    void GAWrapperWeb::EnableMemoryHistogram(bool value) {
        Eval(Call("enableHealthEvent", BoolToJS(value)));
    }

    void GAWrapperWeb::EnableHealthHardwareInfo(bool value) {
        Eval(Call("enableHealthEvent", BoolToJS(value)));
    }

    void GAWrapperWeb::OnQuit() {
    }

    void GAWrapperWeb::SetWritablePath(std::string const& path) {
        (void)path; // the javascript SDK stores its data in local storage
    }

    std::string GAWrapperWeb::GetExternalUserId() {
        return "";
    }

    void GAWrapperWeb::SetExternalUserId(const std::string &extUserId) {
        (void)extUserId; // not exposed by the javascript SDK
    }

    std::string GAWrapperWeb::GetUserId() {
        return Eval(Call("getUserId"));
    }

    void GAWrapperWeb::SetGlobalCustomEventFields(const std::string &customFields) {
        Eval(Call("setGlobalCustomEventFields", MakeJSFields(customFields)));
    }

    int64_t GAWrapperWeb::GetElapsedSessionTime() {
        return 0;
    }

    int64_t GAWrapperWeb::GetElapsedTimeForPreviousSession() {
        return 0;
    }

    int64_t GAWrapperWeb::GetElapsedTimeFromAllSessions() {
        return 0;
    }
}

#endif // DM_PLATFORM_HTML5
