package com.gameanalytics.sdk;

import com.gameanalytics.sdk.logging.GALogger;
/*import android.os.Handler;
import android.os.Looper;
import java.lang.Runnable;*/

public class GAJNI
{
    private static boolean _listenerAdded = false;
    //private static Handler mainHandler = new Handler(Looper.getMainLooper());

    private static final IRemoteConfigsListener remoteConfigsListener = new IRemoteConfigsListener()
    {
        @Override
        public void onRemoteConfigsUpdated()
        {
            GALogger.i("GameAnalytics: onRemoteConfigsUpdated called");
            onRemoteConfigsUpdatedNative();
            /*mainHandler.post(new Runnable()
            {
                @Override
                public void run()
                {
                    onRemoteConfigsUpdatedNative();
                }
            });*/
        }
    };

    public static void initialize()
    {
        if(!_listenerAdded)
        {
            _listenerAdded = true;
            GALogger.i("GameAnalytics: initialize called");
            GameAnalytics.addRemoteConfigsListener(remoteConfigsListener);
        }
    }

    public static native void onRemoteConfigsUpdatedNative();
}
