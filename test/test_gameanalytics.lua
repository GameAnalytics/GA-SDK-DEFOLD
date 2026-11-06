-- Unit tests for the lua binding of the gameanalytics extension.
--
-- They run before gameanalytics.initialize() (game.project has
-- manual_initialize = 1), so nothing here sends events.

local API = {
	"configureUserId",
	"initialize",
	"addBusinessEvent",
	"addResourceEvent",
	"addProgressionEvent",
	"addDesignEvent",
	"addErrorEvent",
	"addAdEvent",
	"setEnabledEventSubmission",
	"setEnabledInfoLog",
	"setEnabledVerboseLog",
	"setEnabledManualSessionHandling",
	"setCustomDimension01",
	"setCustomDimension02",
	"setCustomDimension03",
	"setGlobalCustomEventFields",
	"startSession",
	"endSession",
	"getRemoteConfigsValueAsString",
	"isRemoteConfigsReady",
	"getRemoteConfigsContentAsString",
	"setRemoteConfigsListener",
	"getRemoteConfigsValueAsNumber",
	"getRemoteConfigsValueAsJson",
	"setExternalUserId",
	"getUserId",
	"getExternalUserId",
	"getABTestingId",
	"getABTestingVariantId",
	"getElapsedSessionTime",
	"getElapsedTimeForPreviousSession",
	"getElapsedTimeFromAllSessions",
	"enableSDKInitEvent",
	"enableFpsHistogram",
	"enableMemoryHistogram",
	"enableHealthHardwareInfo",
}

-- Calls fn(...) and checks that it raises an error containing message.
-- Telescope only exposes its assert_* functions inside test bodies, so this
-- reports failures with error() instead.
local function assert_error_match(message, fn, ...)
	local ok, err = pcall(fn, ...)
	if ok then
		error("expected an error containing '" .. message .. "'", 2)
	end
	if not tostring(err):find(message, 1, true) then
		error("expected an error containing '" .. message .. "', got: " .. tostring(err), 2)
	end
end

return function()
	describe("gameanalytics module", function()
		test("is registered", function()
			assert_type(gameanalytics, "table")
		end)

		test("exposes the whole lua API", function()
			for _, name in ipairs(API) do
				assert_type(gameanalytics[name], "function", "gameanalytics." .. name)
			end
		end)
	end)

	describe("configureUserId", function()
		test("rejects a non string id", function()
			assert_error_match("expected string got: number", gameanalytics.configureUserId, 1)
		end)

		test("rejects an empty id", function()
			assert_error_match("mandatory", gameanalytics.configureUserId, "")
		end)
	end)

	describe("addDesignEvent", function()
		test("rejects options that are not a table", function()
			assert_error_match("expected table got: string", gameanalytics.addDesignEvent, "level:start")
		end)

		test("requires eventId", function()
			assert_error_match("eventId is mandatory", gameanalytics.addDesignEvent, {})
		end)

		test("rejects a non string eventId", function()
			assert_error_match("options.eventId, expected string got: number", gameanalytics.addDesignEvent, { eventId = 1 })
		end)

		test("rejects a non number value", function()
			assert_error_match("options.value, expected number got: string", gameanalytics.addDesignEvent, { eventId = "a:b", value = "1" })
		end)

		test("rejects unknown options", function()
			assert_error_match("Invalid option: 'foo'", gameanalytics.addDesignEvent, { eventId = "a:b", foo = 1 })
		end)
	end)

	describe("addBusinessEvent", function()
		test("rejects options that are not a table", function()
			assert_error_match("expected table got: no value", gameanalytics.addBusinessEvent)
		end)

		test("rejects a non string currency", function()
			assert_error_match("options.currency, expected string got: number", gameanalytics.addBusinessEvent, { currency = 1 })
		end)
	end)

	describe("flag setters", function()
		local setters = {
			"setEnabledInfoLog",
			"setEnabledVerboseLog",
			"setEnabledManualSessionHandling",
			"setEnabledEventSubmission",
			"enableSDKInitEvent",
			"enableFpsHistogram",
			"enableMemoryHistogram",
			"enableHealthHardwareInfo",
		}

		test("reject non boolean flags", function()
			for _, name in ipairs(setters) do
				assert_error_match("flag, expected boolean got: string", gameanalytics[name], "true")
			end
		end)

		test("accept boolean flags", function()
			gameanalytics.setEnabledInfoLog(true)
			gameanalytics.setEnabledVerboseLog(false)
		end)

		-- Ends with everything on: test.settings enables the health events for
		-- the end-to-end check that runs after the unit tests.
		test("health events switch off and on", function()
			gameanalytics.enableFpsHistogram(false)
			gameanalytics.enableMemoryHistogram(false)
			gameanalytics.enableSDKInitEvent(true)
			gameanalytics.enableFpsHistogram(true)
			gameanalytics.enableMemoryHistogram(true)
			gameanalytics.enableHealthHardwareInfo(true)
		end)
	end)

	describe("custom dimensions", function()
		test("reject non string dimensions", function()
			assert_error_match("expected string got: number", gameanalytics.setCustomDimension01, 1)
			assert_error_match("expected string got: number", gameanalytics.setCustomDimension02, 1)
			assert_error_match("expected string got: number", gameanalytics.setCustomDimension03, 1)
		end)

		test("reject non string global custom event fields", function()
			assert_error_match("expected string got: table", gameanalytics.setGlobalCustomEventFields, {})
		end)
	end)

	describe("remote configs", function()
		test("are not ready before initialize", function()
			assert_false(gameanalytics.isRemoteConfigsReady())
		end)

		test("return a string for a key", function()
			assert_type(gameanalytics.getRemoteConfigsValueAsString({ key = "missing", defaultValue = "fallback" }), "string")
		end)

		test("require a key", function()
			assert_error_match("key is mandatory", gameanalytics.getRemoteConfigsValueAsString, {})
		end)

		test("reject unknown options", function()
			assert_error_match("Invalid option: 'foo'", gameanalytics.getRemoteConfigsValueAsString, { key = "k", foo = "bar" })
		end)

		test("content is a string", function()
			assert_type(gameanalytics.getRemoteConfigsContentAsString(), "string")
		end)

		test("listener must be a function", function()
			assert_error_match("expected function got: string", gameanalytics.setRemoteConfigsListener, "listener")
		end)

		test("return a number for a key", function()
			assert_type(gameanalytics.getRemoteConfigsValueAsNumber({ key = "missing", defaultValue = 42 }), "number")
		end)

		test("number values require a key", function()
			assert_error_match("key is mandatory", gameanalytics.getRemoteConfigsValueAsNumber, { defaultValue = 1 })
		end)

		test("number defaults must be numbers", function()
			assert_error_match("options.defaultValue, expected number got: string", gameanalytics.getRemoteConfigsValueAsNumber, { key = "k", defaultValue = "1" })
		end)

		test("json values take a key", function()
			assert_type(gameanalytics.getRemoteConfigsValueAsJson("missing"), "string")
			assert_error_match("key, expected string got: number", gameanalytics.getRemoteConfigsValueAsJson, 1)
		end)
	end)

	describe("user and A/B testing ids", function()
		test("are strings", function()
			assert_type(gameanalytics.getUserId(), "string")
			assert_type(gameanalytics.getExternalUserId(), "string")
			assert_type(gameanalytics.getABTestingId(), "string")
			assert_type(gameanalytics.getABTestingVariantId(), "string")
		end)

		test("external user id must be a string", function()
			assert_error_match("id, expected string got: number", gameanalytics.setExternalUserId, 1)
		end)
	end)

	describe("session times", function()
		test("are numbers of seconds", function()
			assert_type(gameanalytics.getElapsedSessionTime(), "number")
			assert_type(gameanalytics.getElapsedTimeForPreviousSession(), "number")
			assert_type(gameanalytics.getElapsedTimeFromAllSessions(), "number")
		end)
	end)
end
