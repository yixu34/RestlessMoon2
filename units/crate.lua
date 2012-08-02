name            = "crate"
mesh            = "media/mdl_etc_crate.x"
hitPoints       = 15
hitSound        = "media/sounds/soundFXCD/Hit_MetalImpact06.bbq"
deathSound	    = "media/sounds/soundFXCD/Hit_MetalImpact06.bbq"
footStepSound   = ""
speed           = 0
collisionDamage = 0
staticEntity    = 1
diesOnImpact    = 0
diesAfterTime   = 0
coeffOfRestitution = 0
weight             = 0
dropsItemsAtDeath = 1
deathMode       = 2 -- disappear on death
onHitMode       = 0

boundBoxMinX    = -50
boundBoxMinY    = 0
boundBoxMinZ    = -50

boundBoxMaxX    = 50
boundBoxMaxY    = 100
boundBoxMaxZ    = 50

radius          = 50

--
--Drop Table Data
--

--values must be less than 100
dropTable = {}
dropTable[1] = 30   --health
dropTable[2] = 10   --battle rifle ammo
dropTable[3] = 1    --rocket launcher ammo
dropTable[4] = 10   --shotgun ammo
dropTable[5] = 10   --grenade ammo
dropTable[6] = 1    --flamethrower ammo
