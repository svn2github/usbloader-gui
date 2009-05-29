/****************************************************************************
 * libwiigui
 *
 * gui_gamecarousel.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"
#include "../wpad.h"

#include <unistd.h>
#include "gui_gamecarousel.h"
#include "../cfg.h"

#include <string.h>
#include <math.h>
#include <sstream>

#define SCALE		0.8f
#define DEG_OFFSET	7
#define RADIUS		780
#define IN_SPEED	175
#define SHIFT_SPEED	100
#define PAGESIZE	9

extern const int vol;

/**
 * Constructor for the GuiGameCarousel class.
 */
GuiGameCarousel::GuiGameCarousel(int w, int h, struct discHdr * l, int gameCnt, const char *themePath, const u8 *imagebg, int selected, int offset)
{
	width = w;
	height = h;
	this->gameCnt = gameCnt;
	gameList = l;
	pagesize = (gameCnt < PAGESIZE) ? gameCnt : PAGESIZE;
	listOffset = (offset == 0) ? this->FindMenuItem(-1, 1) : offset;
	selectable = true;
	selectedItem = selected - offset;
	focus = 1;					 // allow focus
	firstPic = 0;
	char imgPath[100];

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigHeldA = new GuiTrigger;
	trigL = new GuiTrigger;
	trigL->SetButtonOnlyTrigger(-1, WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT, PAD_BUTTON_LEFT);
	trigR = new GuiTrigger;
	trigR->SetButtonOnlyTrigger(-1, WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT, PAD_BUTTON_RIGHT);
	trigPlus = new GuiTrigger;
	trigPlus->SetButtonOnlyTrigger(-1, WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS, 0);
	trigMinus = new GuiTrigger;
	trigMinus->SetButtonOnlyTrigger(-1, WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS, 0);

	btnSoundClick = new GuiSound(button_click2_pcm, button_click2_pcm_size, SOUND_PCM, vol);
	btnSoundOver = new GuiSound(button_over_pcm, button_over_pcm_size, SOUND_PCM, vol);

	snprintf(imgPath, sizeof(imgPath), "%sstartgame_arrow_left.png", CFG.theme_path);
	imgLeft = new GuiImageData(imgPath, startgame_arrow_left_png);
	snprintf(imgPath, sizeof(imgPath), "%sstartgame_arrow_right.png", CFG.theme_path);
	imgRight = new GuiImageData(imgPath, startgame_arrow_right_png);

	int btnHeight = (int) lround(sqrt(RADIUS*RADIUS - 90000)-RADIUS-50);

	btnLeftImg = new GuiImage(imgLeft);
	btnLeft = new GuiButton(imgLeft->GetWidth(), imgLeft->GetHeight());
	btnLeft->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	btnLeft->SetPosition(20, btnHeight);
	btnLeft->SetParent(this);
	btnLeft->SetImage(btnLeftImg);
	btnLeft->SetSoundOver(btnSoundOver);
	btnLeft->SetTrigger(trigA);
	btnLeft->SetTrigger(trigL);
	btnLeft->SetTrigger(trigMinus);
	btnLeft->SetEffectGrow();

	btnRightImg = new GuiImage(imgRight);
	btnRight = new GuiButton(imgRight->GetWidth(), imgRight->GetHeight());
	btnRight->SetParent(this);
	btnRight->SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	btnRight->SetPosition(-20, btnHeight);
	btnRight->SetImage(btnRightImg);
	btnRight->SetSoundOver(btnSoundOver);
	btnRight->SetTrigger(trigA);
	btnRight->SetTrigger(trigR);
	btnRight->SetTrigger(trigPlus);
	btnRight->SetEffectGrow();

	gameIndex = new int[pagesize];
	game = new GuiButton * [pagesize];
	coverImg = new GuiImage * [pagesize];
	cover = new GuiImageData * [pagesize];
	bob = new int[pagesize];

	for(int i=0; i<pagesize; i++) {
		bob[i]=i;
	}

	char ID[4];
	char IDfull[7];
	for(int i=0; i < pagesize; i++) {

		struct discHdr *header = &gameList[(listOffset+i)%gameCnt];
		snprintf (ID,sizeof(ID),"%c%c%c", header->id[0], header->id[1], header->id[2]);
		snprintf (IDfull,sizeof(IDfull),"%c%c%c%c%c%c", header->id[0], header->id[1], header->id[2],header->id[3], header->id[4], header->id[5]);

		snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, IDfull);
								 //Load full id image
		cover[i] = new GuiImageData(imgPath,0);
		if (!cover[i]->GetImage()) {
			delete cover[i];
			snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, ID);
								 //Load short id image
			cover[i] = new GuiImageData(imgPath, 0);
			if (!cover[i]->GetImage()) {
				delete cover[i];
				snprintf(imgPath, sizeof(imgPath), "%snoimage.png", CFG.covers_path);
								 //Load no image
				cover[i] = new GuiImageData(imgPath, nocover_png);
			}
		}

		coverImg[i] = new GuiImage(cover[i]);
		coverImg[i]->SetScale(SCALE);
		coverImg[i]->SetWidescreen(CFG.widescreen);
		game[i] = new GuiButton(122,244);
		game[i]->SetParent(this);
		game[i]->SetAlignment(ALIGN_CENTRE,ALIGN_MIDDLE);
		game[i]->SetPosition(0,740);
		game[i]->SetImage(coverImg[i]);
		game[i]->SetRumble(false);
		game[i]->SetTrigger(trigA);
		game[i]->SetSoundClick(btnSoundClick);
		game[i]->SetEffect(EFFECT_GOROUND, IN_SPEED,  90-(pagesize-2*i-1)*DEG_OFFSET/2, RADIUS, 180, 1, 0, RADIUS);
	}
}


/**
 * Destructor for the GuiGameCarousel class.
 */
GuiGameCarousel::~GuiGameCarousel()
{

	delete imgRight;
	delete imgLeft;
	delete btnLeftImg;
	delete btnRightImg;
	delete btnRight;
	delete btnLeft;

	delete trigA;
	delete btnSoundClick;

	for(int i=0; i<pagesize; i++) {
		delete game[i];
		delete coverImg[i];
		delete cover[i];
	}
	delete [] gameIndex;
	delete [] bob;
	delete [] game;
	delete [] coverImg;
	delete [] cover;
}


void GuiGameCarousel::SetFocus(int f)
{
	LOCK(this);
	focus = f;

	for(int i=0; i<pagesize; i++)
		game[i]->ResetState();

	if(f == 1)
		game[bob[selectedItem]]->SetState(STATE_SELECTED);
}


void GuiGameCarousel::ResetState()
{
	LOCK(this);
	if(state != STATE_DISABLED) {
		state = STATE_DEFAULT;
		stateChan = -1;
	}

	for(int i=0; i<pagesize; i++) {
		game[i]->ResetState();
	}
}


int GuiGameCarousel::GetOffset()
{
	return listOffset;
}


int GuiGameCarousel::GetClickedOption()
{
	int found = -1;
	for(int i=0; i<pagesize; i++) {
		if(game[bob[i]]->GetState() == STATE_CLICKED) {
			game[bob[i]]->SetState(STATE_SELECTED);
			found = listOffset+i;
			break;
		}
	}
	return found;
}


int GuiGameCarousel::GetSelectedOption()
{
	int found = -1;
	for(int i=0; i<pagesize; i++) {
		if(game[bob[i]]->GetState() == STATE_SELECTED) {
			game[bob[i]]->SetState(STATE_SELECTED);
			found = listOffset+i;
			break;
		}
	}
	return found;
}


/****************************************************************************
 * FindMenuItem
 *
 * Help function to find the next visible menu item on the list
 ***************************************************************************/

int GuiGameCarousel::FindMenuItem(int currentItem, int direction)
{
	int nextItem = currentItem + direction;

	if(nextItem < 0 || nextItem >= gameCnt)
		if(gameCnt <= pagesize)
			return -1;
		else
			nextItem = (nextItem < 0) ? nextItem + gameCnt : nextItem - gameCnt;

	if(strlen(get_title(&gameList[nextItem])) > 0)
		return nextItem;
	else
		return FindMenuItem(nextItem, direction);
}


/**
 * Draw the button on screen
 */
void GuiGameCarousel::Draw()
{
	LOCK(this);
	if(!this->IsVisible())
		return;

	int next = listOffset;

	for(int i=0; i<pagesize; i++) {
		if(next >= 0) {
			game[bob[i]]->Draw();
			next = this->FindMenuItem(next, 1);
		} else break;
	}

	if(gameCnt > pagesize) {
		btnRight->Draw();
		btnLeft->Draw();
	}

	this->UpdateEffects();
}


void GuiGameCarousel::Update(GuiTrigger * t)
{
	LOCK(this);
	if(state == STATE_DISABLED || !t)
		return;

	if(!(game[0]->GetEffect() || game[0]->GetEffectOnOver())) {
		for(int i=0; i<pagesize; i++) {
			game[i]->SetEffectGrow();
		}
	}

	btnRight->Update(t);
	btnLeft->Update(t);

	int next = listOffset;

	char ID[4];
	char IDfull[7];
	char imgPath[100];

	for(int i=0; i<pagesize; i++) {
		if(next >= 0) {
			if(game[bob[i]]->GetState() == STATE_DISABLED) {
				game[bob[i]]->SetVisible(true);
				game[bob[i]]->SetState(STATE_DEFAULT);
			}
			gameIndex[i] = next;
			next = this->FindMenuItem(next, 1);
		} else {
			game[bob[i]]->SetVisible(false);
			game[bob[i]]->SetState(STATE_DISABLED);
		}

		if(focus) {
			if(i != selectedItem && game[bob[i]]->GetState() == STATE_SELECTED)
				game[bob[i]]->ResetState();
			else if(i == selectedItem && game[bob[i]]->GetState() == STATE_DEFAULT);
				game[bob[selectedItem]]->SetState(STATE_SELECTED, t->chan);
		}
		game[bob[i]]->Update(t);

		if(game[bob[i]]->GetState() == STATE_SELECTED) {
			selectedItem = i;
		}
	}

	// navigation
	if(!focus || game[0]->GetEffect() || gameCnt <= pagesize)
		return; // skip navigation

	if (t->Left()  || btnLeft->GetState() == STATE_CLICKED) {
		WPAD_ScanPads();
		u16 buttons = 0;
		for(int i=0; i<4; i++)
			buttons |= WPAD_ButtonsHeld(i);
		if(!((buttons & WPAD_BUTTON_A) || (buttons & WPAD_BUTTON_MINUS) || t->Left())) {
			btnLeft->ResetState();
			return;
		}

		for(int i=0; i<pagesize; i++) {
			game[i]->StopEffect();
		}
		listOffset++;
		if (listOffset > (gameCnt-1))
			listOffset=0;
		firstPic++;
		if (firstPic > (pagesize-1))
			firstPic=0;
		
		for (int i=0; i<pagesize; i++) {
			bob[i] = (firstPic+i)%pagesize;
		}

		struct discHdr *header = &gameList[(listOffset + pagesize-1) % gameCnt];
		snprintf (ID,sizeof(ID),"%c%c%c", header->id[0], header->id[1], header->id[2]);
		snprintf (IDfull,sizeof(IDfull),"%c%c%c%c%c%c", header->id[0], header->id[1], header->id[2],header->id[3], header->id[4], header->id[5]);
		delete cover[bob[pagesize-1]];
		snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, IDfull); //Load full id image
		cover[bob[pagesize-1]] = new GuiImageData(imgPath,0);
		if (!cover[bob[pagesize-1]]->GetImage()) {
			delete cover[bob[pagesize-1]];
			snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, ID); //Load short id image
			cover[bob[pagesize-1]] = new GuiImageData(imgPath, 0);
			if (!cover[bob[pagesize-1]]->GetImage()) {
				delete cover[bob[pagesize-1]];
				snprintf(imgPath, sizeof(imgPath), "%snoimage.png", CFG.covers_path); //Load no image
				cover[bob[pagesize-1]] = new GuiImageData(imgPath, nocover_png);
			}
		}
		delete coverImg[bob[pagesize-1]];
		coverImg[bob[pagesize-1]] = new GuiImage(cover[bob[pagesize-1]]);
		coverImg[bob[pagesize-1]]->SetScale(SCALE);
		coverImg[bob[pagesize-1]]->SetWidescreen(CFG.widescreen);
		game[bob[pagesize-1]]->SetImage(coverImg[bob[pagesize-1]]);
		game[bob[pagesize-1]]->SetPosition(0, RADIUS);

		for (int i=0; i<pagesize; i++) {
			//game[bob[i]]->SetEffect(EFFECT_GOROUND, -125, 7, 780, 256+7*i, 1, 0, 740);
			game[bob[i]]->SetEffect(EFFECT_GOROUND, -SHIFT_SPEED, DEG_OFFSET, RADIUS, 270-(pagesize-2*i-3)*DEG_OFFSET/2, 1, 0, RADIUS);
		}
	}

	else if(t->Right()  || btnRight->GetState() == STATE_CLICKED) {
		WPAD_ScanPads();
		u16 buttons = 0;
		for(int i=0; i<4; i++)
			buttons |= WPAD_ButtonsHeld(i);
		if(!((buttons & WPAD_BUTTON_A) || (buttons & WPAD_BUTTON_PLUS) || t->Right())) {
			btnRight->ResetState();
			return;
		}

		for(int i=0; i<pagesize; i++) {
			game[i]->StopEffect();
		}
		listOffset--;
		if (listOffset<0)
			listOffset=(gameCnt-1);		
		firstPic--;
		if (firstPic<0)
			firstPic=(pagesize-1);

		for(int i=0; i<pagesize; i++) {
			bob[i] = (firstPic+i)%pagesize;
		}

		struct discHdr *header = &gameList[listOffset];
		snprintf (ID,sizeof(ID),"%c%c%c", header->id[0], header->id[1], header->id[2]);
		snprintf (IDfull,sizeof(IDfull),"%c%c%c%c%c%c", header->id[0], header->id[1], header->id[2],header->id[3], header->id[4], header->id[5]);
		delete cover[bob[0]];
		snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, IDfull); //Load full id image
		cover[bob[0]] = new GuiImageData(imgPath,0);
		if (!cover[bob[0]]->GetImage()) {
			delete cover[bob[0]];
			snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, ID); //Load short id image
			cover[bob[0]] = new GuiImageData(imgPath, 0);
			if (!cover[bob[0]]->GetImage()) {
				delete cover[bob[0]];
				snprintf(imgPath, sizeof(imgPath), "%snoimage.png", CFG.covers_path); //Load no image
				cover[bob[0]] = new GuiImageData(imgPath, nocover_png);
			}
		}
		delete coverImg[bob[0]];
		coverImg[bob[0]] = new GuiImage(cover[bob[0]]);
		coverImg[bob[0]]->SetScale(SCALE);
		coverImg[bob[0]]->SetWidescreen(CFG.widescreen);
		game[bob[0]]->SetImage(coverImg[bob[0]]);
		game[bob[0]]->SetPosition(0, RADIUS);

		for(int i=0; i<pagesize; i++) {
			//game[bob[i]]->SetEffect(EFFECT_GOROUND, 125, 7, 780, 242+7*i, 1, 0, 740);
			game[bob[i]]->SetEffect(EFFECT_GOROUND, SHIFT_SPEED, DEG_OFFSET, RADIUS, 270-(pagesize-2*i+1)*DEG_OFFSET/2, 1, 0, RADIUS);
		}
	}

	if(updateCB)
		updateCB(this);
}


void GuiGameCarousel::Reload(struct discHdr * l, int count)
{
	LOCK(this);
	gameCnt = count;
	gameList = l;
	pagesize = (gameCnt < PAGESIZE) ? gameCnt : PAGESIZE;
	listOffset = this->FindMenuItem(-1, 1);
	selectedItem = 0 + listOffset;
	focus = 1;
	firstPic = 0;
	char imgPath[100];

	for(int i=0; i<pagesize; i++) {
		bob[i]=i;
	}

	char ID[4];
	char IDfull[7];
	for(int i=0; i < pagesize; i++) {

		struct discHdr *header = &gameList[(listOffset+i)%gameCnt];
		snprintf (ID,sizeof(ID),"%c%c%c", header->id[0], header->id[1], header->id[2]);
		snprintf (IDfull,sizeof(IDfull),"%c%c%c%c%c%c", header->id[0], header->id[1], header->id[2],header->id[3], header->id[4], header->id[5]);

		snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, IDfull);
								 //Load full id image
		cover[i] = new GuiImageData(imgPath,0);
		if (!cover[i]->GetImage()) {
			delete cover[i];
			snprintf(imgPath, sizeof(imgPath), "%s%s.png", CFG.covers_path, ID);
								 //Load short id image
			cover[i] = new GuiImageData(imgPath, 0);
			if (!cover[i]->GetImage()) {
				delete cover[i];
				snprintf(imgPath, sizeof(imgPath), "%snoimage.png", CFG.covers_path);
								 //Load no image
				cover[i] = new GuiImageData(imgPath, nocover_png);
			}
		}

		coverImg[i] = new GuiImage(cover[i]);
		coverImg[i]->SetScale(SCALE);
		coverImg[i]->SetWidescreen(CFG.widescreen);
		game[i] = new GuiButton(122,244);
		game[i]->SetParent(this);
		game[i]->SetAlignment(ALIGN_CENTRE,ALIGN_MIDDLE);
		game[i]->SetPosition(0,740);
		game[i]->SetImage(coverImg[i]);
		game[i]->SetRumble(false);
		game[i]->SetTrigger(trigA);
		game[i]->SetSoundClick(btnSoundClick);
		game[i]->SetEffect(EFFECT_GOROUND, IN_SPEED,  180-(pagesize-2*i-1)*DEG_OFFSET/2, RADIUS, 180, 1, 0, RADIUS);
	}
}
