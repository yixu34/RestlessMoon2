name               = "Airship"
isHuman            = 0
isAI               = 1
staticEntity       = 0
weapon1            = "AirShipWeapon" 
mesh               = "media/mdl_nme_airship.x"
hitPoints          = 1000
hitSound           = "media/sounds/soundFXCD/airShipHit.bbq"
deathSound	       = "media/sounds/soundFXCD/airshipDie.bbq"
footStepSound      = ""
speed              = 500
collisionDamage    = 0
coeffOfRestitution = 0
weight             = 1
diesOnImpact       = 0
diesAfterTime      = 0
dropsItemsAtDeath  = 1
deathMode          = 1 -- explodes on death
onHitMode          = 3 -- sparks on getting hit

boundBoxMinX       = -800
boundBoxMinY       = 360
boundBoxMinZ       = -800

boundBoxMaxX       = 800
boundBoxMaxY       = 650
boundBoxMaxZ       = 310

radius             = 500

cameraOffsetY      = 250
cameraOffsetZ      = -75

--
-- Animation data
--
animations = {}

-- THIS ONE IS THE ONLY ANIMATION
animations[1] = 0
animations[2] = 3.76
animations[3] = 2

animations[4] = 0
animations[5] = 3.76
animations[6] = 2

animations[7] = 0
animations[8] = 3.76
animations[9] = 2

animations[10] = 0
animations[11] = 3.76
animations[12] = 2

--
--Drop Table Data
--

--values must be less than 100
dropTable = {}
dropTable[1] = 16.6    --health
dropTable[2] = 16.6    --battle rifle ammo
dropTable[3] = 16.6    --rocket launcher ammo
dropTable[4] = 16.6    --shotgun ammo
dropTable[5] = 16.6    --grenade ammo
dropTable[6] = 16.6    --flamethrower ammo
