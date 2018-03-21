#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<fcntl.h>
#include <malloc.h>

#include<linux/fb.h>

#include <jpeglib.h>
#include <jerror.h>

/*	RGB565转RGB24函数
	*@rgb565: 指向存放rgb565数据的起始地址
	*@rgb24：指向存放rgb24数据的起始地址
	*@width：屏幕（分辨率）的宽度
	*@height：屏幕（分辨率）的高度
*/
int RGB565_to_RGB24(unsigned char *rgb565,unsigned char *rgb24,int width,int height)
{
     int i;
     int whole = width*height;
	unsigned char r,g,b;
	unsigned short int *pix565;
	
	pix565 = (unsigned short int *)rgb565;
	
		for(i = 0;i < whole;i++)
		{    
			r = ((*pix565)>>11)&0x1f;
			*rgb24 = (r<<3) | (r>>2);
			rgb24++;
			g = ((*pix565)>>5)&0x3f;
			*rgb24 = (g<<2) | (g>>4);
			rgb24++;
			b = (*pix565)&0x1f;
			*rgb24 = (b<<3) | (b>>2);
			rgb24++;
			pix565++;    
				
		}
		return 1;
	}
	
/*jpeg压缩函数
*	@rgb：指向存放rgb24数据的起始地址
*	@width：屏幕（分辨率）的宽度
*	@height：屏幕（分辨率）的高度
*/
int jpeg_compress(unsigned char *rgb,int width,int height)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPROW row_pointer[1];
	int row_stride;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
		//输出文件名为：out.jpg
	if ((outfile = fopen("out.jpg", "wb")) == NULL)
	{
		printf("can not open out.jpg\n");
		return -1;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	//输入数据格式为RGB
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	//压缩质量为80
	jpeg_set_quality(&cinfo, 80, TRUE );
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = width * 3;

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &rgb[cinfo.next_scanline * row_stride];
			(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	return 1;
}
	
	
int main()
{
	
	int fd;
	struct fb_var_screeninfo fb_var_info;
	struct fb_fix_screeninfo fb_fix_info;
	unsigned char *trgb;
	unsigned char *rgb;
	int buffer_size;
	
	//打开framebuffer设备
	fd = open("/dev/fb0",O_RDONLY);
	if(fd < 0)
	{
		printf("can not open dev\n");
		exit(1);
	}
	
	//获取LCD的可变参数
	ioctl(fd,FBIOGET_VSCREENINFO,&fb_var_info);
	//一个像素多少位    
	printf("bits_per_pixel: %d\n",fb_var_info.bits_per_pixel);
	//x分辨率
	printf("xres: %d\n",fb_var_info.xres);
	//y分辨率
	printf("yres: %d\n",fb_var_info.yres);
	//r分量长度（bit）
	printf("red_length: %d\n",fb_var_info.red.length);
	//g分量长度（bit）
	printf("green_length: %d\n",fb_var_info.green.length);
	//b分量长度（bit）
	printf("blue_length: %d\n",fb_var_info.blue.length);
	//t(透明度)分量长度（bit）
	printf("transp_length: %d\n",fb_var_info.transp.length);
	//r分量偏移
	printf("red_offset: %d\n",fb_var_info.red.offset);
	//g分量偏移
	printf("green_offset: %d\n",fb_var_info.green.offset);
	//b分量偏移
	printf("blue_offset: %d\n",fb_var_info.blue.offset);
	//t分量偏移
	printf("transp_offset: %d\n",fb_var_info.transp.offset);

	//获取LCD的固定参数
	ioctl(fd,FBIOGET_FSCREENINFO,&fb_fix_info);
	//一帧大小
	printf("smem_len: %d\n",fb_fix_info.smem_len);
	//一行大小
	printf("line_length: %d\n",fb_fix_info.line_length);

	//一帧大小
	buffer_size = (fb_var_info.xres * fb_var_info.yres * fb_var_info.bits_per_pixel/8);

	trgb = (unsigned char *)malloc(buffer_size);
	if(trgb==NULL)
		exit(0);
	rgb = (unsigned char *)malloc(fb_var_info.xres * fb_var_info.yres * 3);
	if(rgb==NULL)
	{
		goto here;
	}
	//获取一帧数据
	if(read(fd,trgb,buffer_size) < 0)
	{
		 printf("reaf failed!\n");
		 goto read_fail;
	}
	//格式转换
	//RGB565_to_RGB24(trgb,rgb,fb_var_info.xres,fb_var_info.yres);
	//jpeg压缩
	if(jpeg_compress(rgb,fb_var_info.xres,fb_var_info.yres)<0)
		printf("compress failed!\n");        

read_fail:
	free(rgb);
here:
	free(trgb);
 
	close(fd);
 
	return 0;
}
