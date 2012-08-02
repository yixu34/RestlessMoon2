name               = "Soldier"
isHuman            = 0
isAI               = 1
staticEntity       = 0
weapon1            = "SoldierWeapon" 
mesh               = "media/mdl_nme_soldier.x"
hitPoints          = 50
hitSound           = "media/sounds/soundFXCD/soldier_hurt.bbq"
deathSound	   = "media/sounds/soundFXCD/soldier_die.bbq"
footStepSound      = "media/sounds/soundFXCD/Foot_Conc_ShoesLand3.bbq"
speed              = 500
collisionDamage    = 0
coeffOfRestitution = 0
weight             = 1
diesOnImpact       = 0
diesAfterTime      = 0
dropsItemsAtDeath  = 1
deathMode          = 0 -- leaves corpse on death
onHitMode          = 1 -- emits red blood

boundBoxMinX       = -100
boundBoxMinY       = 0
boundBoxMinZ       = -80

boundBoxMaxX       = 100
boundBoxMaxY       = 335
boundBoxMaxZ       = 80

radius             = 165

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

--
--Drop Table Data
--

--values must be less than 100
dropTable = {}
dropTable[1] = 20    --health
dropTable[2] = 10    --battle rifle ammo
dropTable[3] = 0    --rocket launcher ammo
dropTable[4] = 10    --shotgun ammo
dropTable[5] = 10    --grenade ammo
dropTable[6] = 0    --flamethrower ammo
