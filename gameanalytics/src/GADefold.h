#ifndef GA_DEFOLD_H
#define GA_DEFOLD_H

// Defold side of the shared GameAnalytics C++ wrapper. Everything the lua
// binding talks to goes through GAWrapper, this header only decides which
// implementation it gets and feeds that implementation whatever the engine
// has to hand over (see the ENGINE HOOK blocks in the vendored wrappers).

#include "GAWrapper.h"

struct HttpService;

namespace gameanalytics
{
    namespace defold
    {
        // The wrapper for the platform this engine was built for, created the
        // first time it is asked for. Never null on a supported platform.
        gameanalytics::GAWrapper* Get();

        // The platform's name in game.project settings: "ios", "android",
        // "html5", "osx", "windows" or "linux".
        const char* PlatformName();

        // Hands the engine over to the wrapper: the JavaVM and the game
        // Activity on android, the javascript bridge on html5, nothing
        // anywhere else. Safe to call more than once, but call it at least
        // once from AppInitializeExtension.
        void BindEngine();

        // The engine's HTTP service (dmsdk/dlib/http.h), which the desktop
        // wrapper sends its requests through. Call before the wrapper is
        // initialized. Ignored on the other platforms and for null.
        void SetHttpService(HttpService* service);

        // Frame rate for the FPS histogram of the health events. The SDK
        // samples the tracker from its own thread, where lua is out of
        // reach, so the extension measures the rate itself: OnFrame() counts
        // the engine's frames (call it from UpdateExtension), FpsTracker()
        // is the function to hand to GAWrapper::EnableFpsHistogram.
        void OnFrame();
        gameanalytics::FPSTracker FpsTracker();

        // Drops the wrapper created by Get(). Call from AppFinalizeExtension.
        void ReleaseEngine();
    }
}

#endif // GA_DEFOLD_H
