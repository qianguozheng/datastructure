local loop=require("uloop")
uloop.init()

function t1()
    print("1000 ms timer run")
    print("t1 runs only once")
end

uloop.timer(t1, 1000)

local timer
function t2()
    print("2000 ms timer run")
    timer:set(2000)
end

timer = uloop.timer(t2)
timer:set(2000)

uloop.timer(function() print("3000 ms timer run"); end, 3000):cancel()
uloop.run()
