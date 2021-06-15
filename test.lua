local Bfptr = require "bfptr"

local t = {1,2,3,4,5.5,6,7}
for i = 1, 7 do
    print(Bfptr.topk(t, i))
end
