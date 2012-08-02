--
--These are options for starting up the game.
--
screen_width  = 1024
screen_height = 768 
windowed      = 0
cursor        = "media/cursor.png"

function onTestEvent(x, y)
    scriptLog(string.format("onTestEvent() called:  %d %d", x, y))
end
registerCallback("ev_test", "onTestEvent")
