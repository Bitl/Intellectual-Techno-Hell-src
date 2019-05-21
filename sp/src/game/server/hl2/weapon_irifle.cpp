//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: This is the incendiary rifle.
//
//=============================================================================


#include "cbase.h"
#include "npcevent.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "soundent.h"
#include "player.h"
#include "IEffects.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "weapon_flaregun.h"
#include "datacache/imdlcache.h"
#include "hl2_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef ITH2

#define	FASTEST_REFIRE_TIME		0.1f
//###########################################################################
//	>> CWeaponIRifle
//###########################################################################

class CWeaponIRifle : public CHLSelectFireMachineGun
{
public:

	CWeaponIRifle();

	DECLARE_SERVERCLASS();
	DECLARE_CLASS(CWeaponIRifle, CHLSelectFireMachineGun);

	void	Precache( void );

	virtual void	SecondaryAttack(void);
	virtual void	FireModeLogic(int burstsize, float firerate, int firemode);
	virtual int		GetMinBurst(void) { return 3; }
	virtual int		GetMaxBurst(void) { return 4; }
	virtual bool			Reload(void);
	float	GetFireRate(void) { return 0.1f; }

	int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_5DEGREES;
		return cone;
	}

	DECLARE_ACTTABLE();
};


IMPLEMENT_SERVERCLASS_ST(CWeaponIRifle, DT_WeaponIRifle)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_irifle, CWeaponIRifle );
PRECACHE_WEAPON_REGISTER(weapon_irifle);

//---------------------------------------------------------
// Activity table
//---------------------------------------------------------
acttable_t	CWeaponIRifle::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_ML, true },
};
IMPLEMENT_ACTTABLE(CWeaponIRifle);

//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
CWeaponIRifle::CWeaponIRifle()
{
	m_bReloadsSingly = true;

	m_iFireMode = FIREMODE_FULLAUTO;

	m_bCanUseSemi = false;
	m_bCanUseAuto = false;
	
	m_fMinRange1		= 65;
	m_fMinRange2		= 65;
	m_fMaxRange1		= 200;
	m_fMaxRange2		= 200;
}

//---------------------------------------------------------
//---------------------------------------------------------
void CWeaponIRifle::Precache( void )
{
	BaseClass::Precache();
	PrecacheScriptSound( "Flare.Touch" );

	PrecacheScriptSound( "Weapon_FlareGun.Burn" );

	UTIL_PrecacheOther( "env_flare" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponIRifle::Reload(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();
	if (!pOwner)
		return false;

	// If I don't have any spare ammo, I can't reload
	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return false;

	bool bReload = false;

	// If you don't have clips, then don't try to reload them.
	if (UsesClipsForAmmo1())
	{
		// need to reload primary clip?
		int primary = MIN(GetMaxClip1() - m_iClip1, pOwner->GetAmmoCount(m_iPrimaryAmmoType));
		if (primary != 0)
		{
			bReload = true;
		}
	}

	if (UsesClipsForAmmo2())
	{
		// need to reload secondary clip?
		int secondary = MIN(GetMaxClip2() - m_iClip2, pOwner->GetAmmoCount(m_iSecondaryAmmoType));
		if (secondary != 0)
		{
			bReload = true;
		}
	}

	if (!bReload)
		return false;

	WeaponSound(RELOAD);
	SendWeaponAnim(ACT_VM_RELOAD);

	// Play the player's reload animation
	if (pOwner->IsPlayer())
	{
		((CBasePlayer *)pOwner)->SetAnimation(PLAYER_RELOAD);
	}

	MDLCACHE_CRITICAL_SECTION();
	float flSequenceEndTime = gpGlobals->curtime + 0.2;
	pOwner->SetNextAttack(flSequenceEndTime);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = flSequenceEndTime;

	m_bInReload = true;

	return true;
}

void CWeaponIRifle::FireModeLogic(int burstsize, float firerate, int firemode)
{
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return;

	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	// Abort here to handle burst and auto fire modes
	if ((UsesClipsForAmmo1() && m_iClip1 == 0) || (!UsesClipsForAmmo1() && !pPlayer->GetAmmoCount(m_iPrimaryAmmoType)))
		return;

	m_nShotsFired++;

	pPlayer->DoMuzzleFlash();

	// To make the firing framerate independent, we may have to fire more than one bullet here on low-framerate systems, 
	// especially if the weapon we're firing has a really fast rate of fire.
	int iBulletsToFire = 0;
	float fireRate = firerate;

	while (m_flNextPrimaryAttack <= gpGlobals->curtime)
	{
		// MUST call sound before removing a round from the clip of a CHLMachineGun
		WeaponSound(SINGLE, m_flNextPrimaryAttack);
		m_flNextPrimaryAttack = m_flNextPrimaryAttack + fireRate;
		iBulletsToFire++;
	}

	// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
	if (UsesClipsForAmmo1())
	{
		if (iBulletsToFire > burstsize)
			iBulletsToFire = burstsize;

		if (iBulletsToFire > m_iClip1)
			iBulletsToFire = m_iClip1;

		m_iClip1 -= iBulletsToFire;
	}

	// Fire the bullets
	CFlare *pFlare = CFlare::Create(pPlayer->Weapon_ShootPosition(), pPlayer->EyeAngles(), pPlayer, FLARE_DURATION);

	if (pFlare == NULL)
		return;

	Vector forward;
	pPlayer->EyeVectors(&forward);

	pFlare->SetAbsVelocity(forward * 1500);

	//Factor in the view kick
	AddViewKick();

	if (!m_iClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	if (firemode == FIREMODE_BURST)
	{
		if (m_nShotsFired == burstsize)
		{
			m_flNextPrimaryAttack = gpGlobals->curtime + GetCycleRate();
			m_nShotsFired = 0;
		}
	}
	else if (firemode == FIREMODE_SEMI)
	{
		if (m_nShotsFired == burstsize)
		{
			m_flSoonestPrimaryAttack = gpGlobals->curtime + FASTEST_REFIRE_TIME;
			m_nShotsFired = 0;
		}
	}
}

void CWeaponIRifle::SecondaryAttack(void)
{
	// nothing
}
#endif