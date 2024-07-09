#include "../includes.h"

Resolver g_resolver{ };;

void Resolver::ResolveAngles( Player* player, LagComp::LagRecord_t* record ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	// if we are in nospread mode, force all players pitches to down.
	// TODO; we should check thei actual pitch and up too, since those are the other 2 possible angles.
	// this should be somehow combined into some iteration that matches with the air angle iteration.
	if( !record->m_bDidShot ) {
		if( g_cfg[ XOR( "cheat_mode" ) ].get<int>( ) == 1 ) {
			switch( data->m_missed_shots % 6 ) {
			case 0:
			case 1:
				record->m_angEyeAngles.x = player->m_angEyeAngles( ).x = 90.f;
				break;

			case 2:
			case 3:
				record->m_angEyeAngles.x = player->m_angEyeAngles( ).x = 0.f;
				break;
			case 4:
			case 5:
				record->m_angEyeAngles.x = player->m_angEyeAngles( ).x = -90.f;
				break;

			default:
				break;
			}
		}

		// we arrived here we can do the acutal resolve.
		//if (record->m_mode == Modes::RESOLVE_STAND)
		ResolveStand( data, record );
	}
}

void Resolver::ResolveStand( AimPlayer* data, LagComp::LagRecord_t* record ) {
	// get the players max rotation.
	float max_rotation = record->m_pEntity->GetMaxBodyRotation( );

	// setup a starting brute angle.
	float resolve_value = 50.f;

	if( !record->m_pState )
		return;

	const auto info = g_anims.GetAnimationInfo( record->m_pEntity );
	if( !info )
		return;

}

float Resolver::ResolveShot( AimPlayer* data, LagComp::LagRecord_t* record ) {
	float flPseudoFireYaw = math::NormalizedAngle( math::CalcAngle( record->m_pMatrix[ 8 ].GetOrigin( ), g_cl.m_local->m_BoneCache( ).m_pCachedBones[ 0 ].GetOrigin( ) ).y );

	if( data->m_extending ) {
		float flLeftFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y + 58.f ) ) );
		float flRightFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y - 58.f ) ) );

		//g_notify.add( tfm::format( XOR( "found shot record on %s: [ yaw: %i ]" ), game::GetPlayerName( record->m_player->index( ) ), int( flLeftFireYawDelta > flRightFireYawDelta ? -58.f : 58.f ) ) );

		return flLeftFireYawDelta > flRightFireYawDelta ? -58.f : 58.f;
	}
	else {
		float flLeftFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y + 29.f ) ) );
		float flRightFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y - 29.f ) ) );

		//g_notify.add( tfm::format( XOR( "found shot record on %s: [ yaw: %i ]" ), game::GetPlayerName( record->m_player->index( ) ), int( flLeftFireYawDelta > flRightFireYawDelta ? -29.f : 29.f ) ) );

		return flLeftFireYawDelta > flRightFireYawDelta ? -29.f : 29.f;
	}
}