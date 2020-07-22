# GA-SDK-DEFOLD
GameAnalytics SDK for Defold.

Documentation is located [here](https://gameanalytics.com/docs/defold-sdk).  

If you have any issues or feedback regarding the SDK, please contact our friendly support team [here](https://gameanalytics.com/contact).

> :information_source:
>
> The Defold SDK include support for **iOS**, **Android**, **HTML5**, **Windows**, **Mac** and **Linux** platforms
>
> Requirements:
> * **Defold:** 1.2.117+  &nbsp;
> * **iOS:** iOS 7+ &nbsp;
> * **Android:** Android API Level 14 &nbsp;  
>   
> **Build size:**   
> Note that download size differ from the actual build size.   
>   
> **Android:**   
> The SDK build size in a native Android app is only around **49Kb** and the dependecies take up to **820Kb** depending if your app already uses some of the same dependencies.   
>   
> **iOS:**   
> The SDK build size in a native iOS app is only around **242Kb** (armv7) / **259Kb** (armv8).

Changelog
---------
<!--(CHANGELOG_TOP)-->
**3.2.0**
* added remote configs listener method

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
