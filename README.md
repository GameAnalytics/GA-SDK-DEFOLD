# GA-SDK-DEFOLD
GameAnalytics SDK for Defold.

Documentation is located [here](https://gameanalytics.com/docs/defold-sdk).  

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
