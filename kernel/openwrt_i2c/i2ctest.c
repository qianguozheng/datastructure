/* Test I2C device work right or not, below function can be called in uboot to check whether the hardware is right or not.
 * We can also using this to test whether Linux system support I2C operation.
 * 
 * Author: Richard. 
 * 		   Dong WenCai.
 * Date:   2015-10-9
 * 
 * Actually, below code is for MT7620A chip, and code copy from openwrt i2c buses.
 * */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/udp.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/inet.h>
#include <linux/netfilter.h>
#include <net/route.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#if 1
#define BIT(n)      (1 << n)
#define REG_CONFIG_REG      0x00
#define REG_CLKDIV_REG      0x04
#define REG_DEVADDR_REG     0x08
#define REG_ADDR_REG        0x0C
#define REG_DATAOUT_REG     0x10
#define REG_DATAIN_REG      0x14
#define REG_STATUS_REG      0x18
#define REG_STARTXFR_REG    0x1C
#define REG_BYTECNT_REG     0x20

#define I2C_STARTERR        BIT(4)
#define I2C_ACKERR      BIT(3)
#define I2C_DATARDY     BIT(2)
#define I2C_SDOEMPTY        BIT(1)
#define I2C_BUSY        BIT(0)

#define I2C_DEVADLEN_7      (6 << 2)
#define I2C_ADDRDIS     BIT(1)

#define I2C_RETRY       0x400

#define CLKDIV_VALUE        200 // clock rate is 40M, 40M / (200*2) = 100k (standard i2c bus rate).
//#define CLKDIV_VALUE      50 // clock rate is 40M, 40M / (50*2) = 400k (fast i2c bus rate).

#define READ_CMD        0x01
#define WRITE_CMD       0x00
#define WRITE_CMD_NOSTOP 0x04
#define READ_BLOCK              64

#define I2C_M_WR_NOSTOP     0x0004

#define RGBLED_DEVADDR      0x45
#define AXP209_DEVADDR      0x34

struct i2c_msg {
    __u16 addr; /* slave address            */
    __u16 flags;
#define I2C_M_TEN       0x0010  /* this is a ten bit chip address */
#define I2C_M_RD        0x0001  /* read data, from slave to master */
#define I2C_M_STOP      0x8000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART       0x4000  /* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR  0x2000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK    0x1000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK     0x0800  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN      0x0400  /* length will be first received byte */
    __u16 len;      /* msg length               */
    __u8 *buf;      /* pointer to msg data          */
};
static void  *membase = (void *)0xb0000900;
static void rt_i2c_w32(u32 val, unsigned reg)
{
    //iowrite32(val, membase + reg);
    *(u32 *)(membase + reg) = val;
}

static u32 rt_i2c_r32(unsigned reg)
{
    //return ioread32(membase + reg);
    return *(u32 *)(membase + reg);
}
static inline int rt_i2c_wait_tx_done(void)
{
    int retries = I2C_RETRY;

    do {
        if (!retries--)
            break;
    } while(!(rt_i2c_r32(REG_STATUS_REG) & I2C_SDOEMPTY));

    return (retries < 0);
}
static inline int rt_i2c_wait_rx_done(void)
{
    int retries = I2C_RETRY;

    do {
        if (!retries--)
            break;
    } while(!(rt_i2c_r32(REG_STATUS_REG) & I2C_DATARDY));

    return (retries < 0);
}
static inline int rt_i2c_wait_idle(void)
{
    int retries = I2C_RETRY;

    do {
        if (!retries--)
            break;
    } while(rt_i2c_r32(REG_STATUS_REG) & I2C_BUSY);

    return (retries < 0);
}
static int rt_i2c_handle_msg( struct i2c_msg* msg)
{
    int i = 0, j = 0, pos = 0;
    int nblock = msg->len / READ_BLOCK;
        int rem = msg->len % READ_BLOCK;

    if (msg->flags & I2C_M_TEN) {
        return -1;
    }

    if (msg->flags & I2C_M_RD) {
        for (i = 0; i < nblock; i++) {
            rt_i2c_wait_idle();
            rt_i2c_w32(READ_BLOCK - 1, REG_BYTECNT_REG);
            rt_i2c_w32(READ_CMD, REG_STARTXFR_REG);
            for (j = 0; j < READ_BLOCK; j++) {
                if (rt_i2c_wait_rx_done())
                    return -1;
                msg->buf[pos++] = rt_i2c_r32(REG_DATAIN_REG);
            }
        }

        rt_i2c_wait_idle();
        rt_i2c_w32(rem - 1, REG_BYTECNT_REG);
        rt_i2c_w32(READ_CMD, REG_STARTXFR_REG);
        for (i = 0; i < rem; i++) {
            if (rt_i2c_wait_rx_done())
                return -1;
            msg->buf[pos++] = rt_i2c_r32(REG_DATAIN_REG);
        }
    }
    else if(msg->flags & I2C_M_WR_NOSTOP){
        rt_i2c_wait_idle();
        rt_i2c_w32(msg->len - 1, REG_BYTECNT_REG);
        for (i = 0; i < msg->len; i++) {
            rt_i2c_w32(msg->buf[i], REG_DATAOUT_REG);
            rt_i2c_w32(WRITE_CMD_NOSTOP, REG_STARTXFR_REG);
            if (rt_i2c_wait_tx_done())
                return -1;
        }
    }
    else {
        rt_i2c_wait_idle();
        rt_i2c_w32(msg->len - 1, REG_BYTECNT_REG);
        for (i = 0; i < msg->len; i++) {
            rt_i2c_w32(msg->buf[i], REG_DATAOUT_REG);
            rt_i2c_w32(WRITE_CMD, REG_STARTXFR_REG);
            if (rt_i2c_wait_tx_done())
                return -1;
        }
    }

    return 0;
}

static int rt_i2c_master_xfer( struct i2c_msg *m, int n)
{
    int i = 0;
    int ret = 0;

    if (rt_i2c_wait_idle()) {
        return 0;
    }

    rt_i2c_w32(m->addr, REG_DEVADDR_REG);
    rt_i2c_w32(I2C_DEVADLEN_7 | I2C_ADDRDIS, REG_CONFIG_REG);
    rt_i2c_w32(CLKDIV_VALUE, REG_CLKDIV_REG);

    for (i = 0; i < n && !ret; i++)
        ret = rt_i2c_handle_msg(&m[i]);

    if (ret) {
        return 0;
    }

    return n;
}
unsigned char rt_i2c_read(unsigned char dev_addr,
               unsigned char sub_addr,
               unsigned char *data, int len)
{
    struct i2c_msg msgs[2];
    msgs[0].addr = dev_addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &sub_addr;

    msgs[1].addr = dev_addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = len;
    msgs[1].buf = data;
    return rt_i2c_master_xfer(msgs, 2) == 2;
}
unsigned char rt_i2c_write(unsigned char dev_addr,
               unsigned char sub_addr,
               unsigned char *data, int len)
{
    struct i2c_msg msgs[1];
    unsigned char buf[len + 1];

    msgs[0].addr = dev_addr;
    msgs[0].flags = 0;
    msgs[0].len = len+1;
    msgs[0].buf = buf;
    msgs[0].buf[0] = sub_addr;
    memcpy(msgs[0].buf+1, data, len);
    return rt_i2c_master_xfer(msgs, 1) == 1;
}

static void delay()
{
    unsigned short i = 0;
    for( ; i; i++);
}
static unsigned char wait_irq()
{
    unsigned short times = 0;
    unsigned char value = 0x00;
    while(++ times)
    {
        rt_i2c_read(RGBLED_DEVADDR, 0x02, &value, 1);
        if(value & 0xE0)
            break;
        delay();
    }
    return times;
}
void rgbled_init(void)
{
    unsigned char value = 0xff;
    unsigned char r,g,b;
    r = g = b = 0x00;
    rt_i2c_write(AXP209_DEVADDR, 0x82, &value, 1);
    value = 0xE1;
    rt_i2c_write(RGBLED_DEVADDR, 0x01, &value, 1);
    value = 0x07;
    rt_i2c_write(RGBLED_DEVADDR, 0x30, &value, 1);
    value = 0x03;
    rt_i2c_write(RGBLED_DEVADDR, 0x31, &value, 1);
    rt_i2c_write(RGBLED_DEVADDR, 0x32, &value, 1);
    rt_i2c_write(RGBLED_DEVADDR, 0x33, &value, 1);
    rt_i2c_write(RGBLED_DEVADDR, 0x34, &b, 1);
    rt_i2c_write(RGBLED_DEVADDR, 0x35, &g, 1);
    rt_i2c_write(RGBLED_DEVADDR, 0x36, &r, 1);
    rt_i2c_read(AXP209_DEVADDR, 0xB9, &value, 1);
    value = value * 100 / 0x7f;
    if(value < 5)
    {
        r = 255;
        g = 0;
        b = 0;
        value = 0x13;
        rt_i2c_write(RGBLED_DEVADDR, 0x33, &value, 1);
        rt_i2c_write(RGBLED_DEVADDR, 0x34, &b, 1);
        rt_i2c_write(RGBLED_DEVADDR, 0x35, &g, 1);
        rt_i2c_write(RGBLED_DEVADDR, 0x36, &r, 1);
        rt_i2c_write(RGBLED_DEVADDR, 0x3f, &value, 1);
        value = 0x80;
    if(wait_irq())
            rt_i2c_write(AXP209_DEVADDR, 0x32, &value, 1);
    }
    else if(value)
    {
       r = 255; 
       g = 156;
       b = 00;
       value = 0x13;
       rt_i2c_write(RGBLED_DEVADDR, 0x31, &value, 1);
       rt_i2c_write(RGBLED_DEVADDR, 0x32, &value, 1);
       rt_i2c_write(RGBLED_DEVADDR, 0x33, &value, 1);
       rt_i2c_write(RGBLED_DEVADDR, 0x34, &b, 1);
       rt_i2c_write(RGBLED_DEVADDR, 0x35, &g, 1);
       rt_i2c_write(RGBLED_DEVADDR, 0x36, &r, 1);
       value = 0x13;
       rt_i2c_write(RGBLED_DEVADDR, 0x37, &value, 1);
       value = 0x03;
       rt_i2c_write(RGBLED_DEVADDR, 0x3A, &value, 1);
       rt_i2c_write(RGBLED_DEVADDR, 0x3D, &value, 1);
       value = 0x00;
       rt_i2c_write(RGBLED_DEVADDR, 0x3f, &value, 1);
    }
    printf("elec %x\n", value);
}
#endif

static int __init myhook_init(void)
{
	unsigned char value;
	value =0xff;
	
	rt_i2c_read(0x50, 0x00, &value, 1);
	prink("%d", value);
    return 0;//nf_register_hook(&nfho);
}

static void __exit myhook_fini(void)
{
    //nf_unregister_hook(&nfho);
}

module_init(myhook_init);
module_exit(myhook_fini);
