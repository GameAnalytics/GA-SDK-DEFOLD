// Vendored copy of code/android/GAWrapperAndroid.cpp from the shared GameAnalytics C++ wrapper.
// Keep it in sync with the source, do not change it here.
//
// Defold compiles every source under src/ for every platform, so the whole
// file is wrapped in a platform guard. That is the only change made to it.

#if defined(DM_PLATFORM_ANDROID)

#include "GAWrapperAndroid.h"

#include <android/log.h>

namespace gameanalytics 
{
        constexpr const char* LOG_TAG = "GameAnalytics";
        constexpr const char* GAMEANALYTICS_CLASS_NAME = "com/gameanalytics/sdk/GameAnalytics";
        constexpr const char* RECEIPT_INFO_CLASS_NAME = "com/gameanalytics/sdk/events/ReceiptInfo";

        // Logs and clears a pending java exception. A JNI call made while one
        // is pending aborts the process, so every call into the SDK checks.
        // Returns true when there was one.
        static bool ClearJavaException(JNIEnv* env, const char* context)
        {
            if(!env || !env->ExceptionCheck())
            {
                return false;
            }

            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Java exception in %s ***", context);
            env->ExceptionDescribe();
            env->ExceptionClear();
            return true;
        }

        // GetStaticMethodID leaves a NoSuchMethodError pending when the method
        // or signature doesn't match the SDK, which the caller only logs.
        static jmethodID GetStaticMethod(JNIEnv* env, jclass jClass, const char* name, const char* signature)
        {
            jmethodID method = env->GetStaticMethodID(jClass, name, signature);
            if(!method)
            {
                ClearJavaException(env, name);
            }
            return method;
        }

        static FPSTracker androidFPSTracker;
        static std::function<void()> androidRemoteConfigListener;

        GAWrapperAndroid::GAWrapperAndroid():
            GAWrapper()
        {
        }

        GAWrapperAndroid::~GAWrapperAndroid()
        {
        }

        // -----------------------------------------------------------------
        // Default JNI context, used when the engine does not provide one.
        // See the ENGINE HOOK block in GAWrapperAndroid.h.
        // -----------------------------------------------------------------

        static JavaVM* javaVM        = nullptr;
        static jobject gameActivity  = nullptr;   // global ref
        static jobject classLoader   = nullptr;   // global ref
        static jmethodID loadClassMethod = nullptr;

        namespace
        {
            // Attaches the calling thread to the JavaVM on first use and
            // detaches it again when the thread exits, so the wrapper can be
            // called from any thread the engine happens to be on.
            struct ScopedJni
            {
                ScopedJni() = default;

                ~ScopedJni()
                {
                    if(attached && javaVM)
                    {
                        javaVM->DetachCurrentThread();
                    }

                    env = nullptr;
                    attached = false;
                }

                JNIEnv* Get()
                {
                    if(env)
                    {
                        return env;
                    }

                    if(!javaVM)
                    {
                        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "*** No JavaVM available, call gameanalytics::android::SetJavaVM() ***");
                        return nullptr;
                    }

                    void* tmp = nullptr;
                    if(javaVM->GetEnv(&tmp, JNI_VERSION_1_6) == JNI_OK)
                    {
                        env = static_cast<JNIEnv*>(tmp);
                        return env;
                    }

                    if(javaVM->AttachCurrentThread(&env, nullptr) == JNI_OK)
                    {
                        attached = true;
                        return env;
                    }

                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to attach thread to the JavaVM ***");
                    env = nullptr;
                    return nullptr;
                }

                private:

                    JNIEnv* env   = nullptr;
                    bool attached = false;
            };

            thread_local ScopedJni tlsJni;

            void CacheClassLoader(JNIEnv* env)
            {
                if(!env || classLoader)
                {
                    return;
                }

                // Any class of this app will do, it is only used to reach its loader.
                jclass anchor = env->FindClass(GAMEANALYTICS_CLASS_NAME);
                if(!anchor)
                {
                    env->ExceptionClear();
                    return;
                }

                jclass classClass = env->GetObjectClass(anchor);
                jmethodID getClassLoader = env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
                jobject loader = env->CallObjectMethod(anchor, getClassLoader);

                if(loader)
                {
                    jclass loaderClass = env->FindClass("java/lang/ClassLoader");
                    loadClassMethod = env->GetMethodID(loaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
                    classLoader = env->NewGlobalRef(loader);
                    env->DeleteLocalRef(loaderClass);
                    env->DeleteLocalRef(loader);
                }

                env->DeleteLocalRef(classClass);
                env->DeleteLocalRef(anchor);
            }
        }

        namespace android
        {
            void SetJavaVM(JavaVM* vm)
            {
                javaVM = vm;

                JNIEnv* env = nullptr;
                void* tmp = nullptr;
                if(vm && vm->GetEnv(&tmp, JNI_VERSION_1_6) == JNI_OK)
                {
                    env = static_cast<JNIEnv*>(tmp);
                    CacheClassLoader(env);
                }
            }

            void SetGameActivity(JNIEnv* env, jobject activity)
            {
                if(!env)
                {
                    return;
                }

                if(gameActivity)
                {
                    env->DeleteGlobalRef(gameActivity);
                    gameActivity = nullptr;
                }

                if(activity)
                {
                    gameActivity = env->NewGlobalRef(activity);
                }
            }

            void SetClassLoader(JNIEnv* env, jobject loader)
            {
                if(!env)
                {
                    return;
                }

                if(classLoader)
                {
                    env->DeleteGlobalRef(classLoader);
                    classLoader = nullptr;
                }

                if(loader)
                {
                    jclass loaderClass = env->FindClass("java/lang/ClassLoader");
                    loadClassMethod = env->GetMethodID(loaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
                    classLoader = env->NewGlobalRef(loader);
                    env->DeleteLocalRef(loaderClass);
                }
            }
        }

        JNIEnv* GAWrapperAndroid::GetJavaEnv()
        {
            return tlsJni.Get();
        }

        jclass GAWrapperAndroid::FindJavaClass(const char* className)
        {
            JNIEnv* env = GetJavaEnv();
            if(!env || !className)
            {
                return nullptr;
            }

            // FindClass only sees the app's classes on threads created by java,
            // so go through the cached ClassLoader whenever we have one.
            if(classLoader && loadClassMethod)
            {
                std::string dotted = className;
                for(char& c : dotted)
                {
                    if(c == '/')
                    {
                        c = '.';
                    }
                }

                jstring j_name = env->NewStringUTF(dotted.c_str());
                jclass found = static_cast<jclass>(env->CallObjectMethod(classLoader, loadClassMethod, j_name));
                env->DeleteLocalRef(j_name);

                if(env->ExceptionCheck())
                {
                    env->ExceptionClear();
                    found = nullptr;
                }

                if(found)
                {
                    return found;
                }
            }

            jclass found = env->FindClass(className);
            if(env->ExceptionCheck())
            {
                env->ExceptionClear();
                found = nullptr;
            }

            return found;
        }

        jobject GAWrapperAndroid::GetGameActivity()
        {
            return gameActivity;
        }

        jclass GAWrapperAndroid::GetGameAnalyticsClass()
        {
            return FindJavaClass(GAMEANALYTICS_CLASS_NAME);
        }

        std::string GAWrapperAndroid::ConvertJString(jstring s)
        {
            JNIEnv* env = GetJavaEnv();
            if(env && s)
            {
                const char* cstr   = env->GetStringUTFChars(s, 0);
                std::string result = cstr;
                env->ReleaseStringUTFChars(s, cstr);
                return result;
            }
            else
            {
                return "";
            }
        }

        void GAWrapperAndroid::SetAvailableCustomDimensions01(const std::vector<std::string>& list)
        {
            constexpr const char* methodName = "configureAvailableCustomDimensions01";

            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "([Ljava/lang/String;)V");

                if (jMethod)
                {
                    jstring str;
                    jsize len = list.size();
                    jobjectArray j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (std::string const& s : list)
                    {
                        str = env->NewStringUTF(s.c_str());
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    ClearJavaException(env, methodName);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetAvailableCustomDimensions02(const std::vector<std::string>& list)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureAvailableCustomDimensions02";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (std::string const& s : list)
                    {
                        str = env->NewStringUTF(s.c_str());
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    ClearJavaException(env, methodName);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetAvailableCustomDimensions03(const std::vector<std::string>& list)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureAvailableCustomDimensions03";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (std::string const& s : list)
                    {
                        str = env->NewStringUTF(s.c_str());
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    ClearJavaException(env, methodName);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetAvailableResourceCurrencies(const std::vector<std::string>& list)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureAvailableResourceCurrencies";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (std::string const& s : list)
                    {
                        str = env->NewStringUTF(s.c_str());
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    ClearJavaException(env, methodName);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetAvailableResourceItemTypes(const std::vector<std::string>& list)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            constexpr const char* methodName = "configureAvailableResourceItemTypes";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (std::string const& s : list)
                    {
                        str = env->NewStringUTF(s.c_str());
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    ClearJavaException(env, methodName);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetBuild(std::string const& build)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureBuild";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(build.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetAutoDetectAppVersion(bool flag)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            const char* methodName = "configureAutoDetectAppVersion";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        std::string GAWrapperAndroid::CallStringMethod(std::string const& name)
        {
            constexpr const char* signature = "()Ljava/lang/String;";

            std::string result = "";

            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            if(env && jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, name.c_str(), signature);
                if(jMethod)
                {
                    jstring jString = (jstring)env->CallStaticObjectMethod(jClass, jMethod);
                    if(!ClearJavaException(env, name.c_str()) && jString)
                    {
                        result = ConvertJString(jString);
                        env->DeleteLocalRef(jString);
                    }
                }

                env->DeleteLocalRef(jClass);
            }

            return result;
        }

        std::string GAWrapperAndroid::GetUserId()
        {
            return CallStringMethod("getUserId");
        }

        std::string GAWrapperAndroid::GetExternalUserId()
        {
            return CallStringMethod("getExternalUserId");
        }

        void GAWrapperAndroid::SetCustomUserId(std::string const& userId)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureUserId";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(userId.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetExternalUserId(std::string const& userId)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureExternalUserId";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(userId.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetSDKVersion(std::string const& gameEngineSdkVersion)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureSdkGameEngineVersion";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(gameEngineSdkVersion.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetGameEngineVersion(std::string const& gameEngineSdkVersion)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "configureGameEngineVersion";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(gameEngineSdkVersion.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::Initialize(std::string const& gameKey, std::string const& gameSecret)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "initialize";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V");

                if(jMethod)
                {
                    jobject activity = GetGameActivity();
                    jstring j_gameKey = env->NewStringUTF(gameKey.c_str());
                    jstring j_gameSecret = env->NewStringUTF(gameSecret.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, activity, j_gameKey, j_gameSecret);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_gameKey);
                    env->DeleteLocalRef(j_gameSecret);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddBusinessEvent(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const&cartType, std::string const& receipt, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addBusinessEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_currency = env->NewStringUTF(currency.c_str());
                    jstring j_itemType = env->NewStringUTF(itemType.c_str());
                    jstring j_itemId = env->NewStringUTF(itemId.c_str());
                    jstring j_cartType = env->NewStringUTF(cartType.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_currency, amount, j_itemType, j_itemId, j_cartType, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_currency);
                    env->DeleteLocalRef(j_itemType);
                    env->DeleteLocalRef(j_itemId);
                    env->DeleteLocalRef(j_cartType);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddBusinessEventWithReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType,
                                             std::string const& receipt, std::string const& store, std::string const& signature, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addBusinessEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_currency = env->NewStringUTF(currency.c_str());
                    jstring j_itemType = env->NewStringUTF(itemType.c_str());
                    jstring j_itemId = env->NewStringUTF(itemId.c_str());
                    jstring j_cartType = env->NewStringUTF(cartType.c_str());
                    jstring j_receipt = env->NewStringUTF(receipt.c_str());
                    jstring j_store = env->NewStringUTF(store.c_str());
                    jstring j_signature = env->NewStringUTF(signature.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_currency, amount, j_itemType, j_itemId, j_cartType, j_receipt, j_store, j_signature, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_currency);
                    env->DeleteLocalRef(j_itemType);
                    env->DeleteLocalRef(j_itemId);
                    env->DeleteLocalRef(j_cartType);
                    env->DeleteLocalRef(j_receipt);
                    env->DeleteLocalRef(j_store);
                    env->DeleteLocalRef(j_signature);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddBusinessEventWithValidation(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType,
                                             std::string const& store, std::string const& productId, std::string const& purchaseToken, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addBusinessEvent";

            if(jClass)
            {
                jclass jReceiptClass = FindJavaClass(RECEIPT_INFO_CLASS_NAME);
                if(!jReceiptClass)
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", RECEIPT_INFO_CLASS_NAME);
                    env->DeleteLocalRef(jClass);
                    return;
                }

                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Lcom/gameanalytics/sdk/events/ReceiptInfo;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jmethodID jReceiptInit = env->GetMethodID(jReceiptClass, "<init>", "()V");
                    jfieldID jStoreField = env->GetFieldID(jReceiptClass, "store", "Ljava/lang/String;");
                    jfieldID jProductIdField = env->GetFieldID(jReceiptClass, "productId", "Ljava/lang/String;");
                    jfieldID jPurchaseTokenField = env->GetFieldID(jReceiptClass, "purchaseToken", "Ljava/lang/String;");

                    jobject j_receiptInfo = env->NewObject(jReceiptClass, jReceiptInit);

                    // keep the java default ("google_play") unless a store is explicitly given
                    jstring j_store = nullptr;
                    if(!store.empty())
                    {
                        j_store = env->NewStringUTF(store.c_str());
                        env->SetObjectField(j_receiptInfo, jStoreField, j_store);
                    }

                    jstring j_productId = env->NewStringUTF(productId.c_str());
                    jstring j_purchaseToken = env->NewStringUTF(purchaseToken.c_str());
                    env->SetObjectField(j_receiptInfo, jProductIdField, j_productId);
                    env->SetObjectField(j_receiptInfo, jPurchaseTokenField, j_purchaseToken);

                    jstring j_currency = env->NewStringUTF(currency.c_str());
                    jstring j_itemType = env->NewStringUTF(itemType.c_str());
                    jstring j_itemId = env->NewStringUTF(itemId.c_str());
                    jstring j_cartType = env->NewStringUTF(cartType.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());

                    env->CallStaticVoidMethod(jClass, jMethod, j_currency, amount, j_itemType, j_itemId, j_cartType, j_receiptInfo, j_fields, mergeFields);
                    ClearJavaException(env, methodName);

                    env->DeleteLocalRef(j_currency);
                    env->DeleteLocalRef(j_itemType);
                    env->DeleteLocalRef(j_itemId);
                    env->DeleteLocalRef(j_cartType);
                    env->DeleteLocalRef(j_fields);
                    if(j_store)
                    {
                        env->DeleteLocalRef(j_store);
                    }
                    env->DeleteLocalRef(j_productId);
                    env->DeleteLocalRef(j_purchaseToken);
                    env->DeleteLocalRef(j_receiptInfo);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jReceiptClass);
                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddBusinessEventAndAutoFetchReceipt(std::string const& currency, int amount, std::string const& itemType, std::string const& itemId, std::string const& cartType, std::string const& fields, bool mergeFields)
        {
            __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "Cannot auto fetch the receipt, sending regular business event...");
            return AddBusinessEvent(currency, amount, itemType, itemId, cartType, "", fields, mergeFields);
        }

        void GAWrapperAndroid::AddResourceEvent(EGAResourceFlowType flowType, std::string const& currency, float amount, std::string const& itemType, std::string const& itemId, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addResourceEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(ILjava/lang/String;FLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_currency = env->NewStringUTF(currency.c_str());
                    jstring j_itemType = env->NewStringUTF(itemType.c_str());
                    jstring j_itemId = env->NewStringUTF(itemId.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(flowType), j_currency, amount, j_itemType, j_itemId, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_currency);
                    env->DeleteLocalRef(j_itemType);
                    env->DeleteLocalRef(j_itemId);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddProgressionEvent(EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addProgressionEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_progression01 = env->NewStringUTF(progression01.c_str());
                    jstring j_progression02 = env->NewStringUTF(progression02.c_str());
                    jstring j_progression03 = env->NewStringUTF(progression03.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(progressionStatus), j_progression01, j_progression02, j_progression03, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_progression01);
                    env->DeleteLocalRef(j_progression02);
                    env->DeleteLocalRef(j_progression03);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddProgressionEventWithScore(EGAProgressionStatus progressionStatus, std::string const& progression01, std::string const& progression02, std::string const& progression03, int score, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addProgressionEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;DLjava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_progression01 = env->NewStringUTF(progression01.c_str());
                    jstring j_progression02 = env->NewStringUTF(progression02.c_str());
                    jstring j_progression03 = env->NewStringUTF(progression03.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(progressionStatus), j_progression01, j_progression02, j_progression03, (double)score, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_progression01);
                    env->DeleteLocalRef(j_progression02);
                    env->DeleteLocalRef(j_progression03);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddDesignEvent(std::string const& eventId, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            const char* methodName = "addDesignEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_eventId = env->NewStringUTF(eventId.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_eventId, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_eventId);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddDesignEventWithValue(std::string const& eventId, float value, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addDesignEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;DLjava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_eventId = env->NewStringUTF(eventId.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_eventId, value, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_eventId);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddErrorEvent(EGAErrorSeverity severity, std::string const& message, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addErrorEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(ILjava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_message = env->NewStringUTF(message.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(severity), j_message, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_message);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddAdEvent(EGAAdAction action, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addAdEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_adSdkName = env->NewStringUTF(adSdkName.c_str());
                    jstring j_adPlacement = env->NewStringUTF(adPlacement.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(action), static_cast<int>(adType), j_adSdkName, j_adPlacement, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_adSdkName);
                    env->DeleteLocalRef(j_adPlacement);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddAdEventWithDuration(EGAAdAction action, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, int64_t duration, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addAdEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(IILjava/lang/String;Ljava/lang/String;JLjava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_adSdkName = env->NewStringUTF(adSdkName.c_str());
                    jstring j_adPlacement = env->NewStringUTF(adPlacement.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(action), static_cast<int>(adType), j_adSdkName, j_adPlacement, duration, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_adSdkName);
                    env->DeleteLocalRef(j_adPlacement);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::AddAdEventWithReason(EGAAdAction action, EGAAdType adType, std::string const& adSdkName, std::string const& adPlacement, EGAAdError noAdReason, std::string const& fields, bool mergeFields)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "addAdEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(IILjava/lang/String;Ljava/lang/String;ILjava/lang/String;Z)V");

                if(jMethod)
                {
                    jstring j_adSdkName = env->NewStringUTF(adSdkName.c_str());
                    jstring j_adPlacement = env->NewStringUTF(adPlacement.c_str());
                    jstring j_fields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, static_cast<int>(action), static_cast<int>(adType), j_adSdkName, j_adPlacement, noAdReason, j_fields, mergeFields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_adSdkName);
                    env->DeleteLocalRef(j_adPlacement);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetInfoLog(bool flag)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setEnabledInfoLog";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetVerboseLog(bool flag)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setEnabledVerboseLog";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetEnabledManualSessionHandling(bool flag)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setEnabledManualSessionHandling";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetEnabledErrorReporting(bool flag)
        {
            JNIEnv *env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char *methodName = "setEnabledErrorReporting";

            if (jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if (jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetEnabledEventSubmission(bool flag, bool doCacheLocally)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setEnabledEventSubmission";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(ZZ)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag, doCacheLocally);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetGlobalCustomEventFields(std::string const& fields)
        {
            constexpr const char* methodName = "setGlobalCustomEventFields";

            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            if(env && jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring jfields = env->NewStringUTF(fields.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, jfields);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(jfields);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }
                
                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetCustomDimension01(std::string const& customDimension)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setCustomDimension01";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_customDimension = env->NewStringUTF(customDimension.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_customDimension);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_customDimension);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetCustomDimension02(std::string const& customDimension)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setCustomDimension02";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_customDimension = env->NewStringUTF(customDimension.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_customDimension);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_customDimension);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::SetCustomDimension03(std::string const& customDimension)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setCustomDimension03";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_customDimension = env->NewStringUTF(customDimension.c_str());
                    env->CallStaticVoidMethod(jClass, jMethod, j_customDimension);
                    ClearJavaException(env, methodName);
                    env->DeleteLocalRef(j_customDimension);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::StartSession()
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "startSession";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::EndSession()
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "endSession";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()V");
                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        double GAWrapperAndroid::GetRemoteConfigsValueAsNumber(std::string const& key, double defaultValue) {
            return defaultValue;
        }

        std::string GAWrapperAndroid::GetRemoteConfigsValueAsJson(std::string const& key) {
            return "";
        }

        void GAWrapperAndroid::RegisterRemoteConfigListener(RemoteConfigListener listener)
        {
            androidRemoteConfigListener = [this, listener]()
            {
                if(listener)
                {
                    listener(GetRemoteConfigsContentAsString());
                }
            };

            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            constexpr const char* methodName = "addRemoteConfigsListener";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Lcom/gameanalytics/sdk/IRemoteConfigsListener;)V");

                if(jMethod)
                {
                    jclass nativeRemoteConfigsListenerClass = FindJavaClass("com/gameanalytics/sdk/NativeRemoteConfigsListener");

                    jmethodID ctor = env->GetMethodID(nativeRemoteConfigsListenerClass, "<init>", "()V");
                    jobject javaListener = env->NewObject(nativeRemoteConfigsListenerClass, ctor);

                    env->CallStaticVoidMethod(jClass, jMethod, javaListener);
                    ClearJavaException(env, methodName);

                    env->DeleteLocalRef(javaListener);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        std::string GAWrapperAndroid::GetRemoteConfigsValueAsString(std::string const& key, std::string const& defaultValue)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "getRemoteConfigsValueAsString";
            std::string result;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_key = env->NewStringUTF(key.c_str());
                    jstring j_defaultValue = env->NewStringUTF(defaultValue.c_str());
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod, j_key, j_defaultValue);
                    if(!ClearJavaException(env, methodName) && j_s)
                    {
                        result = ConvertJString(j_s);
                        env->DeleteLocalRef(j_s);
                    }
                    env->DeleteLocalRef(j_key);
                    env->DeleteLocalRef(j_defaultValue);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        bool GAWrapperAndroid::IsRemoteConfigsReady()
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "isRemoteConfigsReady";
            bool result = false;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()Z");

                if(jMethod)
                {
                    result = env->CallStaticBooleanMethod(jClass, jMethod);
                    if(ClearJavaException(env, methodName))
                    {
                        result = false;
                    }
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        std::string GAWrapperAndroid::GetRemoteConfigsContentAsString()
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "getRemoteConfigsContentAsString";
            std::string result;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod);
                    if(!ClearJavaException(env, methodName) && j_s)
                    {
                        result = ConvertJString(j_s);
                        env->DeleteLocalRef(j_s);
                    }
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        std::string GAWrapperAndroid::GetABTestingId()
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "getABTestingId";
            std::string result;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod);
                    if(!ClearJavaException(env, methodName) && j_s)
                    {
                        result = ConvertJString(j_s);
                        env->DeleteLocalRef(j_s);
                    }
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        std::string GAWrapperAndroid::GetABTestingVariantId()
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            constexpr const char* methodName = "getABTestingVariantId";
            std::string result;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod);
                    if(!ClearJavaException(env, methodName) && j_s)
                    {
                        result = ConvertJString(j_s);
                        env->DeleteLocalRef(j_s);
                    }
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        void GAWrapperAndroid::EnableSDKInitEvent(bool value)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "enableSDKInitEvent";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, value);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::EnableFpsHistogram(FPSTracker tracker, bool value)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();

            constexpr const char* methodName = "enableFpsHistogram";
            constexpr const char* methodNameCustomTracker = "setCustomFPSTracker";

            androidFPSTracker = tracker;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                jmethodID jTrackerMethod = GetStaticMethod(env, jClass, methodNameCustomTracker, "(Lcom/gameanalytics/sdk/health/IFpsTracker;)V");
                jclass nativeFPSTrackerClass = FindJavaClass("com/gameanalytics/sdk/health/NativeFpsTracker");

                jmethodID ctor = env->GetMethodID(nativeFPSTrackerClass, "<init>", "()V");
                jobject javaTracker = env->NewObject(nativeFPSTrackerClass, ctor);

                if(jMethod && jTrackerMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, value);
                    ClearJavaException(env, methodName);
                    if(value)
                    {
                        env->CallStaticVoidMethod(jClass, jTrackerMethod, javaTracker);
                        ClearJavaException(env, methodNameCustomTracker);
                    }
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(javaTracker);
                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::EnableMemoryHistogram(bool value)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "enableMemoryHistogram";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, value);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::EnableHealthHardwareInfo(bool value)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "enableHealthHardwareInfo";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, value);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::EnableAdvertisingId(bool value)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            constexpr const char* methodName = "setGAIDTracking";

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, value);
                    ClearJavaException(env, methodName);
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void GAWrapperAndroid::OnQuit()
        {
            return;
        }

        void GAWrapperAndroid::SetWritablePath(std::string const& path)
        {
            (void)path;
        }

        int64_t GAWrapperAndroid::CallLongMethod(const char* methodName)
        {
            JNIEnv* env = GetJavaEnv();
            jclass jClass = GetGameAnalyticsClass();
            int64_t result = 0;

            if(jClass)
            {
                jmethodID jMethod = GetStaticMethod(env, jClass, methodName, "()J");

                if(jMethod)
                {
                    jlong value = env->CallStaticLongMethod(jClass, jMethod);
                    if(!ClearJavaException(env, methodName))
                    {
                        result = static_cast<int64_t>(value);
                    }
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find method %s ***", methodName);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        int64_t GAWrapperAndroid::GetElapsedSessionTime()
        {
            return CallLongMethod("getElapsedSessionTime");
        }

        int64_t GAWrapperAndroid::GetElapsedTimeForPreviousSession()
        {
            // The android SDK has no previous session time.
            return 0;
        }

        int64_t GAWrapperAndroid::GetElapsedTimeFromAllSessions()
        {
            return CallLongMethod("getElapsedTimeFromAllSessions");
        }
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_gameanalytics_sdk_health_NativeFpsTracker_getFPSNative(JNIEnv *env, jobject _this) 
{
    return gameanalytics::androidFPSTracker ? gameanalytics::androidFPSTracker() : 0.0f;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gameanalytics_sdk_NativeRemoteConfigsListener_onRemoteConfigsUpdatedNative(JNIEnv *env, jobject _this)
{
    if(gameanalytics::androidRemoteConfigListener)
    {
        gameanalytics::androidRemoteConfigListener();
    }
}

// Called automatically when the java side loads the native library. This is
// what makes the default JNI context work without any engine involvement.
//
// ENGINE HOOK: game engines ship their own JNI_OnLoad and there can only be
// one per shared library. Build with GA_NO_JNI_ONLOAD defined when the SDK is
// linked into such an engine, and call gameanalytics::android::SetJavaVM()
// from the engine's own JNI_OnLoad instead.
#ifndef GA_NO_JNI_ONLOAD

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;
    gameanalytics::android::SetJavaVM(vm);
    return JNI_VERSION_1_6;
}

#endif // GA_NO_JNI_ONLOAD

#endif // DM_PLATFORM_ANDROID
