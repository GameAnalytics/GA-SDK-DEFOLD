# GameAnalytics SDK for Defold

Track player progression, monetization, ads, errors and custom design events from your Defold game, and change your game's behavior remotely with Remote Configs and A/B testing.

* [GameAnalytics documentation for Defold](https://gameanalytics.com/docs/s/article/Defold-SDK-Setup)
* Questions or problems: [contact our support team](https://gameanalytics.com/contact)

## Requirements

| | |
|---|---|
| Defold | 1.14 or newer |
| iOS | 12.0 or newer (devices and the simulator) |
| Android | 5.0 (API level 21) or newer |
| Desktop | macOS (Apple silicon and Intel), Windows (64-bit), Linux (64-bit) |
| Web | HTML5 (`wasm-web`) |

You need a game in your [GameAnalytics account](https://tool.gameanalytics.com) and its **game key** and **secret key** for every platform you build for.

## Installation

1. Open your `game.project` and add the SDK to **Project > Dependencies**:

   ```
   https://github.com/GameAnalytics/GA-SDK-DEFOLD/archive/refs/tags/<version>.zip
   ```

   Replace `<version>` with the [release](https://github.com/GameAnalytics/GA-SDK-DEFOLD/tags) you want.

2. Select **Project > Fetch Libraries** in the editor.

3. Add your keys to `game.project` (see [Configuration](#configuration)).

That's all: the extension starts GameAnalytics when the game starts, and the `gameanalytics` lua module is available in all your scripts.

## Configuration

Everything is set in a `[gameanalytics]` section of your `game.project`. Open the file in a text editor to add it:

```ini
[gameanalytics]
game_key_ios = <your iOS game key>
secret_key_ios = <your iOS secret key>
game_key_android = <your Android game key>
secret_key_android = <your Android secret key>
game_key_html5 = <your web game key>
secret_key_html5 = <your web secret key>

build_ios = 1.0.0
build_android = 1.0.0
build_html5 = 1.0.0

custom_dimensions_01 = ninja,samurai
resource_currencies = gold,gems
resource_item_types = boost,lives

enable_info_log = 1
```

### Keys and builds

Each platform has its own game in GameAnalytics, so the keys and the build version are set per platform. Replace `<platform>` with `ios`, `android`, `html5`, `osx`, `windows` or `linux`.

| Setting | Description |
|---|---|
| `game_key_<platform>` | The game key of the platform's game. Required. |
| `secret_key_<platform>` | The secret key of the platform's game. Required. |
| `build_<platform>` | The build version sent with every event, e.g. `1.0.0`. |
| `auto_detect_app_version` | `1` to send the app's version (iOS, Android) as the build instead. Default `0`. |
| `write_path_<platform>` | Desktop only (`osx`, `windows`, `linux`): the folder the SDK keeps its data in, instead of its default (`~/GameAnalytics` on macOS and Linux, `%LOCALAPPDATA%\GameAnalytics` on Windows). The folder must exist, the SDK only creates its own subfolder in it. `$VAR`, `${VAR}`, `%VAR%` and a leading `~` are expanded, e.g. `%APPDATA%/MyGame`. |

### Allowed values

Custom dimensions, resource currencies and resource item types must be declared before the SDK starts. Each setting is a comma separated list of up to 20 values.

| Setting | Used by |
|---|---|
| `custom_dimensions_01`, `custom_dimensions_02`, `custom_dimensions_03` | `gameanalytics.setCustomDimension01` / `02` / `03` |
| `resource_currencies` | the `currency` of resource events |
| `resource_item_types` | the `itemType` of resource events |

### Behavior

| Setting | Description | Default |
|---|---|---|
| `manual_initialize` | `1` to start the SDK yourself with `gameanalytics.initialize()`, e.g. after asking the player for consent. | `0` |
| `use_custom_id` | `1` to start the SDK when you set your own user id with `gameanalytics.configureUserId(id)`. | `0` |
| `use_manual_session_handling` | `1` to start and end sessions yourself with `gameanalytics.startSession()` and `gameanalytics.endSession()`. By default sessions follow the app going to the foreground and background. | `0` |
| `enable_event_submission` | `0` to keep events on the device instead of sending them. | `1` |
| `enable_error_reporting` | `1` to send an error event when the game crashes. | `0` |
| `enable_info_log` | `1` for basic SDK logging, useful during integration. | `0` |
| `enable_verbose_log` | `1` to also log every event and request. | `0` |

When neither `manual_initialize` nor `use_custom_id` is set, the SDK starts as soon as the game starts.

### Health events

[Health events](https://docs.gameanalytics.com/event-types/health-events/) report how the game performs on the players' devices: the time from launch to the SDK start, and histograms of the frame rate and memory use over the session. They are off by default and shown in GameAnalytics to Pro accounts.

| Setting | Description | Default |
|---|---|---|
| `enable_sdk_init_event` | `1` to send the SDK init event with the app's boot time. | `0` |
| `enable_fps_histogram` | `1` to sample the frame rate once a second and send the histogram at the end of the session. The extension measures the rate from the engine's update, values are capped at 120. | `0` |
| `enable_memory_histogram` | `1` to sample the memory use every 5 seconds and send the histogram at the end of the session. | `0` |
| `enable_health_hardware_info` | `1` to add the device's hardware (CPU, GPU, cores) to the health events. Experimental. | `0` |

Games with `manual_initialize = 1` can also switch these at runtime with `gameanalytics.enableSDKInitEvent(flag)` and friends (see the [API reference](#api-reference)), before calling `initialize()`. On the web the four settings are one switch in the JavaScript SDK: any of them enables all health events.

## Sending events

Every event function takes a table of options. Calling one with a missing or wrong option raises a lua error, so mistakes show up during development.

### Business events

In-app purchases. `amount` is in cents.

```lua
gameanalytics.addBusinessEvent({
    currency = "USD",
    amount = 99,
    itemType = "boost",
    itemId = "megaBoost",
    cartType = "shop",
})
```

`currency`, `itemType` and `itemId` are required. To validate the purchase, add the store receipt:

* iOS: `receipt = "<base64 receipt>"`, or `autoFetchReceipt = true` to let the SDK read it
* Android: `receipt = "<purchase data>"` and `signature = "<signature>"`

### Resource events

The flow of your in-game currencies.

```lua
gameanalytics.addResourceEvent({
    flowType = "Source", -- "Source" (gained) or "Sink" (spent)
    currency = "gold",
    amount = 100,
    itemType = "boost",
    itemId = "daily_reward",
})
```

`currency`, `amount`, `itemType` and `itemId` are required. `currency` and `itemType` must be one of the values declared in `game.project`, and `amount` must be greater than zero.

### Progression events

Levels, worlds and other stages of your game, with up to three levels of hierarchy.

```lua
gameanalytics.addProgressionEvent({
    progressionStatus = "Complete", -- "Start", "Complete" or "Fail"
    progression01 = "world1",
    progression02 = "level1",       -- optional
    progression03 = "boss",         -- optional
    score = 1200,                   -- optional
})
```

### Design events

Anything else you want to track, as a colon separated event id with an optional value.

```lua
gameanalytics.addDesignEvent({ eventId = "tutorial:step3:completed" })
gameanalytics.addDesignEvent({ eventId = "settings:volume", value = 0.8 })
```

### Error events

```lua
gameanalytics.addErrorEvent({
    severity = "Warning", -- "Debug", "Info", "Warning", "Error" or "Critical"
    message = "Could not load the level data",
})
```

### Ad events

```lua
gameanalytics.addAdEvent({
    adAction = "Show",        -- "Clicked", "Show", "FailedShow" or "RewardReceived"
    adType = "Interstitial",  -- "Video", "RewardedVideo", "Playable", "Interstitial", "OfferWall" or "Banner"
    adSdkName = "admob",
    adPlacement = "level_end",
})
```

Add `duration = <seconds>` for how long a rewarded video was watched, or `noAdReason` for why an ad failed to show (`"Unknown"`, `"Offline"`, `"NoFill"`, `"InternalError"`, `"InvalidRequest"` or `"UnableToPrecache"`).

### Custom fields

Every event takes optional `customFields`, a JSON string with your own key-value pairs (strings or numbers). They are kept in the raw event data for exports.

```lua
gameanalytics.addDesignEvent({
    eventId = "shop:opened",
    customFields = json.encode({ source = "main_menu", coins = 120 }),
})
```

Fields that should go with every event can be set once with `gameanalytics.setGlobalCustomEventFields(json)`. Pass `mergeFields = true` with an event to combine its fields with the global ones instead of replacing them.

## Custom dimensions

Segment your players, e.g. by class or by the variant of a feature they see. Each value must be declared in `game.project`.

```lua
gameanalytics.setCustomDimension01("ninja")
```

## Remote Configs and A/B testing

Remote Configs are values you set in the GameAnalytics tool and read in your game, without releasing a new build. They are downloaded when the SDK starts.

```lua
-- called whenever the remote configs are downloaded or updated
gameanalytics.setRemoteConfigsListener(function(self)
    local difficulty = gameanalytics.getRemoteConfigsValueAsNumber({ key = "difficulty", defaultValue = 1 })
    local title = gameanalytics.getRemoteConfigsValueAsString({ key = "shop_title", defaultValue = "Shop" })
end)

if gameanalytics.isRemoteConfigsReady() then
    -- all values as a JSON string
    print(gameanalytics.getRemoteConfigsContentAsString())
end
```

Set the listener before the SDK starts (in your first script's `init`, or before calling `gameanalytics.initialize()`) so the first download isn't missed.

When the player is part of an A/B test, `gameanalytics.getABTestingId()` and `gameanalytics.getABTestingVariantId()` return the test and the variant they are in.

## User ids

```lua
gameanalytics.getUserId()               -- the id GameAnalytics uses for this player
gameanalytics.setExternalUserId("1234") -- link the player to your own account system
gameanalytics.getExternalUserId()
```

To use your own id as the GameAnalytics user id, set `use_custom_id = 1` and call `gameanalytics.configureUserId(id)`, which also starts the SDK.

## API reference

| Function | Description |
|---|---|
| `initialize()` | Starts the SDK when `manual_initialize = 1`. |
| `configureUserId(id)` | Sets your own user id and starts the SDK when `use_custom_id = 1`. |
| `addBusinessEvent(options)` | See [Business events](#business-events). |
| `addResourceEvent(options)` | See [Resource events](#resource-events). |
| `addProgressionEvent(options)` | See [Progression events](#progression-events). |
| `addDesignEvent(options)` | See [Design events](#design-events). |
| `addErrorEvent(options)` | See [Error events](#error-events). |
| `addAdEvent(options)` | See [Ad events](#ad-events). |
| `setCustomDimension01(value)`, `02`, `03` | Sets a custom dimension. |
| `setGlobalCustomEventFields(json)` | Custom fields added to every event. |
| `startSession()`, `endSession()` | Starts or ends a session when `use_manual_session_handling = 1`. |
| `setEnabledManualSessionHandling(flag)` | Switches manual session handling at runtime. |
| `setEnabledEventSubmission(flag)` | Stops or resumes sending events. |
| `setEnabledInfoLog(flag)`, `setEnabledVerboseLog(flag)` | Switches SDK logging at runtime. |
| `setRemoteConfigsListener(function(self) end)` | Called when the remote configs are downloaded or updated. |
| `isRemoteConfigsReady()` | `true` once the remote configs are downloaded. |
| `getRemoteConfigsValueAsString({ key, defaultValue })` | A remote config value as a string. |
| `getRemoteConfigsValueAsNumber({ key, defaultValue })` | A remote config value as a number. |
| `getRemoteConfigsValueAsJson(key)` | A remote config value as a JSON string. |
| `getRemoteConfigsContentAsString()` | All remote configs as a JSON string. |
| `getABTestingId()`, `getABTestingVariantId()` | The player's A/B test and variant, empty when they are in none. |
| `getUserId()` | The GameAnalytics user id. |
| `setExternalUserId(id)`, `getExternalUserId()` | Your own id for the player. |
| `getElapsedSessionTime()` | Length of the current session, in seconds. |
| `getElapsedTimeForPreviousSession()` | Length of the previous session, in seconds. |
| `getElapsedTimeFromAllSessions()` | Total time over all sessions, in seconds. |
| `enableSDKInitEvent(flag)`, `enableFpsHistogram(flag)`, `enableMemoryHistogram(flag)`, `enableHealthHardwareInfo(flag)` | Switches the [health events](#health-events), call before `initialize()`. |

## Platform notes

**Android**
* The SDK leaves out Google's App Set ID library, so App Set IDs are not sent. Games that already include `play-services-appset` (for example through an ads or Firebase extension) send them as usual.
* Error reporting (`enable_error_reporting = 1`) needs the `androidx.core` library, which games usually get from another extension.
* `getElapsedTimeForPreviousSession()` returns 0.

**iOS**
* The SDK ships its privacy manifest (`PrivacyInfo.xcprivacy`), which Defold adds to your app.

**Web**
* External user ids and session times are not available and return empty values or 0.
* `setEnabledEventSubmission` has no effect.
* The health events have a single switch: any of the four settings or functions enables all of them.

**Desktop**
* Requests go through Defold's own HTTP client, so no extra libraries are needed.
* `getElapsedSessionTime()` is only meaningful once a session has started.

## Upgrading from 4.x

* Defold 1.14 or newer is required.
* Remove the `defold-openssl` dependency from your `game.project`, the SDK no longer needs it.
* 32-bit Windows (`x86-win32`), 32-bit iOS (`armv7-ios`) and the `js-web` target are no longer supported, following Defold.
* New functions: `getUserId`, `getExternalUserId`, `setExternalUserId`, `getABTestingId`, `getABTestingVariantId`, `getRemoteConfigsValueAsNumber`, `getRemoteConfigsValueAsJson` and the session time functions.
* Remote Configs on iOS are now returned as JSON, like on the other platforms.

Changelog
---------
<!--(CHANGELOG_TOP)-->
**4.3.0**
* updated for defold 1.8.1

**4.2.6**
* fixed bug in internal error reporting

**4.2.5**
* added event uuid for events sent

**4.2.4**
* fixed progression tries for desktop platforms

**4.2.3**
* removed imei identifiers and other alternative identifiers from user identifier logic (android)

**4.2.2**
* added error events to be sent for invalid custom event fields used
* added optional mergeFields argument to event methods to merge with global custom fields instead of overwrite them

**4.2.1**
* fixed missing custom event fields for when trying to fix missing session end events

**4.2.0**
* added global custom event fields function to allow to add custom fields to events sent automatically by the SDK

**4.1.4**
* added functionality to force a new user in a/b testing without having to uninstall app first, simply use custom user id function to set a new user id which hasn't been used yet

**4.1.3**
* fixed bug with custom event fields

**4.1.2**
* fixed errors for building for windows

**4.1.1**
* custom fields fixes

**4.1.0**
* added custom event fields feature

**4.0.0**
* Changed user identifier logic in preparation for Google changes to GAID. User id for a new install is now a randomised GUID. Existing installs that update SDK will continue using previous identifier logic. It is recommended to update as soon as possible to reduce impact on calculated metrics.

**3.3.11**
* it should now be possible to not show idfa consent dialog if you don't have any third party code that needs to use idfa (ios)
* prepared for google advertising identifier changes (will not use google advertising identifier when user has opted out) (android)

**3.3.10**
* added idfa consent status field to events

**3.3.9**
* added manifest for web

**3.3.8**
* updated client ts validator

**3.3.7**
* fixed dependencies for iOS

**3.3.6**
* removed memory info from automatic crash reports

**3.3.5**
* corrected ad event annotation

**3.3.4**
* crash fix for when using keys for remote configs that are not present and not specifying default value

**3.3.3**
* added manual initialize option to use when setting remote configs listener

**3.3.2**
* bug fix for remote configs functions crash

**3.3.1**
* improved user identifier flow for ios (ios)

**3.3.0**
* added ad event

**3.2.0**
* added remote configs listener method
* updated user identifier flow to prepare for iOS 14 IDFA changes (ios)

**3.1.10**
* small fix for ios builds

**3.1.9**
* fixed progression event with scores (android)

**3.1.8**
* fixed getRemoteConfigsValueAsString, isRemoteConfigsReady and getRemoteConfigsContentAsString for html5 builds

**3.1.7**
* fixed bug to not send stored events from previous sessions (offline events or session end events not sent yet) by games on the same domain (html5)
* this bug fix can potentially affect metrics slightly so be aware of this as old stored events (offline events and session end events not sent yet) in games will not be sent with this new fix because internal keys for storing events in localstorage have changed now (html5)

**3.1.6**
* improved event processing queue thread (android)

**3.1.5**
* cryptojs bug fix (html5)

**3.1.4**
* added session_num to init request

**3.1.3**
* added auto detect app version for build field option

**3.1.2**
* removed facebook, gender and birthyear functions
* updated curl libs

**3.1.1**
* A/B testing fixes

**3.1.0**
* moved openSSL to seperate Defold extension. REMEMBER to add it to your project's dependencies. See documentation for details.

**3.0.3**
* fixed getRemoteConfigsValueAsString bug

**3.0.2**
* remote configs fixed

**3.0.1**
* fixed events bug (ios, html5)

**3.0.0**
* Remote Config calls have been updated and the old calls have deprecated. Please see GA documentation for the new SDK calls and migration guide
* A/B testing support added

**2.3.2**
* fixed html5 build errors

**2.3.1**
* fixed crash for mac and win32 builds

**2.3.0**
* improved device identifier flow (android)
* OPS: refactored IMEI code out into a seperate library, please check the documentation page for how to use it now (only relevant for apps using IMEI ids) (android)

**2.2.8**
* corrected error reporting for desktop platforms to call previous handler as well

**2.2.7**
* added option to enable/disable GA error reporting

**2.2.6**
* bug fix for too big log files for desktop platforms

**2.2.5**
* added check if log files and database can't be created

**2.2.4**
* fixed progression event bug
* fixed hanging background thread for desktop platforms

**2.2.3**
* fixed compile error for 64-bit android builds

**2.2.2**
* bug fix for html5 builds

**2.2.1**
* migrated to use c strings in c++ native sdk to support better for linux platforms

**2.2.0**
* added writable path setting option for desktop platforms

**2.1.3**
* linux library updated to correct one (changed from using clang to gcc)

**2.1.2**
* fixed linux library

**2.1.1**
* fixes to setEnabledEventSubmission

**2.1.0**
* added enable/disable event submission function

**2.0.2**
* fixed business event validation

**2.0.1**
* fixed thread hanging on shutdown for desktop platforms

**2.0.0**
* added command center funcitonality

**1.2.4**
* added custom dimensions to design and error events
* fixed linux support to work again

**1.2.3**
* fixed session length bug
* fixed not allowing to add events when session is not started

**1.2.2**
* more fixes openssl lib for osx (osx)

**1.2.1**
* fixes openssl lib for osx (osx)

**1.2.0**
* linux support added

**1.1.7**
* openssl lib for windows modified to not clash with Defold duplicate symbols (windows)

**1.1.6**
* fixed build bug for windows and mac targets (windows, mac)

**1.1.5**
* fixed progression events to work with progression03 as well (alsways sent empty before)

**1.1.4**
* ios library fixed (ios)

**1.1.3**
* bug fix: events now sent with https on windows (windows)

**1.1.2**
* bug fix: events now sent with https on mac (mac)

**1.1.1**
* JNI memory leak bug fix (android)

**1.1.0**
* added windows support

**1.0.2**
* fixed native html5 lib (html5)

**1.0.1**
* bug fix when building to html5 (html5)

**1.0.0**
* initial release
