// Defold compiles every source under src/ for every platform, and only the
// desktop platforms use the C++ SDK.
#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)

#include "GAHttpClientDefold.h"

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

#include <dmsdk/dlib/http.h>

namespace gameanalytics
{
    namespace defold
    {
        namespace http
        {
            namespace
            {
                HttpService* service = 0;

                // How long one request may take before the SDK gets a failure.
                const uint32_t TIMEOUT_SECONDS = 30;

                // Shared with the HTTP worker thread, which may still deliver
                // the completion after sendRequest() has given up waiting.
                struct Exchange
                {
                    std::mutex mutex;
                    std::condition_variable done;
                    bool completed = false;
                    GAHttpClient::Response response;
                };

                HttpCallbackResult OnResponse(HttpRequest*, void* userData, const HttpResponseInfo* info)
                {
                    std::shared_ptr<Exchange>* holder = (std::shared_ptr<Exchange>*)userData;
                    Exchange& exchange = **holder;

                    switch(HttpResponseGetEvent(info))
                    {
                        case HTTP_RESPONSE_EVENT_DATA:
                        {
                            const uint8_t* data = (const uint8_t*)HttpResponseGetData(info);
                            std::lock_guard<std::mutex> lock(exchange.mutex);
                            exchange.response.packet.insert(exchange.response.packet.end(), data, data + HttpResponseGetDataSize(info));
                            break;
                        }
                        case HTTP_RESPONSE_EVENT_COMPLETE:
                        {
                            HttpResult result = HttpResponseGetResult(info);
                            {
                                std::lock_guard<std::mutex> lock(exchange.mutex);
                                // Any status the server answered with goes to the SDK,
                                // which handles 4xx/5xx itself. -1 means no response.
                                bool answered = result == HTTP_RESULT_OK || result == HTTP_RESULT_NOT_200_OK;
                                exchange.response.code = answered ? HttpResponseGetStatusCode(info) : -1;
                                exchange.completed = true;
                            }
                            exchange.done.notify_one();
                            delete holder;
                            break;
                        }
                        default:
                            break;
                    }

                    return HTTP_CALLBACK_RESULT_CONTINUE;
                }
            }

            void SetService(HttpService* httpService)
            {
                if(httpService)
                {
                    service = httpService;
                }
            }

            HttpService* GetService()
            {
                return service;
            }

            GAHttpClient::Response GAHttpClientDefold::sendRequest(std::string const& url, std::string const& auth, std::vector<uint8_t> const& payloadData, bool useGzip, void*)
            {
                Response failed;

                if(!service)
                {
                    return failed;
                }

                HttpRequest* request = 0;
                if(HttpNewRequest(&request) != HTTP_RESULT_OK)
                {
                    return failed;
                }

                // Same request as the SDK's cURL client: a JSON POST,
                // `auth` is the whole "Authorization: ..." header.
                bool ok = HttpSetMethod(request, "POST") == HTTP_RESULT_OK
                    && HttpSetURL(request, url.c_str()) == HTTP_RESULT_OK
                    && HttpAddHeader(request, "Content-Type: application/json") == HTTP_RESULT_OK
                    && HttpAddHeader(request, auth.c_str()) == HTTP_RESULT_OK
                    && (!useGzip || HttpAddHeader(request, "Content-Encoding: gzip") == HTTP_RESULT_OK)
                    && HttpSetRequestBody(request, payloadData.data(), (uint32_t)payloadData.size()) == HTTP_RESULT_OK
                    && HttpSetIgnoreCache(request, 1) == HTTP_RESULT_OK
                    && HttpSetTimeout(request, TIMEOUT_SECONDS * 1000000) == HTTP_RESULT_OK;

                std::shared_ptr<Exchange> exchange = std::make_shared<Exchange>();
                std::shared_ptr<Exchange>* holder = new std::shared_ptr<Exchange>(exchange);

                HttpRequestHandle handle = HTTP_REQUEST_HANDLE_INVALID;
                if(!ok
                    || HttpSetResponseCallback(request, OnResponse, holder) != HTTP_RESULT_OK
                    || HttpPushRequest(service, request, &handle) != HTTP_RESULT_OK)
                {
                    // Not pushed, so the request and the holder are still ours.
                    HttpDeleteRequest(request);
                    delete holder;
                    return failed;
                }

                std::unique_lock<std::mutex> lock(exchange->mutex);
                bool completed = exchange->done.wait_for(lock, std::chrono::seconds(TIMEOUT_SECONDS + 10), [&]{ return exchange->completed; });
                if(!completed)
                {
                    lock.unlock();
                    HttpCancelRequest(service, handle);
                    return failed;
                }

                return exchange->response;
            }
        }
    }
}

#endif // desktop platforms
