/*
 *  dlgInput.cpp - SDL GUI dialog for C64 input options
 *
 *  (C) 2006 Bernd Lachner
 *  (C) 2017 Anders Gidenstam  Modified for SDL2 and extended.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "sysdeps.h"
#include "sdlgui.h"

#include "Prefs.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

enum INPUTDLG {
	box_main,
	box_joysticks,
	text_joysticks,
	CB_SWAP_JS
};
static int PORT1_BASE;
static int PORT2_BASE;
static int OK;

static SGOBJ* create_dialog();
static void   free_dialog(SGOBJ* dlg);
static void create_radio_button(SGOBJ &button, int y, string text);
static vector<string> create_joystick_list();

/* The keyboard dialog: */
/* Spalte, Zeile, Länge, Höhe*/
static const int BREAK1 = 8;
static const int LENGTH = 11;
static SGOBJ inputdlg[] =
{
	{ SGBOX, SG_BACKGROUND, 0, 0, 0, 35, 25, NULL },
	{ SGBOX, 0, 0, 1, 2, 33, 22, NULL },
	{ SGTEXT, 0, 0, 2, 1, 11, 1, " Joysticks"},
	{ SGCHECKBOX, SG_SELECTABLE, 0, 2, 3, 30, 1, "Swap Joysticks"},
	{ SGBOX, 0, 0, 2, 5, 31, 7, NULL },
	{ SGTEXT, 0, 0, 4, 4, 7, 1, "Port 1"},
	{ SGBOX, 0, 0, 2, 14, 31, 7, NULL },
	{ SGTEXT, 0, 0, 4, 13, 7, 1, "Port 2"},

	{ SGBUTTON, SG_SELECTABLE|SG_EXIT|SG_DEFAULT, 0, 4, 22, 6, 1, "OK"},
	{ SGBUTTON, SG_SELECTABLE|SG_EXIT, 0, 12, 22, 6, 1, "Cancel"},

	{ -1, 0, 0, 0,0, 0,0, NULL }
};

void Dialog_Input(Prefs &prefs)
{
	SGOBJ* dlg = create_dialog();
	dlg[CB_SWAP_JS].state |= prefs.JoystickSwap ? SG_SELECTED : 0;
	dlg[PORT1_BASE + prefs.Joystick1Port].state |= SG_SELECTED;
	dlg[PORT2_BASE + prefs.Joystick2Port].state |= SG_SELECTED;

	if (SDLGui_DoDialog(dlg) == OK) {
		prefs.JoystickSwap = dlg[CB_SWAP_JS].state & SG_SELECTED;
		for (int idx = PORT1_BASE; idx < PORT2_BASE; ++idx) {
			if (dlg[idx].state & SG_SELECTED) {
				prefs.Joystick1Port = idx - PORT1_BASE;
			}
		}
		for (int idx = PORT2_BASE; idx < OK; ++idx) {
			if (dlg[idx].state & SG_SELECTED) {
				prefs.Joystick2Port = idx - PORT2_BASE;
			}
		}
	}
	free_dialog(dlg);
}

static SGOBJ *create_dialog()
{
	vector<string> js = create_joystick_list();
	SGOBJ *dlg = new SGOBJ[LENGTH + 2*js.size()];
	int idx = 0;
	for (idx = 0; idx < BREAK1; ++idx) {
		dlg[idx] = inputdlg[idx];
	}
	int y = 5;
	PORT1_BASE = idx;
	for (int i = 0; i < js.size(); i++) {
		create_radio_button(dlg[idx++], y++, js[i].c_str());
	}
	y = 14;
	PORT2_BASE = idx;
	for (int i = 0; i < js.size(); i++) {
		create_radio_button(dlg[idx++], y++, js[i].c_str());
	}
	OK = idx;
	for (int i = BREAK1; i < LENGTH; ++i) {
		dlg[idx++] = inputdlg[i];
	}
	return dlg;
}

static void   free_dialog(SGOBJ* dlg)
{
	for (int idx = PORT1_BASE; idx < OK; ++idx) {
		free(dlg[idx].txt);
	}
	delete[] dlg;
}

static void create_radio_button(SGOBJ &button, int y, string text)
{
	button.type = SGCHECKBOX;
	button.flags = SG_SELECTABLE|SG_RADIO;
	button.state = 0;
	button.x = 3;
	button.y = y;
	button.w = 10;
	button.h = 1;
	button.txt = strdup(text.c_str());
}

static vector<string> create_joystick_list()
{
	vector<string> joysticks;
	joysticks.push_back("None");
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		SDL_Joystick *joystick = SDL_JoystickOpen(i);
		joysticks.push_back(string(SDL_JoystickName(joystick)));
		if (SDL_JoystickGetAttached(joystick)) {
			// FIXME: Joysticks should not be left opened here -
			//        but how avoid closing the ones used?!
//			SDL_JoystickClose(joystick);
		}
	}
	return joysticks;
}
