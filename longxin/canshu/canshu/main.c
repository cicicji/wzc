
#include <stdio.h>

#include "ls1b.h"
#include "mips.h"

#include "bsp.h"
#include "libc/lwmem.h"
#include "ls1x_can.h"
#include "ls1x_fb.h"
#include "ls1b_gpio.h"
#include "src/hdc2080/hdc2080.h"


char LCD_display_mode[] = LCD_480x800;
#define S_LEN   64

int main(void)
{
    printk("\r\nmain() function.\r\n");
    
    //初始化内存堆
    lwmem_initialize(0);
    
    unsigned char sta = 0;
    float temp = 0,hum = 0,press = 0,eleva = 0,lx = 0;
    float Cantemp = 0,Canhum = 0;
    uint16_t temp_data,hum_data;
    static char cnt = 0;
    unsigned char Key = 0;
    unsigned char buf[20] = {0};
    I2C1_init();
    Get_HDC_ID();
    //打开显示
    fb_open();
    delay_ms(200);
    
    //初始化CAN1控制器、设置CAN模式&速率
    ls1x_can_init(devCAN1, NULL);
	ls1x_can_open(devCAN1, NULL);
	ls1x_can_ioctl(devCAN1, IOCTL_CAN_SET_CORE, CAN_CORE_20B); 	    /* set mode: CAN_CORE_20B */
	ls1x_can_ioctl(devCAN1, IOCTL_CAN_SET_SPEED,  CAN_SPEED_250K); 	/* set speed: CAN_SPEED_500K */
    ls1x_can_ioctl(devCAN1, IOCTL_CAN_START, NULL);          	    /* start It */

    unsigned char s1[S_LEN+1], s2[S_LEN+1];;
    unsigned int tx_count = 0;
    int wr_cnt,rd_cnt;
    CANMsg_t msg,rmsg;
    msg.id = 2; // MSG_ID;
	msg.extended = 1;
	msg.rtr = 0;
	msg.len = 4;
	
    fb_textout(50, 30, "工业多点温度测量系统");
    
    /*
     * 裸机主循环
     */
    for (;;)
    {
                    delay_ms(1000);
                    fb_fillrect(50, 50, 600, 220, cidxBLACK);
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
                    msg.data[0] = data_buf[0];
                    msg.data[1] = data_buf[1];
                    msg.data[2] = data_buf[2];
                    msg.data[3] = data_buf[3];
                    //CAN1发送数据
		            ls1x_can_write(devCAN1, (const void *)&msg, sizeof(msg), NULL);
                    //CAN1接收数据
                    ls1x_can_read(devCAN1, (void *)&rmsg, sizeof(rmsg), NULL);
                    //转换温湿度数值
                    temp_data = ((uint16_t)rmsg.data[1] << 8) | rmsg.data[0];
		            hum_data = ((uint16_t)rmsg.data[3] << 8) | rmsg.data[2];
		            Cantemp = temp_data / 65535.0 *165 - 40;
		            Canhum = hum_data / 65535.0 * 100;
		            //显示接收数据
		            fb_textout(50, 150, "Can总线接收温湿度数据：");
		            sprintf((char *)buf,"%.1f度", Cantemp);
		            fb_textout(50, 170, "接收温度值：");
		            fb_textout(200, 170, buf);
		            memset(buf, 0, sizeof(buf));
		            sprintf((char *)buf,"%.0f%%RH", Canhum);
		            fb_textout(50, 190, "接收湿度值：");
		            fb_textout(200, 190, buf);
		            memset(buf, 0, sizeof(buf));
		            
    }

    return 0;
}



