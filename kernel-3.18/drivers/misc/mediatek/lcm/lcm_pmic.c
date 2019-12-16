/*
 * Copyright (C) 2015 MediaTek Inc.
 * Copyright (C) 2017 FIH Mobile Limited.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * NOTE: This file has been modified by FIH Mobile Limited.
 * Modifications are licensed under the License.
 */

#include <linux/regulator/consumer.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>

#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#endif

#define BBOX_LCM_POWER_STATUS_ABNORMAL          do {printk("BBox;%s: LCM POWER status abnormal\n", __func__); printk("BBox::UEC;0::6\n");} while (0);

#define LDO_GEN1(_supply,min_voltage,max_voltage)	\
	{	\
		.supply_name= #_supply,	\
		.min_uV= (min_voltage),	\
		.max_uV= (max_voltage),	\
	}

struct lcm_device {
	struct regulator *reg;
	char *supply_name;
	int min_uV;
	int max_uV;
};

static struct lcm_device lcm_ldos[] = {
#if 0
	{NULL,"vdd28",2800000,2800000},
	{NULL,"vddi18",2800000,2800000},
#else
	LDO_GEN1(vdd28,2800000,2800000),
	LDO_GEN1(vddi18,1800000,1800000),
#endif
};

/* get(vgp6) LDO supply */
static int lcm_get_vgp_supply(struct device *dev)
{
	int i;
	int ret = 0;

	pr_debug("LCM: lcm_get_vgp_supply is going\n");

	for (i = 0; i < ARRAY_SIZE(lcm_ldos); i++)
	{
		pr_debug("LCM: lcm_get_vgp_supply is going lcm_ldos[%d]->supply_name=%s\n",i,lcm_ldos[i].supply_name);
		//lcm_ldos[i].reg = devm_regulator_get(dev, lcm_ldos[i].supply_name);
#if 1
		lcm_ldos[i].reg = regulator_get(dev, lcm_ldos[i].supply_name);
#endif
		if (NULL == lcm_ldos[i].reg || IS_ERR(lcm_ldos[i].reg)) {
			ret = PTR_ERR(lcm_ldos[i].reg);
			pr_debug("failed to get LCM LDO=%s, err=%d\n",lcm_ldos[i].supply_name, ret);
			continue;
		}

		pr_debug("LCM: lcm get supply ok.\n");

		/* get current voltage settings */
		ret = regulator_get_voltage(lcm_ldos[i].reg);
		pr_debug("lcm LDO voltage = %d in LK stage\n", ret);
	}

	return ret;
}

int lcm_vgp_supply_enable(void)
{
	int i;
	int ret = 0;
	unsigned int volt;

	pr_debug("LCM: lcm_vgp_supply_enable\n");

	for (i = 0; i < ARRAY_SIZE(lcm_ldos); i++)
	{
		if (NULL == lcm_ldos[i].reg || IS_ERR(lcm_ldos[i].reg) )
		{
			pr_debug("LCM: lcm_ldos[%d]->supply_name=%s is going wrong \n",i,lcm_ldos[i].supply_name);
			continue;
		}
		pr_debug("LCM: lcm_ldos[%d]->supply_name=%s is going right \n",i,lcm_ldos[i].supply_name);
		pr_debug("LCM: set regulator voltage lcm_vgp voltage to 2.8V\n");
		/* set(vgp1) voltage to 2.8V */
		ret = regulator_set_voltage(lcm_ldos[i].reg, lcm_ldos[i].min_uV, lcm_ldos[i].max_uV);
		if (ret != 0) {
			pr_err("LCM: lcm failed to set lcm_vgp voltage: %d\n", ret);
			continue;
		}

		/* get(vgp1) voltage settings again */
		volt = regulator_get_voltage(lcm_ldos[i].reg);
		if (volt == lcm_ldos[i].min_uV)
			pr_debug("LCM: check regulator voltage=%d pass!\n",lcm_ldos[i].min_uV);
		else
			pr_err("LCM: check regulator voltage=%d fail! (voltage: %d)\n",lcm_ldos[i].min_uV, volt);

		ret = regulator_enable(lcm_ldos[i].reg);
		if (ret != 0) {
			pr_err("LCM: Failed to enable %s: %d\n",lcm_ldos[i].supply_name, ret);
			BBOX_LCM_POWER_STATUS_ABNORMAL
			continue;
		}
	}

	return ret;
}

int lcm_vgp_supply_disable(void)
{
	int i;
	int ret = 0;
	unsigned int isenable;

	pr_debug("LCM: lcm_vgp_supply_disable\n");

	for (i = 0; i < ARRAY_SIZE(lcm_ldos); i++)
	{
		if (NULL == lcm_ldos[i].reg || IS_ERR(lcm_ldos[i].reg) )
		{
			pr_debug("LCM: lcm_ldos[%d]->supply_name=%s is going wrong \n",i,lcm_ldos[i].supply_name);
			continue;
		}
		pr_debug("LCM: lcm_ldos[%d]->supply_name=%s is going right \n",i,lcm_ldos[i].supply_name);

		/* disable regulator */
		isenable = regulator_is_enabled(lcm_ldos[i].reg);

		pr_debug("LCM: lcm query regulator enable status[0x%d]\n", isenable);

		if (isenable) {
			ret = regulator_disable(lcm_ldos[i].reg);
			if (ret != 0) {
				pr_err("LCM: lcm failed to disable %s: %d\n", lcm_ldos[i].supply_name, ret);
				BBOX_LCM_POWER_STATUS_ABNORMAL
				continue;
			}
			/* verify */
			isenable = regulator_is_enabled(lcm_ldos[i].reg);
			if (!isenable)
				pr_debug("LCM: lcm regulator disable pass\n");
		}
	}

	return ret;
}

/*
void lcm_request_gpio_control(void)
{
	gpio_request(GPIO_LCD_PWR_EN, "GPIO_LCD_PWR_EN");
	pr_debug("[KE/LCM] GPIO_LCD_PWR_EN = 0x%x\n", GPIO_LCD_PWR_EN);

	gpio_request(GPIO_LCD_PWR2_EN, "GPIO_LCD_PWR2_EN");
	pr_debug("[KE/LCM] GPIO_LCD_PWR2_EN = 0x%x\n", GPIO_LCD_PWR2_EN);

	gpio_request(GPIO_LCD_RST_EN, "GPIO_LCD_RST_EN");
	pr_debug("[KE/LCM] GPIO_LCD_RST_EN = 0x%x\n", GPIO_LCD_RST_EN);

	gpio_request(GPIO_LCD_STB_EN, "GPIO_LCD_STB_EN");
	pr_debug("[KE/LCM] GPIO_LCD_STB_EN =  0x%x\n", GPIO_LCD_STB_EN);

	gpio_request(GPIO_LCD_BRIDGE_EN, "GPIO_LCD_BRIDGE_EN");
	pr_debug("[KE/LCM] GPIO_LCD_BRIDGE_EN = 0x%x\n", GPIO_LCD_BRIDGE_EN);
}*/

static int lcm_probe(struct device *dev)
{
	lcm_get_vgp_supply(dev);
	lcm_vgp_supply_enable();
	return 0;
}

static const struct of_device_id lcm_of_ids[] = {
	{.compatible = "mediatek,lcm",},
	{}
};

static struct platform_driver lcm_driver = {
//	.probe = lcm_probe,
	.driver = {
		   .name = "mtk_lcm",
		   .owner = THIS_MODULE,
		   .probe = lcm_probe,
#ifdef CONFIG_OF
		   .of_match_table = lcm_of_ids,
#endif
		   },
};

static int __init lcm_init(void)
{
	pr_debug("LCM: Register lcm driver\n");
	if (platform_driver_register(&lcm_driver)) {
		pr_err("LCM: failed to register disp driver\n");
		return -ENODEV;
	}

	return 0;
}


static void __exit lcm_exit(void)
{
	platform_driver_unregister(&lcm_driver);
	pr_debug("LCM: Unregister lcm driver done\n");
}

late_initcall(lcm_init);
module_exit(lcm_exit);
MODULE_AUTHOR("mediatek");
MODULE_DESCRIPTION("Display subsystem Driver");
MODULE_LICENSE("GPL");

#ifdef CONFIG_RT5081A_PMU_DSV
static int regulator_inited;

#ifdef CONFIG_RT5081A_PMU_DSV_EXTPIN
static int ext_gpio_pin;
#else
static struct regulator *disp_bias_pos;
static struct regulator *disp_bias_neg;
#endif /* CONFIG_RT5081A_PMU_DSV_EXTPIN */

int display_bias_regulator_init(void)
{
#ifdef CONFIG_RT5081A_PMU_DSV_EXTPIN
	struct device_node *np = NULL;
	struct device_node *dsv_np = NULL;
	int ret = 0;

	if (regulator_inited)
		return ret;

	np = of_find_node_by_name(NULL, "rt5081a_pmu_dts");
	if (np)
		dsv_np = of_get_child_by_name(np, "dsv");
	if (dsv_np) {
#if (!defined(CONFIG_MTK_GPIO) || defined(CONFIG_MTK_GPIOLIB_STAND))
		ret = of_get_named_gpio(dsv_np, "extpin_gpio", 0);
		if (ret < 0)
			goto lcm_regu_init_err;
		ext_gpio_pin = ret;
#else
		ret = of_property_read_u32(dsv_np, "extpin_gpio_num", &ext_gpio_pin);
		if (ret < 0)
			goto lcm_regu_init_err;
#endif /* CONFIG_MTK_GPIO, CONFIG_MTK_GPIOLIB_STAND */
		ret = gpio_request_one(ext_gpio_pin, GPIOF_OUT_INIT_HIGH, "mtk_dsv_extpin");
		if (ret < 0) {
			pr_err("%s gpio request fail\n", __func__);
			goto lcm_regu_init_err;
		}
	} else {
		pr_err("%s no dsv of node\n", __func__);
		goto lcm_regu_init_err;
	}

	regulator_inited = 1;
	pr_info("%s: success, ext_gpio_pin = %d\n", __func__, ext_gpio_pin);

	return 0;

lcm_regu_init_err:
	regulator_inited = 1;
	return -EINVAL;
#else
	int ret = 0;

	if (regulator_inited)
		return ret;

	/* please only get regulator once in a driver */
	disp_bias_pos = regulator_get(NULL, "dsv_pos");
	if (IS_ERR(disp_bias_pos)) { /* handle return value */
		ret = PTR_ERR(disp_bias_pos);
		pr_err("get dsv_pos fail, error: %d\n", ret);
		return ret;
	}

	disp_bias_neg = regulator_get(NULL, "dsv_neg");
	if (IS_ERR(disp_bias_neg)) { /* handle return value */
		ret = PTR_ERR(disp_bias_pos);
		pr_err("get dsv_neg fail, error: %d\n", ret);
		return ret;
	}

	pr_info("%s: success\n", __func__);
	regulator_inited = 1;

	return ret; /* must be 0 */
#endif /* CONFIG_RT5081A_PMU_DSV_EXTPIN */
}
EXPORT_SYMBOL(display_bias_regulator_init);

int display_bias_enable(void)
{
	int ret = 0;
	int retval = 0;

	if (!regulator_inited)
		return ret;

#ifdef CONFIG_RT5081A_PMU_DSV_EXTPIN
	gpio_set_value(ext_gpio_pin, 1);
	ret = gpio_get_value(ext_gpio_pin);
	retval = ret ? 0 : -EINVAL;
	if (retval < 0)
		pr_err("%s fail\n", __func__);
	return retval;
#else
	/* set voltage with min & max*/
	ret = regulator_set_voltage(disp_bias_pos, 5400000, 5400000);
	if (ret < 0)
		pr_err("set voltage disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_set_voltage(disp_bias_neg, 5400000, 5400000);
	if (ret < 0)
		pr_err("set voltage disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	/* enable regulator */
	ret = regulator_enable(disp_bias_pos);
	if (ret < 0) {
		pr_err("enable regulator disp_bias_pos fail, ret = %d\n", ret);
		BBOX_LCM_POWER_STATUS_ABNORMAL
	}
	retval |= ret;

	ret = regulator_enable(disp_bias_neg);
	if (ret < 0) {
		pr_err("enable regulator disp_bias_neg fail, ret = %d\n", ret);
		BBOX_LCM_POWER_STATUS_ABNORMAL
	}
	retval |= ret;

	return retval;
#endif /* CONFIG_RT5081A_PMU_DSV_EXTPIN */
}
EXPORT_SYMBOL(display_bias_enable);

int display_bias_disable(void)
{
	int ret = 0;
	int retval = 0;

	if (!regulator_inited)
		return ret;

#ifdef CONFIG_RT5081A_PMU_DSV_EXTPIN
	gpio_set_value(ext_gpio_pin, 0);
	ret = gpio_get_value(ext_gpio_pin);
	retval = ret ? -EINVAL : 0;
	if (retval < 0)
		pr_err("%s fail\n", __func__);
	return retval;
#else
	ret = regulator_disable(disp_bias_neg);
	if (ret < 0) {
		pr_err("disable regulator disp_bias_neg fail, ret = %d\n", ret);
		BBOX_LCM_POWER_STATUS_ABNORMAL
	}
	retval |= ret;

	ret = regulator_disable(disp_bias_pos);
	if (ret < 0) {
		pr_err("disable regulator disp_bias_pos fail, ret = %d\n", ret);
		BBOX_LCM_POWER_STATUS_ABNORMAL
	}
	retval |= ret;

	return retval;
#endif /* CONFIG_RT5081A_PMU_DSV_EXTPIN */
}
EXPORT_SYMBOL(display_bias_disable);

#else
int display_bias_regulator_init(void)
{
	return 0;
}
EXPORT_SYMBOL(display_bias_regulator_init);

int display_bias_enable(void)
{
	return 0;
}
EXPORT_SYMBOL(display_bias_enable);

int display_bias_disable(void)
{
	return 0;
}
EXPORT_SYMBOL(display_bias_disable);
#endif

