//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "NewSingleplayerGameDialog.h"

#include <stdio.h>

using namespace vgui;

#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include <vgui_controls/ComboBox.h>
#include "tier1/convar.h"

// for SRC
#include <vstdlib/random.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#define RANDOM_MAP "#GameUI_RandomMap"

void OpenNewSingleplayerGameDialog(void)
{
	CNewSingleplayerGameDialog* pCNewSingleplayerGameDialog = new CNewSingleplayerGameDialog(NULL);
	pCNewSingleplayerGameDialog->Activate();
}

ConCommand mapselection("mapselection", OpenNewSingleplayerGameDialog);

ConVar cl_ith_mapcount("cl_ith_mapcount", "1");

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CNewSingleplayerGameDialog::CNewSingleplayerGameDialog(vgui::Panel *parent) : BaseClass(NULL, "NewSingleplayerGameDialog")
{
	SetSize(348, 460);
	//SetOKButtonText("#GameUI_Start");
	
	// we can use this if we decide we want to put "listen server" at the end of the game name
	m_pMapList = new ComboBox(this, "MapList", 12, false);
	m_pMapList->SetVerticalScrollbar(false);

	LoadControlSettings("Resource/NewSingleplayerGameDialog.res");

	LoadMapList();
	SetSizeable(false);
	SetDeleteSelfOnClose(true);
	MoveToCenterOfScreen();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CNewSingleplayerGameDialog::~CNewSingleplayerGameDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNewSingleplayerGameDialog::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *command - 
//-----------------------------------------------------------------------------
void CNewSingleplayerGameDialog::OnCommand(const char *command)
{
	if ( !stricmp( command, "Ok" ) )
	{
		char szMapName[64];
		Q_strncpy(szMapName, GetMapName(), sizeof( szMapName ));

		char szMapCommand[1024];

		// create the command to execute
		Q_snprintf(szMapCommand, sizeof( szMapCommand ), "disconnect\nmaxplayers 1\nmap %s\nprogress_enable\n", szMapName);

		// exec
		engine->ClientCmd_Unrestricted(szMapCommand);
		OnClose();
		return;
	}

	BaseClass::OnCommand( command );
}

void CNewSingleplayerGameDialog::OnKeyCodeTyped(KeyCode code)
{
	// force ourselves to be closed if the escape key it pressed
	if (code == KEY_ESCAPE)
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodeTyped(code);
	}
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CNewSingleplayerGameDialog::LoadMaps(const char *pszPathID)
{
	for (int iMapIndex = 1; iMapIndex <= cl_ith_mapcount.GetInt(); iMapIndex++)
	{
		char mapname[256];
		Q_snprintf(mapname, sizeof(mapname), "ith_level%i", iMapIndex);
		m_pMapList->AddItem( mapname, new KeyValues( "data", "mapname", mapname ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CNewSingleplayerGameDialog::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();

	// add special "name" to represent loading a randomly selected map
	m_pMapList->AddItem( RANDOM_MAP, new KeyValues( "data", "mapname", RANDOM_MAP ) );

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	const char *pathID = "MOD";

	// Load the GameDir maps
	LoadMaps( pathID ); 

	// set the first item to be selected
	m_pMapList->ActivateItem( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CNewSingleplayerGameDialog::IsRandomMapSelected()
{
	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");
	if (!stricmp( mapname, RANDOM_MAP ))
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CNewSingleplayerGameDialog::GetMapName()
{
	int count = m_pMapList->GetItemCount();

	// if there is only one entry it's the special "select random map" entry
	if( count <= 1 )
		return NULL;

	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");
	if (!strcmp( mapname, RANDOM_MAP ))
	{
		int which = RandomInt( 1, count - 1 );
		mapname = m_pMapList->GetItemUserData( which )->GetString("mapname");
	}

	return mapname;
}

//-----------------------------------------------------------------------------
// Purpose: Sets currently selected map in the map combobox
//-----------------------------------------------------------------------------
void CNewSingleplayerGameDialog::SetMap(const char *mapName)
{
	for (int i = 0; i < m_pMapList->GetItemCount(); i++)
	{
		if (!m_pMapList->IsItemIDValid(i))
			continue;

		if (!stricmp(m_pMapList->GetItemUserData(i)->GetString("mapname"), mapName))
		{
			m_pMapList->ActivateItem(i);
			break;
		}
	}
}
