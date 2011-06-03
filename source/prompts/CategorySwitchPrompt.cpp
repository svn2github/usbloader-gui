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
#include "CategorySwitchPrompt.hpp"
#include "settings/CGameCategories.hpp"
#include "settings/CSettings.h"
#include "language/gettext.h"

CategorySwitchPrompt::CategorySwitchPrompt()
    : CategoryPrompt(tr("Show Categories"))
{
    categoryChanged.connect(this, &CategorySwitchPrompt::OnCategoryChange);
    nextCategory.connect(this, &CategorySwitchPrompt::OnNextCategory);
    previousCategory.connect(this, &CategorySwitchPrompt::OnPreviousCategory);
    checkBoxClicked.connect(this, &CategorySwitchPrompt::OnCheckboxClicked);
}

void CategorySwitchPrompt::OnNextCategory()
{
    GameCategories.CategoryList.goToNextCicle();
}

void CategorySwitchPrompt::OnPreviousCategory()
{
    GameCategories.CategoryList.goToPreviousCicle();
}

void CategorySwitchPrompt::OnCategoryChange(GuiText *catTxt, GuiText *positionTxt, GuiCheckbox *checkBox)
{
    catTxt->SetText(tr(GameCategories.CategoryList.getCurrentName().c_str()));
    positionTxt->SetTextf("%i / %i", GameCategories.CategoryList.pos()+1, GameCategories.CategoryList.size());
    checkBox->SetChecked(false);

    for(u32 i = 0; i < Settings.EnabledCategories.size(); ++i)
    {
        if(Settings.EnabledCategories[i] == GameCategories.CategoryList.getCurrentID())
        {
            checkBox->SetChecked(true);
            break;
        }
    }
}

void CategorySwitchPrompt::OnCheckboxClicked(GuiCheckbox *checkBox)
{
    u32 i;
    for(i = 0; i < Settings.EnabledCategories.size(); ++i)
    {
        if(Settings.EnabledCategories[i] == GameCategories.CategoryList.getCurrentID())
        {
            if(!checkBox->IsChecked())
                Settings.EnabledCategories.erase(Settings.EnabledCategories.begin()+i);
            break;
        }
    }

    if(i == Settings.EnabledCategories.size() && checkBox->IsChecked())
    {
        Settings.EnabledCategories.push_back(GameCategories.CategoryList.getCurrentID());
    }
}
