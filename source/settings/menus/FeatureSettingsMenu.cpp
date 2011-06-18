/****************************************************************************
 * Copyright (C) 2011
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#include <unistd.h>
#include "FeatureSettingsMenu.hpp"
#include "settings/CGameCategories.hpp"
#include "settings/GameTitles.h"
#include "settings/CSettings.h"
#include "settings/SettingsPrompts.h"
#include "network/Wiinnertag.h"
#include "network/networkops.h"
#include "FileOperations/fileops.h"
#include "prompts/PromptWindows.h"
#include "language/gettext.h"

static const char * OnOffText[] =
{
    trNOOP( "OFF" ),
    trNOOP( "ON" )
};

FeatureSettingsMenu::FeatureSettingsMenu()
    : SettingsMenu(tr("Sound Settings"), &GuiOptions, MENU_NONE)
{
    int Idx = 0;
    Options->SetName(Idx++, "%s", tr( "Titles from WiiTDB" ));
    Options->SetName(Idx++, "%s", tr( "Cache Titles" ));
    Options->SetName(Idx++, "%s", tr( "Wiinnertag" ));
    Options->SetName(Idx++, "%s", tr( "Import Categories" ));

    SetOptionValues();
}

void FeatureSettingsMenu::SetOptionValues()
{
    int Idx = 0;

    //! Settings: Titles from WiiTDB
    Options->SetValue(Idx++, "%s", tr( OnOffText[Settings.titlesOverride] ));

    //! Settings: Cache Titles
    Options->SetValue(Idx++, "%s", tr( OnOffText[Settings.CacheTitles] ));

    //! Settings: Wiinnertag
    Options->SetValue(Idx++, "%s", tr( OnOffText[Settings.Wiinnertag] ));

    //! Settings: Import categories from WiiTDB
    Options->SetValue(Idx++, " ");
}

int FeatureSettingsMenu::GetMenuInternal()
{
    int ret = optionBrowser->GetClickedOption();

    if (ret < 0)
        return MENU_NONE;

    int Idx = -1;


    //! Settings: Titles from WiiTDB
    if (ret == ++Idx)
    {
        if (++Settings.titlesOverride >= MAX_ON_OFF) Settings.titlesOverride = 0;
    }

    //! Settings: Cache Titles
    else if (ret == ++Idx)
    {
        if (++Settings.CacheTitles >= MAX_ON_OFF) Settings.CacheTitles = 0;

        if(Settings.CacheTitles) //! create new cache file
            GameTitles.LoadTitlesFromWiiTDB(Settings.titlestxt_path);
    }

    //! Settings: Winnertag
    else if (ret == ++Idx)
    {
        if (++Settings.Wiinnertag >= MAX_ON_OFF) Settings.Wiinnertag = 0;

        if(Settings.Wiinnertag == ON && !Settings.autonetwork)
        {
            int choice = WindowPrompt(tr("Warning"), tr("Wiinnertag requires you to enable automatic network connect on application start. Do you want to enable it now?"), tr("Yes"), tr("Cancel"));
            if(choice)
            {
                Settings.autonetwork = ON;
                if(!IsNetworkInit())
                    Initialize_Network();
            }
        }

        if(Settings.Wiinnertag == ON && !CheckFile(Settings.WiinnertagPath))
        {
            int choice = WindowPrompt(tr("Warning"), tr("No Wiinnertag.xml found in the config path. Do you want an example file created?"), tr("Yes"), tr("No"));
            if(choice)
            {
                if(Wiinnertag::CreateExample(Settings.WiinnertagPath))
                {
                    char text[200];
                    snprintf(text, sizeof(text), "%s %s", tr("An example file was created here:"), Settings.WiinnertagPath);
                    WindowPrompt(tr("Success"), text, tr("OK"));
                }
            }
        }
    }

    //! Settings: Import categories from WiiTDB
    else if (ret == ++Idx)
    {
        int choice = WindowPrompt(tr("Import Categories"), tr("Are you sure you want to import game categories from WiiTDB?"), tr("Yes"), tr("Cancel"));
        if(choice)
        {
            char xmlpath[300];
            snprintf(xmlpath, sizeof(xmlpath), "%swiitdb.xml", Settings.titlestxt_path);
            if(!GameCategories.ImportFromWiiTDB(xmlpath))
            {
                WindowPrompt(tr("Error"), tr("Could not open the WiiTDB.xml file."), tr("OK"));
            }
            else
            {
                GameCategories.Save();
                GameCategories.CategoryList.goToFirst();
                WindowPrompt(tr("Import Categories"), tr("Import operation successfully completed."), tr("OK"));
            }
        }
    }

    SetOptionValues();

    return MENU_NONE;
}
