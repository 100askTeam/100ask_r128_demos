/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2014 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"
///#include "led.h"

#if COMPILE_GAME3

#define TUBE_COUNT 4

///#define BLACK	0
///#define WHITE	1

typedef struct 
{
	byte x;
	byte y;
}tube_s;

static bool btnExit(void);
static bool btnBounce(void);
static bool btnPause(void);
static display_t draw(void);
void _DrawPoint(uint8_t x,uint8_t y,uint8_t t);
static void judgeOverlap(void);//�ж��Ƿ��ص����һ������� 
//static const uint eepHighscore EEMEM;

int lives = 4;
byte _GRAM[64]={0};	   //64x1bit  ��8 �߶�64

// Storage Vars
static uint highscore;
static uint score;
static bool bounce;
static bool newHighscore;
static bool overlap;
static bool gameState = false;


static tube_s tubes[TUBE_COUNT];

static const byte tube_top[] PROGMEM ={
	0xFF,0xD5,0xAB,0xD5,0x83,0x81,0x83,0x81,0x83,0x81,0x83,0x81,0x83,0x81,0x83,0x81,0xD5,0xFF,
};

static const byte tube[] PROGMEM ={
	0xFF,0x55,0xAA,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0xFF,
};

static const byte thingImg[] PROGMEM ={
	0x40,0xF8,0xEC,0x2C,0x2C,0x38,0xF0,0x10,//0xD0,0x30,0xE8,0x4C,0x4C,0x9C,0xF0,
//	0x02,0x1F,0x37,0x34,0x34,0x1C,0x0F,0x08,0x0B,0x0C,0x17,0x32,0x32,0x39,0x0F,
};

void draw_bitmap_set(image_s *img)
{
	 uint8_t t,temp;
	 uint8_t x,y=0;
  // draw_bitmap(img->x, img->y, img->bitmap, img->width, img->height, img->invert, img->offsetY);
	//��ȡ�� 14-21 �����е����ݣ��������ֻ��Ҫ����һ��ͼ���λ�þ��б��ֿ���Ϊ8 �߶�Ϊ64
	//��ôλ�÷����֪����  8x8
	//    x---y
	//   14   img->y
	//   21   y+8   
	for(x=0;x<8;x++)       //��ȡ8�������λͼ����oled��buff������л��ƣ����
		{	
			temp=	img->bitmap[x];
     	for(t=0;t<8;t++)
			{
			   	  if(temp&0x01) _DrawPoint(x,img->y+y,1);
             temp=temp>>1;
							y++;
			}			
			y=0;
		}  
		
		//ĿǰΪֹ�õ���һ��8x64��buff�����������ںͺ����OLEDBuff���жԱȣ�����Ƿ��ص�
}

void draw_bitmap_s2(image_s *img)
{
  draw_bitmap(img->x, img->y, img->bitmap, img->width, img->height, img->invert, img->offsetY);
}

void game3_start()
{
  ///pwrmgr_setState(PWR_ACTIVE_DISPLAY, PWR_STATE_BUSY);
	menu_close();
	
	srand(millis());

	display_setDrawFunc(draw);
	buttons_setFuncs(btnPause, btnBounce, btnExit);
	
	byte a = 0;
	
	LOOP(TUBE_COUNT, i)
	{
		tubes[i].x = a;
		a += 42;
	}
	
	//��ʾ��Ϣ��ʼ��
	score=0;
	newHighscore = false;
  overlap=false;
	gameState = false;
	lives = 4;
}

static bool btnExit()
{
	if(lives == 255)
		game3_start();
	else
	 {
		///pwrmgr_setState(PWR_ACTIVE_DISPLAY, PWR_STATE_NONE);	
		animation_start(display_load, ANIM_MOVE_OFF);
   }
	return true;
}

static bool btnBounce()
{
	bounce = true;
	return true;
}




static bool btnPause()
{
	gameState=!gameState;
	return false;
}


static display_t draw()     //����һ�����޷��ж�����͹ܵ��Ƿ��ص�
{
	static double y;
	static double yVel;
	static byte tubeOffset;

if(gameState){
	if(bounce)
	{
		yVel = -2;
		bounce = false;
	}
	else if((byte)y > 200)
	{
		yVel = 0;
		y = 0;
	}
	else if((byte)y < FRAME_HEIGHT - 8)
	{
		if(yVel < 0)
			yVel += 0.1;
		else
			yVel += 0.06;
	}		
	else
	{
		yVel = 0;
		y = FRAME_HEIGHT - 8;
	}		
}
	y += yVel;

//	image_s img = {16, y, thingImg, 7, 8, WHITE, NOINVERT, 0};
		image_s img = {16, y, thingImg, 7, 8,  NOINVERT, 0};

	draw_bitmap_set(&img);   //������������ 

	
	
	
	
	char buff[6];
	if(lives != 255)
	{
		
		img.width = 16;
	img.height = 8;
	LOOP(TUBE_COUNT, i)
	{
		img.bitmap = tube;
		img.x = tubes[i].x;
		
		LOOP(2, h)      //���Ϲܵ�
		{
			img.y = (tubes[i].y + (8*h)) - 16;
			draw_bitmap_s2(&img);
		}
		
		LOOP(2, h)      //���¹ܵ�
		{
			img.y = (tubes[i].y + (8*h) + 48 + 16) - 16;
			draw_bitmap_s2(&img);
		}
		
	 if(gameState)
		img.x -= 1;
		img.width = 18;
		img.bitmap = tube_top;     //�ܵ���������
		img.y = tubes[i].y + (8*2) - 16;
		draw_bitmap_s2(&img);
		img.y = (tubes[i].y + (8*2) + 48 + 16) - 16 - (8*3);
		draw_bitmap_s2(&img);
	 
	 if(gameState)
		tubes[i].x--;
		if(tubes[i].x > 200 && tubes[i].x < 240)
		{
			tubes[i].y = rand() % 16;
			tubes[i].x = FRAME_WIDTH + 16;
		}			
	}
	
	tubeOffset++;
	if(tubeOffset > 25)
		tubeOffset = 0;
	
	
  judgeOverlap();     //�ж��ص����滭����
		
		

		// Draw score
		sprintf_P(buff, PSTR("%u"), score/80);
		draw_string(buff, false, FRAME_WIDTH - (7*5), 1);

		// Draw lives
		LOOP(lives, i)
			draw_bitmap(32 + (8*i), 1, livesImg, 7, 8, NOINVERT, 0);
	}
	else
	{
		// Draw end game stuff

		draw_string_P(PSTR(STR_GAMEOVER), false, 20, 0);
		draw_string_P(PSTR(STR_SCORE), false, 20, 16);
		draw_string_P(PSTR(STR_HIGHSCORE), false, 20, 32);

		if(newHighscore)
			draw_string_P(PSTR(STR_NEWHIGHSCORE), false, 20, 48);

		sprintf_P(buff, PSTR("%u"), score);
		draw_string(buff, false, 96, 16);

		sprintf_P(buff, PSTR("%u"), highscore);
		draw_string(buff, false, 96, 32);
	}
	
	
	
	return DISPLAY_BUSY;
}	



//���� 
//x:0~63
//y:0~63
//t:1 ��� 0,���				   
void _DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{	
	//��Ӧ byte OLED_GRAM[512];	  // x= 64 ,y=8
	//�任һ����ʽ
	uint8_t pos,bx,temp=0;
//	if(x>7||y>63)return;//������Χ��.
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
  if(t) _GRAM[8*pos+x] |= temp;  // ������Ч�������Ŵ�� ����Ϊy ������Ϊx
	 else _GRAM[8*pos+x]&=~temp;	

}



//�ж��Ƿ��ص����һ������� 
static void judgeOverlap(void)
{
	int t,i;
	for(t=0;t<8;t++)
	{   
		for(i=0;i<8;i++)
		{
			 //ͷʹ�����bug���Ĳ�����
//		  if(((oledBuffer[20+t*128]>>i)&0x01) == ((_GRAM[t*8+6]>>i)&0x01))
			{		
        overlap=true;			
			}
		}
	}
	
	if(overlap)
	{
		overlap=false;
				lives--;
				
				if(lives == 255)
						{
							// Check for new highscore
							if(score > highscore)
							{
								highscore = score;
								newHighscore = true;

							}
							else
								newHighscore = false;

							
							///led_flash(LED_RED, 250, 255);
							buzzer_buzz(250, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);
						}	
						else
						{
							///led_flash(LED_GREEN, 30, 255);
							buzzer_buzz(100, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);
						}
	}
	
	score++;
	draw_bitmap(14,0, _GRAM, 8, 64, NOINVERT, 0);
    memset(&_GRAM, 0x00, 64);
}



#endif
