-- test file.
-- execute with: LUA_CPATH="/usr/local/openresty/lualib/?.so;;" /usr/local/openresty/luajit/bin/luajit phone.lua

local phone = require("luaphonenumber")

local result = phone.parse("+34609666666", "ES", "en", "ES")

for key, value in pairs(result) do
    print(key, value)
end
