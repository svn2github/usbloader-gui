#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

#include <string>
#include <stdio.h>
#include <gctypes.h>
#include <vector>
#include "usbloader/disc.h"

typedef struct _GameCFG
{
	char id[7];
	short video;
	short aspectratio;
	short language;
	short ocarina;
	short vipatch;
	short ios;
	short parentalcontrol;
	short errorfix002;
	short iosreloadblock;
	short loadalternatedol;
	u32 alternatedolstart;
	short patchcountrystrings;
	std::string alternatedolname;
	short returnTo;
	short sneekVideoPatch;
	short NandEmuMode;
	std::string NandEmuPath;
	short Hooktype;
	short WiirdDebugger;
	short Locked;

	void operator=(const struct _GameCFG &game)
	{
		memcpy(this->id, game.id, sizeof(game.id));
		this->video = game.video;
		this->aspectratio = game.aspectratio;
		this->language = game.language;
		this->ocarina = game.ocarina;
		this->vipatch = game.vipatch;
		this->ios = game.ios;
		this->parentalcontrol = game.parentalcontrol;
		this->errorfix002 = game.errorfix002;
		this->iosreloadblock = game.iosreloadblock;
		this->loadalternatedol = game.loadalternatedol;
		this->alternatedolstart = game.alternatedolstart;
		this->patchcountrystrings = game.patchcountrystrings;
		this->alternatedolname = game.alternatedolname;
		this->returnTo = game.returnTo;
		this->sneekVideoPatch = game.sneekVideoPatch;
		this->NandEmuMode = game.NandEmuMode;
		this->NandEmuPath = game.NandEmuPath;
		this->Hooktype = game.Hooktype;
		this->WiirdDebugger = game.WiirdDebugger;
		this->Locked = game.Locked;
	}
} GameCFG;

class CGameSettings
{
	public:
		//!Constructor
		CGameSettings();
		//!Destructor
		~CGameSettings();
		//!Load
		bool Load(const char * path);
		//!Save
		bool Save();
		//!AddGame
		bool AddGame(const GameCFG & NewGame);
		//!Reset
		bool RemoveAll();
		//!Overload Reset for one Game
		bool Remove(const char * id);
		bool Remove(const u8 * id) { return Remove((const char *) id); };
		bool Remove(const struct discHdr * game) { if(!game) return false; else return Remove(game->id); };
		//!Get GameCFG
		GameCFG * GetGameCFG(const char * id);
		//!Overload
		GameCFG * GetGameCFG(const u8 * id) { return GetGameCFG((const char *) id); };
		//!Overload
		GameCFG * GetGameCFG(const struct discHdr * game) { if(!game) return NULL; else return GetGameCFG(game->id); };
		//!Quick settings to PEGI conversion
		static int GetPartenalPEGI(int parentalsetting);
		//!Set the default configuration block
		void SetDefault(GameCFG &game);
	protected:
		bool ReadGameID(const char * src, char * GameID, int size);
		bool SetSetting(GameCFG & game, const char *name, const char *value);
		bool ValidVersion(FILE * file);
		//!Find the config file in the default paths
		bool FindConfig();

		void ParseLine(char *line);
		void TrimLine(std::string &dest, const char *src, char stopChar);
		std::string ConfigPath;
		std::vector<GameCFG> GameList;
		GameCFG DefaultConfig;
};

extern CGameSettings GameSettings;

#endif
