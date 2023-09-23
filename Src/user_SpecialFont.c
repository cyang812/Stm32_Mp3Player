#include "main.h"

const uint8_t special_font_dab[][72] = {

	// ( �������
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,
	0x10,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,
	0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,
	0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x08,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	// ) �������
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x40,
	0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,
	0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,
	0x20,0x00,0x00,0x20,0x00,0x00,0x40,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

uint8_t special_font_idx(uint16_t unicode)
{
	uint8_t font_idx;
	switch (unicode)
		{
		case 0x3014:  // ������
			font_idx = 0;
			break;
		case 0x3015:  // ������
			font_idx = 1;
			break;
		case 0xff08:  // ������
			font_idx = 1;
			break;
		case 0xff09:  // ������
			font_idx = 2;
			break;
		case 0x300a:  // ��
			font_idx = 3;
			break;
		case 0x300b:  //  ��
			font_idx = 4;
			break;
		default:
			font_idx = 0xff;
			break;
		}
	printf("special font: %#x font_idx: %d\n",unicode,font_idx);
	return font_idx;
};

uint8_t Get_Special_font(uint8_t *buff,uint16_t unicode)
{
	uint8_t flag = OK;
	uint8_t font_idx;

	font_idx = special_font_idx(unicode);
	if(font_idx == 0xff) // û�ҵ���Ӧ�ַ�����
	{
		flag = ERROR; // not found
	}
	else
	{
		for(int i=0; i<72; i++)
		{
			buff[i] = special_font_dab[font_idx][i];
		}
	}
	printf("get_special_font status:%d\n",flag);
	return flag;
}
