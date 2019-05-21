//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef WEAPON_ERRORLAUNCHER_H
#define WEAPON_ERRORLAUNCHER_H

#ifdef _WIN32
#pragma once
#endif

#include "basehlcombatweapon.h"
#include "Sprite.h"
#include "npcevent.h"
#include "beam_shared.h"

class CWeaponErrorLauncher;
class CLaserDotError;
class RocketTrail;
 
//###########################################################################
//	>> CMissileError		(missile launcher class is below this one!)
//###########################################################################
class CMissileError : public CBaseCombatCharacter
{
	DECLARE_CLASS( CMissileError, CBaseCombatCharacter );

public:
	static const int EXPLOSION_RADIUS = 200;

	CMissileError();
	~CMissileError();

#ifdef HL1_DLL
	Class_T Classify( void ) { return CLASS_NONE; }
#else
	Class_T Classify( void ) { return CLASS_MISSILE; }
#endif
	
	void	Spawn( void );
	void	Precache( void );
	void	MissileTouch( CBaseEntity *pOther );
	void	Explode( void );
	void	ShotDown( void );
	void	AccelerateThink( void );
	void	AugerThink( void );
	void	IgniteThink( void );
	void	SeekThink( void );
	void	DumbFire( void );
	void	SetGracePeriod( float flGracePeriod );

	int		OnTakeDamage_Alive( const CTakeDamageInfo &info );
	void	Event_Killed( const CTakeDamageInfo &info );
	
	virtual float	GetDamage() { return m_flDamage; }
	virtual void	SetDamage(float flDamage) { m_flDamage = flDamage; }

	unsigned int PhysicsSolidMaskForEntity( void ) const;

	CHandle<CWeaponErrorLauncher>		m_hOwner;

	static CMissileError *Create( const Vector &vecOrigin, const QAngle &vecAngles, edict_t *pentOwner );

	void CreateDangerSounds( bool bState ){ m_bCreateDangerSounds = bState; }

	static void AddCustomDetonator( CBaseEntity *pEntity, float radius, float height = -1 );
	static void RemoveCustomDetonator( CBaseEntity *pEntity );

protected:
	virtual void DoExplosion();	
	virtual void ComputeActualDotPosition(CLaserDotError *pLaserDot, Vector *pActualDotPosition, float *pHomingSpeed);
	virtual int AugerHealth() { return m_iMaxHealth - 20; }

	// Creates the smoke trail
	void CreateSmokeTrail( void );

	// Gets the shooting position 
	void GetShootPosition(CLaserDotError *pLaserDot, Vector *pShootPosition);

	CHandle<RocketTrail>	m_hRocketTrail;
	float					m_flAugerTime;		// Amount of time to auger before blowing up anyway
	float					m_flMarkDeadTime;
	float					m_flDamage;

	struct CustomDetonator_t
	{
		EHANDLE hEntity;
		float radiusSq;
		float halfHeight;
	};

	static CUtlVector<CustomDetonator_t> gm_CustomDetonators;

private:
	float					m_flGracePeriodEndsAt;
	bool					m_bCreateDangerSounds;

	DECLARE_DATADESC();
};


//-----------------------------------------------------------------------------
// Laser dot control
//-----------------------------------------------------------------------------
CBaseEntity *CreateErrorLaserDot( const Vector &origin, CBaseEntity *pOwner, bool bVisibleDot );
void SetErrorLaserDotTarget( CBaseEntity *pLaserDot, CBaseEntity *pTarget );
void EnableErrorLaserDot(CBaseEntity *pLaserDot, bool bEnable);

//-----------------------------------------------------------------------------
// RPG
//-----------------------------------------------------------------------------
class CWeaponErrorLauncher : public CBaseHLCombatWeapon
{
	DECLARE_CLASS( CWeaponErrorLauncher, CBaseHLCombatWeapon );
public:

	CWeaponErrorLauncher();
	~CWeaponErrorLauncher();

	DECLARE_SERVERCLASS();

	void	Precache( void );

	void	PrimaryAttack( void );
	virtual float GetFireRate( void ) { return 1; };
	void	ItemPostFrame( void );

	void	Activate( void );
	void	DecrementAmmo( CBaseCombatCharacter *pOwner );

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
	void	StartGuiding( void );
	void	StopGuiding( void );
	void	ToggleGuiding( void );
	bool	IsGuiding( void );

	void	NotifyRocketDied( void );

	bool	HasAnyAmmo( void );

	void	SuppressGuiding( bool state = true );

	void	CreateLaserPointer( void );
	void	UpdateLaserPosition( Vector vecMuzzlePos = vec3_origin, Vector vecEndPos = vec3_origin );
	Vector	GetLaserPosition( void );
	void	StartLaserEffects( void );
	void	StopLaserEffects( void );
	void	UpdateLaserEffects( void );

	// NPC RPG users cheat and directly set the laser pointer's origin
	void	UpdateNPCLaserPosition( const Vector &vecTarget );
	void	SetNPCLaserPosition( const Vector &vecTarget );
	const Vector &GetNPCLaserPosition( void );

	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_3DEGREES;
		return cone;
	}
	
	CBaseEntity *GetMissile( void ) { return m_hMissile; }

	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
	
protected:

	bool				m_bInitialStateUpdate;
	bool				m_bGuiding;
	bool				m_bHideGuiding;		//User to override the player's wish to guide under certain circumstances
	Vector				m_vecNPCLaserDot;
	CHandle<CLaserDotError>	m_hLaserDot;
	CHandle<CMissileError>	m_hMissile;
	CHandle<CSprite>	m_hLaserMuzzleSprite;
	CHandle<CBeam>		m_hLaserBeam;
};

#endif // WEAPON_RPG_H
