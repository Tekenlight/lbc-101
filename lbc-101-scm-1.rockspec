package = "lbc-101"
version = "scm-1"
source = {
   url = "git+https://github.com/Tekenlight/lbc-101"
}
description = {
   summary = "To build the library and run a simple test, just do make.",
   detailed = [[
To build the library and run a simple test, just do make.
If Lua is not installed in /usr/local, tell make:
	make LUA_TOPDIR=/var/tmp/lhf/lua-5.3.5/install]],
   homepage = "https://github.com/Tekenlight/lbc-101",
   license = [[ MIT License

Copyright (c) 2021 Tekenlight

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.]]

}
dependencies = {}
build = {
	type = "builtin",
	modules = {
		bigdecimal = "bigdecimal.lua",
		bc = {
			sources = {
				"lbc.c",
				"src/number.c"
			},
			incdirs = { "src" }
		}
	},
}
