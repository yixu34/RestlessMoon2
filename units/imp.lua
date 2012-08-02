name               = "Imp"
isHuman            = 0
isAI               = 1
staticEntity       = 0
weapon1            = "ImpWeapon" 
mesh               = "media/mdl_nme_imp.x"
hitPoints          = 30
hitSound           = "media/sounds/soundFXCD/FatMonster_HurtGrowl1.bbq"
deathSound	   = "media/sounds/soundFXCD/HogMonster_DeathRoar1.bbq"
footStepSound      = "media/sounds/soundFXCD/Foot_Conc_ShoesLand3.bbq"
speed              = 500
collisionDamage    = 0
coeffOfRestitution = 0
weight             = 1
diesOnImpact       = 0
diesAfterTime      = 0
dropsItemsAtDeath  = 1
deathMode          = 0 -- leaves corpse on death
onHitMode          = 2 -- emits green blood

boundBoxMinX       = -70
boundBoxMinY       = 0
boundBoxMinZ       = -100

boundBoxMaxX       = 70
boundBoxMaxY       = 165
boundBoxMaxZ       = 90

radius             = 110

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

-- THIS ONE IS THE DEATH CYCLE
animations[10] = 15
animations[11] = 22.4
animations[12] = 7

--
--Drop Table Data
--

--values must be less than 100
dropTable = {}
dropTable[1] = 16.6  --health
dropTable[2] = 10    --battle rifle ammo
dropTable[3] = 0     --rocket launcher ammo
dropTable[4] = 1     --shotgun ammo
dropTable[5] = 10    --grenade ammo
dropTable[6] = 0     --flamethrower ammo
