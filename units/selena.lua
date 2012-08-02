name               = "Selena"
isHuman            = 1
isAI               = 0
staticEntity       = 0
mesh               = "media/mdl_firstperson.x"
hitPoints          = 100
hitSound           = "media/sounds/Selenahit08.bbq"
deathSound	       = "media/sounds/soundFXCD/Female_Grunt01.bbq"
footStepSound      = "media/sounds/soundFXCD/Foot_Conc_ShoesLand3.bbq"
speed              = 800
collisionDamage    = 0
coeffOfRestitution = 0
weight             = 1
diesOnImpact       = 0
diesAfterTime      = 0
dropsItemsAtDeath  = 0
deathMode          = 0 -- leaves corpse on death
onHitMode          = 1 -- emits red blood

boundBoxMinX       = -50
boundBoxMinY       = 100
boundBoxMinZ       = -50

boundBoxMaxX       = 50
boundBoxMaxY       = 300
boundBoxMaxZ       = 50

radius             = 150

cameraOffsetY      = 250
cameraOffsetZ      = -75

--
-- Animation data
--
animations = {}

-- Idle
animations[1] = 0
animations[2] = 3.75
animations[3] = 1

-- Run/shoot
animations[4] = 0
animations[5] = 3.75
animations[6] = 1

-- Reload
animations[7] = 3.75
animations[8] = 7.5
animations[9] = 1 -- Leave this alone; weapons have different reload speeds.

-- Death
animations[10] = 7.5
animations[11] = 11.15
animations[12] = 3
