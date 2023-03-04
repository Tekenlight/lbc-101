local bc = require("bc");

local function gc()
	bc.cleanup();
end

local bc_meta = {
	__gc = gc
}
debug.setmetatable({}, bc_meta);
bc.digits(64);

return bc;
