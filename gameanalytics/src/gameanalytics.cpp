#define EXTENSION_NAME GameAnalytics
#define MODULE_NAME "gameanalytics"
#define LIB_NAME "GameAnalytics"

// Defold SDK. DLIB_LOG_DOMAIN is defined by the build for every extension.
#include <dmsdk/sdk.h>
#include <dmsdk/dlib/context_registry.h>
#include <dmsdk/dlib/http.h>

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

#include <assert.h>
#include <string.h>
#include <atomic>
#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>

#include "GADefold.h"

#define VERSION "5.0.0"

bool g_GameAnalytics_initialized = false;
bool use_custom_id = false;
const char* game_key = NULL;
const char* secret_key = NULL;
bool manual_initialize = false;
dmScript::LuaCallbackInfo* g_remote_configs_listener = 0;

// Set from whatever thread the SDK reports remote config updates on, the lua
// listener is then called from the extension update on the main thread.
std::atomic<bool> g_remote_configs_updated(false);
bool g_remote_configs_listener_registered = false;

static int stringCmpi(const char *s1, const char *s2)
{
    int i=0;
    for(i=0; s1[i]!='\0'; i++)
    {
        if( toupper(s1[i])!=toupper(s2[i]) )
            return 1;
    }
    return s2[i] == '\0' ? 0 : 1;
}

static bool isStringNullOrEmpty(const char* s)
{
    return s == NULL || strlen(s) == 0;
}

// Splits a comma separated list from game.project, e.g. "gold,gems".
static std::vector<std::string> splitList(const char* list)
{
    std::vector<std::string> result;
    std::string item;

    for(const char* c = list; ; c++)
    {
        if(*c == ',' || *c == '\0')
        {
            if(!item.empty())
            {
                result.push_back(item);
            }
            item.clear();

            if(*c == '\0')
            {
                break;
            }
        }
        else
        {
            item += *c;
        }
    }

    return result;
}

// A game.project setting that is set per platform, e.g. gameanalytics.game_key_ios.
static const char* getPlatformSetting(dmConfigFile::HConfig config, const char* name)
{
    std::string key = std::string("gameanalytics.") + name + "_" + gameanalytics::defold::PlatformName();
    return dmConfigFile::GetString(config, key.c_str(), 0);
}

static void appendEnvironmentVariable(std::string& out, std::string const& name)
{
    const char* value = getenv(name.c_str());
    if(value)
    {
        out += value;
    }
}

// Expands $NAME, ${NAME} and %NAME% in a path from game.project, plus a
// leading ~, so the write path can be written the same way on every platform.
static std::string expandEnvironmentVariables(const char* input)
{
    std::string text(input);
    std::string out;

    for(size_t i = 0; i < text.size(); )
    {
        char c = text[i];
        if(c == '~' && i == 0 && (text.size() == 1 || text[1] == '/' || text[1] == '\\'))
        {
            appendEnvironmentVariable(out, getenv("HOME") ? "HOME" : "USERPROFILE");
            i++;
        }
        else if(c == '%' && text.find('%', i + 1) != std::string::npos)
        {
            size_t end = text.find('%', i + 1);
            appendEnvironmentVariable(out, text.substr(i + 1, end - i - 1));
            i = end + 1;
        }
        else if(c == '$' && i + 1 < text.size() && text[i + 1] == '{' && text.find('}', i + 2) != std::string::npos)
        {
            size_t end = text.find('}', i + 2);
            appendEnvironmentVariable(out, text.substr(i + 2, end - i - 2));
            i = end + 1;
        }
        else if(c == '$' && i + 1 < text.size() && (isalpha((unsigned char)text[i + 1]) || text[i + 1] == '_'))
        {
            size_t end = i + 1;
            while(end < text.size() && (isalnum((unsigned char)text[end]) || text[end] == '_'))
            {
                end++;
            }
            appendEnvironmentVariable(out, text.substr(i + 1, end - i - 1));
            i = end;
        }
        else
        {
            out += c;
            i++;
        }
    }

    return out;
}

static void initializeWithKeys()
{
    if(isStringNullOrEmpty(game_key) || isStringNullOrEmpty(secret_key))
    {
        dmLogError("No gameanalytics.game_key or gameanalytics.secret_key set in game.project for target platform!");
        return;
    }

    gameanalytics::defold::Get()->Initialize(game_key, secret_key);
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
        gameanalytics::defold::Get()->SetCustomUserId(s);
        initializeWithKeys();
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
        initializeWithKeys();
    }
    else
    {
        dmLogWarning("Manual initialize is not enabled\n");
    }

    return 0;
}

// [Lua] gameanalytics.addBusinessEvent( options )
static int addBusinessEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char *currency = "";
    lua_Integer amount = 0;
    const char *itemType = "";
    const char *itemId = "";
    const char *cartType = "";
    const char *receipt = "";
    const char *signature = "";
    bool autoFetchReceipt = false;
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

    // Each wrapper does what its platform can with a receipt: ios validates
    // it, android validates it with the signature, the others send the event
    // without it.
    if(autoFetchReceipt)
    {
        gameanalytics::defold::Get()->AddBusinessEventAndAutoFetchReceipt(currency, (int)amount, itemType, itemId, cartType, fields, mergeFields);
    }
    else if(!isStringNullOrEmpty(receipt) || !isStringNullOrEmpty(signature))
    {
        gameanalytics::defold::Get()->AddBusinessEventWithReceipt(currency, (int)amount, itemType, itemId, cartType, receipt, "google_play", signature, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::Get()->AddBusinessEvent(currency, (int)amount, itemType, itemId, cartType, "", fields, mergeFields);
    }

    return 0;
}

// [Lua] gameanalytics.addResourceEvent( options )
static int addResourceEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    EGAResourceFlowType flowType = EGAResourceFlowType::undefined;
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
                        flowType = EGAResourceFlowType::source;
                    }
                    else if(stringCmpi(flowTypeString, "Sink") == 0)
                    {
                        flowType = EGAResourceFlowType::sink;
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
                    mergeFields = lua_toboolean(L, -1);
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

    gameanalytics::defold::Get()->AddResourceEvent(flowType, currency, (float)amount, itemType, itemId, fields, mergeFields);

    return 0;
}

// [Lua] gameanalytics.addProgressionEvent( options )
static int addProgressionEvent( lua_State *L )
{
    DM_LUA_STACK_CHECK(L, 0);
    EGAProgressionStatus progressionStatus = EGAProgressionStatus::undefined;
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
                        progressionStatus = EGAProgressionStatus::complete;
                    }
                    else if(stringCmpi(progressionStatusString, "Start") == 0)
                    {
                        progressionStatus = EGAProgressionStatus::start;
                    }
                    else if(stringCmpi(progressionStatusString, "Fail") == 0)
                    {
                        progressionStatus = EGAProgressionStatus::fail;
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
        gameanalytics::defold::Get()->AddProgressionEventWithScore(progressionStatus, progression01, progression02, progression03, (int)score, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::Get()->AddProgressionEvent(progressionStatus, progression01, progression02, progression03, fields, mergeFields);
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
        gameanalytics::defold::Get()->AddDesignEventWithValue(eventId, (float)value, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::Get()->AddDesignEvent(eventId, fields, mergeFields);
    }

    return 0;
}

// [Lua] gameanalytics.addErrorEvent( options )
static int addErrorEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    EGAErrorSeverity severity = EGAErrorSeverity::undefined;
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
                        severity = EGAErrorSeverity::critical;
                    }
                    else if(stringCmpi(severityString, "Debug") == 0)
                    {
                        severity = EGAErrorSeverity::debug;
                    }
                    else if(stringCmpi(severityString, "Error") == 0)
                    {
                        severity = EGAErrorSeverity::error;
                    }
                    else if(stringCmpi(severityString, "Info") == 0)
                    {
                        severity = EGAErrorSeverity::info;
                    }
                    else if(stringCmpi(severityString, "Warning") == 0)
                    {
                        severity = EGAErrorSeverity::warning;
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

    gameanalytics::defold::Get()->AddErrorEvent(severity, message, fields, mergeFields);

    return 0;
}

// [Lua] gameanalytics.addAdEvent( options )
static int addAdEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    EGAAdAction adAction = EGAAdAction::undefined;
    EGAAdType adType = EGAAdType::undefined;
    const char *adSdkName = "";
    const char *adPlacement = "";
    lua_Integer duration = 0;
    bool sendDuration = false;
    EGAAdError noAdReason = EGAAdError::undefined;
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
                        adAction = EGAAdAction::clicked;
                    }
                    else if (stringCmpi(adActionString, "Show") == 0)
                    {
                        adAction = EGAAdAction::show;
                    }
                    else if (stringCmpi(adActionString, "FailedShow") == 0)
                    {
                        adAction = EGAAdAction::failedshow;
                    }
                    else if (stringCmpi(adActionString, "RewardReceived") == 0)
                    {
                        adAction = EGAAdAction::rewardreceived;
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
                        adType = EGAAdType::video;
                    }
                    else if (stringCmpi(adTypeString, "RewardedVideo") == 0)
                    {
                        adType = EGAAdType::rewardedvideo;
                    }
                    else if (stringCmpi(adTypeString, "Playable") == 0)
                    {
                        adType = EGAAdType::playable;
                    }
                    else if (stringCmpi(adTypeString, "Interstitial") == 0)
                    {
                        adType = EGAAdType::interstitial;
                    }
                    else if (stringCmpi(adTypeString, "OfferWall") == 0)
                    {
                        adType = EGAAdType::offerwall;
                    }
                    else if (stringCmpi(adTypeString, "Banner") == 0)
                    {
                        adType = EGAAdType::banner;
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
                        noAdReason = EGAAdError::unknown;
                    }
                    else if (stringCmpi(noAdReasonString, "Offline") == 0)
                    {
                        noAdReason = EGAAdError::offline;
                    }
                    else if (stringCmpi(noAdReasonString, "NoFill") == 0)
                    {
                        noAdReason = EGAAdError::nofill;
                    }
                    else if (stringCmpi(noAdReasonString, "InternalError") == 0)
                    {
                        noAdReason = EGAAdError::internalerror;
                    }
                    else if (stringCmpi(noAdReasonString, "InvalidRequest") == 0)
                    {
                        noAdReason = EGAAdError::invalidrequest;
                    }
                    else if (stringCmpi(noAdReasonString, "UnableToPrecache") == 0)
                    {
                        noAdReason = EGAAdError::unabletoprecache;
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
        gameanalytics::defold::Get()->AddAdEventWithDuration(adAction, adType, adSdkName, adPlacement, (int64_t)duration, fields, mergeFields);
    }
    else if(noAdReason != EGAAdError::undefined)
    {
        gameanalytics::defold::Get()->AddAdEventWithReason(adAction, adType, adSdkName, adPlacement, noAdReason, fields, mergeFields);
    }
    else
    {
        gameanalytics::defold::Get()->AddAdEvent(adAction, adType, adSdkName, adPlacement, fields, mergeFields);
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

    gameanalytics::defold::Get()->SetInfoLog(lua_toboolean(L, 1));

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

    gameanalytics::defold::Get()->SetVerboseLog(lua_toboolean(L, 1));

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

    gameanalytics::defold::Get()->SetEnabledManualSessionHandling(lua_toboolean(L, 1));

    return 0;
}

// Health events. These are normally switched on in game.project, the lua
// functions are for games that initialize by hand and decide at runtime.

// [Lua] gameanalytics.enableSDKInitEvent( flag )
static int enableSDKInitEvent(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.enableSDKInitEvent(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::Get()->EnableSDKInitEvent(lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.enableFpsHistogram( flag )
static int enableFpsHistogram(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.enableFpsHistogram(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::Get()->EnableFpsHistogram(gameanalytics::defold::FpsTracker(), lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.enableMemoryHistogram( flag )
static int enableMemoryHistogram(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.enableMemoryHistogram(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::Get()->EnableMemoryHistogram(lua_toboolean(L, 1));

    return 0;
}

// [Lua] gameanalytics.enableHealthHardwareInfo( flag )
static int enableHealthHardwareInfo(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TBOOLEAN)
    {
        return luaL_error(L, "gameanalytics.enableHealthHardwareInfo(flag): flag, expected boolean got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::Get()->EnableHealthHardwareInfo(lua_toboolean(L, 1));

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

    gameanalytics::defold::Get()->SetEnabledEventSubmission(lua_toboolean(L, 1), true);

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

    gameanalytics::defold::Get()->SetCustomDimension01(s);

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

    gameanalytics::defold::Get()->SetCustomDimension02(s);

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

    gameanalytics::defold::Get()->SetCustomDimension03(s);

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

    gameanalytics::defold::Get()->SetGlobalCustomEventFields(s);

    return 0;
}

// [Lua] gameanalytics.startSession()
static int startSession(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::Get()->StartSession();

    return 0;
}

// [Lua] gameanalytics.endSession()
static int endSession(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    gameanalytics::defold::Get()->EndSession();

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

    std::string result = gameanalytics::defold::Get()->GetRemoteConfigsValueAsString(cc_key, useDefaultValue ? defaultValue : "");
    lua_pushstring(L, result.c_str());

    return 1;
}

// [Lua] gameanalytics.isRemoteConfigsReady()
static int isRemoteConfigsReady(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    bool result = gameanalytics::defold::Get()->IsRemoteConfigsReady();
    lua_pushboolean( L, result ? 1 : 0 );
    return 1;
}

// [Lua] gameanalytics.getRemoteConfigsContentAsString()
static int getRemoteConfigsContentAsString(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    std::string result = gameanalytics::defold::Get()->GetRemoteConfigsContentAsString();
    lua_pushstring(L, result.c_str());
    return 1;
}

// [Lua] gameanalytics.setRemoteConfigsListener(remoteConfigsListener)
static int setRemoteConfigsListener(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TFUNCTION)
    {
        return luaL_error(L, "gameanalytics.setRemoteConfigsListener(listener): listener, expected function got: %s", LuaTypeName(L, 1));
    }

    if (g_remote_configs_listener)
    {
        dmScript::DestroyCallback(g_remote_configs_listener);
    }
    g_remote_configs_listener = dmScript::CreateCallback(L, 1);
    assert(dmScript::IsCallbackValid(g_remote_configs_listener));

    // The wrapper keeps every listener it is given, so register only once and
    // let the lua callback above be swapped.
    if(!g_remote_configs_listener_registered)
    {
        g_remote_configs_listener_registered = true;
        gameanalytics::defold::Get()->RegisterRemoteConfigListener([](std::string const&)
        {
            g_remote_configs_updated = true;
        });
    }
    return 0;
}

// [Lua] gameanalytics.getRemoteConfigsValueAsNumber(options)
static int getRemoteConfigsValueAsNumber(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    const char *cc_key = "";
    lua_Number defaultValue = 0;

    if(lua_type(L, 1) != LUA_TTABLE)
    {
        return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsNumber(options): options, expected table got: %s", LuaTypeName(L, 1));
    }

    for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1))
    {
        const char *key = lua_tostring(L, -2);

        if(UTF8IsEqual(key, KeyOptionsKey))
        {
            if(lua_type(L, -1) != LUA_TSTRING)
            {
                return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsNumber(options): options.%s, expected string got: %s", KeyOptionsKey, LuaTypeName(L, -1));
            }
            cc_key = lua_tostring(L, -1);
        }
        else if(UTF8IsEqual(key, DefaultValueOptionsKey))
        {
            if(lua_type(L, -1) != LUA_TNUMBER)
            {
                return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsNumber(options): options.%s, expected number got: %s", DefaultValueOptionsKey, LuaTypeName(L, -1));
            }
            defaultValue = lua_tonumber(L, -1);
        }
        else
        {
            return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsNumber(options): Invalid option: '%s'", key);
        }
    }

    if(isStringNullOrEmpty(cc_key))
    {
        return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsNumber(options): options.%s is mandatory and can't be null or empty", KeyOptionsKey);
    }

    lua_pushnumber(L, gameanalytics::defold::Get()->GetRemoteConfigsValueAsNumber(cc_key, defaultValue));
    return 1;
}

// [Lua] gameanalytics.getRemoteConfigsValueAsJson(key)
static int getRemoteConfigsValueAsJson(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    if(lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsJson(key): key, expected string got: %s", LuaTypeName(L, 1));
    }

    const char *key = lua_tostring(L, 1);
    if(isStringNullOrEmpty(key))
    {
        return luaL_error(L, "gameanalytics.getRemoteConfigsValueAsJson(key): key is mandatory and can't be null or empty");
    }

    std::string result = gameanalytics::defold::Get()->GetRemoteConfigsValueAsJson(key);
    lua_pushstring(L, result.c_str());
    return 1;
}

// [Lua] gameanalytics.setExternalUserId(id)
static int setExternalUserId(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    if(lua_type(L, 1) != LUA_TSTRING)
    {
        return luaL_error(L, "gameanalytics.setExternalUserId(id): id, expected string got: %s", LuaTypeName(L, 1));
    }

    gameanalytics::defold::Get()->SetExternalUserId(lua_tostring(L, 1));
    return 0;
}

// Pushes the string a wrapper getter returns.
#define STRING_GETTER(name, getter)                                         \
    static int name(lua_State *L)                                           \
    {                                                                       \
        DM_LUA_STACK_CHECK(L, 1);                                           \
        std::string result = gameanalytics::defold::Get()->getter();        \
        lua_pushstring(L, result.c_str());                                  \
        return 1;                                                           \
    }

// Pushes the time in seconds a wrapper getter returns.
#define TIME_GETTER(name, getter)                                           \
    static int name(lua_State *L)                                           \
    {                                                                       \
        DM_LUA_STACK_CHECK(L, 1);                                           \
        lua_pushnumber(L, (lua_Number)gameanalytics::defold::Get()->getter()); \
        return 1;                                                           \
    }

// [Lua] gameanalytics.getUserId()
STRING_GETTER(getUserId, GetUserId)
// [Lua] gameanalytics.getExternalUserId()
STRING_GETTER(getExternalUserId, GetExternalUserId)
// [Lua] gameanalytics.getABTestingId()
STRING_GETTER(getABTestingId, GetABTestingId)
// [Lua] gameanalytics.getABTestingVariantId()
STRING_GETTER(getABTestingVariantId, GetABTestingVariantId)

// [Lua] gameanalytics.getElapsedSessionTime()
TIME_GETTER(getElapsedSessionTime, GetElapsedSessionTime)
// [Lua] gameanalytics.getElapsedTimeForPreviousSession()
TIME_GETTER(getElapsedTimeForPreviousSession, GetElapsedTimeForPreviousSession)
// [Lua] gameanalytics.getElapsedTimeFromAllSessions()
TIME_GETTER(getElapsedTimeFromAllSessions, GetElapsedTimeFromAllSessions)

#undef STRING_GETTER
#undef TIME_GETTER

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
        {"getRemoteConfigsValueAsNumber", getRemoteConfigsValueAsNumber},
        {"getRemoteConfigsValueAsJson", getRemoteConfigsValueAsJson},

        {"setExternalUserId", setExternalUserId},
        {"getUserId", getUserId},
        {"getExternalUserId", getExternalUserId},
        {"getABTestingId", getABTestingId},
        {"getABTestingVariantId", getABTestingVariantId},

        {"getElapsedSessionTime", getElapsedSessionTime},
        {"getElapsedTimeForPreviousSession", getElapsedTimeForPreviousSession},
        {"getElapsedTimeFromAllSessions", getElapsedTimeFromAllSessions},

        {"enableSDKInitEvent", enableSDKInitEvent},
        {"enableFpsHistogram", enableFpsHistogram},
        {"enableMemoryHistogram", enableMemoryHistogram},
        {"enableHealthHardwareInfo", enableHealthHardwareInfo},

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
    gameanalytics::defold::BindEngine();
    gameanalytics::defold::SetHttpService((HttpService*)ContextRegistryGet(ExtensionAppParamsGetContextRegistry((ExtensionAppParams*)params), HTTP_SERVICE_CONTEXT_NAME));
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Lua extension\n", MODULE_NAME);

    // Usually already set from AppInitializeExtension, this is in case the
    // engine only registers the HTTP service for the extension params.
    gameanalytics::defold::SetHttpService((HttpService*)ContextRegistryGet(ExtensionParamsGetContextRegistry((ExtensionParams*)params), HTTP_SERVICE_CONTEXT_NAME));

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
    bool enable_sdk_init_event = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_sdk_init_event", 0) == 1;
    bool enable_fps_histogram = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_fps_histogram", 0) == 1;
    bool enable_memory_histogram = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_memory_histogram", 0) == 1;
    bool enable_health_hardware_info = dmConfigFile::GetInt(params->m_ConfigFile, "gameanalytics.enable_health_hardware_info", 0) == 1;

    game_key = getPlatformSetting(params->m_ConfigFile, "game_key");
    secret_key = getPlatformSetting(params->m_ConfigFile, "secret_key");
    build = getPlatformSetting(params->m_ConfigFile, "build");

    // Only the desktop wrapper uses it, the others keep the SDK's own storage.
    const char* write_path = getPlatformSetting(params->m_ConfigFile, "write_path");
    if(write_path)
    {
        std::string path = expandEnvironmentVariables(write_path);
        dmLogInfo("write_path=%s\n", path.c_str());
        gameanalytics::defold::Get()->SetWritablePath(path);
    }

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
        gameanalytics::defold::Get()->SetInfoLog(true);
    }
    if(enable_verbose_log)
    {
        gameanalytics::defold::Get()->SetVerboseLog(true);
    }

    gameanalytics::defold::Get()->SetEnabledEventSubmission(enable_event_submission, true);
    gameanalytics::defold::Get()->SetEnabledErrorReporting(use_error_reporting);

    // Before the SDK starts: the init event measures the time up to that point.
    if(enable_sdk_init_event)
    {
        gameanalytics::defold::Get()->EnableSDKInitEvent(true);
    }
    if(enable_fps_histogram)
    {
        gameanalytics::defold::Get()->EnableFpsHistogram(gameanalytics::defold::FpsTracker(), true);
    }
    if(enable_memory_histogram)
    {
        gameanalytics::defold::Get()->EnableMemoryHistogram(true);
    }
    if(enable_health_hardware_info)
    {
        gameanalytics::defold::Get()->EnableHealthHardwareInfo(true);
    }

    if(build)
    {
        gameanalytics::defold::Get()->SetBuild(build);
    }
    if(auto_detect_app_version)
    {
        gameanalytics::defold::Get()->SetAutoDetectAppVersion(true);
    }

    if(dimensions_01)
    {
        gameanalytics::defold::Get()->SetAvailableCustomDimensions01(splitList(dimensions_01));
    }
    if(dimensions_02)
    {
        gameanalytics::defold::Get()->SetAvailableCustomDimensions02(splitList(dimensions_02));
    }
    if(dimensions_03)
    {
        gameanalytics::defold::Get()->SetAvailableCustomDimensions03(splitList(dimensions_03));
    }
    if(resource_currencies)
    {
        gameanalytics::defold::Get()->SetAvailableResourceCurrencies(splitList(resource_currencies));
    }
    if(resource_item_types)
    {
        gameanalytics::defold::Get()->SetAvailableResourceItemTypes(splitList(resource_item_types));
    }

    if(use_manual_session_handling)
    {
        gameanalytics::defold::Get()->SetEnabledManualSessionHandling(true);
    }

    // Both versions are reported as "defold <version>". The engine version
    // comes from sys.get_engine_info().version.
    lua_State* L = params->m_L;
    lua_getglobal(L, "sys");
    lua_getfield(L, -1, "get_engine_info");
    lua_call(L, 0, 1);
    lua_getfield(L, -1, "version");
    const char* engineVersion = lua_tostring(L, -1);
    std::string engine_version = std::string("defold ") + (engineVersion ? engineVersion : "");
    lua_pop(L, 3);

    gameanalytics::defold::Get()->SetSDKVersion(std::string("defold ") + VERSION);
    gameanalytics::defold::Get()->SetGameEngineVersion(engine_version);

    if(!use_custom_id && !manual_initialize)
    {
        initializeWithKeys();
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

static dmExtension::Result UpdateExtension(dmExtension::Params* params)
{
    gameanalytics::defold::OnFrame();

    if(!g_remote_configs_updated.exchange(false) || !g_remote_configs_listener)
    {
        return dmExtension::RESULT_OK;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(g_remote_configs_listener);
    DM_LUA_STACK_CHECK(L, 0);

    if(!dmScript::SetupCallback(g_remote_configs_listener))
    {
        dmLogWarning("SetupCallback failed for remote configs");
        return dmExtension::RESULT_OK;
    }

    dmScript::PCall(L, 1, 0);
    dmScript::TeardownCallback(g_remote_configs_listener);

    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeExtension");
    gameanalytics::defold::ReleaseEngine();
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeExtension");
    gameanalytics::defold::Get()->OnQuit();

    if (g_remote_configs_listener && params->m_L == dmScript::GetCallbackLuaContext(g_remote_configs_listener))
    {
        dmScript::DestroyCallback(g_remote_configs_listener);
        g_remote_configs_listener = 0;
    }

    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeExtension, AppFinalizeExtension, InitializeExtension, UpdateExtension, 0, FinalizeExtension)
