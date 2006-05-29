// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright(C) 2005 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// $Log$
// Revision 1.13  2006/01/23 00:37:14  fraggle
// Make the network waiting screen not thrash the CPU so much.
//
// Revision 1.12  2006/01/14 00:27:16  fraggle
// Set the window caption and title
//
// Revision 1.11  2006/01/14 00:13:04  fraggle
// Detect if disconnected from the server while waiting for the game start.
//
// Revision 1.10  2006/01/14 00:10:54  fraggle
// Change the format of color commands.  Reorganise the waiting dialog.
//
// Revision 1.9  2006/01/13 23:56:00  fraggle
// Add text-mode I/O functions.
// Use text-mode screen for the waiting screen.
//
// Revision 1.8  2006/01/12 02:11:52  fraggle
// Game start packets
//
// Revision 1.7  2006/01/10 22:14:13  fraggle
// Shut up compiler warnings
//
// Revision 1.6  2006/01/09 02:03:39  fraggle
// Send clients their player number, and indicate on the waiting screen
// which client we are.
//
// Revision 1.5  2006/01/08 17:52:45  fraggle
// Play some random music for the players while waiting for the game to
// start.
//
// Revision 1.4  2006/01/08 05:04:50  fraggle
// Don't grab the mouse on the net waiting screen
//
// Revision 1.3  2006/01/07 20:08:11  fraggle
// Send player name and address in the waiting data packets.  Display these
// on the waiting screen, and improve the waiting screen appearance.
//
// Revision 1.2  2006/01/02 21:50:26  fraggle
// Restructure the waiting screen code.  Establish our own separate event
// loop while waiting for the game to start, to avoid affecting the original
// code too much.  Move some _gui variables to net_client.c.
//
// Revision 1.1  2005/12/30 18:58:22  fraggle
// Fix client code to correctly send reply to server on connection.
// Add "waiting screen" while waiting for the game to start.
// Hook in the new networking code into the main game code.
//
//
// Graphical stuff related to the networking code:
//
//  * The client waiting screen when we are waiting for the server to
//    start the game.
//   

#include <ctype.h>

#include "config.h"
#include "doomstat.h"

#include "i_system.h"
#include "i_timer.h"
#include "i_video.h"

#include "net_client.h"
#include "net_gui.h"
#include "net_server.h"

#include "textscreen.h"

static txt_window_t *window;
static txt_label_t *player_labels[MAXPLAYERS];
static txt_label_t *ip_labels[MAXPLAYERS];

static void EscapePressed(TXT_UNCAST_ARG(widget), void *unused)
{
    TXT_Shutdown();
    I_Quit();
}

static void StartGame(TXT_UNCAST_ARG(widget), void *unused)
{
    NET_CL_StartGame();
}

static void BuildGUI(void)
{
    char buf[50];
    txt_table_t *table;
    txt_window_action_t *cancel;
    int i;
    
    TXT_SetDesktopTitle(PACKAGE_STRING);
    
    window = TXT_NewWindow("Waiting for game start...");
    table = TXT_NewTable(3);
    TXT_AddWidget(window, table);

    // Add spacers

    TXT_AddWidget(table, NULL);
    TXT_AddWidget(table, TXT_NewStrut(25, 1));
    TXT_AddWidget(table, TXT_NewStrut(17, 1));

    // Player labels
    
    for (i=0; i<MAXPLAYERS; ++i)
    {
        sprintf(buf, " %i. ", i + 1);
        TXT_AddWidget(table, TXT_NewLabel(buf));
        player_labels[i] = TXT_NewLabel("");
        ip_labels[i] = TXT_NewLabel("");
        TXT_AddWidget(table, player_labels[i]);
        TXT_AddWidget(table, ip_labels[i]);
    }

    TXT_AddWidget(window, TXT_NewStrut(0, 1));

    cancel = TXT_NewWindowAction(KEY_ESCAPE, "Cancel");
    TXT_SignalConnect(cancel, "pressed", EscapePressed, NULL);

    TXT_SetWindowAction(window, TXT_HORIZ_LEFT, cancel);
}

static void UpdateGUI(void)
{
    txt_window_action_t *startgame;
    int i;

    for (i=0; i<MAXPLAYERS; ++i)
    {
        txt_color_t color = TXT_COLOR_BRIGHT_WHITE;

        if (i == net_player_number)
        {
            color = TXT_COLOR_YELLOW;
        }

        TXT_SetFGColor(player_labels[i], color);
        TXT_SetFGColor(ip_labels[i], color);

        if (i < net_clients_in_game)
        {
            TXT_SetLabel(player_labels[i], net_player_names[i]);
            TXT_SetLabel(ip_labels[i], net_player_addresses[i]);
        }
        else
        {
            TXT_SetLabel(player_labels[i], "");
            TXT_SetLabel(ip_labels[i], "");
        }
    }

    if (net_client_controller)
    {
        startgame = TXT_NewWindowAction(' ', "Start game");
        TXT_SignalConnect(startgame, "pressed", StartGame, NULL);
    }
    else
    {
        startgame = NULL;
    }

    TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, startgame);
}

void NET_WaitForStart(void)
{
    TXT_Init();
    I_SetWindowCaption();
    I_SetWindowIcon();

    BuildGUI();

    while (net_waiting_for_start)
    {
        UpdateGUI();

        TXT_DispatchEvents();
        TXT_DrawDesktop();

        NET_CL_Run();
        NET_SV_Run();

        if (!net_client_connected)
        {
            I_Error("Disconnected from server");
        }

        I_Sleep(50);
    }
    
    TXT_Shutdown();
}

