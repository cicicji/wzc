
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
    
    //初始化内存堆
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
    
    //按键初始化
    KEY_Init();
    I2C1_init();
    Get_HDC_ID();
    //打开显示
    fb_open();
    delay_ms(200);
    //SPI0控制器初始化
    ls1x_spi_initialize(busSPI0);
    /*
    * 备注：W25X40芯片内部写有PMON，占用空间约268K，因此测试W25X40读写功能时，必须在268K空间以外
    * 本例程测试地址为300-512K空间
    * nor flash特性，写数据之前必须要先擦除，且芯片擦除次数是有寿命限制的
    */
    unsigned int id,wroffset = 0x51000,rdoffset = 0x51000;
	char sbuf[20];
	//初始化W25X40芯片
	ls1x_w25x40_init(busSPI0, NULL);
	//获取W25X40芯片ID
	ls1x_w25x40_ioctl(busSPI0, IOCTL_W25X40_READ_ID, &id);
    sprintf((char *)sbuf,"W25X40 ID:%x\n",id);
	unsigned char tx_count = 0;
	unsigned char rbuf[16],tbuf[16];
	unsigned char s1[65],s2[65];
    
    fb_textout(50, 30, "传感器模块测试");
    
    
    /*
     * 裸机主循环
     */
    for (;;)
    {
		delay_ms(1000);
		fb_fillrect(50, 50, 600, 300, cidxBLACK);
		fb_textout(50, 50, "温湿度传感器");
		fb_textout(50, 70, "CURRENT TEMPERATURE(当前温度): ");
		fb_textout(50, 90, "CURRENT HUMIDITY(当前湿度): ");
		HDC_Get_Temp_Hum(&temp, &hum);
		/* 在LCD上显示当前温度 */
		sprintf((char *)buf,"%.1f度", temp);
		fb_textout(300, 70, buf);
		memset(buf, 0, sizeof(buf));
		/* 在LCD上显示当前湿度 */
		sprintf((char *)buf,"%.0f%%RH", hum);
		fb_textout(300, 90, buf);
		memset(buf, 0, sizeof(buf));
		printf("温度：%.2f\r\n",temp);
		printf("湿度：%.2f\r\n",hum);
		if(temp >= 34)
		{
			LED8_ON();
			fb_textout(50, 110, "指示灯状态：LED8_ON ");
		}
		else
		{
			LED8_OFF();
			fb_textout(50, 110, "指示灯状态：LED8_OFF");
		}
		
		//从W25X40芯片读取温湿度数据
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
		
		//记录温湿度数据
		if(num==16)
		{
		    num=0;
        }
		tbuf[num]=data_buf[0];
		tbuf[num+1]=data_buf[1];
		tbuf[num+2]=data_buf[2];
		tbuf[num+3]=data_buf[3];
		num+=4;
		//按下按键记录存储温湿度数据
        Key = KEY_Scan();
		if(Key==4)
		{
		    fb_textout(50, 250, "开始存储当前温湿度");
		    ls1x_w25x40_ioctl(busSPI0, IOCTL_W25X40_ERASE_4K, wroffset);  //spinor flash写之前都要先擦除
		    ls1x_w25x40_write(busSPI0, tbuf, 16, &wroffset);
        }
       	/* 在LCD上显示当前温度 */
        fb_textout(50, 150, "STORAGE TEMPERATURE(存储温度): ");
        sprintf((char *)buf,"%.1f度--%.1f度--%.1f度--%.1f度", Rtemp1,Rtemp2,Rtemp3,Rtemp4);
		fb_textout(50, 170, buf);
		memset(buf, 0, sizeof(buf));
        fb_textout(50, 190, "STORAGE HUMIDITY(存储湿度): ");
        /* 在LCD上显示当前湿度 */
		sprintf((char *)buf,"%.0f%%RH--%.0f%%RH--%.0f%%RH--%.0f%%RH", Rhum1,Rhum2,Rhum3,Rhum4);
		fb_textout(50, 210, buf);
		memset(buf, 0, sizeof(buf));
    }
    return 0;
}



