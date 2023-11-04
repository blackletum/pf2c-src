//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include "c_tf_player.h"
#include "clientmode_tf.h"
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>

using namespace vgui;

static const char* s_CondImageNames[] =
{
	"ConditionInfectedImage",
	"ConditionDizzyImage",
	"ConditionPoisonedImage",
	"ConditionTranqedImage",
	"ConditionSlowedImage"
};
#define CONDIMAGE_COUNT 5

class CHudPlayerConditions : public CHudElement, public EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE( CHudPlayerConditions, EditablePanel );

	CHudPlayerConditions( const char *name );

	virtual void ApplySchemeSettings( IScheme* pScheme );
	virtual void Init();
	virtual bool ShouldDraw();
	virtual void Paint();
	virtual void OnThink();

private:
	ImagePanel* m_pCondImages[ sizeof( s_CondImageNames ) ];
	bool m_bCondStates[ sizeof( s_CondImageNames ) ];

	float m_flNextThink;
};

DECLARE_HUDELEMENT( CHudPlayerConditions );
CHudPlayerConditions::CHudPlayerConditions( const char *pName ) : CHudElement( pName ), BaseClass( NULL, "HudPlayerConditions" )
{
	SetParent( g_pClientMode->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );
	for ( int i = 0; i < CONDIMAGE_COUNT; ++i )
	{
		m_pCondImages[ i ] = new ImagePanel( this, s_CondImageNames[ i ] );
	}

	m_flNextThink = 0.0f;
}

void CHudPlayerConditions::Init()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudPlayerConditions::ApplySchemeSettings( IScheme* pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudPlayerConditions.res" );

	BaseClass::ApplySchemeSettings( pScheme );
}

bool CHudPlayerConditions::ShouldDraw()
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	// if we are spectating another player first person, check this player
	if ( pPlayer && ( pPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) )
	{
		pPlayer = ToTFPlayer( pPlayer->GetObserverTarget() );
	}

	return ( pPlayer && pPlayer->IsAlive() );
}

void CHudPlayerConditions::OnThink()
{
	C_TFPlayer* pPlayer = ToTFPlayer( C_BasePlayer::GetLocalPlayer() );

	if ( pPlayer )
	{
		// this is bad. Oh well
		m_bCondStates[ 0 ] = pPlayer->m_Shared.InCond( TF_COND_INFECTED );
		m_bCondStates[ 1 ] = pPlayer->m_Shared.InCond( TF_COND_DIZZY );
		m_bCondStates[ 2 ] = pPlayer->m_Shared.InCond( TF_COND_HALLUCINATING );
		m_bCondStates[ 3 ] = pPlayer->m_Shared.InCond( TF_COND_TRANQUILIZED );
		m_bCondStates[ 4 ] = pPlayer->m_Shared.InCond( TF_COND_LEG_DAMAGED );
	}
	m_flNextThink = gpGlobals->curtime + 0.1f;
}


void CHudPlayerConditions::Paint()
{
	int i;
	int total = 0;
	for ( i = 0; i < CONDIMAGE_COUNT; ++i )
	{
		if( m_bCondStates[ i ] != m_pCondImages[ i ]->IsVisible() )
			m_pCondImages[ i ]->SetVisible( m_bCondStates[ i ] );
		total += m_bCondStates[ i ];
	}
	if ( total > 0 )
	{
		int _x, _y, _width, _tall;
		GetBounds( _x, _y, _width, _tall );

		// int iStart = ( int )( ( _width / 2.0f ) - ( ( _tall * total ) * 0.5f ) ); // Horizonal
		// Start from the bottom of the panel and fill up
		int iStart = _tall - _width; // icons are squares and should be the same width as the panel
		for ( i = 0; i < CONDIMAGE_COUNT; ++i )
		{
			if ( m_bCondStates[ i ] )
			{
				m_pCondImages[ i ]->SetPos( 0, iStart );
				int iAlpha = ( int )( fabs( sin( gpGlobals->curtime * 2 ) ) * 256.0 );
				m_pCondImages[ i ]->SetAlpha( iAlpha );
				iStart -= _width;
			}
		}
	}
}
