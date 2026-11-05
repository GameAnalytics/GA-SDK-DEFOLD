-- End-to-end check against the GameAnalytics servers, shared by the test
-- runner (test/test.script) and the sample's "Run self test" button.
--
-- It initializes the SDK, waits for the remote configs (which proves the init
-- request made it through the platform's HTTP stack), sends one event of each
-- type and then waits for the event queue to flush. Whether the events were
-- accepted only shows in the SDK's log ("Event queue: N events sent"), so CI
-- checks that line in the output.
--
-- Usage:
--   local e2e = require "test.e2e"
--   local run = e2e.start({ initialize = true, log = print })
--   -- every frame, until it returns a result:
--   local result = run:update(dt)   -- nil, or { ok, retryable, message }
--
-- A result is retryable when it timed out waiting for the servers, so a CI
-- run can try again; anything else is a real failure.

local M = {}

M.TIMEOUT = 30 -- seconds to wait for the remote configs
M.FLUSH = 12   -- seconds to let the event queue send what was added

local function send_events()
	gameanalytics.addDesignEvent({ eventId = "e2e:design", value = 1 })
	gameanalytics.addBusinessEvent({ currency = "USD", amount = 1, itemType = "boost", itemId = "e2e_item", cartType = "e2e" })
	gameanalytics.addResourceEvent({ flowType = "Source", currency = "gold", amount = 1, itemType = "boost", itemId = "e2e_item" })
	gameanalytics.addProgressionEvent({ progressionStatus = "Start", progression01 = "e2e" })
	gameanalytics.addProgressionEvent({ progressionStatus = "Complete", progression01 = "e2e", score = 1 })
	gameanalytics.addErrorEvent({ severity = "Info", message = "e2e error event" })
	gameanalytics.addAdEvent({ adAction = "Show", adType = "Interstitial", adSdkName = "e2e", adPlacement = "e2e" })
end

local Run = {}
Run.__index = Run

function Run:finish(ok, retryable, message)
	self.result = { ok = ok, retryable = retryable, message = message }
	self.log("E2E: " .. (ok and "PASS" or (retryable and "TIMEOUT" or "FAIL")) .. " - " .. message)
	return self.result
end

function Run:update(dt)
	if self.result then
		return self.result
	end
	self.t = self.t + dt

	if self.state == "waiting" then
		if gameanalytics.isRemoteConfigsReady() then
			self.log(string.format("E2E: remote configs ready after %.1fs", self.t))
			local user_id = gameanalytics.getUserId()
			if user_id == "" then
				return self:finish(false, false, "initialized but the user id is empty")
			end
			local ok, err = pcall(send_events)
			if not ok then
				return self:finish(false, false, "sending events failed: " .. tostring(err))
			end
			self.log("E2E: events added, waiting " .. M.FLUSH .. "s for the queue to send them")
			self.state = "flushing"
			self.t = 0
		elseif self.t > M.TIMEOUT then
			return self:finish(false, true, "no remote configs after " .. M.TIMEOUT .. "s")
		end
	elseif self.state == "flushing" and self.t > M.FLUSH then
		return self:finish(true, false, "initialized, remote configs received, events sent to the queue")
	end
end

-- options.initialize: call gameanalytics.initialize() first
-- options.log: function(message), print by default
function M.start(options)
	options = options or {}
	local run = setmetatable({ t = 0, state = "waiting", log = options.log or print }, Run)
	if options.initialize then
		local ok, err = pcall(gameanalytics.initialize)
		if not ok then
			run:finish(false, false, "initialize failed: " .. tostring(err))
		end
	end
	run.log("E2E: started, waiting up to " .. M.TIMEOUT .. "s for the remote configs")
	return run
end

return M
