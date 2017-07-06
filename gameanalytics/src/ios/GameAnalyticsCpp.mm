#include <vector>
#include <string>
#import "ios/GameAnalytics.h"
#import "ios/GameAnalyticsCpp.h"

static bool isStringNullOrEmpty(const char* s)
{
	return s == NULL || strlen(s) == 0;
}

void GameAnalyticsCpp::configureAvailableCustomDimensions01(const std::vector<std::string>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (std::string s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.c_str()]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableCustomDimensions01:list_array];
}

void GameAnalyticsCpp::configureAvailableCustomDimensions02(const std::vector<std::string>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (std::string s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.c_str()]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableCustomDimensions02:list_array];
}

void GameAnalyticsCpp::configureAvailableCustomDimensions03(const std::vector<std::string>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (std::string s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.c_str()]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableCustomDimensions03:list_array];
}

void GameAnalyticsCpp::configureAvailableResourceCurrencies(const std::vector<std::string>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (std::string s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.c_str()]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableResourceCurrencies:list_array];
}

void GameAnalyticsCpp::configureAvailableResourceItemTypes(const std::vector<std::string>& list) {
    NSMutableArray * tmpary = [[NSMutableArray alloc] initWithCapacity: list.size()];
    for (std::string s : list)
    {
        [tmpary addObject: [NSString stringWithUTF8String: s.c_str()]];
    }
    NSArray *list_array = tmpary;
    [GameAnalytics configureAvailableResourceItemTypes:list_array];
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

    [GameAnalytics initializeWithGameKey:gameKeyString gameSecret:gameSecretString];
}

void GameAnalyticsCpp::addBusinessEvent(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType, const char *receipt) {
    NSString *currencyString = !isStringNullOrEmpty(currency) ? [NSString stringWithUTF8String:currency] : nil;
    NSInteger amountInteger = (NSInteger)amount;
    NSString *itemTypeString = !isStringNullOrEmpty(itemType) ? [NSString stringWithUTF8String:itemType] : nil;
    NSString *itemIdString = !isStringNullOrEmpty(itemId) ? [NSString stringWithUTF8String:itemId] : nil;
    NSString *cartTypeString = !isStringNullOrEmpty(cartType) ? [NSString stringWithUTF8String:cartType] : nil;
    NSString *receiptString = !isStringNullOrEmpty(receipt) ? [NSString stringWithUTF8String:receipt] : nil;

    [GameAnalytics addBusinessEventWithCurrency:currencyString
                                         amount:amountInteger
                                       itemType:itemTypeString
                                         itemId:itemIdString
                                       cartType:cartTypeString
                                        receipt:receiptString];
}

void GameAnalyticsCpp::addBusinessEventAndAutoFetchReceipt(const char *currency, int amount, const char *itemType, const char *itemId, const char *cartType) {
    NSString *currencyString = !isStringNullOrEmpty(currency) ? [NSString stringWithUTF8String:currency] : nil;
    NSInteger amountInteger = (NSInteger)amount;
    NSString *itemTypeString = !isStringNullOrEmpty(itemType) ? [NSString stringWithUTF8String:itemType] : nil;
    NSString *itemIdString = !isStringNullOrEmpty(itemId) ? [NSString stringWithUTF8String:itemId] : nil;
    NSString *cartTypeString = !isStringNullOrEmpty(cartType) ? [NSString stringWithUTF8String:cartType] : nil;

    [GameAnalytics addBusinessEventWithCurrency:currencyString
                                         amount:amountInteger
                                       itemType:itemTypeString
                                         itemId:itemIdString
                                       cartType:cartTypeString
                               autoFetchReceipt:TRUE];
}

void GameAnalyticsCpp::addResourceEvent(int flowType, const char *currency, float amount, const char *itemType, const char *itemId) {
    NSString *currencyString = !isStringNullOrEmpty(currency) ? [NSString stringWithUTF8String:currency] : nil;
    NSNumber *amountNumber = [NSNumber numberWithFloat:amount];
    NSString *itemTypeString = !isStringNullOrEmpty(itemType) ? [NSString stringWithUTF8String:itemType] : nil;
    NSString *itemIdString = !isStringNullOrEmpty(itemId) ? [NSString stringWithUTF8String:itemId] : nil;

    [GameAnalytics addResourceEventWithFlowType:(GAResourceFlowType)flowType
                                       currency:currencyString
                                         amount:amountNumber
                                       itemType:itemTypeString
                                         itemId:itemIdString];
}

void GameAnalyticsCpp::addProgressionEvent(int progressionStatus, const char *progression01, const char *progression02, const char *progression03) {
    NSString *progression01String = !isStringNullOrEmpty(progression01) ? [NSString stringWithUTF8String:progression01] : nil;
    NSString *progression02String = !isStringNullOrEmpty(progression02) ? [NSString stringWithUTF8String:progression02] : nil;
    NSString *progression03String = !isStringNullOrEmpty(progression03) ? [NSString stringWithUTF8String:progression03] : nil;

    [GameAnalytics addProgressionEventWithProgressionStatus:(GAProgressionStatus)progressionStatus
                                              progression01:progression01String
                                              progression02:progression02String
                                              progression03:progression03String];
}

void GameAnalyticsCpp::addProgressionEventWithScore(int progressionStatus, const char *progression01, const char *progression02, const char *progression03, int score) {
    NSString *progression01String = !isStringNullOrEmpty(progression01) ? [NSString stringWithUTF8String:progression01] : nil;
    NSString *progression02String = !isStringNullOrEmpty(progression02) ? [NSString stringWithUTF8String:progression02] : nil;
    NSString *progression03String = !isStringNullOrEmpty(progression03) ? [NSString stringWithUTF8String:progression03] : nil;

    [GameAnalytics addProgressionEventWithProgressionStatus:(GAProgressionStatus)progressionStatus
                                              progression01:progression01String
                                              progression02:progression02String
                                              progression03:progression03String
                                                      score:score];
}

void GameAnalyticsCpp::addDesignEvent(const char *eventId) {
    NSString *eventIdString = !isStringNullOrEmpty(eventId) ? [NSString stringWithUTF8String:eventId] : nil;

    [GameAnalytics addDesignEventWithEventId:eventIdString value:nil];
}

void GameAnalyticsCpp::addDesignEventWithValue(const char *eventId, float value) {
    NSString *eventIdString = !isStringNullOrEmpty(eventId) ? [NSString stringWithUTF8String:eventId] : nil;
    NSNumber *valueNumber = [NSNumber numberWithFloat:value];

    [GameAnalytics addDesignEventWithEventId:eventIdString value:valueNumber];
}

void GameAnalyticsCpp::addErrorEvent(int severity, const char *message) {
    NSString *messageString = !isStringNullOrEmpty(message) ? [NSString stringWithUTF8String:message] : nil;

    [GameAnalytics addErrorEventWithSeverity:(GAErrorSeverity)severity message:messageString];
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

void GameAnalyticsCpp::setFacebookId(const char *facebookId) {
    NSString *facebookIdString = !isStringNullOrEmpty(facebookId) ? [NSString stringWithUTF8String:facebookId] : nil;
    [GameAnalytics setFacebookId:facebookIdString];
}

void GameAnalyticsCpp::setGender(const char *gender) {
    NSString *genderString = !isStringNullOrEmpty(gender) ? [NSString stringWithUTF8String:gender] : nil;
    [GameAnalytics setGender:genderString];
}

void GameAnalyticsCpp::setBirthYear(int birthYear) {
    NSInteger birthYearInteger = (NSInteger)birthYear;
    [GameAnalytics setBirthYear:birthYearInteger];
}

void GameAnalyticsCpp::startSession() {
    [GameAnalytics startSession];
}

void GameAnalyticsCpp::endSession() {
    [GameAnalytics endSession];
}
