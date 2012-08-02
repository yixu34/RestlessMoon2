addWeaponToCache("Battle Rifle", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"media/sounds/soundFXCD/battlerifleempty.bbq", --weaponEmptySound
			0.091, --weaponCoolDown       
			1.5, --weaponReloadDelay    
			50, --weaponClipSize       
			500, --weaponMaxAmmo        
			250, --weaponStartAmmo      
			1, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.025, --weaponMinInaccuracy  
			0.025, --weaponMaxInaccuracy  
			"media/mdl_bullet.x", --projectileMesh               
			3000, --projectileSpeed              
			20, --projectileLife               
			4, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			0, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0 , --projectileRadialDamage 
			1, --how many bullet offsets      
			-15, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			1 --number of projectiles
			)

addWeaponToCache("Shotgun", --weaponName
			"media/sounds/soundFXCD/Shotgun_Reload.bbq", --reloadSound
			"media/sounds/soundFXCD/shotgunempty.bbq", --weaponEmptySound
			0.83, --weaponCoolDown       
			1.5, --weaponReloadDelay    
			10, --weaponClipSize       
			80, --weaponMaxAmmo        
			40, --weaponStartAmmo      
			1, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.04, --weaponMinInaccuracy  
			0.04, --weaponMaxInaccuracy  
			"media/mdl_bullet.x", --projectileMesh               
			3000, --projectileSpeed              
			20, --projectileLife               
			6, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			0, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/soundFXCD/Shotgun_Shot.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0 , --projectileRadialDamage       
			1, --how many bullet offsets      
			-15, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			8 --number of projectiles
			)

addWeaponToCache("Grenade", --weaponName
			"", --reloadSound
			"", --weaponEmptySound
			0.7, --weaponCoolDown 
			0, --weaponReloadDelay    
			5, --weaponClipSize       
			5, --weaponMaxAmmo        
			3, --weaponStartAmmo      
			1, --weaponChangeDelay    
			"", --weaponEquipSound     
			0, --weaponMinInaccuracy  
			0, --weaponMaxInaccuracy  
			"media/mdl_grenade.x", --projectileMesh               
			2250, --projectileSpeed              
			1.5, --projectileLife               
			0, --projectileDamage             
			1, --projectileCoeffOfRestitution 
			0, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			1, --projectileGravitySensitive   
			"media/sounds/soundFXCD/Explo_Large_Electricity2.bbq", --projectileHitSound           
			"", --projectileLaunchSound        
			"", --projectileIntermedSound      
			100, --projectileRadialDamage       
			1, --how many bullet offsets      
			-15, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset
			1 --number of projectiles 
			)

addWeaponToCache("Rocket Launcher", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_RocketLauncher1.bbq", --reloadSound
			"media/sounds/soundFXCD/rocketlauncherempty.bbq", --weaponEmptySound
			0.9, --weaponCoolDown
			1.5, --weaponReloadDelay    
			2, --weaponClipSize       
			8, --weaponMaxAmmo        
			4, --weaponStartAmmo      
			1, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.02, --weaponMinInaccuracy  	
			0.02, --weaponMaxInaccuracy  
			"media/mdl_rocket.x", --projectileMesh
			4000, --projectileSpeed              
			5, --projectileLife               
			100, --projectileDamage             
			1, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"media/sounds/soundFXCD/Explo_Small_11.bbq", --projectileHitSound           
			"media/sounds/soundFXCD/RocketLauncher_Shot44.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			150, --projectileRadialDamage       
			1, --how many bullet offsets      
			-15, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset
			1 --number of projectiles 
			)
addWeaponToCache("Flamethrower", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Flamethrower1.bbq", --reloadSound
			"media/sounds/soundFXCD/FlameThrowerEmpty.bbq", --weaponEmptySound
			0.06, --weaponCoolDown
			3, --weaponReloadDelay    
			50, --weaponClipSize       
			300, --weaponMaxAmmo        
			150, --weaponStartAmmo      
			1, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.18, --weaponMinInaccuracy  	
			0.18, --weaponMaxInaccuracy  
			"media/mdl_sprite.x", --projectileMesh
			900, --projectileSpeed              
			0.67, --projectileLife               
			5, --projectileDamage             
			1, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/soundFXCD/FlameThrower_ShotBurst4.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			-15, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset
			5 --number of projectiles 
			)
addWeaponToCache("EliteWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_rocket.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			10, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			2, --how many bullet offsets      
			-30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset
			30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("ImpWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_sprite.x", --projectileMesh
			3000, --projectileSpeed              
			15, --projectileLife               
			2.0, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			30, --bulletXOffset 
			35, --bulletYOffset 
			-100, --bulletZOffset
			1 --number of projectiles 
			)
addWeaponToCache("SoldierWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_bullet.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			5, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			-20, --bulletXOffset 
			185, --bulletYOffset 
			-150, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("ToughEliteWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_rocket.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			15, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			2, --how many bullet offsets      
			-30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset
			30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("ToughImpWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_sprite.x", --projectileMesh
			3000, --projectileSpeed              
			15, --projectileLife               
			5, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			30, --bulletXOffset 
			35, --bulletYOffset 
			-100, --bulletZOffset
			1 --number of projectiles 
			)
addWeaponToCache("ToughSoldierWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_bullet.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			10, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			-20, --bulletXOffset 
			185, --bulletYOffset 
			-150, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("BadassEliteWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_rocket.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			20, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			2, --how many bullet offsets      
			-30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset
			30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("BadassImpWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_sprite.x", --projectileMesh
			3000, --projectileSpeed              
			15, --projectileLife               
			10, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			30, --bulletXOffset 
			35, --bulletYOffset 
			-100, --bulletZOffset
			1 --number of projectiles 
			)
addWeaponToCache("BadassSoldierWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			100, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_bullet.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			15, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			-20, --bulletXOffset 
			185, --bulletYOffset 
			-150, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("BadeLunoWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			4000, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_rocket.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			30, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			-30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("TyrellWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.3, --weaponCoolDown
			2, --weaponReloadDelay    
			4000, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.05, --weaponMinInaccuracy  	
			0.05, --weaponMaxInaccuracy  
			"media/mdl_rocket.x", --projectileMesh
			6000, --projectileSpeed              
			15, --projectileLife               
			30, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			1, --how many bullet offsets      
			-30, --bulletXOffset 
			220, --bulletYOffset 
			-155, --bulletZOffset 
			1 --number of projectiles 
			)
addWeaponToCache("AirShipWeapon", --weaponName
			"media/sounds/soundFXCD/Gun_Reload_Weapon24.bbq", --reloadSound
			"", --weaponEmptySound
			0.05, --weaponCoolDown
			2, --weaponReloadDelay    
			4000, --weaponClipSize       
			4000, --weaponMaxAmmo        
			4000, --weaponStartAmmo      
			0, --weaponChangeDelay    
			"", --weaponEquipSound     
			0.01, --weaponMinInaccuracy  	
			0.01, --weaponMaxInaccuracy  
			"media/mdl_bullet.x", --projectileMesh
			8000, --projectileSpeed              
			15, --projectileLife               
			3, --projectileDamage             
			0, --projectileCoeffOfRestitution 
			1, --projectileWeight             
			1, --projectileDiesOnImpact
			1, --projectileDiesAfterTime
			0, --projectileGravitySensitive   
			"", --projectileHitSound           
			"media/sounds/battlerifle.bbq", --projectileLaunchSound        
			"", --projectileIntermedSound      
			0, --projectileRadialDamage       
			2, --how many bullet offsets      
			-80, --bulletXOffset 
			280, --bulletYOffset 
			-900, --bulletZOffset 
			80, --bulletXOffset 
			280, --bulletYOffset 
			-900, --bulletZOffset 
			1 --number of projectiles 
			)
