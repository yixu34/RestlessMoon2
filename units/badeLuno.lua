name               = "badeLuno"
isHuman            = 0
isAI               = 1
staticEntity       = 0
weapon1            = "BadeLunoWeapon" 
mesh               = "media/mdl_nme_badeluno.x"
hitPoints          = 1000
hitSound           = "media/sounds/soundFXCD/tyrellHit.bbq"
deathSound	       = "media/sounds/soundFXCD/badeDie.bbq"
footStepSound      = "media/sounds/soundFXCD/Foot_Conc_ShoesLand3.bbq"
speed              = 500
collisionDamage    = 0
coeffOfRestitution = 0
weight             = 1
diesOnImpact       = 0
diesAfterTime      = 0
dropsItemsAtDeath  = 0
deathMode          = 0 -- leaves corpse on death
onHitMode          = 3 -- emits sparks

boundBoxMinX       = -50
boundBoxMinY       = 0
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

-- THIS ONE IS THE IDLE
animations[1] = 3.95
animations[2] = 11.25
animations[3] = 1

-- THIS ONE IS THE RUN CYCLE
animations[4] = 0
animations[5] = 3.76
animations[6] = 3

-- THIS ONE IS THE RELOAD
animations[7] = 11.15
animations[8] = 14.5
animations[9] = 1.6

-- THIS ONE IS THE DEATH CYCLE.
animations[10] = 15
animations[11] = 22.4
animations[12] = 6

--animations[4] = 3.95
--animations[5] = 11.25
--animations[6] = 1

--animations[7] = 11.25
--animations[8] = 15
--animations[9] = 1
