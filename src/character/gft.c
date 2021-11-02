/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gft.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Christmas Parents structure
enum
{
	gft_ArcMain_Idle0,
	gft_ArcMain_Idle1,
	gft_ArcMain_Idle2,
	gft_ArcMain_Idle3,
	gft_ArcMain_Idle4,
	gft_ArcMain_Idle5,
	gft_ArcMain_Idle6,
	gft_ArcMain_Idle7,
	gft_ArcMain_Idle8,
	gft_ArcMain_Idle9,
	gft_ArcMain_Idle10,
	gft_ArcMain_Idle11,

	gft_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[gft_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_gft;

//Christmas Parents definitions
static const CharFrame char_gft_frame[] = {
	{gft_ArcMain_Idle0, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle1, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle2, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle3, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle4, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle5, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle6, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle7, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle8, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle9, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle10, {0,   0, 256, 256}, {130, 192}},
	{gft_ArcMain_Idle11, {0,   0, 256, 256}, {130, 192}},

};

static const Animation char_gft_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  ASCR_BACK, 1}}, //CharAnim_Idle
};

//Christmas Parents functions
void Char_gft_SetFrame(void *user, u8 frame)
{
	Char_gft *this = (Char_gft*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gft_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_gft_Tick(Character *character)
{
	Char_gft *this = (Char_gft*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_gft_SetFrame);
	Character_Draw(character, &this->tex, &char_gft_frame[this->frame]);
}

void Char_gft_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_gft_Free(Character *character)
{
	Char_gft *this = (Char_gft*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_gft_New(fixed_t x, fixed_t y)
{
	//Allocate Christmas Parents object
	Char_gft *this = Mem_Alloc(sizeof(Char_gft));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_gft_New] Failed to allocate Christmas Parents object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_gft_Tick;
	this->character.set_anim = Char_gft_SetAnim;
	this->character.free = Char_gft_Free;
	
	Animatable_Init(&this->character.animatable, char_gft_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-110,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",   //gft_ArcMain_Idle0
		"idle1.tim",   //gft_ArcMain_Idle1
		"idle2.tim",   //gft_ArcMain_Idle2
		"idle3.tim",   
		"idle4.tim", 
		"idle5.tim", 
		"idle6.tim", 
		"idle7.tim", 
		"idle8.tim", 
		"idle9.tim", 
		"idle10.tim", 
		"idle11.tim", 

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
