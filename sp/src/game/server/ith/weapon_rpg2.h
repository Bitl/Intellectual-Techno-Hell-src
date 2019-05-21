//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef WEAPON_RPG2_H
#define WEAPON_RPG2_H

#ifdef _WIN32
#pragma once
#endif

#include "basehlcombatweapon.h"
#include "Sprite.h"
#include "npcevent.h"
#include "beam_shared.h"

class CWeaponRPG2;

//-----------------------------------------------------------------------------
// RPG
//-----------------------------------------------------------------------------
class CWeaponRPG2 : public CBaseHLCombatWeapon
{
	DECLARE_CLASS( CWeaponRPG2, CBaseHLCombatWeapon );
public:

	CWeaponRPG2();
	~CWeaponRPG2();

	DECLARE_SERVERCLASS();

	void	Precache( void );

	void	PrimaryAttack( void );
	void	SecondaryAttack(void);
	void	FireRightGun(void);
	void	FireLeftGun(void);
	void	FireBothGuns(void);
	virtual float GetFireRate( void ) { return 1; };
	void	ItemPostFrame( void );

	void	Activate( void );
	void	DecrementAmmo( CBaseCombatCharacter *pOwner );
	void	Decrement2Ammo( CBaseCombatCharacter *pOwner );

	bool	Deploy( void );
	bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	bool	Reload( void );
	bool	WeaponShouldBeLowered( void );
	bool	Lower( void );

	virtual void Drop( const Vector &vecVelocity );

	int		GetMinBurst() { return 1; }
	int		GetMaxBurst() { return 1; }
	float	GetMinRestTime() { return 4.0; }
	float	GetMaxRestTime() { return 4.0; }

	bool	WeaponLOSCondition( const Vector &ownerPos, const Vector &targetPos, bool bSetConditions );
	int		WeaponRangeAttack1Condition( float flDot, float flDist );

	void	Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

	bool	HasAnyAmmo( void );

	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_3DEGREES;
		return cone;
	}

	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
	
protected:
	bool bFlip;
};

#endif // WEAPON_RPG_H
