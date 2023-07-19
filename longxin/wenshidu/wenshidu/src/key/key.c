/*
 * key.c
 *
 * created: 2021/4/29
 *  author:
 */
#include "ls1b_gpio.h"
#include "key.h"

/*按键IO初始化函数 */
void KEY_Init( void )
{
	/* 配置按键IO为输入模式 */
	gpio_enable( KEY_1, DIR_IN );
	gpio_enable( KEY_2, DIR_IN );
	gpio_enable( KEY_3, DIR_IN );
	gpio_enable( KEY_UP, DIR_IN );
}
/*按键扫描函数 */
unsigned char KEY_Scan()
{
	if ( gpio_read(KEY_1) == 0 )
	{
		delay_ms( 10);                 /* 延长很小一段时间 */
		if ( gpio_read( KEY_1 ) == 0 )  /* 表示的确被按下了（消抖） */
		{
			while ( gpio_read( KEY_1 ) == 0 );  /* 等待抖动完成 */
			return 1;
		}
	}
	if ( gpio_read( KEY_2 ) == 0 )
	{
		delay_ms( 10 );                 /* 延长很小一段时间 */
		if ( gpio_read( KEY_2 ) == 0 )  /* 表示的确被按下了（消抖） */
		{
			while ( gpio_read( KEY_2 ) == 0 );  /* 等待抖动完成 */
			return 2;
		}
	}
	if ( gpio_read( KEY_3 ) ==0 )
	{
		delay_ms( 10 );             /* 延长很小一段时间 */
		if ( gpio_read( KEY_3 ) ==0 )    /* 表示的确被按下了（消抖） */
		{
			while ( gpio_read( KEY_3 ) == 0 );  /* 等待抖动完成 */
			return 3;
		}
	}
	if ( gpio_read( KEY_UP ) == 1 )
	{
		delay_ms( 10 );                 /* 延长很小一段时间 */
		if ( gpio_read( KEY_UP ) == 1 )  /* 表示的确被按下了（消抖） */
		{
			while ( gpio_read( KEY_UP ) == 1 );      /* 等待抖动完成 */
			return 4;
		}
	}
	return 0;
}



