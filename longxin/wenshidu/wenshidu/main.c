
#include <stdio.h>

#include "ls1b.h"
#include "mips.h"

#include "bsp.h"
#include "libc/lwmem.h"
#include "ls1x_fb.h"
#include "ls1b_gpio.h"
#include "src/hdc2080/hdc2080.h"
#include "ls1x_spi_bus.h"
#include "spi/w25x40.h"
#include "src/key/key.h"

char LCD_display_mode[] = LCD_480x800;

int main(void)
{
    printk("\r\nmain() function.\r\n");
    
    //��ʼ���ڴ��
    lwmem_initialize(0);
    
    unsigned char sta = 0;
    float temp = 0,hum = 0,press = 0,eleva = 0,lx = 0;
    float Rtemp1=0,Rtemp2=0,Rtemp3=0,Rtemp4=0;
    float Rhum1=0,Rhum2=0,Rhum3=0,Rhum4=0;
    uint16_t temp_data,hum_data;
    static char cnt = 0;
    unsigned char Key = 0;
    unsigned char buf[40] = {0};
    unsigned char num=0;
    
    //������ʼ��
    KEY_Init();
    I2C1_init();
    Get_HDC_ID();
    //����ʾ
    fb_open();
    delay_ms(200);
    //SPI0��������ʼ��
    ls1x_spi_initialize(busSPI0);
    /*
    * ��ע��W25X40оƬ�ڲ�д��PMON��ռ�ÿռ�Լ268K����˲���W25X40��д����ʱ��������268K�ռ�����
    * �����̲��Ե�ַΪ300-512K�ռ�
    * nor flash���ԣ�д����֮ǰ����Ҫ�Ȳ�������оƬ�������������������Ƶ�
    */
    unsigned int id,wroffset = 0x51000,rdoffset = 0x51000;
	char sbuf[20];
	//��ʼ��W25X40оƬ
	ls1x_w25x40_init(busSPI0, NULL);
	//��ȡW25X40оƬID
	ls1x_w25x40_ioctl(busSPI0, IOCTL_W25X40_READ_ID, &id);
    sprintf((char *)sbuf,"W25X40 ID:%x\n",id);
	unsigned char tx_count = 0;
	unsigned char rbuf[16],tbuf[16];
	unsigned char s1[65],s2[65];
    
    fb_textout(50, 30, "������ģ�����");
    
    
    /*
     * �����ѭ��
     */
    for (;;)
    {
		delay_ms(1000);
		fb_fillrect(50, 50, 600, 300, cidxBLACK);
		fb_textout(50, 50, "��ʪ�ȴ�����");
		fb_textout(50, 70, "CURRENT TEMPERATURE(��ǰ�¶�): ");
		fb_textout(50, 90, "CURRENT HUMIDITY(��ǰʪ��): ");
		HDC_Get_Temp_Hum(&temp, &hum);
		/* ��LCD����ʾ��ǰ�¶� */
		sprintf((char *)buf,"%.1f��", temp);
		fb_textout(300, 70, buf);
		memset(buf, 0, sizeof(buf));
		/* ��LCD����ʾ��ǰʪ�� */
		sprintf((char *)buf,"%.0f%%RH", hum);
		fb_textout(300, 90, buf);
		memset(buf, 0, sizeof(buf));
		printf("�¶ȣ�%.2f\r\n",temp);
		printf("ʪ�ȣ�%.2f\r\n",hum);
		if(temp >= 34)
		{
			LED8_ON();
			fb_textout(50, 110, "ָʾ��״̬��LED8_ON ");
		}
		else
		{
			LED8_OFF();
			fb_textout(50, 110, "ָʾ��״̬��LED8_OFF");
		}
		
		//��W25X40оƬ��ȡ��ʪ������
		ls1x_w25x40_read(busSPI0, rbuf, 16, &rdoffset);
		temp_data = ((uint16_t)rbuf[1] << 8) | rbuf[0];
		hum_data = ((uint16_t)rbuf[3] << 8) | rbuf[2];
		Rtemp1 = temp_data / 65535.0 *165 - 40;
		Rhum1 = hum_data / 65535.0 * 100;

		temp_data = ((uint16_t)rbuf[5] << 8) | rbuf[4];
		hum_data = ((uint16_t)rbuf[7] << 8) | rbuf[6];
		Rtemp2 = temp_data / 65535.0 *165 - 40;
		Rhum2 = hum_data / 65535.0 * 100;

		temp_data = ((uint16_t)rbuf[9] << 8) | rbuf[8];
		hum_data = ((uint16_t)rbuf[11] << 8) | rbuf[10];
		Rtemp3 = temp_data / 65535.0 *165 - 40;
		Rhum3 = hum_data / 65535.0 * 100;

		temp_data = ((uint16_t)rbuf[13] << 8) | rbuf[12];
		hum_data = ((uint16_t)rbuf[15] << 8) | rbuf[14];
		Rtemp4 = temp_data / 65535.0 *165 - 40;
		Rhum4 = hum_data / 65535.0 * 100;
		
		//��¼��ʪ������
		if(num==16)
		{
		    num=0;
        }
		tbuf[num]=data_buf[0];
		tbuf[num+1]=data_buf[1];
		tbuf[num+2]=data_buf[2];
		tbuf[num+3]=data_buf[3];
		num+=4;
		//���°�����¼�洢��ʪ������
        Key = KEY_Scan();
		if(Key==4)
		{
		    fb_textout(50, 250, "��ʼ�洢��ǰ��ʪ��");
		    ls1x_w25x40_ioctl(busSPI0, IOCTL_W25X40_ERASE_4K, wroffset);  //spinor flashд֮ǰ��Ҫ�Ȳ���
		    ls1x_w25x40_write(busSPI0, tbuf, 16, &wroffset);
        }
       	/* ��LCD����ʾ��ǰ�¶� */
        fb_textout(50, 150, "STORAGE TEMPERATURE(�洢�¶�): ");
        sprintf((char *)buf,"%.1f��--%.1f��--%.1f��--%.1f��", Rtemp1,Rtemp2,Rtemp3,Rtemp4);
		fb_textout(50, 170, buf);
		memset(buf, 0, sizeof(buf));
        fb_textout(50, 190, "STORAGE HUMIDITY(�洢ʪ��): ");
        /* ��LCD����ʾ��ǰʪ�� */
		sprintf((char *)buf,"%.0f%%RH--%.0f%%RH--%.0f%%RH--%.0f%%RH", Rhum1,Rhum2,Rhum3,Rhum4);
		fb_textout(50, 210, buf);
		memset(buf, 0, sizeof(buf));
    }
    return 0;
}



