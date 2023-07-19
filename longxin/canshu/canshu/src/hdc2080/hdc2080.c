/*
 * hdc2080.c
 *
 * created: 2021/6/5
 *  author: 
 */
 #include "hdc2080.h"
 #include "ls1b_gpio.h"
 #include "ls1x_i2c_bus.h"
 #include <stdint.h>

 
 #define HDC2080_ADDRESS 0x40
 #define HDC2080_Write 0
 #define HDC2080_Read  1
 
 #define Temp_Hum_addr  0x00    //��ʪ�ȼĴ�������ʼ��ַ
 #define Meas_Conf_addr 0x0f
 #define Device_addr    0xfe
 
 #define LED8_IO 52
 
 unsigned char data_buf[4] = {0};//�������ֽڷֱ�Ϊ�¶ȣ������ֽ�Ϊʪ��
 /************************************************************************
** ���ܣ�  ��HDC2080д������
** ������
           @reg_buf:�Ĵ�����ַ
           @buf:���ݻ�������
           @len:д���ݳ���
** ����ֵ��0,�ɹ�;-1,ʧ��.
*************************************************************************/
static char HDC_WR_Data(unsigned char reg_addr, unsigned char *buf, int len)
{
	int ret=0;

	//��ʼ�ź�
	ret = ls1x_i2c_send_start(busI2C1,NULL);
    if(ret < 0)
    {
        printf("send_start error!!!\r\n");
        return -1;
    }

 	//���ʹӻ���ַ��д����
 	ret = ls1x_i2c_send_addr(busI2C1, HDC2080_ADDRESS, HDC2080_Write);
 	if(ret < 0)
    {
        printf("send_addr error!!!\r\n");
        return -1;
    }

    //���ͼĴ�����ַ
	ret = ls1x_i2c_write_bytes(busI2C1, &reg_addr, 1);
	if(ret < 0)
    {
        printf("write_bytes_reg error!!!\r\n");
        return -1;
    }

    //��������
	ret = ls1x_i2c_write_bytes(busI2C1, buf, len);
	if(ret < 0)
    {
        printf("write_bytes error!!!\r\n");
        return -1;
    }

    //����ֹͣ�ź�
    ret = ls1x_i2c_send_stop(busI2C1,NULL);
    if(ret < 0)
    {
        printf("send_stop error!!!\r\n");
        return -1;
    }

	return ret;
}

/************************************************************************
** ���ܣ�  ��HDC2080��������
** ������
           @reg_buf:�Ĵ����ĵ�ַ
           @buf:���ݻ�������
           @len:д���ݳ���
** ����ֵ��0,�ɹ�;-1,ʧ��.
*************************************************************************/
static char HDC_RD_Data(unsigned char reg_addr,unsigned char *buf,int len)
{
	int ret=0;

 	//��ʼ�ź�
	ret = ls1x_i2c_send_start(busI2C1,NULL);
    if(ret < 0)
    {
        printf("send_start error!!!\r\n");
        return -1;
    }

 	//���ʹӻ���ַ��д����
 	ret = ls1x_i2c_send_addr(busI2C1, HDC2080_ADDRESS, HDC2080_Write);
 	if(ret < 0)
    {
        printf("send_addr_W error!!!\r\n");
        return -1;
    }

    //���ͼĴ�����ַ
	ret = ls1x_i2c_write_bytes(busI2C1, &reg_addr, 1);
	if(ret < 0)
    {
        printf("write_bytes_reg error!!!\r\n");
        return -1;
    }

    //����ֹͣ�ź�
    ret = ls1x_i2c_send_stop(busI2C1,NULL);
    if(ret < 0)
    {
        printf("send_stop error!!!\r\n");
        return -1;
    }

    //��ʼ�ź�
	ret = ls1x_i2c_send_start(busI2C1,NULL);
    if(ret < 0)
    {
        printf("send_start error!!!\r\n");
        return -1;
    }

    //���ʹӻ���ַ�Ͷ�����
 	ret = ls1x_i2c_send_addr(busI2C1, HDC2080_ADDRESS, HDC2080_Read);
 	if(ret < 0)
    {
        printf("send_addr_R error!!!\r\n");
        return -1;
    }

    //��ȡ����
    ls1x_i2c_read_bytes(busI2C1,buf,len);
    if(ret < 0)
    {
        printf("read_bytes_Data error!!!\r\n");
        return -1;
    }

    //����ֹͣ�ź�
    ret = ls1x_i2c_send_stop(busI2C1,NULL);
    if(ret < 0)
    {
        printf("send_stop error!!!\r\n");
        return -1;
    }

    return 0;
}

 /************************************************************************
 ** ���ܣ�  ��ʼ��I2C1
 ** ˵������ʼ��I2C1ʱҪ�Ƚ�GPIO38/39����ΪI2C1����
*************************************************************************/
 void I2C1_init(void)
 {
    //��GPIO38/39����Ϊ��ͨ����
    gpio_disable(38);
    gpio_disable(39);

    //��GPIO38/39����ΪI2C1����
    LS1B_MUX_CTRL0 |= 1 << 24;

    //��ʼ��I2C1������
    ls1x_i2c_initialize(busI2C1);
 }
 
/************************************************************************
 ** ���ܣ���ȡHDC�豸��ID
 ** ˵����������ֵΪ0x07d0���⵽���豸
*************************************************************************/
 void Get_HDC_ID(void)
 {
     unsigned char Device_ID[2];
     
     gpio_enable(LED8_IO, DIR_OUT);
     gpio_write(LED8_IO, 1);
     
     HDC_RD_Data(Device_addr, Device_ID, 2);
     printf("HDC2080���豸IDΪ��%#x%x\r\n",Device_ID[1],Device_ID[0]);
 }
 
/************************************************************************
 ** ���ܣ�  ��ȡ��ʪ��ֵ
 ** ˵����
*************************************************************************/
 void HDC_Get_Temp_Hum(float *temp, float *hum)
 {
     //unsigned char data_buf[4] = {0};//�������ֽڷֱ�Ϊ�¶ȣ������ֽ�Ϊʪ��
     unsigned char HDC_Conf = 0x01; //��ʼ����
     uint16_t temp_data,hum_data;
     
     //��ʼ����
     HDC_WR_Data(Meas_Conf_addr, &HDC_Conf, 1);
     delay_us(10);
     //��ȡ����
     HDC_RD_Data(Temp_Hum_addr, data_buf, 4);
     
     
     temp_data = ((uint16_t)data_buf[1] << 8) | data_buf[0];
     hum_data = ((uint16_t)data_buf[3] << 8) | data_buf[2];
     
     *temp = temp_data / 65535.0 *165 - 40;
     *hum = hum_data / 65535.0 * 100;
 }

void LED8_ON(void)
{
    gpio_write(LED8_IO, 0);
}

void LED8_OFF(void)
{
    gpio_write(LED8_IO, 1);
}

