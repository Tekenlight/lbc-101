local bc = require("bc");

local function gc()
	--print(debug.getinfo(1).source, debug.getinfo(1).currentline);
	--print("Will do nothing for now");
	--bc.cleanup();
	--print(debug.getinfo(1).source, debug.getinfo(1).currentline);
end

local bc_meta = {
	__gc = gc
}

_G.bigdecimal_init = {};
--debug.setmetatable(_G.bigdecimal_init, bc_meta);
bc.digits(64);

return bc;
