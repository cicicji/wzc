/*
 * key.c
 *
 * created: 2021/4/29
 *  author:
 */
#include "ls1b_gpio.h"
#include "key.h"

/*����IO��ʼ������ */
void KEY_Init( void )
{
	/* ���ð���IOΪ����ģʽ */
	gpio_enable( KEY_1, DIR_IN );
	gpio_enable( KEY_2, DIR_IN );
	gpio_enable( KEY_3, DIR_IN );
	gpio_enable( KEY_UP, DIR_IN );
}
/*����ɨ�躯�� */
unsigned char KEY_Scan()
{
	if ( gpio_read(KEY_1) == 0 )
	{
		delay_ms( 10);                 /* �ӳ���Сһ��ʱ�� */
		if ( gpio_read( KEY_1 ) == 0 )  /* ��ʾ��ȷ�������ˣ������� */
		{
			while ( gpio_read( KEY_1 ) == 0 );  /* �ȴ�������� */
			return 1;
		}
	}
	if ( gpio_read( KEY_2 ) == 0 )
	{
		delay_ms( 10 );                 /* �ӳ���Сһ��ʱ�� */
		if ( gpio_read( KEY_2 ) == 0 )  /* ��ʾ��ȷ�������ˣ������� */
		{
			while ( gpio_read( KEY_2 ) == 0 );  /* �ȴ�������� */
			return 2;
		}
	}
	if ( gpio_read( KEY_3 ) ==0 )
	{
		delay_ms( 10 );             /* �ӳ���Сһ��ʱ�� */
		if ( gpio_read( KEY_3 ) ==0 )    /* ��ʾ��ȷ�������ˣ������� */
		{
			while ( gpio_read( KEY_3 ) == 0 );  /* �ȴ�������� */
			return 3;
		}
	}
	if ( gpio_read( KEY_UP ) == 1 )
	{
		delay_ms( 10 );                 /* �ӳ���Сһ��ʱ�� */
		if ( gpio_read( KEY_UP ) == 1 )  /* ��ʾ��ȷ�������ˣ������� */
		{
			while ( gpio_read( KEY_UP ) == 1 );      /* �ȴ�������� */
			return 4;
		}
	}
	return 0;
}



