/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ben.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Christmas Parents structure
enum
{
	ben_ArcMain_Idle0,
	ben_ArcMain_Idle1,
	ben_ArcMain_Idle2,
	ben_ArcMain_Idle3,
	ben_ArcMain_LeftA0,
	ben_ArcMain_LeftA1,
	ben_ArcMain_LeftB0,
	ben_ArcMain_LeftB1,
	ben_ArcMain_DownA0,
	ben_ArcMain_DownA1,
	ben_ArcMain_DownB0,
	ben_ArcMain_DownB1,
	ben_ArcMain_UpA0,
	ben_ArcMain_UpA1,
	ben_ArcMain_UpB0,
	ben_ArcMain_UpB1,
	ben_ArcMain_RightA0,
	ben_ArcMain_RightA1,
	ben_ArcMain_RightB0,
	ben_ArcMain_RightB1,
	
	ben_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[ben_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_ben;

//Christmas Parents definitions
static const CharFrame char_ben_frame[] = {
	{ben_ArcMain_Idle0, {0,   0, 219, 196}, {130, 192}}, //0 idle 1
	{ben_ArcMain_Idle1, {0,   0, 219, 192}, {131, 188}}, //1 idle 2
	{ben_ArcMain_Idle2, {0,   0, 221, 193}, {130, 189}}, //2 idle 3
	{ben_ArcMain_Idle3, {0,   0, 217, 196}, {127, 192}}, //3 idle 4
	
	{ben_ArcMain_LeftA0, {0,   0, 197, 201}, {123, 202}}, //4 left a 1
	{ben_ArcMain_LeftA1, {0,   0, 201, 201}, {125, 202}}, //5 left a 2
	{ben_ArcMain_LeftB0, {0,   0, 197, 200}, {122, 188}}, //6 left b 1
	{ben_ArcMain_LeftB1, {0,   0, 201, 200}, {125, 188}}, //7 left b 2
	
	{ben_ArcMain_DownA0, {0,   0, 213, 189}, {120, 190}}, //8 down a 1
	{ben_ArcMain_DownA1, {0,   0, 213, 190}, {122, 191}}, //9 down a 2
	{ben_ArcMain_DownB0, {0,   0, 213, 189}, {121, 177}}, //10 down b 1
	{ben_ArcMain_DownB1, {0,   0, 213, 190}, {123, 178}}, //11 down b 2
	
	{ben_ArcMain_UpA0, {0,   0, 199, 203}, {117, 204}}, //12 up a 1
	{ben_ArcMain_UpA1, {0,   0, 201, 203}, {120, 204}}, //13 up a 2
	{ben_ArcMain_UpB0, {0,   0, 197, 204}, {117, 205}}, //14 up b 1
	{ben_ArcMain_UpB1, {0,   0, 201, 202}, {119, 203}}, //15 up b 2
	
	{ben_ArcMain_RightA0, {0,   0, 239, 187}, {128, 188}}, //16 right a 1
	{ben_ArcMain_RightA1, {0,   0, 239, 191}, {130, 192}}, //17 right a 2
	{ben_ArcMain_RightB0, {0,   0, 239, 189}, {128, 176}}, //18 right b 1
	{ben_ArcMain_RightB1, {0,   0, 239, 191}, {130, 178}}, //19 right b 2
};

static const Animation char_ben_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_DownAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){14, 15, ASCR_BACK, 1}},         //CharAnim_UpAlt
	{2, (const u8[]){16, 17, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){18, 19, ASCR_BACK, 1}},         //CharAnim_RightAlt
};

//Christmas Parents functions
void Char_ben_SetFrame(void *user, u8 frame)
{
	Char_ben *this = (Char_ben*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ben_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_ben_Tick(Character *character)
{
	Char_ben *this = (Char_ben*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_ben_SetFrame);
	Character_Draw(character, &this->tex, &char_ben_frame[this->frame]);
}

void Char_ben_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_ben_Free(Character *character)
{
	Char_ben *this = (Char_ben*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_ben_New(fixed_t x, fixed_t y)
{
	//Allocate Christmas Parents object
	Char_ben *this = Mem_Alloc(sizeof(Char_ben));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_ben_New] Failed to allocate Christmas Parents object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_ben_Tick;
	this->character.set_anim = Char_ben_SetAnim;
	this->character.free = Char_ben_Free;
	
	Animatable_Init(&this->character.animatable, char_ben_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-110,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BEN.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",   //ben_ArcMain_Idle0
		"idle1.tim",   //ben_ArcMain_Idle1
		"idle2.tim",   //ben_ArcMain_Idle2
		"idle3.tim",   //ben_ArcMain_Idle3
		"lefta0.tim",  //ben_ArcMain_LeftA0
		"lefta1.tim",  //ben_ArcMain_LeftA1
		"leftb0.tim",  //ben_ArcMain_LeftB0
		"leftb1.tim",  //ben_ArcMain_LeftB1
		"downa0.tim",  //ben_ArcMain_DownA0
		"downa1.tim",  //ben_ArcMain_DownA1
		"downb0.tim",  //ben_ArcMain_DownB0
		"downb1.tim",  //ben_ArcMain_DownB1
		"upa0.tim",    //ben_ArcMain_UpA0
		"upa1.tim",    //ben_ArcMain_UpA1
		"upb0.tim",    //ben_ArcMain_UpB0
		"upb1.tim",    //ben_ArcMain_UpB1
		"righta0.tim", //ben_ArcMain_RightA0
		"righta1.tim", //ben_ArcMain_RightA1
		"rightb0.tim", //ben_ArcMain_RightB0
		"rightb1.tim", //ben_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
