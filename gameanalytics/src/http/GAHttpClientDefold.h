#ifndef GA_HTTP_CLIENT_DEFOLD_H
#define GA_HTTP_CLIENT_DEFOLD_H

// ENGINE HOOK (GAWrapperCpp.h): HTTP transport for the desktop platforms.
//
// The desktop libs are the "static-no-deps" C++ SDK, which has no HTTP client
// of its own, so requests go through the engine's HTTP service
// (dmsdk/dlib/http.h). That also keeps curl and OpenSSL out of the extension.

#include <string>
#include <vector>

#include "GameAnalytics/GAHttpClient.h"

struct HttpService;

namespace gameanalytics
{
    namespace defold
    {
        namespace http
        {
            // The engine's HTTP service the client sends through. Set it before
            // the SDK is initialized; null is ignored.
            void SetService(HttpService* service);
            HttpService* GetService();

            class GAHttpClientDefold:
                public GAHttpClient
            {
                public:

                    virtual void initialize() override {}

                    virtual void cleanup() override {}

                    // Called from the SDK's own thread, which blocks until the
                    // engine has finished the request.
                    virtual Response sendRequest(std::string const& url, std::string const& auth, std::vector<uint8_t> const& payloadData, bool useGzip, void* userData) override;
            };
        }
    }
}

#endif // GA_HTTP_CLIENT_DEFOLD_H
