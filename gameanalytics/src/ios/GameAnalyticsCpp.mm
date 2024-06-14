#include <vector>
#import "ios/GameAnalytics.h"
#import "ios/GameAnalyticsCpp.h"
#include <string.h>
#include <stdio.h>
#define DLIB_LOG_DOMAIN "GameAnalytics"
#include <dmsdk/dlib/log.h>

dmScript::LuaCallbackInfo* _remote_configs_listener;

@interface GARemoteConfigsDefoldDelegate : NSObject<GARemoteConfigsDelegate>
{
}

- (void) onRemoteConfigsUpdated;

@end

@implementation GARemoteConfigsDefoldDelegate

- (void)onRemoteConfigsUpdated
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

@end

GARemoteConfigsDefoldDelegate* ga_rc_delegate = nil;

static bool isStringNullOrEmpty(const char* s)
{
    return s == NULL || strlen(s) == 0;
}

void GameAnalyticsCpp::configureAvailableCustomDimensions01(const std::vector<CharArray>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (CharArray s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.array]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableCustomDimensions01:list_array];
}

void GameAnalyticsCpp::configureAvailableCustomDimensions02(const std::vector<CharArray>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (CharArray s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.array]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableCustomDimensions02:list_array];
}

void GameAnalyticsCpp::configureAvailableCustomDimensions03(const std::vector<CharArray>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (CharArray s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.array]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableCustomDimensions03:list_array];
}

void GameAnalyticsCpp::configureAvailableResourceCurrencies(const std::vector<CharArray>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (CharArray s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.array]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableResourceCurrencies:list_array];
}

void GameAnalyticsCpp::configureAvailableResourceItemTypes(const std::vector<CharArray>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (CharArray s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.array]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableResourceItemTypes:list_array];
}

void GameAnalyticsCpp::configureAutoDetectAppVersion(bool flag) {
    [GameAnalytics configureAutoDetectAppVersion:flag];
}

void GameAnalyticsCpp::configureBuild(const char *build) {
    NSString *buildString = !isStringNullOrEmpty(build) ? [NSString stringWithUTF8String:build] : nil;
    [GameAnalytics configureBuild:buildString];
}

void GameAnalyticsCpp::configureUserId(const char *userId) {
    NSString *userIdString = !isStringNullOrEmpty(userId) ? [NSString stringWithUTF8String:userId] : nil;
    [GameAnalytics configureUserId:userIdString];
}

void GameAnalyticsCpp::configureSdkGameEngineVersion(const char *gameEngineSdkVersion) {
    NSString *gameEngineSdkVersionString = !isStringNullOrEmpty(gameEngineSdkVersion) ? [NSString stringWithUTF8String:gameEngineSdkVersion] : nil;
    [GameAnalytics configureSdkVersion:gameEngineSdkVersionString];
}

void GameAnalyticsCpp::configureGameEngineVersion(const char *gameEngineVersion) {
    NSString *gameEngineVersionString = !isStringNullOrEmpty(gameEngineVersion) ? [NSString stringWithUTF8String:gameEngineVersion] : nil;
    [GameAnalytics configureEngineVersion:gameEngineVersionString];
}

void GameAnalyticsCpp::initialize(const char *gameKey, const char *gameSecret) {
    NSString *gameKeyString = !isStringNullOrEmpty(gameKey) ? [NSString stringWithUTF8String:gameKey] : nil;
    NSString *gameSecretString = !isStringNullOrEmpty(gameSecret) ? [NSString stringWithUTF8String:gameSecret] : nil;

    ga_rc_delegate = [[GARemoteConfigsDefoldDelegate alloc] init];
    [GameAnalytics setRemoteConfigsDelegate:ga_rc_delegate];

    [GameAnalytics initializeWithGameKey:gameKeyString gameSecret:gameSecretString];
}

void GameAnalyticsCpp::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt, const char *fields, bool mergeFields) {
    NSString *currencyString = !isStringNullOrEmpty(currency) ? [NSString stringWithUTF8String:currency] : nil;
    NSInteger amountInteger = (NSInteger)amount;
    NSString *itemTypeString = !isStringNullOrEmpty(itemType) ? [NSString stringWithUTF8String:itemType] : nil;
    NSString *itemIdString = !isStringNullOrEmpty(itemId) ? [NSString stringWithUTF8String:itemId] : nil;
    NSString *cartTypeString = !isStringNullOrEmpty(cartType) ? [NSString stringWithUTF8String:cartType] : nil;
    NSString *receiptString = !isStringNullOrEmpty(receipt) ? [NSString stringWithUTF8String:receipt] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addBusinessEventWithCurrency:currencyString
                                         amount:amountInteger
                                       itemType:itemTypeString
                                         itemId:itemIdString
                                       cartType:cartTypeString
                                        receipt:receiptString
                                         customFields:fields_dict
                                         mergeFields:mergeFields];
}

void GameAnalyticsCpp::addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *fields, bool mergeFields) {
    NSString *currencyString = !isStringNullOrEmpty(currency) ? [NSString stringWithUTF8String:currency] : nil;
    NSInteger amountInteger = (NSInteger)amount;
    NSString *itemTypeString = !isStringNullOrEmpty(itemType) ? [NSString stringWithUTF8String:itemType] : nil;
    NSString *itemIdString = !isStringNullOrEmpty(itemId) ? [NSString stringWithUTF8String:itemId] : nil;
    NSString *cartTypeString = !isStringNullOrEmpty(cartType) ? [NSString stringWithUTF8String:cartType] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addBusinessEventWithCurrency:currencyString
                                         amount:amountInteger
                                       itemType:itemTypeString
                                         itemId:itemIdString
                                       cartType:cartTypeString
                               autoFetchReceipt:TRUE
                                         customFields:fields_dict
                                         mergeFields:mergeFields];
}

#import <objc/runtime.h>

void GameAnalyticsCpp::addResourceEvent(int flowType, const char *currency, float amount, const char *itemType, const char *itemId, const char *fields, bool mergeFields) {
    NSString *currencyString = !isStringNullOrEmpty(currency) ? [NSString stringWithUTF8String:currency] : nil;
    NSNumber *amountNumber = [NSNumber numberWithFloat:amount];
    NSString *itemTypeString = !isStringNullOrEmpty(itemType) ? [NSString stringWithUTF8String:itemType] : nil;
    NSString *itemIdString = !isStringNullOrEmpty(itemId) ? [NSString stringWithUTF8String:itemId] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addResourceEventWithFlowType:(GAResourceFlowType)flowType
                                       currency:currencyString
                                         amount:amountNumber
                                        itemType:itemTypeString
                                        itemId:itemIdString
                                        customFields:fields_dict
                                        mergeFields:(BOOL)mergeFields];                  
}

void GameAnalyticsCpp::addProgressionEvent(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, const char *fields, bool mergeFields) {
    NSString *progression01String = !isStringNullOrEmpty(progression01) ? [NSString stringWithUTF8String:progression01] : nil;
    NSString *progression02String = !isStringNullOrEmpty(progression02) ? [NSString stringWithUTF8String:progression02] : nil;
    NSString *progression03String = !isStringNullOrEmpty(progression03) ? [NSString stringWithUTF8String:progression03] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addProgressionEventWithProgressionStatus:(GAProgressionStatus)progressionStatus
                                              progression01:progression01String
                                              progression02:progression02String
                                              progression03:progression03String
                                                     customFields:fields_dict
                                                     mergeFields:mergeFields];
}

void GameAnalyticsCpp::addProgressionEventWithScore(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score, const char *fields, bool mergeFields) {
    NSString *progression01String = !isStringNullOrEmpty(progression01) ? [NSString stringWithUTF8String:progression01] : nil;
    NSString *progression02String = !isStringNullOrEmpty(progression02) ? [NSString stringWithUTF8String:progression02] : nil;
    NSString *progression03String = !isStringNullOrEmpty(progression03) ? [NSString stringWithUTF8String:progression03] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addProgressionEventWithProgressionStatus:(GAProgressionStatus)progressionStatus
                                              progression01:progression01String
                                              progression02:progression02String
                                              progression03:progression03String
                                                      score:score
                                                     customFields:fields_dict
                                                     mergeFields:mergeFields];
}

void GameAnalyticsCpp::addDesignEvent(const char *eventId, const char *fields, bool mergeFields) {
    NSString *eventIdString = !isStringNullOrEmpty(eventId) ? [NSString stringWithUTF8String:eventId] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addDesignEventWithEventId:eventIdString value:nil customFields:fields_dict mergeFields:mergeFields];
}

void GameAnalyticsCpp::addDesignEventWithValue(const char *eventId, float value, const char *fields, bool mergeFields) {
    NSString *eventIdString = !isStringNullOrEmpty(eventId) ? [NSString stringWithUTF8String:eventId] : nil;
    NSNumber *valueNumber = [NSNumber numberWithFloat:value];
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addDesignEventWithEventId:eventIdString value:valueNumber customFields:fields_dict mergeFields:mergeFields];
}

void GameAnalyticsCpp::addErrorEvent(int severity, const char *message, const char *fields, bool mergeFields) {
    NSString *messageString = !isStringNullOrEmpty(message) ? [NSString stringWithUTF8String:message] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addErrorEventWithSeverity:(GAErrorSeverity)severity message:messageString customFields:fields_dict mergeFields:mergeFields];
}

void GameAnalyticsCpp::addAdEvent(int adAction, int adType, const char *adSdkName, const char *adPlacement, const char *fields, bool mergeFields)
{
    NSString *adSdkNameString = !isStringNullOrEmpty(adSdkName) ? [NSString stringWithUTF8String:adSdkName] : nil;
    NSString *adPlacementString = !isStringNullOrEmpty(adPlacement) ? [NSString stringWithUTF8String:adPlacement] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addAdEventWithAction:(GAAdAction)adAction adType:(GAAdType)adType adSdkName:adSdkNameString adPlacement:adPlacementString customFields:fields_dict mergeFields:mergeFields];
}

void GameAnalyticsCpp::addAdEventWithDuration(int adAction, int adType, const char *adSdkName, const char *adPlacement, int duration, const char *fields, bool mergeFields)
{
    NSString *adSdkNameString = !isStringNullOrEmpty(adSdkName) ? [NSString stringWithUTF8String:adSdkName] : nil;
    NSString *adPlacementString = !isStringNullOrEmpty(adPlacement) ? [NSString stringWithUTF8String:adPlacement] : nil;
    NSInteger durationInteger = (NSInteger)duration;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addAdEventWithAction:(GAAdAction)adAction adType:(GAAdType)adType adSdkName:adSdkNameString adPlacement:adPlacementString duration:durationInteger customFields :fields_dict mergeFields:mergeFields];
}

void GameAnalyticsCpp::addAdEventWithNoAdReason(int adAction, int adType, const char *adSdkName, const char *adPlacement, int noAdReason, const char *fields, bool mergeFields)
{
    NSString *adSdkNameString = !isStringNullOrEmpty(adSdkName) ? [NSString stringWithUTF8String:adSdkName] : nil;
    NSString *adPlacementString = !isStringNullOrEmpty(adPlacement) ? [NSString stringWithUTF8String:adPlacement] : nil;
    NSString *fieldsString = fields != NULL ? [NSString stringWithUTF8String:fields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }

    [GameAnalytics addAdEventWithAction:(GAAdAction)adAction adType:(GAAdType)adType adSdkName:adSdkNameString adPlacement:adPlacementString noAdReason:(GAAdError)noAdReason customFields:fields_dict mergeFields:mergeFields];
}

void GameAnalyticsCpp::setEnabledInfoLog(bool flag) {
    [GameAnalytics setEnabledInfoLog:flag];
}

void GameAnalyticsCpp::setEnabledVerboseLog(bool flag) {
    [GameAnalytics setEnabledVerboseLog:flag];
}

void GameAnalyticsCpp::setEnabledManualSessionHandling(bool flag) {
    [GameAnalytics setEnabledManualSessionHandling:flag];
}

void GameAnalyticsCpp::setEnabledEventSubmission(bool flag) {
    [GameAnalytics setEnabledEventSubmission:flag];
}

void GameAnalyticsCpp::setEnabledErrorReporting(bool flag) {
    [GameAnalytics setEnabledErrorReporting:flag];
}

void GameAnalyticsCpp::setCustomDimension01(const char *customDimension) {
    NSString *customDimensionString = !isStringNullOrEmpty(customDimension) ? [NSString stringWithUTF8String:customDimension] : nil;
    [GameAnalytics setCustomDimension01:customDimensionString];
}

void GameAnalyticsCpp::setCustomDimension02(const char *customDimension) {
    NSString *customDimensionString = !isStringNullOrEmpty(customDimension) ? [NSString stringWithUTF8String:customDimension] : nil;
    [GameAnalytics setCustomDimension02:customDimensionString];
}

void GameAnalyticsCpp::setCustomDimension03(const char *customDimension) {
    NSString *customDimensionString = !isStringNullOrEmpty(customDimension) ? [NSString stringWithUTF8String:customDimension] : nil;
    [GameAnalytics setCustomDimension03:customDimensionString];
}

void GameAnalyticsCpp::setGlobalCustomEventFields(const char *customFields) {
    NSString *fieldsString = customFields != NULL ? [NSString stringWithUTF8String:customFields] : nil;
    NSDictionary *fields_dict = nil;
    if (fieldsString) {
        fields_dict = [NSJSONSerialization JSONObjectWithData:[fieldsString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    }
    [GameAnalytics setGlobalCustomEventFields:fields_dict];
}

void GameAnalyticsCpp::startSession() {
    [GameAnalytics startSession];
}

void GameAnalyticsCpp::endSession() {
    [GameAnalytics endSession];
}

std::vector<char> GameAnalyticsCpp::getRemoteConfigsValueAsString(const char *key) {
    NSString *keyString = key != NULL ? [NSString stringWithUTF8String:key] : nil;
    NSString *returnValue = [GameAnalytics getRemoteConfigsValueAsString:keyString];

    std::vector<char> result;
    if(returnValue != nil)
    {
        const char *s = [returnValue UTF8String];
        size_t size = strlen(s);
        for(size_t i = 0; i < size; ++i)
        {
            result.push_back(s[i]);
        }
    }
    result.push_back('\0');
    return result;
}

std::vector<char> GameAnalyticsCpp::getRemoteConfigsValueAsString(const char *key, const char *defaultValue) {
    NSString *keyString = key != NULL ? [NSString stringWithUTF8String:key] : nil;
    NSString *defaultValueString = key != NULL ? [NSString stringWithUTF8String:defaultValue] : nil;
    NSString *returnValue = [GameAnalytics getRemoteConfigsValueAsString:keyString defaultValue:defaultValueString];

    std::vector<char> result;
    if(returnValue != nil)
    {
        const char *s = [returnValue UTF8String];
        size_t size = strlen(s);
        for(size_t i = 0; i < size; ++i)
        {
            result.push_back(s[i]);
        }
    }
    result.push_back('\0');
    return result;
}

bool GameAnalyticsCpp::isRemoteConfigsReady() {
    return [GameAnalytics isRemoteConfigsReady] ? true : false;
}

void GameAnalyticsCpp::setRemoteConfigsListener(dmScript::LuaCallbackInfo* listener)
{
    dmLogWarning("setRemoteConfigsListener called");
    _remote_configs_listener = listener;
}

std::vector<char> GameAnalyticsCpp::getRemoteConfigsContentAsString() {
    NSString *returnValue = [GameAnalytics getRemoteConfigsContentAsString];

    std::vector<char> resultVector;
    if(returnValue != nil)
    {
        const char *s = [returnValue UTF8String];
        size_t size = strlen(s);
        for(size_t i = 0; i < size; ++i)
        {
            resultVector.push_back(s[i]);
        }
    }
    resultVector.push_back('\0');
    return resultVector;
}
