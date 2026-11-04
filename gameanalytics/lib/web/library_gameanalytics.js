// The javascript GameAnalytics SDK reports remote config updates to the page.
// GAWrapperWeb installs a listener that calls
// window.GameAnalyticsOnRemoteConfigsUpdated(), so this is where that function
// is defined and pointed back at the native side of the extension.

var GameAnalyticsDefoldLibrary = {

    GameAnalyticsDefold_InstallRemoteConfigsBridge: function()
    {
        window.GameAnalyticsOnRemoteConfigsUpdated = function()
        {
            _GameAnalyticsDefold_OnRemoteConfigsUpdated();
        };
    }
};

mergeInto(LibraryManager.library, GameAnalyticsDefoldLibrary);
