local bc = require("bc");

local function gc()
	bc.cleanup();
end

local bc_meta = {
	__gc = gc
}

_G.bigdecimal_init = {};
debug.setmetatable(_G.bigdecimal_init, bc_meta);
bc.digits(64);

return bc;
