#define EXTENSION_NAME GameAnalytics
#define MODULE_NAME "gameanalytics"
#define LIB_NAME "GameAnalytics"

// Defold SDK
#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)

#define LuaTypeName(L, pos) lua_typename(L, lua_type(L, pos))
#define UTF8IsEqual(utf8str1, utf8str2) (strcmp(utf8str1, utf8str2) == 0)

// options keys
#define GameKeyOptionsKey "gameKey"
#define GameSecretOptionsKey "gameSecret"
#define CurrencyOptionsKey "currency"
#define AmountOptionsKey "amount"
#define ItemTypeOptionsKey "itemType"
#define ItemIdOptionsKey "itemId"
#define CartTypeOptionsKey "cartType"
#define ReceiptOptionsKey "receipt"
#define SignatureOptionsKey "signature"
#define AutoFetchReceiptOptionsKey "autoFetchReceipt"
#define FlowTypeOptionsKey "flowType"
#define ProgressionStatusOptionsKey "progressionStatus"
#define Progression01OptionsKey "progression01"
#define Progression02OptionsKey "progression02"
#define Progression03OptionsKey "progression03"
#define ScoreOptionsKey "score"
#define EventIdOptionsKey "eventId"
#define ValueOptionsKey "value"
#define SeverityOptionsKey "severity"
#define MessageOptionsKey "message"
#define AdActionOptionsKey "adAction"
#define AdTypeOptionsKey "adType"
#define AdSdkNameOptionsKey "adSdkName"
#define AdPlacementOptionsKey "adPlacement"
#define DurationOptionsKey "duration"
#define NoAdReasonOptionsKey "noAdReason"
#define CustomFieldsOptionsKey "customFields"
#define MergeFieldsOptionsKey "mergeFields"
#define KeyOptionsKey "key"
#define DefaultValueOptionsKey "defaultValue"

#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_LINUX)
#include <string.h>
#include <limits.h>     /* PATH_MAX */
#include <sys/stat.h>   /* mkdir(2) */
#include <errno.h>
#include <wordexp.h>
#elif defined(DM_PLATFORM_WINDOWS)
#include <string.h>
#include <errno.h>
#include <direct.h>
#include <regex>
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#endif

#include <assert.h>
#include <vector>
#include <cctype>

#include "GameAnalyticsDefold.h"

#define VERSION "4.3.0"

bool g_GameAnalytics_initialized = false;
bool use_custom_id = false;
const char* game_key = NULL;
const char* secret_key = NULL;
bool manual_initialize = false;
dmScript::LuaCallbackInfo* g_remote_configs_listener;

static int stringCmpi(const char *s1, const char *s2)
{
    int i=0;
    for(i=0; s1[i]!='\0'; i++)
    {
        if( toupper(s1[i])!=toupper(s2[i]) )
            return 1;
    }
    return 0;
}

static bool isStringNullOrEmpty(const char* s)
{
    return s == NULL || strlen(s) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lua implementation

// [Lua] gameanalytics.configureUserId( id )
static int configureUserId(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "ERROR: gameanalytics.configureUserId(id): id, expected string got: %s", LuaTypeName(L, 1));
    }

    const char *s = lua_tostring(L, 1);

    if(isStringNullOrEmpty(s))
    {
        return luaL_error(L, "gameanalytics.configureUserId(id): id is mandatory and can't be null or empty");
    }

    if(use_custom_id)
    {
        dmLogInfo("Initializing with custom id: %s\n", s);
        gameanalytics::defold::GameAnalytics::configureUserId(s);
        gameanalytics::defold::GameAnalytics::initialize(game_key, secret_key);
    }
    else
    {
        dmLogWarning("Custom id is not enabled\n");
    }

    return 0;
}

// [Lua] gameanalytics.initialize()
static int initialize(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    if(manual_initialize)
    {
        gameanalytics::defold::GameAnalytics::initialize(game_key, secret_key);
    }
    else
    {
        dmLogWarning("Manual initialize is not enabled\n");
    }

    return 0;
}

#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_LINUX)
int mkdir_p(const char *path)
{
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    dmStrlCpy(_path, path, sizeof(_path));

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) {
                if (errno != EEXIST)
                    return -1;
            }

            *p = '/';
        }
    }

    if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}
#elif defined(DM_PLATFORM_WINDOWS)
void autoExpandEnvironmentVariables( std::string & text ) {
    static std::regex env( "%.+%" );
    std::smatch match;
    while ( std::regex_search( text, match, env ) ) {
        std::string m = match[0].str();
        m.erase(std::remove(m.begin(), m.end(), '%'), m.end());
        dmLogInfo("match=%s\n", m.c_str());
        const char * s = std::getenv( m.c_str() );
        const std::string var( s == NULL ? "" : s );
        text.replace( match[0].first, match[0].second, var );
    }
}

std::string expandEnvironmentVariables( const std::string & input ) {
    std::string text = input;
    autoExpandEnvironmentVariables( text );
    return text;
}

int mkdir_p(const char *path)
{
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    dmStrlCpy(_path, path, sizeof(_path));

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            /* Temporarily truncate */
            *p = '\0';

            if (_mkdir(_path) != 0) {
                if (errno != EEXIST)
                    return -1;
            }

            *p = '/';
        }
    }

    if (_mkdir(_path) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}
#endif

// [Lua] gameanalytics.addBusinessEvent( options )
static int addBusinessEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char *currency = "";
    lua_Integer amount = 0;
    const char *itemType = "";
    const char *itemId = "";
    const char *cartType = "";
#if defined(DM_PLATFORM_IOS)
    const char *receipt = "";
    bool autoFetchReceipt = false;
#elif defined(DM_PLATFORM_ANDROID)
    const char *receipt = "";
    const char *signature = "";
#endif
    const char *fields = "";
    bool mergeFields = false;

    if(lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if(UTF8IsEqual(key, CurrencyOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    currency = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", CurrencyOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, AmountOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TNUMBER)
                {
                    amount = lua_tointeger(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected number got: %s", AmountOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, ItemTypeOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    itemType = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", ItemTypeOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, ItemIdOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    itemId = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", ItemIdOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, CartTypeOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    cartType = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", CartTypeOptionsKey, LuaTypeName(L, -1));
                }
            }
#if defined(DM_PLATFORM_ANDROID) || defined(DM_PLATFORM_IOS)
            else if(UTF8IsEqual(key, ReceiptOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    receipt = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", ReceiptOptionsKey, LuaTypeName(L, -1));
                }
            }
#endif
#if defined(DM_PLATFORM_ANDROID)
            else if(UTF8IsEqual(key, SignatureOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    signature = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", SignatureOptionsKey, LuaTypeName(L, -1));
                }
            }
#endif
#if defined(DM_PLATFORM_IOS)
            else if(UTF8IsEqual(key, AutoFetchReceiptOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    autoFetchReceipt = lua_toboolean(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected boolean got: %s", AutoFetchReceiptOptionsKey, LuaTypeName(L, -1));
                }
            }
#endif
            else if(UTF8IsEqual(key, CustomFieldsOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    fields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected string got: %s", CustomFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, MergeFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    mergeFields = lua_toboolean(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s, expected boolean got: %s", MergeFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.addBusinessEvent(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.addBusinessEvent(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    if(isStringNullOrEmpty(currency))
    {
        return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s is mandatory and can't be null or empty", CurrencyOptionsKey);
    }
    if(isStringNullOrEmpty(itemType))
    {
        return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s is mandatory and can't be null or empty", ItemTypeOptionsKey);
    }
    if(isStringNullOrEmpty(itemId))
    {
        return luaL_error(L, "gameanalytics.addBusinessEvent(options): options.%s is mandatory and can't be null or empty", ItemIdOptionsKey);
    }

#if defined(DM_PLATFORM_IOS)
    if(autoFetchReceipt)
    {
        gameanalytics::defold::GameAnalytics::addBusinessEventAndAutoFetchReceipt(currency, amount, itemType, itemId, cartType, fields, mergeFields);
    }
    else
#endif
    {
#if defined(DM_PLATFORM_IOS)
        gameanalytics::defold::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType, receipt, fields, mergeFields);
#elif defined(DM_PLATFORM_ANDROID)
        gameanalytics::defold::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType, receipt, signature, fields, mergeFields);
#else
        gameanalytics::defold::GameAnalytics::addBusinessEvent(currency, amount, itemType, itemId, cartType, fields, mergeFields);
#endif
    }

    return 0;
}

// [Lua] gameanalytics.addResourceEvent( options )
static int addResourceEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::EGAResourceFlowType flowType = (gameanalytics::defold::EGAResourceFlowType)0;
    const char *currency = "";
    lua_Number amount = 0;
    const char *itemType = "";
    const char *itemId = "";
    const char *fields = "";
    bool mergeFields = false;

    if(lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if(UTF8IsEqual(key, FlowTypeOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    const char *flowTypeString = lua_tostring(L, -1);
                    if(stringCmpi(flowTypeString, "Source") == 0)
                    {
                        flowType = gameanalytics::defold::Source;
                    }
                    else if(stringCmpi(flowTypeString, "Sink") == 0)
                    {
                        flowType = gameanalytics::defold::Sink;
                    }
                    else
                    {
                        return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected value {Sink or Source} got: %s", FlowTypeOptionsKey, flowTypeString);
                    }
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected string got: %s", CurrencyOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, CurrencyOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    currency = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected string got: %s", CurrencyOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, AmountOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TNUMBER)
                {
                    amount = lua_tonumber(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected number got: %s", AmountOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, ItemTypeOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    itemType = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected string got: %s", ItemTypeOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, ItemIdOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    itemId = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected string got: %s", ItemIdOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, CustomFieldsOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    fields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected string got: %s", CustomFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, MergeFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    mergeFields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s, expected boolean got: %s", MergeFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.addResourceEvent(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.addResourceEvent(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    if(isStringNullOrEmpty(currency))
    {
        return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s is mandatory and can't be null or empty", CurrencyOptionsKey);
    }
    if(amount <= 0)
    {
        return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s is mandatory and must be greater than zero", AmountOptionsKey);
    }
    if(isStringNullOrEmpty(itemType))
    {
        return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s is mandatory and can't be null or empty", ItemTypeOptionsKey);
    }
    if(isStringNullOrEmpty(itemId))
    {
        return luaL_error(L, "gameanalytics.addResourceEvent(options): options.%s is mandatory and can't be null or empty", ItemIdOptionsKey);
    }

    gameanalytics::defold::GameAnalytics::addResourceEvent(flowType, currency, amount, itemType, itemId, fields, mergeFields);

    return 0;
}

// [Lua] gameanalytics.addProgressionEvent( options )
static int addProgressionEvent( lua_State *L )
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::EGAProgressionStatus progressionStatus = (gameanalytics::defold::EGAProgressionStatus)0;
    const char *progression01 = "";
    const char *progression02 = "";
    const char *progression03 = "";
    lua_Integer score = 0;
    bool sendScore = false;
    const char *fields = "";
    bool mergeFields = false;

    if(lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if(UTF8IsEqual(key, ProgressionStatusOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    const char *progressionStatusString = lua_tostring(L, -1);
                    if(stringCmpi(progressionStatusString, "Complete") == 0)
                    {
                        progressionStatus = gameanalytics::defold::Complete;
                    }
                    else if(stringCmpi(progressionStatusString, "Start") == 0)
                    {
                        progressionStatus = gameanalytics::defold::Start;
                    }
                    else if(stringCmpi(progressionStatusString, "Fail") == 0)
                    {
                        progressionStatus = gameanalytics::defold::Fail;
                    }
                    else
                    {
                        return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected value {Complete, Fail or Start} got: %s", ProgressionStatusOptionsKey, progressionStatusString);
                    }
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected string got: %s", ProgressionStatusOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, Progression01OptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    progression01 = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected string got: %s", Progression01OptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, Progression02OptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    progression02 = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected string got: %s", Progression02OptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, Progression03OptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    progression03 = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected string got: %s", Progression03OptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, ScoreOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TNUMBER)
                {
                    score = lua_tointeger(L, -1);
                    sendScore = true;
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected number got: %s", ScoreOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, CustomFieldsOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    fields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected string got: %s", CustomFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, MergeFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    mergeFields = lua_toboolean(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s, expected string got: %s", MergeFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.addProgressionEvent(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.addProgressionEvent(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    if(isStringNullOrEmpty(progression01))
    {
        return luaL_error(L, "gameanalytics.addProgressionEvent(options): options.%s is mandatory and can't be null or empty", Progression01OptionsKey);
    }

    if(sendScore)
    {
        gameanalytics::defold::GameAnalytics::addProgressionEvent(progressionStatus, progression01, progression02, progression03, score, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::GameAnalytics::addProgressionEvent(progressionStatus, progression01, progression02, progression03, fields, mergeFields);
    }

    return 0;
}

// [Lua] gameanalytics.addDesignEvent( options )
static int addDesignEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char *eventId = "";
    lua_Number value = 0;
    bool sendValue = false;
    const char *fields = "";
    bool mergeFields = false;

    if(lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if(UTF8IsEqual(key, EventIdOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    eventId = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addDesignEvent(options): options.%s, expected string got: %s", EventIdOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, ValueOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TNUMBER)
                {
                    value = lua_tonumber(L, -1);
                    sendValue = true;
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addDesignEvent(options): options.%s, expected number got: %s", ValueOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, CustomFieldsOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    fields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addDesignEvent(options): options.%s, expected string got: %s", CustomFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, MergeFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    mergeFields = lua_toboolean(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addDesignEvent(options): options.%s, expected boolean got: %s", MergeFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.addDesignEvent(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.addDesignEvent(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    if(isStringNullOrEmpty(eventId))
    {
        return luaL_error(L, "gameanalytics.addDesignEvent(options): options.%s is mandatory and can't be null or empty", EventIdOptionsKey);
    }

    if(sendValue)
    {
        gameanalytics::defold::GameAnalytics::addDesignEvent(eventId, value, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::GameAnalytics::addDesignEvent(eventId, fields, mergeFields);
    }

    return 0;
}

// [Lua] gameanalytics.addErrorEvent( options )
static int addErrorEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::EGAErrorSeverity severity = (gameanalytics::defold::EGAErrorSeverity)0;
    const char *message = "";
    const char *fields = "";
    bool mergeFields = false;

    if(lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if(UTF8IsEqual(key, SeverityOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    const char *severityString = lua_tostring(L, -1);

                    if(stringCmpi(severityString, "Critical") == 0)
                    {
                        severity = gameanalytics::defold::Critical;
                    }
                    else if(stringCmpi(severityString, "Debug") == 0)
                    {
                        severity = gameanalytics::defold::Debug;
                    }
                    else if(stringCmpi(severityString, "Error") == 0)
                    {
                        severity = gameanalytics::defold::Error;
                    }
                    else if(stringCmpi(severityString, "Info") == 0)
                    {
                        severity = gameanalytics::defold::Info;
                    }
                    else if(stringCmpi(severityString, "Warning") == 0)
                    {
                        severity = gameanalytics::defold::Warning;
                    }
                    else
                    {
                        return luaL_error(L, "gameanalytics.addErrorEvent(options): options.%s, expected value {Critical, Debug, Error, Info or Warning} got: %s", SeverityOptionsKey, severityString);
                    }
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addErrorEvent(options): options.%s, expected string got: %s", EventIdOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, MessageOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    message = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addErrorEvent(options): options.%s, expected string got: %s", MessageOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, CustomFieldsOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    fields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addErrorEvent(options): options.%s, expected string got: %s", CustomFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, MergeFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    mergeFields = lua_toboolean(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addErrorEvent(options): options.%s, expected boolean got: %s", MergeFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.addErrorEvent(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.addErrorEvent(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::GameAnalytics::addErrorEvent(severity, message, fields, mergeFields);

    return 0;
}

// [Lua] gameanalytics.addAdEvent( options )
static int addAdEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::EGAAdAction adAction = (gameanalytics::defold::EGAAdAction)0;
    gameanalytics::defold::EGAAdType adType = (gameanalytics::defold::EGAAdType)0;
    const char *adSdkName = "";
    const char *adPlacement = "";
    lua_Integer duration = 0;
    bool sendDuration = false;
    gameanalytics::defold::EGAAdError noAdReason = (gameanalytics::defold::EGAAdError)0;
    const char *fields = "";
    bool mergeFields = false;

    if (lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if (UTF8IsEqual(key, AdActionOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TSTRING)
                {
                    const char *adActionString = lua_tostring(L, -1);

                    if (stringCmpi(adActionString, "Clicked") == 0)
                    {
                        adAction = gameanalytics::defold::Clicked;
                    }
                    else if (stringCmpi(adActionString, "Show") == 0)
                    {
                        adAction = gameanalytics::defold::Show;
                    }
                    else if (stringCmpi(adActionString, "FailedShow") == 0)
                    {
                        adAction = gameanalytics::defold::FailedShow;
                    }
                    else if (stringCmpi(adActionString, "RewardReceived") == 0)
                    {
                        adAction = gameanalytics::defold::RewardReceived;
                    }
                    else
                    {
                        return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected value {Clicked, Show, FailedShow or RewardReceived} got: %s", AdActionOptionsKey, adActionString);
                    }
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected string got: %s", AdActionOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, AdTypeOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TSTRING)
                {
                    const char *adTypeString = lua_tostring(L, -1);

                    if (stringCmpi(adTypeString, "Video") == 0)
                    {
                        adType = gameanalytics::defold::Video;
                    }
                    else if (stringCmpi(adTypeString, "RewardedVideo") == 0)
                    {
                        adType = gameanalytics::defold::RewardedVideo;
                    }
                    else if (stringCmpi(adTypeString, "Playable") == 0)
                    {
                        adType = gameanalytics::defold::Playable;
                    }
                    else if (stringCmpi(adTypeString, "Interstitial") == 0)
                    {
                        adType = gameanalytics::defold::Interstitial;
                    }
                    else if (stringCmpi(adTypeString, "OfferWall") == 0)
                    {
                        adType = gameanalytics::defold::OfferWall;
                    }
                    else if (stringCmpi(adTypeString, "Banner") == 0)
                    {
                        adType = gameanalytics::defold::Banner;
                    }
                    else
                    {
                        return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected value {Video, RewardedVideo, Playable, Interstitial, OfferWall or Banner} got: %s", AdTypeOptionsKey, adTypeString);
                    }
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected string got: %s", AdTypeOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, AdSdkNameOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TSTRING)
                {
                    adSdkName = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected string got: %s", AdSdkNameOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, AdPlacementOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TSTRING)
                {
                    adPlacement = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected string got: %s", AdPlacementOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, DurationOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TNUMBER)
                {
                    duration = lua_tointeger(L, -1);
                    sendDuration = true;
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected number got: %s", DurationOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, NoAdReasonOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TSTRING)
                {
                    const char *noAdReasonString = lua_tostring(L, -1);

                    if (stringCmpi(noAdReasonString, "Unknown") == 0)
                    {
                        noAdReason = gameanalytics::defold::Unknown;
                    }
                    else if (stringCmpi(noAdReasonString, "Offline") == 0)
                    {
                        noAdReason = gameanalytics::defold::Offline;
                    }
                    else if (stringCmpi(noAdReasonString, "NoFill") == 0)
                    {
                        noAdReason = gameanalytics::defold::NoFill;
                    }
                    else if (stringCmpi(noAdReasonString, "InternalError") == 0)
                    {
                        noAdReason = gameanalytics::defold::InternalError;
                    }
                    else if (stringCmpi(noAdReasonString, "InvalidRequest") == 0)
                    {
                        noAdReason = gameanalytics::defold::InvalidRequest;
                    }
                    else if (stringCmpi(noAdReasonString, "UnableToPrecache") == 0)
                    {
                        noAdReason = gameanalytics::defold::UnableToPrecache;
                    }
                    else
                    {
                        return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected value {Unknown, Offline, NoFill, InternalError, InvalidRequest or UnableToPrecache} got: %s", NoAdReasonOptionsKey, noAdReasonString);
                    }
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected string got: %s", NoAdReasonOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, CustomFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TSTRING)
                {
                    fields = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected string got: %s", CustomFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if (UTF8IsEqual(key, MergeFieldsOptionsKey))
            {
                if (lua_type(L, -1) == LUA_TBOOLEAN)
                {
                    mergeFields = lua_toboolean(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.addAdEvent(options): options.%s, expected boolean got: %s", MergeFieldsOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.addAdEvent(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.addAdEvent(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    if(sendDuration)
    {
        gameanalytics::defold::GameAnalytics::addAdEventWithDuration(adAction, adType, adSdkName, adPlacement, duration, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::GameAnalytics::addAdEventWithNoAdReason(adAction, adType, adSdkName, adPlacement, noAdReason, fields, mergeFields);
    }

    return 0;
}

// [Lua] gameanalytics.setEnabledInfoLog( flag )
static int setEnabledInfoLog(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.setEnabledInfoLog(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::GameAnalytics::setEnabledInfoLog(lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.setEnabledVerboseLog( flag )
static int setEnabledVerboseLog(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.setEnabledVerboseLog(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::GameAnalytics::setEnabledVerboseLog(lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.setEnabledManualSessionHandling( flag )
static int setEnabledManualSessionHandling(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.setEnabledManualSessionHandling(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::GameAnalytics::setEnabledManualSessionHandling(lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.setEnabledEventSubmission( flag )
static int setEnabledEventSubmission(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.setEnabledEventSubmission(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::GameAnalytics::setEnabledEventSubmission(lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.setCustomDimension01( dimension )
static int setCustomDimension01(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "gameanalytics.setCustomDimension01(dimension): dimension, expected string got: %s", LuaTypeName(L, 1));
    }

    const char *s = lua_tostring(L, 1);

    gameanalytics::defold::GameAnalytics::setCustomDimension01(s);

    return 0;
}

// [Lua] gameanalytics.setCustomDimension02( dimension )
static int setCustomDimension02(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "gameanalytics.setCustomDimension02(dimension): dimension, expected string got: %s", LuaTypeName(L, 1));
    }

    const char *s = lua_tostring(L, 1);

    gameanalytics::defold::GameAnalytics::setCustomDimension02(s);

    return 0;
}

// [Lua] gameanalytics.setCustomDimension03( dimension )
static int setCustomDimension03(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "gameanalytics.setCustomDimension03(dimension): dimension, expected string got: %s", LuaTypeName(L, 1));
    }

    const char *s = lua_tostring(L, 1);

    gameanalytics::defold::GameAnalytics::setCustomDimension03(s);

    return 0;
}

// [Lua] gameanalytics.setGlobalCustomEventFields( customFields )
static int setGlobalCustomEventFields(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if (lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "gameanalytics.setGlobalCustomEventFields(customFields): customFields, expected string got: %s", LuaTypeName(L, 1));
    }

    const char *s = lua_tostring(L, 1);

    gameanalytics::defold::GameAnalytics::setGlobalCustomEventFields(s);

    return 0;
}

// [Lua] gameanalytics.startSession()
static int startSession(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::GameAnalytics::startSession();

    return 0;
}

// [Lua] gameanalytics.endSession()
static int endSession(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::GameAnalytics::endSession();

    return 0;
}

// [Lua] gameanalytics.getRemoteConfigsValueAsString(options)
static int getRemoteConfigsValueAsString(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    const char *cc_key = "";
    const char *defaultValue = "";
    bool useDefaultValue = false;

    if(lua_type(L, 1) == LUA_TTABLE)
    {
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
        {
            const char *key = lua_tostring(L, -2);

            if(UTF8IsEqual(key, KeyOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    cc_key = lua_tostring(L, -1);
                }
                else
                {
                    return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsString(options): options.%s, expected string got: %s", KeyOptionsKey, LuaTypeName(L, -1));
                }
            }
            else if(UTF8IsEqual(key, DefaultValueOptionsKey))
            {
                if(lua_type(L, -1) == LUA_TSTRING)
                {
                    defaultValue = lua_tostring(L, -1);
                    useDefaultValue = true;
                }
                else
                {
                    return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsString(options): options.%s, expected string got: %s", DefaultValueOptionsKey, LuaTypeName(L, -1));
                }
            }
            else
            {
                return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsString(options): Invalid option: '%s'", key);
            }
        }
    }
    else
    {
        return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsString(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    if(cc_key == NULL || cc_key[0] == '\0')
    {
        return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsString(options): options.%s is mandatory and can't be null or empty", KeyOptionsKey);
    }

    if(useDefaultValue)
    {
        std::vector<char> result = gameanalytics::defold::GameAnalytics::getRemoteConfigsValueAsString(cc_key, defaultValue);
        lua_pushstring( L, result.data() );
    }
    else
    {
        std::vector<char> result = gameanalytics::defold::GameAnalytics::getRemoteConfigsValueAsString(cc_key, "");
        lua_pushstring( L, result.data() );
    }

    return 1;
}

// [Lua] gameanalytics.isRemoteConfigsReady()
static int isRemoteConfigsReady(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    bool result = gameanalytics::defold::GameAnalytics::isRemoteConfigsReady();
    lua_pushboolean( L, result ? 1 : 0 );
    return 1;
}

// [Lua] gameanalytics.getRemoteConfigsContentAsString()
static int getRemoteConfigsContentAsString(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    std::vector<char> result = gameanalytics::defold::GameAnalytics::getRemoteConfigsContentAsString();
    lua_pushstring( L, result.data() );
    return 1;
}

// [Lua] gameanalytics.setRemoteConfigsListener(remoteConfigsListener)
static int setRemoteConfigsListener(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if (g_remote_configs_listener)
    {
        dmScript::DestroyCallback(g_remote_configs_listener);
    }
    g_remote_configs_listener = dmScript::CreateCallback(L, 1);
    assert(dmScript::IsCallbackValid(g_remote_configs_listener));
    gameanalytics::defold::GameAnalytics::setRemoteConfigsListener(g_remote_configs_listener);
    return 0;
}

////////////////////////////////////////////////////////

static const luaL_reg Module_methods[] =
    {
        {"configureUserId", configureUserId},

        {"initialize", initialize},

        {"addBusinessEvent", addBusinessEvent},
        {"addResourceEvent", addResourceEvent},
        {"addProgressionEvent", addProgressionEvent},
        {"addDesignEvent", addDesignEvent},
        {"addErrorEvent", addErrorEvent},
        {"addAdEvent", addAdEvent},

        {"setEnabledEventSubmission", setEnabledEventSubmission},

        {"setEnabledInfoLog", setEnabledInfoLog},
        {"setEnabledVerboseLog", setEnabledVerboseLog},
        {"setEnabledManualSessionHandling", setEnabledManualSessionHandling},
        {"setCustomDimension01", setCustomDimension01},
        {"setCustomDimension02", setCustomDimension02},
        {"setCustomDimension03", setCustomDimension03},
        {"setGlobalCustomEventFields", setGlobalCustomEventFields},

        {"startSession", startSession},
        {"endSession", endSession},

        {"getRemoteConfigsValueAsString", getRemoteConfigsValueAsString},
        {"isRemoteConfigsReady", isRemoteConfigsReady},
        {"getRemoteConfigsContentAsString", getRemoteConfigsContentAsString},
        {"setRemoteConfigsListener", setRemoteConfigsListener},

        {0, 0}};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Extension interface functions

static dmExtension::Result AppInitializeExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Lua extension\n", MODULE_NAME);

    if(g_GameAnalytics_initialized)
    {
        dmLogError("GameAnalytics already initialized.");
        return dmExtension::RESULT_OK;
    }

    const char* dimensions_01 = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.custom_dimensions_01", 0);
    const char* dimensions_02 = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.custom_dimensions_02", 0);
    const char* dimensions_03 = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.custom_dimensions_03", 0);
    const char* resource_currencies = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.resource_currencies", 0);
    const char* resource_item_types = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.resource_item_types", 0);
    const char* build = NULL;
    bool enable_info_log = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_info_log", 0) == 1;
    bool enable_verbose_log = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_verbose_log", 0) == 1;
    bool enable_event_submission = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_event_submission", 1) == 1;
    bool use_error_reporting = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_error_reporting", 0) == 1;
    use_custom_id = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.use_custom_id", 0) == 1;
    bool use_manual_session_handling = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.use_manual_session_handling", 0) == 1;
    bool auto_detect_app_version = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.auto_detect_app_version", 0) == 1;
    manual_initialize = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.manual_initialize", 0) == 1;

#if defined(DM_PLATFORM_ANDROID)
    game_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.game_key_android", 0);
    secret_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.secret_key_android", 0);
    build = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.build_android", 0);
#elif defined(DM_PLATFORM_IOS)
    game_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.game_key_ios", 0);
    secret_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.secret_key_ios", 0);
    build = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.build_ios", 0);
#elif defined(DM_PLATFORM_HTML5)
    game_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.game_key_html5", 0);
    secret_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.secret_key_html5", 0);
    build = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.build_html5", 0);
#elif defined(DM_PLATFORM_OSX)
    game_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.game_key_osx", 0);
    secret_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.secret_key_osx", 0);
    build = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.build_osx", 0);
    const char* write_path = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.write_path_osx", 0);
    if(write_path)
    {
        wordexp_t p;
        wordexp(write_path, &p, 0);
        dmLogInfo("write_path=%s\n", p.we_wordv[0]);
        mkdir_p(p.we_wordv[0]);
        gameanalytics::defold::GameAnalytics::configureWritablePath(p.we_wordv[0]);
        wordfree(&p);
    }
#elif defined(DM_PLATFORM_WINDOWS)
    game_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.game_key_windows", 0);
    secret_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.secret_key_windows", 0);
    build = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.build_windows", 0);
    const char* write_path = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.write_path_windows", 0);
    if(write_path)
    {
        std::string p(write_path);
        p = expandEnvironmentVariables(p);
        dmLogInfo("write_path=%s\n", p.c_str());
        mkdir_p(p.c_str());
        gameanalytics::defold::GameAnalytics::configureWritablePath(p.c_str());
    }
#elif defined(DM_PLATFORM_LINUX)
    game_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.game_key_linux", 0);
    secret_key = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.secret_key_linux", 0);
    build = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.build_linux", 0);
    const char* write_path = dmConfigFile::GetString(params->m_ConfigFile, "gameanalytics.write_path_linux", 0);
    if(write_path)
    {
        wordexp_t p;
        wordexp(write_path, &p, 0);
        dmLogInfo("write_path=%s\n", p.we_wordv[0]);
        mkdir_p(p.we_wordv[0]);
        gameanalytics::defold::GameAnalytics::configureWritablePath(p.we_wordv[0]);
        wordfree(&p);
    }
#endif

    if(!game_key)
    {
        dmLogError("No gameanalytics.game_key set in game.project for target platform!");
        return dmExtension::RESULT_OK;
    }

    if(!secret_key)
    {
        dmLogError("No gameanalytics.secret_key set in game.project for target platform!");
        return dmExtension::RESULT_OK;
    }

    if(enable_info_log)
    {
        gameanalytics::defold::GameAnalytics::setEnabledInfoLog(true);
    }
    if(enable_verbose_log)
    {
        gameanalytics::defold::GameAnalytics::setEnabledVerboseLog(true);
    }

    if(enable_event_submission)
    {
        gameanalytics::defold::GameAnalytics::setEnabledEventSubmission(true);
    }
    else
    {
        gameanalytics::defold::GameAnalytics::setEnabledEventSubmission(false);
    }

    if(use_error_reporting)
    {
        gameanalytics::defold::GameAnalytics::setEnabledErrorReporting(true);
    }
    else
    {
        gameanalytics::defold::GameAnalytics::setEnabledErrorReporting(false);
    }

    if(build)
    {
        gameanalytics::defold::GameAnalytics::configureBuild(build);
    }
    if(auto_detect_app_version)
    {
        gameanalytics::defold::GameAnalytics::configureAutoDetectAppVersion(true);
    }

    if(dimensions_01)
    {
        gameanalytics::defold::GameAnalytics::configureAvailableCustomDimensions01(dimensions_01);
    }
    if(dimensions_02)
    {
        gameanalytics::defold::GameAnalytics::configureAvailableCustomDimensions02(dimensions_02);
    }
    if(dimensions_03)
    {
        gameanalytics::defold::GameAnalytics::configureAvailableCustomDimensions03(dimensions_03);
    }
    if(resource_currencies)
    {
        gameanalytics::defold::GameAnalytics::configureAvailableResourceCurrencies(resource_currencies);
    }
    if(resource_item_types)
    {
        gameanalytics::defold::GameAnalytics::configureAvailableResourceItemTypes(resource_item_types);
    }

    if(use_manual_session_handling)
    {
        gameanalytics::defold::GameAnalytics::setEnabledManualSessionHandling(true);
    }

    const char *defoldBuildVersion = NULL;

    lua_getglobal(params->m_L, "sys");                       // push 'sys' onto stack
    lua_getfield(params->m_L, -1, "get_engine_info");        // push desired function
    lua_call(params->m_L, 0, 1);                             // call function with 0 arg, 1 return value
    lua_getfield(params->m_L, -1, "version");                // push desired property
    defoldBuildVersion = lua_tostring(params->m_L, -1);      // get return value
    lua_pop(params->m_L, 1);                                 // pop result
    lua_pop(params->m_L, 1);                                 // pop function
    lua_pop(params->m_L, 1);                                 // pop 'sys'
    char sdk_version[65] = "";
    dmSnPrintf(sdk_version, sizeof(sdk_version), "defold %s", VERSION);

    char engine_version[65] = "";
    dmSnPrintf(engine_version, sizeof(engine_version), "defold %s", defoldBuildVersion);

    gameanalytics::defold::GameAnalytics::configureSdkGameEngineVersion(sdk_version);
    gameanalytics::defold::GameAnalytics::configureGameEngineVersion(engine_version);

    if(!use_custom_id && !manual_initialize)
    {
        gameanalytics::defold::GameAnalytics::initialize(game_key, secret_key);
    }
    else if(manual_initialize)
    {
        dmLogInfo("Manual initialize is enabled. Initialize is delayed until gamenanalytics.initialize() has been called.");
    }
    else
    {
        dmLogInfo("Custom id is enabled. Initialize is delayed until custom id has been set.");
    }

    g_GameAnalytics_initialized = true;

    dmLogInfo("GameAnalytics fully initialized!");

    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeExtension");
    gameanalytics::defold::GameAnalytics::onQuit();

    if (params->m_L == dmScript::GetCallbackLuaContext(g_remote_configs_listener))
    {
        dmScript::DestroyCallback(g_remote_configs_listener);
        g_remote_configs_listener = 0;
    }

    return dmExtension::RESULT_OK;
}

#else

static dmExtension::Result AppInitializeExtension(dmExtension::AppParams* params)
{
    dmLogWarning("Registered %s (null) Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    dmLogInfo("InitializeExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeExtension");
    return dmExtension::RESULT_OK;
}

#endif


DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeExtension, AppFinalizeExtension, InitializeExtension, 0, 0, FinalizeExtension)
