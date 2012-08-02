--dofile("scripts/playerbindings.lua")

bind("escape","quit()")

bind("left",   "+yawLeft()")
bind("right",  "+yawRight()")
bind("up",     "+pitchUp()")
bind("down",   "+pitchDown()")
bind("A",      "+strafeLeft()")
bind("D",      "+strafeRight()")
bind("space",  "+jump()")
bind("C",      "+crouch()")
bind("S",      "+walkBackward()")
bind("W",      "+walkForward()")
bind("R",      "reload()")
bind("shift",  "+dash()")
bind("mouse1", "+shoot()")
bind("mwheeldown", "nextWeapon()")
bind("mwheelup",   "previousWeapon()")
bind("Q",      "+nextWeapon()")
bind("Z",      "+previousWeapon()") 
bind("G",      "+throwGrenade()")
bind("1",	   "+toogleWeaponOne()")
bind("2",	   "+toogleWeaponTwo()")
bind("3",	   "+toogleWeaponThree()")
bind("4",	   "+toogleWeaponFour()")
bind("5",	   "+toogleWeaponFive()")
bind("6",	   "+toogleWeaponSix()")
bind("7",	   "+toogleWeaponSeven()")
bind("8",	   "+toogleWeaponEight()")
bind("9",	   "+toogleWeaponNine()")
bind("0",	   "+toogleWeaponTen()")

bind("Y",      'set("aiEnabled", 0)')
bind("U",      'set("aiEnabled", 1)')

bind("`",      "toggleConsole()")
bind("esc",    "cancel()")

bind("P", [[createTextLabelMenu(]] ..
               [['game', 0.3, 0.4,]] ..
               [['Create Crate',                 'editAddEntity("units/crate.lua")',]] ..
               [['Create Normal Imp',            'editAddEntity("units/imp.lua")',]] ..
               [['Create Normal Soldier',        'editAddEntity("units/soldier.lua")',]] ..
               [['Create Normal Elite',          'editAddEntity("units/elite.lua")',]] ..
               [['Create Though Imp',            'editAddEntity("units/toughImp.lua")',]] ..
               [['Create Though Soldier',        'editAddEntity("units/toughSoldier.lua")',]] ..
               [['Create Though Elite',          'editAddEntity("units/toughElite.lua")',]] ..
               [['Create Badass Imp',            'editAddEntity("units/badassImp.lua")',]] ..
               [['Create Badass Soldier',        'editAddEntity("units/badassSoldier.lua")',]] ..
               [['Create Badass Elite',          'editAddEntity("units/badassElite.lua")',]] ..
               [['Create Bade Luno',             'editAddEntity("units/badeLuno.lua")',]] ..
               [['Create Health PowerUp',        'editAddPowerUp("Health")',]] ..
               [['Create Grenade Pickup',        'editAddPowerUp("GrenadeAmmo")',]] ..
               [['Create Battle Rifle Pickup',   'editAddPowerUp("BattleRifleAmmo")',]] ..
               [['Create Flamethrower Pickup',   'editAddPowerUp("FlamethrowerAmmo")',]] ..
               [['Create Rocket Lancher Pickup', 'editAddPowerUp("RocketLauncherAmmo")',]] ..
               [['Create Shotgun Pickup',        'editAddPowerUp("ShotGunAmmo")',]] ..
               [['R', '',]] ..
               [['S', '',]] ..
               [['T', '',]] ..
               [['U', '',]] ..
               [['V', '',]] ..
               [['W', '',]] ..
               [['X', '',]] ..
               [['Y', '',]] ..
               [['Z', '')]])
                               
