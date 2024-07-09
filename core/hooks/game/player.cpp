#include "../../../includes.h"

void Hooks::DoExtraBoneProcessing( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	/*
		zero out animstate player pointer so CCSGOPlayerAnimState::DoProceduralFootPlant will not do anything.

		.text:103BB25D 8B 56 60                                mov     edx, [esi+60h]
		.text:103BB260 85 D2                                   test    edx, edx
		.text:103BB262 0F 84 B4 0E 00 00                       jz      loc_103BC11C
	*/

	// get animstate ptr.
	CCSGOPlayerAnimState* animstate = player->m_PlayerAnimState( );

	// backup pointer.
	Player* backup{ nullptr };

	if ( animstate ) {
		// backup player ptr.
		backup = animstate->m_player;

		// null player ptr, GUWOP gang.
		animstate->m_player = nullptr;
	}

	// call og.
	g_hooks.m_DoExtraBoneProcessing( this, a2, a3, a4, a5, a6, a7 );

	// restore ptr.
	if ( animstate && backup )
		animstate->m_player = backup;
}

void Hooks::BuildTransformations( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	if (!player || !player->IsPlayer())
		return g_hooks.m_player.GetOldMethod<BuildTransformations_t>(Player::BUILDTRANSFORMATIONS)(this, a2, a3, a4, a5, a6, a7);

	// backup jiggle bones.
	auto backup_jiggle_bones = player->get<uintptr_t>( 0x291C );

	// overwrite jiggle bones and refuse the game from calling the
	// code responsible for animating our attachments/weapons.
	player->set( 0x291C, 0 );

	// call og.
	g_hooks.m_player.GetOldMethod<BuildTransformations_t>( Player::BUILDTRANSFORMATIONS )( this, a2, a3, a4, a5, a6, a7 );

	// revert jiggle bones.
	player->set( 0x291C, backup_jiggle_bones );
}

void Hooks::StandardBlendingRules( int a2, int a3, int a4, int a5, int a6 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	if( !player || ( player->index( ) - 1 ) > 63 )
		return g_hooks.m_player.GetOldMethod<StandardBlendingRules_t>( Player::STANDARDBLENDINGRULES )( this, a2, a3, a4, a5, a6 );

	// disable interpolation.
	if (!(player->m_fEffects() & EF_NOINTERP))
		player->m_fEffects( ) |= EF_NOINTERP;

	g_hooks.m_player.GetOldMethod<StandardBlendingRules_t>( Player::STANDARDBLENDINGRULES )( this, a2, a3, a4, a5, a6 );

	// restore interpolation.
	player->m_fEffects( ) &= ~EF_NOINTERP;
}

void Hooks::UpdateClientSideAnimation( ) {
	Player* player = ( Player* )this;

	if( !player || !player->m_bIsLocalPlayer( ) )
		return g_hooks.m_player.GetOldMethod<UpdateClientSideAnimation_t>( Player::UPDATECLIENTSIDEANIMATION )( this );

	if( g_cl.m_update ) {
		g_hooks.m_player.GetOldMethod<UpdateClientSideAnimation_t>( Player::UPDATECLIENTSIDEANIMATION )( this );
	}
}

vec3_t Hooks::Weapon_ShootPosition( vec3_t* ang ) {
	Player* player = ( Player* )this;
	
	// CCSPlayer::Weapon_ShootPosition calls client's version of CCSGOPlayerAnimState::ModifyEyePosition( ... ) 
	// we don't want this to happen - hence we should just return our own Weapon_ShootPosition, to ensure accuracy
	// as close to server as possible.
	return player->Weapon_ShootPosition( );
}

void Hooks::CalcView( vec3_t& eyeOrigin, vec3_t& eyeAngles, float& zNear, float& zFar, float& fov ) {
	Player* player = ( Player* )this;

	if( !player || !player->m_bIsLocalPlayer( ) )
		return g_hooks.m_player.GetOldMethod<CalcView_t>( Player::CALCVIEW )( this, eyeOrigin, eyeAngles, zNear, zFar, fov );
	
	// Prevent CalcView from calling CCSGOPlayerAnimState::ModifyEyePosition( ... ) 
	// this will fix inaccuracies, for example when fakeducking - and will enforce
	// us to use our own rebuilt version of CCSGOPlayerAnimState::ModifyEyePosition from the server.
	auto m_bUseNewAnimstate = player->get<bool>( 0x39E1 );
	
	player->set<int>( 0x39E1, false );
	
	g_hooks.m_player.GetOldMethod<CalcView_t>( Player::CALCVIEW )( this, eyeOrigin, eyeAngles, zNear, zFar, fov );
	
	player->set<int>( 0x39E1, m_bUseNewAnimstate );
}