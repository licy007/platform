#ifndef __DEINTERLACE_3_H__
#define __DEINTERLACE_3_H__

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define    DI_IOC_MAGIC        'D'

#define DI_IO(nr)             _IO(DI_IOC_MAGIC, nr)
#define DI_IOR(nr, size)      _IOR(DI_IOC_MAGIC, nr, size)
#define DI_IOW(nr, size)      _IOW(DI_IOC_MAGIC, nr, size)
#define DI_IOWR(nr, size)     _IOWR(DI_IOC_MAGIC, nr, size)
#define DI_IOCTL_NR(n)        _IOC_NR(n)

#define DI_IOC_GET_VERSION    DI_IOR(0x0, struct di_version)
#define DI_IOC_RESET          DI_IO(0x1)
#define DI_IOC_CHECK_PARA     DI_IO(0x2)
#define DI_IOC_SET_TIMEOUT    DI_IOW(0x3, struct di_timeout_ns)
#define DI_IOC_SET_VIDEO_SIZE DI_IOW(0x4, struct di_size)
#define DI_IOC_SET_DIT_MODE   DI_IOW(0x5, struct di_dit_mode)
#define DI_IOC_SET_TNR_MODE   DI_IOW(0x6, struct di_tnr_mode)
#define DI_IOC_SET_FMD_ENABLE DI_IOW(0x7, struct di_fmd_enable)
#define DI_IOC_PROCESS_FB     DI_IOW(0x8, struct di_process_fb_arg)
#define DI_IOC_SET_VIDEO_CROP DI_IOW(0x9, struct di_rect)
#define DI_IOC_MEM_REQUEST    DI_IOWR(0x10, struct di_mem_arg)
#define DI_IOC_MEM_RELEASE    DI_IOWR(0x11, struct di_mem_arg)
#define DI_IOC_SET_DEMO_CROP  DI_IOW(0x12, struct di_demo_crop_arg)

#define fourcc_code(a, b, c, d) ((unsigned int)(a) | ((unsigned int)(b) << 8) | \
                                ((unsigned int)(c) << 16) | ((unsigned int)(d) << 24))

#define DRM_FORMAT_YUV422   fourcc_code('Y', 'U', '1', '6') /* 2x1 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YUV420   fourcc_code('Y', 'U', '1', '2') /* 2x2 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU420   fourcc_code('Y', 'V', '1', '2') /* 2x2 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_NV12     fourcc_code('N', 'V', '1', '2') /* 2x2 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV21     fourcc_code('N', 'V', '2', '1') /* 2x2 subsampled Cb:Cr plane */

#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long

enum
{
    DI_DIT_INTP_MODE_INVALID = 0x0,
    DI_DIT_INTP_MODE_WEAVE,
    DI_DIT_INTP_MODE_BOB,
    DI_DIT_INTP_MODE_MOTION,
};

enum
{
    DI_DIT_OUT_0FRAME = 0x0,
    DI_DIT_OUT_1FRAME,
    DI_DIT_OUT_2FRAME,
};

enum
{
    DI_TNR_MODE_INVALID = 0,
    DI_TNR_MODE_ADAPTIVE,
    DI_TNR_MODE_FIX,
};

enum
{
    DI_TNR_LEVEL_HIGH = 0,
    DI_TNR_LEVEL_MIDDLE,
    DI_TNR_LEVEL_LOW,
};

struct di_version
{
    u32 version_major;
    u32 version_minor;
    u32 version_patchlevel;
    u32 ip_version;
};

struct di_timeout_ns
{
    u64 wait4start;
    u64 wait4finish;
};

/*
 * @intp_mode: see enum DI_DIT_INTP_MODE_XXX.
 * @out_frame_mode: see enum DI_DIT_OUT_XFRAME.
 */
struct di_dit_mode
{
    u32 intp_mode;
    u32 out_frame_mode;
};

/*
 * @mode:
 * @level:
 */
struct di_tnr_mode
{
    u32 mode;
    u32 level;
};

/* 0:disable; 1:enable. */
struct di_fmd_enable
{
    u32 en;
};

struct di_size
{
    u32 width;
    u32 height;
};

struct di_rect
{
    u32 left;
    u32 top;
    u32 right;
    u32 bottom;
};

/*
 * support dma_buf method or phy_addr_buf method.
 * 1.On dma_buf method:
 *     @y_addr is address-offset of luma-buf to dma_addr;
 *     @cb_addr/@cr_addr is address-offset of chroma-buf to dma_addr.
 * 2.On phy_addr_buf method:
 *     @y_addr is physical address of luma-buf;
 *     @cb_addr/@cr_addr is physical address of chroma-buf.
 *
 * @ystride: line stride of luma buf. unit: byte.
 * @cstride: line stride of chroma buf. unit: byte.
 */
struct di_buf
{
    u64 y_addr;
    u64 cb_addr;
    u64 cr_addr;
    u32 ystride;
    u32 cstride;
};

/*
 * @format: see DRM_FORMAT_XXX in drm_fourcc.h.
 * @dma_buf_fd: dma buf fd that from userspace.
 *    @dma_buf_fd must be invalid(<0) on phy_addr_buf method.
 * @size.width,@size.height: size of pixel datas of image. unit: pixel.
 */
struct di_fb
{
    u32 format;
    int dma_buf_fd;
    struct di_buf buf;
    struct di_size size;
};

/*
 * @base_field:
 *     0: top_field;
 *     1: bottom_field.
 */
struct di_process_fb_arg
{

    u8 is_interlace; /*must be set*/
    u8 is_pulldown; /* fixme: define enum value, not used in driver*/
    u8 top_field_first; /*recorrected by drivers*/
    u8 base_field; /*top_filed or bottom_filed, if use BOB, must be set*/

    struct di_fb in_fb0;
    struct di_fb in_fb1;
    struct di_fb in_fb2;

    struct di_fb out_dit_fb0;
    struct di_fb out_dit_fb1;
    struct di_fb out_tnr_fb0;

};

struct di_mem_arg
{
    unsigned int size;
    unsigned int handle;
    u64 phys_addr;
};

struct di_demo_crop_arg
{
    struct di_rect dit_demo;
    struct di_rect tnr_demo;
};

enum di_mode_e
{
    DI300_MODE_INVALID = 0,
    DI300_MODE_60HZ,
    DI300_MODE_30HZ,
    DI300_MODE_BOB,
    DI300_MODE_WEAVE,
    DI300_MODE_TNR, /* only tnr */
};

typedef struct DiParaT3
{
    unsigned char is_interlace; /*must be set*/
    unsigned char is_pulldown; /* fixme: define enum value, not used in driver*/
    unsigned char top_field_first; /*recorrected by drivers*/
    unsigned char base_field; /*top_filed or bottom_filed, if use BOB, must be set*/

    struct di_fb in_fb0;
    struct di_fb in_fb1;
    struct di_fb in_fb2;

    struct di_fb out_dit_fb0;
    struct di_fb out_dit_fb1;
    struct di_fb out_tnr_fb0;

}DiParaT3;

#endif