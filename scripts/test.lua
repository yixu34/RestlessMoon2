--firstWaitId  = attachProcess("proc_wait", 3500)
--soundId      = createProcess("proc_sound", "blah.wav")
--secondWaitId = createProcess("proc_wait", 3500)
--quitId       = createProcess("proc_quit")

--setNextProcess(firstWaitId, soundId)
--setNextProcess(soundId, secondWaitId)
--setNextProcess(secondWaitId, quitId)
--setNextProcess(secondWaitId, musicId)

--weapons = {"weapon1.lua", "weapon2.lua", "weapon3.lua"}
weapons = {}
weapons[1] = "weapon1.lua"
weapons[2] = "weapon2.lua"
weapons[3] = "weapon3.lua"
