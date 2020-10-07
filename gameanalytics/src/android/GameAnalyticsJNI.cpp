#if defined(DM_PLATFORM_ANDROID)

#include "android/GameAnalyticsJNI.h"
#include <dmsdk/graphics/graphics_native.h>
#define DLIB_LOG_DOMAIN "GameAnalytics"
#include <dmsdk/dlib/log.h>
#include <string.h>
#include <stdio.h>

#define GAMEANALYTICS_CLASS_NAME "com/gameanalytics/sdk/GameAnalytics"
#define GAMEANALYTICS_IMEI_CLASS_NAME "com/gameanalytics/sdk/imei/GAImei"
#define GAJNI_CLASS_NAME "com/gameanalytics/sdk/GAJNI"

namespace gameanalytics {

    static bool GetJniEnv(JNIEnv **env)
    {
        bool did_attach_thread = false;
        *env = nullptr;

        JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
        auto get_env_result = vm->GetEnv((void**)env, JNI_VERSION_1_6);

        if (get_env_result == JNI_EDETACHED)
        {
            if (vm->AttachCurrentThread(env, NULL) == JNI_OK)
            {
                did_attach_thread = true;
            }
            else
            {
                // Failed to attach thread. Throw an exception if you want to.
            }
        }
        else if (get_env_result == JNI_EVERSION)
        {
            // Unsupported JNI version. Throw an exception if you want to.
        }
        return did_attach_thread;
    }

    static bool Detach(JNIEnv* env)
    {
        bool exception = (bool) env->ExceptionCheck();
        env->ExceptionClear();
        JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
        vm->DetachCurrentThread();
        return !exception;
    }

    struct AttachScope
    {
        JNIEnv* m_Env;
        bool m_did_attach;
        AttachScope()
        {
            m_did_attach = GetJniEnv(&m_Env);
        }
        ~AttachScope()
        {
            if(m_did_attach)
            {
                Detach(m_Env);
            }
        }
    };

    dmScript::LuaCallbackInfo* _remote_configs_listener;

    static jclass GetClass(JNIEnv* env, const char* classname)
    {
        jclass activity_class = env->FindClass("android/app/NativeActivity");
        jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
        jobject cls = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
        jclass class_loader = env->FindClass("java/lang/ClassLoader");
        jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

        jstring str_class_name = env->NewStringUTF(classname);
        jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
        env->DeleteLocalRef(str_class_name);
        return outcls;
    }

#ifdef __cplusplus
    extern "C"
    {
#endif
        void jni_configureAvailableCustomDimensions01(const std::vector<CharArray>& list)
        {
            // get the java environment so we can register the native function with the java call
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureAvailableCustomDimensions01";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (CharArray s : list)
                    {
                        str = env->NewStringUTF(s.array);
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureAvailableCustomDimensions02(const std::vector<CharArray>& list)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureAvailableCustomDimensions02";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (CharArray s : list)
                    {
                        str = env->NewStringUTF(s.array);
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureAvailableCustomDimensions03(const std::vector<CharArray>& list)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureAvailableCustomDimensions03";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (CharArray s : list)
                    {
                        str = env->NewStringUTF(s.array);
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureAvailableResourceCurrencies(const std::vector<CharArray>& list)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureAvailableResourceCurrencies";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (CharArray s : list)
                    {
                        str = env->NewStringUTF(s.array);
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureAvailableResourceItemTypes(const std::vector<CharArray>& list)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureAvailableResourceItemTypes";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "([Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring str;
                    jobjectArray j_array = 0;
                    jsize len = list.size();
                    j_array = env->NewObjectArray(len, env->FindClass("java/lang/String"), 0);

                    int i = 0;
                    for (CharArray s : list)
                    {
                        str = env->NewStringUTF(s.array);
                        env->SetObjectArrayElement(j_array, i, str);
                        ++i;
                    }

                    env->CallStaticVoidMethod(jClass, jMethod, j_array);
                    for (i = 0; i < list.size(); ++i)
                    {
                        jobject localRef = env->GetObjectArrayElement(j_array, i);
                        env->DeleteLocalRef(localRef);
                    }
                    env->DeleteLocalRef(j_array);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureBuild(const char *build)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureBuild";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(build);
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureAutoDetectAppVersion(bool flag)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureAutoDetectAppVersion";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureUserId(const char *userId)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureUserId";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(userId);
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureSdkGameEngineVersion(const char *gameEngineSdkVersion)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureSdkGameEngineVersion";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(gameEngineSdkVersion);
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_configureGameEngineVersion(const char *gameEngineVersion)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "configureGameEngineVersion";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_string = env->NewStringUTF(gameEngineVersion);
                    env->CallStaticVoidMethod(jClass, jMethod, j_string);
                    env->DeleteLocalRef(j_string);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_initialize(const char *gameKey, const char *gameSecret, bool use_imei_android)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "initializeWithGameKey";

            if(use_imei_android)
            {
                jclass jClass_imei = GetClass(env, GAMEANALYTICS_IMEI_CLASS_NAME);
                const char* strMethod_imei = "readImei";

                if(jClass_imei)
                {
                    jmethodID jMethod_imei = env->GetStaticMethodID(jClass_imei, strMethod_imei, "()V");

                    if(jMethod_imei)
                    {
                        env->CallStaticVoidMethod(jClass_imei, jMethod_imei);
                    }
                    else
                    {
                        dmLogError("*** Failed to find method %s ***", strMethod_imei);
                    }

                    env->DeleteLocalRef(jClass_imei);
                }
                else
                {
                    dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_IMEI_CLASS_NAME);
                }
            }

            // initialize remote configs listener
            {
                jclass jClass_gajni = GetClass(env, GAJNI_CLASS_NAME);
                const char* strMethod_gajni = "initialize";

                if(jClass_gajni)
                {
                    jmethodID jMethod_gajni = env->GetStaticMethodID(jClass_gajni, strMethod_gajni, "()V");

                    if(jMethod_gajni)
                    {
                        env->CallStaticVoidMethod(jClass_gajni, jMethod_gajni);
                    }
                    else
                    {
                        dmLogError("*** Failed to find method %s ***", strMethod_gajni);
                    }

                    env->DeleteLocalRef(jClass_gajni);
                }
                else
                {
                    dmLogError("*** Failed to find class %s ***", GAJNI_CLASS_NAME);
                }
            }

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V");

                if(jMethod)
                {
                    jobject activity = dmGraphics::GetNativeAndroidActivity();
                    jstring j_gameKey = env->NewStringUTF(gameKey);
                    jstring j_gameSecret = env->NewStringUTF(gameSecret);
                    env->CallStaticVoidMethod(jClass, jMethod, activity, j_gameKey, j_gameSecret);
                    env->DeleteLocalRef(j_gameKey);
                    env->DeleteLocalRef(j_gameSecret);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addBusinessEventWithCurrency";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_currency = env->NewStringUTF(currency);
                    jstring j_itemType = env->NewStringUTF(itemType);
                    jstring j_itemId = env->NewStringUTF(itemId);
                    jstring j_cartType = env->NewStringUTF(cartType);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, j_currency, amount, j_itemType, j_itemId, j_cartType/*, j_fields*/);
                    env->DeleteLocalRef(j_currency);
                    env->DeleteLocalRef(j_itemType);
                    env->DeleteLocalRef(j_itemId);
                    env->DeleteLocalRef(j_cartType);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addBusinessEventWithReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *store, const char *signature, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addBusinessEventWithCurrency";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_currency = env->NewStringUTF(currency);
                    jstring j_itemType = env->NewStringUTF(itemType);
                    jstring j_itemId = env->NewStringUTF(itemId);
                    jstring j_cartType = env->NewStringUTF(cartType);
                    jstring j_receipt = env->NewStringUTF(receipt);
                    jstring j_store = env->NewStringUTF(store);
                    jstring j_signature = env->NewStringUTF(signature);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, j_currency, amount, j_itemType, j_itemId, j_cartType, j_receipt, j_store, j_signature/*, j_fields*/);
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
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addResourceEvent(int flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addResourceEventWithFlowType";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(ILjava/lang/String;FLjava/lang/String;Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_currency = env->NewStringUTF(currency);
                    jstring j_itemType = env->NewStringUTF(itemType);
                    jstring j_itemId = env->NewStringUTF(itemId);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, flowType, j_currency, amount, j_itemType, j_itemId/*, j_fields*/);
                    env->DeleteLocalRef(j_currency);
                    env->DeleteLocalRef(j_itemType);
                    env->DeleteLocalRef(j_itemId);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addProgressionEvent(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addProgressionEventWithProgressionStatus";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_progression01 = env->NewStringUTF(progression01);
                    jstring j_progression02 = env->NewStringUTF(progression02);
                    jstring j_progression03 = env->NewStringUTF(progression03);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, progressionStatus, j_progression01, j_progression02, j_progression03/*, j_fields*/);
                    env->DeleteLocalRef(j_progression01);
                    env->DeleteLocalRef(j_progression02);
                    env->DeleteLocalRef(j_progression03);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addProgressionEventWithScore(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addProgressionEventWithProgressionStatus";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;D)V");

                if(jMethod)
                {
                    jstring j_progression01 = env->NewStringUTF(progression01);
                    jstring j_progression02 = env->NewStringUTF(progression02);
                    jstring j_progression03 = env->NewStringUTF(progression03);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, progressionStatus, j_progression01, j_progression02, j_progression03, (double)score/*, j_fields*/);
                    env->DeleteLocalRef(j_progression01);
                    env->DeleteLocalRef(j_progression02);
                    env->DeleteLocalRef(j_progression03);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addDesignEvent(const char *eventId, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addDesignEventWithEventId";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_eventId = env->NewStringUTF(eventId);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, j_eventId/*, j_fields*/);
                    env->DeleteLocalRef(j_eventId);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addDesignEventWithValue(const char *eventId, float value, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addDesignEventWithEventId";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;D)V");

                if(jMethod)
                {
                    jstring j_eventId = env->NewStringUTF(eventId);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, j_eventId, value/*, j_fields*/);
                    env->DeleteLocalRef(j_eventId);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addErrorEvent(int severity, const char *message, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "addErrorEventWithSeverity";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(ILjava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_message = env->NewStringUTF(message);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, severity, j_message/*, j_fields*/);
                    env->DeleteLocalRef(j_message);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addAdEvent(int adAction, int adType, const char *adSdkName, const char *adPlacement, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv *env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char *strMethod = "addAdEvent";

            if (jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(IILjava/lang/String;Ljava/lang/String;)V");

                if (jMethod)
                {
                    jstring j_adSdkName = env->NewStringUTF(adSdkName);
                    jstring j_adPlacement = env->NewStringUTF(adPlacement);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, adAction, adType, j_adSdkName, j_adPlacement /*, j_fields*/);
                    env->DeleteLocalRef(j_adSdkName);
                    env->DeleteLocalRef(j_adPlacement);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addAdEventWithDuration(int adAction, int adType, const char *adSdkName, const char *adPlacement, int duration, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv *env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char *strMethod = "addAdEvent";

            if (jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(IILjava/lang/String;Ljava/lang/String;J)V");

                if (jMethod)
                {
                    jstring j_adSdkName = env->NewStringUTF(adSdkName);
                    jstring j_adPlacement = env->NewStringUTF(adPlacement);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, adAction, adType, j_adSdkName, j_adPlacement, duration /*, j_fields*/);
                    env->DeleteLocalRef(j_adSdkName);
                    env->DeleteLocalRef(j_adPlacement);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_addAdEventWithNoAdReason(int adAction, int adType, const char *adSdkName, const char *adPlacement, int noAdReason, const char *fields)
        {
            AttachScope attachscope;
            JNIEnv *env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char *strMethod = "addAdEvent";

            if (jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(IILjava/lang/String;Ljava/lang/String;I)V");

                if (jMethod)
                {
                    jstring j_adSdkName = env->NewStringUTF(adSdkName);
                    jstring j_adPlacement = env->NewStringUTF(adPlacement);
                    jstring j_fields = env->NewStringUTF(fields);
                    env->CallStaticVoidMethod(jClass, jMethod, adAction, adType, j_adSdkName, j_adPlacement, noAdReason /*, j_fields*/);
                    env->DeleteLocalRef(j_adSdkName);
                    env->DeleteLocalRef(j_adPlacement);
                    env->DeleteLocalRef(j_fields);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setEnabledInfoLog(bool flag)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setEnabledInfoLog";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setEnabledVerboseLog(bool flag)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setEnabledVerboseLog";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setEnabledManualSessionHandling(bool flag)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setEnabledManualSessionHandling";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setEnabledEventSubmission(bool flag)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setEnabledEventSubmission";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setEnabledErrorReporting(bool flag)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setEnabledErrorReporting";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Z)V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod, flag);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setCustomDimension01(const char *customDimension)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setCustomDimension01";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_customDimension = env->NewStringUTF(customDimension);
                    env->CallStaticVoidMethod(jClass, jMethod, j_customDimension);
                    env->DeleteLocalRef(j_customDimension);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setCustomDimension02(const char *customDimension)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setCustomDimension02";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_customDimension = env->NewStringUTF(customDimension);
                    env->CallStaticVoidMethod(jClass, jMethod, j_customDimension);
                    env->DeleteLocalRef(j_customDimension);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_setCustomDimension03(const char *customDimension)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "setCustomDimension03";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)V");

                if(jMethod)
                {
                    jstring j_customDimension = env->NewStringUTF(customDimension);
                    env->CallStaticVoidMethod(jClass, jMethod, j_customDimension);
                    env->DeleteLocalRef(j_customDimension);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_startSession()
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "startSession";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "()V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        void jni_endSession()
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "endSession";

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "()V");

                if(jMethod)
                {
                    env->CallStaticVoidMethod(jClass, jMethod);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }
        }

        std::vector<char> jni_getRemoteConfigsValueAsString(const char *key)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "getRemoteConfigsValueAsString";
            std::vector<char> resultVector;

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;)Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_key = env->NewStringUTF(key);
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod, j_key);
                    const char* s = env->GetStringUTFChars(j_s, 0);
                    size_t size = strlen(s);
                    for(size_t i = 0; i < size; ++i)
                    {
                        resultVector.push_back(s[i]);
                    }
                    env->ReleaseStringUTFChars(j_s, s);
                    env->DeleteLocalRef(j_key);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            resultVector.push_back('\0');
            return resultVector;
        }

        std::vector<char> jni_getRemoteConfigsValueAsStringWithDefaultValue(const char *key, const char *defaultValue)
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "getRemoteConfigsValueAsString";
            std::vector<char> resultVector;

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_key = env->NewStringUTF(key);
                    jstring j_defaultValue = env->NewStringUTF(defaultValue);
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod, j_key, j_defaultValue);
                    const char* s = env->GetStringUTFChars(j_s, 0);
                    size_t size = strlen(s);
                    for(size_t i = 0; i < size; ++i)
                    {
                        resultVector.push_back(s[i]);
                    }
                    env->ReleaseStringUTFChars(j_s, s);
                    env->DeleteLocalRef(j_key);
                    env->DeleteLocalRef(j_defaultValue);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            resultVector.push_back('\0');
            return resultVector;
        }

        bool jni_isRemoteConfigsReady()
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "isRemoteConfigsReady";
            bool result = false;

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "()Z");

                if(jMethod)
                {
                    result = env->CallStaticBooleanMethod(jClass, jMethod);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            return result;
        }

        void jni_setRemoteConfigsListener(dmScript::LuaCallbackInfo* listener)
        {
            _remote_configs_listener = listener;
        }

        JNIEXPORT void JNICALL Java_com_gameanalytics_sdk_GAJNI_onRemoteConfigsUpdatedNative(JNIEnv* env, jobject)
        {
            if(!_remote_configs_listener)
            {
                dmLogWarning("Received remote configs update but no listener was set!");
                return;
            }

            lua_State* L = dmScript::GetCallbackLuaContext(_remote_configs_listener);
            DM_LUA_STACK_CHECK(L, 0);

            if (!dmScript::SetupCallback(_remote_configs_listener))
            {
                dmLogWarning("SetupCallback failed for remote configs");
                return;
            }

            int ret = dmScript::PCall(L, 1, 0);
            dmScript::TeardownCallback(_remote_configs_listener);
        }

        std::vector<char> jni_getRemoteConfigsContentAsString()
        {
            AttachScope attachscope;
            JNIEnv* env = attachscope.m_Env;
            jclass jClass = GetClass(env, GAMEANALYTICS_CLASS_NAME);
            const char* strMethod = "getRemoteConfigsContentAsString";
            std::vector<char> resultVector;

            if(jClass)
            {
                jmethodID jMethod = env->GetStaticMethodID(jClass, strMethod, "()Ljava/lang/String;");

                if(jMethod)
                {
                    jstring j_s = (jstring)env->CallStaticObjectMethod(jClass, jMethod);
                    const char* s = env->GetStringUTFChars(j_s, 0);
                    size_t size = strlen(s);
                    for(size_t i = 0; i < size; ++i)
                    {
                        resultVector.push_back(s[i]);
                    }
                    env->ReleaseStringUTFChars(j_s, s);
                }
                else
                {
                    dmLogError("*** Failed to find method %s ***", strMethod);
                }

                env->DeleteLocalRef(jClass);
            }
            else
            {
                dmLogError("*** Failed to find class %s ***", GAMEANALYTICS_CLASS_NAME);
            }

            resultVector.push_back('\0');
            return resultVector;
        }
#ifdef __cplusplus
    }
#endif
}

#endif
