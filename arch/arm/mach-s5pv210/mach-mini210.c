/* linux/arch/arm/mach-s5pv210/mach-mini210.c
 *
 * Copyright (c) 2013 Jeff Kent
 *
 * Adapted from mach-smdkc110.c
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/device.h>
#include <linux/dm9000.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <mach/map.h>
#include <mach/regs-clock.h>

#include <plat/regs-serial.h>
#include <plat/regs-srom.h>
#include <plat/gpio-cfg.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <linux/platform_data/i2c-s3c2410.h>
#include <plat/pm.h>
#include <plat/samsung-time.h>
#include <plat/clock.h>

#include "common.h"

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define MINI210_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define MINI210_ULCON_DEFAULT	S3C2410_LCON_CS8

#define MINI210_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

static struct s3c2410_uartcfg mini210_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= MINI210_UCON_DEFAULT,
		.ulcon		= MINI210_ULCON_DEFAULT,
		.ufcon		= MINI210_UFCON_DEFAULT,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= MINI210_UCON_DEFAULT,
		.ulcon		= MINI210_ULCON_DEFAULT,
		.ufcon		= MINI210_UFCON_DEFAULT,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= MINI210_UCON_DEFAULT,
		.ulcon		= MINI210_ULCON_DEFAULT,
		.ufcon		= MINI210_UFCON_DEFAULT,
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= MINI210_UCON_DEFAULT,
		.ulcon		= MINI210_ULCON_DEFAULT,
		.ufcon		= MINI210_UFCON_DEFAULT,
	},
};

static struct resource mini210_dm9000_resources[] = {
	[0] = DEFINE_RES_MEM(S5PV210_PA_SROM_BANK1, 1),
	[1] = DEFINE_RES_MEM(S5PV210_PA_SROM_BANK1 + 8, 1),
	[2] = DEFINE_RES_NAMED(IRQ_EINT(7), 1, NULL, IORESOURCE_IRQ \
				| IORESOURCE_IRQ_HIGHLEVEL),
};

static struct dm9000_plat_data mini210_dm9000_platdata = {
	.flags		= DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM,
	.dev_addr	= { 0x08, 0x90, 0x00, 0xa0, 0x02, 0x10 },
};

static struct platform_device mini210_dm9000 = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mini210_dm9000_resources),
	.resource	= mini210_dm9000_resources,
	.dev		= {
		.platform_data	= &mini210_dm9000_platdata,
	},
};

static struct platform_device *mini210_devices[] __initdata = {
	&s3c_device_hsmmc0,
	&s3c_device_hsmmc1,
	&s3c_device_hsmmc2,
	&s3c_device_hsmmc3,
	&s3c_device_i2c0,
	&s3c_device_i2c1,
	&s3c_device_i2c2,
	&s3c_device_rtc,
	&s3c_device_wdt,
	&mini210_dm9000,
};

static void __init mini210_dm9000_init(void)
{
	unsigned int tmp;

	gpio_request(S5PV210_MP01(1), "nCS1");
	s3c_gpio_cfgpin(S5PV210_MP01(1), S3C_GPIO_SFN(2));
	gpio_free(S5PV210_MP01(1));

	tmp = (5 << S5P_SROM_BCX__TACC__SHIFT);
	__raw_writel(tmp, S5P_SROM_BC1);

	tmp = __raw_readl(S5P_SROM_BW);
	tmp &= (S5P_SROM_BW__CS_MASK << S5P_SROM_BW__NCS1__SHIFT);
	tmp |= (1 << S5P_SROM_BW__NCS1__SHIFT);
	__raw_writel(tmp, S5P_SROM_BW);
}

static struct i2c_board_info mini210_i2c_devs0[] __initdata = {
	{ I2C_BOARD_INFO("24c08", 0x50), },     /* Atmel AT24C04 */
};

static struct i2c_board_info mini210_i2c_devs1[] __initdata = {
	/* To Be Updated */
};

static struct i2c_board_info mini210_i2c_devs2[] __initdata = {
	/* To Be Updated */
};

static void __init mini210_map_io(void)
{
	s5pv210_init_io(NULL, 0);
	s3c24xx_init_clocks(24000000);
	s3c24xx_init_uarts(mini210_uartcfgs, ARRAY_SIZE(mini210_uartcfgs));
	samsung_set_timer_source(SAMSUNG_PWM2, SAMSUNG_PWM4);
}

static void __init mini210_machine_init(void)
{
	s3c_pm_init();

	s3c_i2c0_set_platdata(NULL);
	s3c_i2c1_set_platdata(NULL);
	s3c_i2c2_set_platdata(NULL);
	i2c_register_board_info(0, mini210_i2c_devs0,
			ARRAY_SIZE(mini210_i2c_devs0));
	i2c_register_board_info(1, mini210_i2c_devs1,
			ARRAY_SIZE(mini210_i2c_devs1));
	i2c_register_board_info(2, mini210_i2c_devs2,
			ARRAY_SIZE(mini210_i2c_devs2));

	mini210_dm9000_init();

	platform_add_devices(mini210_devices, ARRAY_SIZE(mini210_devices));
}

MACHINE_START(MINI210, "MINI210")
	/* Maintainer: Jeff Kent <jeff@jkent.net> */
	.atag_offset	= 0x100,
	.init_irq	= s5pv210_init_irq,
	.map_io		= mini210_map_io,
	.init_machine	= mini210_machine_init,
	.init_time	= samsung_timer_init,
	.restart	= s5pv210_restart,
MACHINE_END
