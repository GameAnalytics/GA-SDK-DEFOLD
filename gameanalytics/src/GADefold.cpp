#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)

#include <atomic>

#include <dmsdk/dlib/log.h>
#include <dmsdk/dlib/time.h>

#include "GADefold.h"

#if defined(DM_PLATFORM_ANDROID)
    #include <pthread.h>

    #include <dmsdk/dlib/android.h>
    #include <dmsdk/graphics/graphics_native.h>

    #include "ga/android/GAWrapperAndroid.h"
#elif defined(DM_PLATFORM_IOS)
    #include "ga/ios/GAWrapperIOS.h"
#elif defined(DM_PLATFORM_HTML5)
    #include <emscripten.h>

    #include "ga/web/GAWrapperWeb.h"

    // Implemented in lib/web/library_gameanalytics.js.
    extern "C" void GameAnalyticsDefold_InstallRemoteConfigsBridge();
#else
    #include "GameAnalytics/GameAnalytics.h"
    #include "ga/desktop/GAWrapperCpp.h"
    #include "http/GAHttpClientDefold.h"
#endif

namespace gameanalytics
{
    namespace defold
    {
        namespace
        {
            GAWrapper* wrapper = 0;
            bool bound = false;

#if defined(DM_PLATFORM_HTML5)
            // =============================================================
            // ENGINE HOOK (GAWrapperWeb.h): javascript evaluation.
            //
            // Defold's html5 build is an Emscripten build, so the runtime
            // already carries an evaluator and there is no separate engine
            // bridge to route the snippets through.
            // =============================================================
            class GAWrapperDefoldWeb:
                public GAWrapperWeb
            {
                protected:

                    virtual std::string Eval(std::string const& code) override
                    {
                        char* result = emscripten_run_script_string(code.c_str());
                        return result ? std::string(result) : std::string();
                    }
            };
#endif

#if !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_HTML5)
            // =============================================================
            // ENGINE HOOK (GAWrapperCpp.h): HTTP transport, see
            // http/GAHttpClientDefold.h.
            // =============================================================
            class GAWrapperDefoldCpp:
                public GAWrapperCpp
            {
                protected:

                    virtual void ConfigureHttpClient() override
                    {
                        if(!http::GetService())
                        {
                            dmLogError("The engine has no HTTP service, GameAnalytics events will not be sent");
                            return;
                        }

                        GameAnalytics::configureHttpClient<http::GAHttpClientDefold>();
                    }
            };
#endif

#if defined(DM_PLATFORM_ANDROID)
            // =============================================================
            // ENGINE HOOK (GAWrapperAndroid.h): JNI context.
            //
            // The extension is built with GA_NO_JNI_ONLOAD (see ext.manifest)
            // because Defold links every extension into a single shared
            // library and that library can only hold one JNI_OnLoad, so the
            // JavaVM, the game Activity and the app's ClassLoader are handed
            // over by hand.
            //
            // The ClassLoader matters: the wrapper is called from Defold's
            // engine thread, which java did not create, and FindClass there
            // only sees the system classes, not the GameAnalytics ones.
            // =============================================================
            void BindAndroid()
            {
                android::SetJavaVM(dmGraphics::GetNativeAndroidJavaVM());

                dmAndroid::ThreadAttacher threadAttacher;
                JNIEnv* env = threadAttacher.GetEnv();

                if(!env)
                {
                    dmLogError("Failed to attach to the JavaVM, GameAnalytics will not see the game Activity");
                    return;
                }

                jobject activity = dmGraphics::GetNativeAndroidActivity();
                android::SetGameActivity(env, activity);

                jclass activityClass = env->GetObjectClass(activity);
                jmethodID getClassLoader = env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
                jobject classLoader = env->CallObjectMethod(activity, getClassLoader);
                if(env->ExceptionCheck())
                {
                    env->ExceptionClear();
                    classLoader = 0;
                }

                if(classLoader)
                {
                    android::SetClassLoader(env, classLoader);
                    env->DeleteLocalRef(classLoader);
                }
                else
                {
                    dmLogError("Failed to get the app's ClassLoader, GameAnalytics will not find its java classes");
                }

                env->DeleteLocalRef(activityClass);
            }

            // Detaches a thread GAWrapperDefoldAndroid attached, when it exits.
            pthread_key_t detachKey;
            pthread_once_t detachKeyOnce = PTHREAD_ONCE_INIT;

            void DetachThread(void*)
            {
                JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
                JNIEnv* env = 0;
                if(vm && vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK)
                {
                    vm->DetachCurrentThread();
                }
            }

            void CreateDetachKey()
            {
                pthread_key_create(&detachKey, DetachThread);
            }

            // =============================================================
            // ENGINE HOOK (GAWrapperAndroid.h): JNIEnv for the calling thread.
            //
            // The default caches the JNIEnv per thread, which isn't safe in
            // Defold: dmAndroid::ThreadAttacher detaches the engine thread
            // when it goes out of scope, and the engine uses it all the time,
            // so a cached JNIEnv ends up pointing at a detached thread. Look
            // it up on every call instead, attaching the thread when needed.
            // =============================================================
            class GAWrapperDefoldAndroid:
                public GAWrapperAndroid
            {
                protected:

                    virtual JNIEnv* GetJavaEnv() override
                    {
                        JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
                        if(!vm)
                        {
                            return 0;
                        }

                        JNIEnv* env = 0;
                        if(vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK)
                        {
                            return env;
                        }

                        if(vm->AttachCurrentThread(&env, 0) != JNI_OK)
                        {
                            dmLogError("Failed to attach the thread to the JavaVM");
                            return 0;
                        }

                        // A thread that exits while attached aborts the app.
                        pthread_once(&detachKeyOnce, CreateDetachKey);
                        pthread_setspecific(detachKey, (void*)1);
                        return env;
                    }
            };
#endif
        }

        void BindEngine()
        {
            if(bound)
            {
                return;
            }

            bound = true;

#if defined(DM_PLATFORM_ANDROID)
            BindAndroid();
#endif
        }

        GAWrapper* Get()
        {
            // The engine context has to be in place before the wrapper makes
            // its first call. BindEngine() does nothing once it has run, so
            // asking for it here as well as from AppInitializeExtension only
            // makes the order impossible to get wrong.
            BindEngine();

            if(!wrapper)
            {
#if defined(DM_PLATFORM_ANDROID)
                wrapper = new GAWrapperDefoldAndroid();
#elif defined(DM_PLATFORM_IOS)
                wrapper = new GAWrapperIOS();
#elif defined(DM_PLATFORM_HTML5)
                wrapper = new GAWrapperDefoldWeb();

                // ENGINE HOOK (GAWrapperWeb.h): the javascript SDK reports
                // remote config updates to the page, not to native code.
                // GAWrapperWeb installs a listener that calls
                // window.GameAnalyticsOnRemoteConfigsUpdated(); this is the
                // half that makes that function exist and come back here.
                GameAnalyticsDefold_InstallRemoteConfigsBridge();
#else
                wrapper = new GAWrapperDefoldCpp();
#endif
            }

            return wrapper;
        }

        const char* PlatformName()
        {
#if defined(DM_PLATFORM_ANDROID)
            return "android";
#elif defined(DM_PLATFORM_IOS)
            return "ios";
#elif defined(DM_PLATFORM_HTML5)
            return "html5";
#elif defined(DM_PLATFORM_OSX)
            return "osx";
#elif defined(DM_PLATFORM_WINDOWS)
            return "windows";
#else
            return "linux";
#endif
        }

        void SetHttpService(HttpService* service)
        {
#if !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_HTML5)
            http::SetService(service);
#else
            (void)service;
#endif
        }

        // =================================================================
        // ENGINE HOOK (GAWrapper.h): the FPS tracker of the health events.
        //
        // The engine doesn't publish its frame rate, so it is measured from
        // the extension's update: frames are counted over a one second
        // window and the rate of the last full window is what the SDK gets.
        // The SDK reads it from its own thread, hence the atomic.
        // =================================================================
        namespace
        {
            const uint64_t FPS_WINDOW_US = 1000000;

            std::atomic<float> fps(0.0f);
            uint32_t framesInWindow = 0;
            uint64_t windowStart = 0;
        }

        void OnFrame()
        {
            uint64_t now = dmTime::GetMonotonicTime();
            if(windowStart == 0)
            {
                windowStart = now;
                return;
            }

            framesInWindow++;

            uint64_t elapsed = now - windowStart;
            if(elapsed >= FPS_WINDOW_US)
            {
                fps.store((float)framesInWindow * (float)FPS_WINDOW_US / (float)elapsed);
                framesInWindow = 0;
                windowStart = now;
            }
        }

        gameanalytics::FPSTracker FpsTracker()
        {
            return []() -> float { return fps.load(); };
        }

        void ReleaseEngine()
        {
            delete wrapper;
            wrapper = 0;
        }
    }
}

#if defined(DM_PLATFORM_HTML5)

// Called from the page by the function library_gameanalytics.js installs.
// Exported by hand, the engine is built with hidden visibility.
extern "C" EMSCRIPTEN_KEEPALIVE void GameAnalyticsDefold_OnRemoteConfigsUpdated()
{
    // On html5 the wrapper Get() creates is always the web one.
    if(gameanalytics::defold::wrapper)
    {
        static_cast<gameanalytics::defold::GAWrapperDefoldWeb*>(gameanalytics::defold::wrapper)->NotifyRemoteConfigsUpdated();
    }
}

#endif // DM_PLATFORM_HTML5

#endif // supported platform
