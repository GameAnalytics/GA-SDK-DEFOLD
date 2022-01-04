var GameAnalyticsDefold = {
    $listener: {
        onRemoteConfigsUpdated: function()
        {
            _js_remote_configs_callback();
        }
    },
    js_configureAvailableCustomDimensions01: function(list)
    {
        gameanalytics.GameAnalytics.configureAvailableCustomDimensions01(JSON.parse(UTF8ToString(list)));
    },
    js_configureAvailableCustomDimensions02: function(list)
    {
        gameanalytics.GameAnalytics.configureAvailableCustomDimensions02(JSON.parse(UTF8ToString(list)));
    },
    js_configureAvailableCustomDimensions03: function(list)
    {
        gameanalytics.GameAnalytics.configureAvailableCustomDimensions03(JSON.parse(UTF8ToString(list)));
    },
    js_configureAvailableResourceCurrencies: function(list)
    {
        gameanalytics.GameAnalytics.configureAvailableResourceCurrencies(JSON.parse(UTF8ToString(list)));
    },
    js_configureAvailableResourceItemTypes: function(list)
    {
        gameanalytics.GameAnalytics.configureAvailableResourceItemTypes(JSON.parse(UTF8ToString(list)));
    },
    js_configureSdkGameEngineVersion: function(version)
    {
        gameanalytics.GameAnalytics.configureSdkGameEngineVersion(UTF8ToString(version));
    },
    js_configureGameEngineVersion: function(version)
    {
        gameanalytics.GameAnalytics.configureGameEngineVersion(UTF8ToString(version));
    },
    js_configureBuild: function(build)
    {
        gameanalytics.GameAnalytics.configureBuild(UTF8ToString(build));
    },
    js_configureUserId: function(userId)
    {
        gameanalytics.GameAnalytics.configureUserId(UTF8ToString(userId));
    },
    js_initialize: function(gamekey, gamesecret)
    {
        gameanalytics.GameAnalytics.addRemoteConfigsListener(listener);
        gameanalytics.GameAnalytics.initialize(UTF8ToString(gamekey), UTF8ToString(gamesecret));
    },
    js_setCustomDimension01: function(customDimension)
    {
        gameanalytics.GameAnalytics.setCustomDimension01(UTF8ToString(customDimension));
    },
    js_setCustomDimension02: function(customDimension)
    {
        gameanalytics.GameAnalytics.setCustomDimension02(UTF8ToString(customDimension));
    },
    js_setCustomDimension03: function(customDimension)
    {
        gameanalytics.GameAnalytics.setCustomDimension03(UTF8ToString(customDimension));
    },
    js_setGlobalCustomEventFields: function (customFields)
    {
        var fieldsString = UTF8ToString(customFields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.setCustomDimension03(JSON.parse(fieldsString));
    },
    js_addBusinessEvent: function(currency, amount, itemType, itemId, cartType, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addBusinessEvent(UTF8ToString(currency), amount, UTF8ToString(itemType), UTF8ToString(itemId), UTF8ToString(cartType), JSON.parse(fieldsString), mergeFields);
    },
    js_addResourceEvent: function (flowType, currency, amount, itemType, itemId, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addResourceEvent(flowType, UTF8ToString(currency), amount, UTF8ToString(itemType), UTF8ToString(itemId), JSON.parse(fieldsString), mergeFields);
    },
    js_addProgressionEvent: function (progressionStatus, progression01, progression02, progression03, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addProgressionEvent(progressionStatus, UTF8ToString(progression01), UTF8ToString(progression02), UTF8ToString(progression03), JSON.parse(fieldsString), mergeFields);
    },
    js_addProgressionEventWithScore: function (progressionStatus, progression01, progression02, progression03, score, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addProgressionEvent(progressionStatus, UTF8ToString(progression01), UTF8ToString(progression02), UTF8ToString(progression03), score, JSON.parse(fieldsString), mergeFields);
    },
    js_addDesignEvent: function (eventId, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addDesignEvent(UTF8ToString(eventId), JSON.parse(fieldsString), mergeFields);
    },
    js_addDesignEventWithValue: function (eventId, value, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addDesignEvent(UTF8ToString(eventId), value, JSON.parse(fieldsString), mergeFields);
    },
    js_addErrorEvent: function (severity, message, fields, mergeFields)
    {
        var fieldsString = UTF8ToString(fields);
        fieldsString = fieldsString ? fieldsString : "{}";
        gameanalytics.GameAnalytics.addErrorEvent(severity, UTF8ToString(message), JSON.parse(fieldsString), mergeFields);
    },
    js_setEnabledInfoLog: function(enabled)
    {
        gameanalytics.GameAnalytics.setEnabledInfoLog(enabled);
    },
    js_setEnabledVerboseLog: function(enabled)
    {
        gameanalytics.GameAnalytics.setEnabledVerboseLog(enabled);
    },
    js_setManualSessionHandling: function(enabled)
    {
        gameanalytics.GameAnalytics.setEnabledManualSessionHandling(enabled);
    },
    js_setEventSubmission: function(enabled)
    {
        gameanalytics.GameAnalytics.setEnabledEventSubmission(enabled);
    },
    js_startSession: function()
    {
        gameanalytics.GameAnalytics.startSession();
    },
    js_endSession: function()
    {
        gameanalytics.GameAnalytics.endSession();
    },
    js_onStop: function()
    {
        gameanalytics.GameAnalytics.onStop();
    },
    js_getRemoteConfigsValueAsString: function(key)
    {
        var returnStr = gameanalytics.GameAnalytics.getRemoteConfigsValueAsString(UTF8ToString(key));
        var bufferSize = lengthBytesUTF8(returnStr) + 1;
        var buffer = _malloc(bufferSize);
        stringToUTF8(returnStr, buffer, bufferSize);
        return buffer;
    },
    js_getRemoteConfigsValueAsStringWithDefaultValue: function(key, defaultValue)
    {
        var returnStr = gameanalytics.GameAnalytics.getRemoteConfigsValueAsString(UTF8ToString(key), UTF8ToString(defaultValue));
        var bufferSize = lengthBytesUTF8(returnStr) + 1;
        var buffer = _malloc(bufferSize);
        stringToUTF8(returnStr, buffer, bufferSize);
        return buffer;
    },
    js_isRemoteConfigsReady: function()
    {
        return gameanalytics.GameAnalytics.isRemoteConfigsReady();
    },
    js_getRemoteConfigsContentAsString: function()
    {
        var returnStr = gameanalytics.GameAnalytics.getRemoteConfigsContentAsString();
        var bufferSize = lengthBytesUTF8(returnStr) + 1;
        var buffer = _malloc(bufferSize);
        stringToUTF8(returnStr, buffer, bufferSize);
        return buffer;
    }
}

autoAddDeps(GameAnalyticsDefold, '$listener');
mergeInto(LibraryManager.library, GameAnalyticsDefold);
