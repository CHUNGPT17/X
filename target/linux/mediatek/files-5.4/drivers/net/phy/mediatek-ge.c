// SPDX-License-Identifier: GPL-2.0+
#include <linux/bitfield.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/pinctrl/consumer.h>
#include <linux/phy.h>

#define MTK_GPHY_ID_MT7530 0x03a29412
#define MTK_GPHY_ID_MT7531 0x03a29441
#ifdef CONFIG_MEDIATEK_GE_PHY_SOC
#define MTK_GPHY_ID_MT7981 0x03a29461
#define MTK_GPHY_ID_MT7988 0x03a29481
#endif

#define MTK_EXT_PAGE_ACCESS		0x1f
#define MTK_PHY_PAGE_STANDARD		0x0000
#define MTK_PHY_PAGE_EXTENDED		0x0001
#define MTK_PHY_PAGE_EXTENDED_2		0x0002
#define MTK_PHY_PAGE_EXTENDED_3		0x0003
/* Registers on Page 3 */
#define MTK_PHY_LPI_REG_14				(0x14)
#define   MTK_PHY_LPI_WAKE_TIMER_1000_MASK	GENMASK(8, 0)

#define MTK_PHY_LPI_REG_1c				(0x1c)
#define   MTK_PHY_SMI_DET_ON_THRESH_MASK		GENMASK(13, 8)
/*******************************/

#define MTK_PHY_PAGE_EXTENDED_2A30	0x2a30
#define MTK_PHY_PAGE_EXTENDED_52B5	0x52b5

#define ANALOG_INTERNAL_OPERATION_MAX_US	(20)
#define ZCAL_CTRL_MIN				(0)
#define ZCAL_CTRL_MAX				(63)
#define TXRESERVE_MIN				(0)
#define TXRESERVE_MAX				(7)

#define MTK_PHY_ANARG_RG			(0x10)
#define   MTK_PHY_TCLKOFFSET_MASK		GENMASK(12, 8)

/* Registers on MDIO_MMD_VEND1 */
enum {
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_0TO1 = 0,
	MTK_PHY_1st_OVERSHOOT_LEVEL_0TO1,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_0TO1,
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_1TO0,
	MTK_PHY_1st_OVERSHOOT_LEVEL_1TO0,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_1TO0,
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_0TON1, /* N means negative */
	MTK_PHY_1st_OVERSHOOT_LEVEL_0TON1,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_0TON1,
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_N1TO0,
	MTK_PHY_1st_OVERSHOOT_LEVEL_N1TO0,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_N1TO0,
	MTK_PHY_TX_MLT3_END,
};

#define MTK_PHY_TXVLD_DA_RG			(0x12)
#define   MTK_PHY_DA_TX_I2MPB_A_GBE_MASK	GENMASK(15, 10)
#define   MTK_PHY_DA_TX_I2MPB_A_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_A2		(0x16)
#define   MTK_PHY_DA_TX_I2MPB_A_HBT_MASK	GENMASK(15, 10)
#define   MTK_PHY_DA_TX_I2MPB_A_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_B1		(0x17)
#define   MTK_PHY_DA_TX_I2MPB_B_GBE_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_B_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_B2		(0x18)
#define   MTK_PHY_DA_TX_I2MPB_B_HBT_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_B_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_C1		(0x19)
#define   MTK_PHY_DA_TX_I2MPB_C_GBE_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_C_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_C2		(0x20)
#define   MTK_PHY_DA_TX_I2MPB_C_HBT_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_C_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_D1		(0x21)
#define   MTK_PHY_DA_TX_I2MPB_D_GBE_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_D_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_D2		(0x22)
#define   MTK_PHY_DA_TX_I2MPB_D_HBT_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_D_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TANA_CAL_MODE			(0xc1)
#define MTK_PHY_TANA_CAL_MODE_SHIFT		(8)

#define MTK_PHY_RXADC_CTRL_RG7			(0xc6)
#define   MTK_PHY_DA_AD_BUF_BIAS_LP_MASK	GENMASK(9, 8)

#define MTK_PHY_RXADC_CTRL_RG9			(0xc8)
#define   MTK_PHY_DA_RX_PSBN_TBT_MASK		GENMASK(14, 12)
#define   MTK_PHY_DA_RX_PSBN_HBT_MASK		GENMASK(10, 8)
#define   MTK_PHY_DA_RX_PSBN_GBE_MASK		GENMASK(6, 4)
#define   MTK_PHY_DA_RX_PSBN_LP_MASK		GENMASK(2, 0)

#define MTK_PHY_LDO_OUTPUT_V			(0xd7)

#define MTK_PHY_RG_ANA_CAL_RG0			(0xdb)
#define   MTK_PHY_RG_CAL_CKINV			BIT(12)
#define   MTK_PHY_RG_ANA_CALEN			BIT(8)
#define   MTK_PHY_RG_REXT_CALEN			BIT(4)
#define   MTK_PHY_RG_ZCALEN_A			BIT(0)

#define MTK_PHY_RG_ANA_CAL_RG1			(0xdc)
#define   MTK_PHY_RG_ZCALEN_B			BIT(12)
#define   MTK_PHY_RG_ZCALEN_C			BIT(8)
#define   MTK_PHY_RG_ZCALEN_D			BIT(4)
#define   MTK_PHY_RG_TXVOS_CALEN		BIT(0)

#define MTK_PHY_RG_ANA_CAL_RG2			(0xdd)
#define   MTK_PHY_RG_TXG_CALEN_A		BIT(12)
#define   MTK_PHY_RG_TXG_CALEN_B		BIT(8)
#define   MTK_PHY_RG_TXG_CALEN_C		BIT(4)
#define   MTK_PHY_RG_TXG_CALEN_D		BIT(0)

#define MTK_PHY_RG_ANA_CAL_RG5			(0xe0)
#define   MTK_PHY_RG_REXT_TRIM_MASK		GENMASK(13, 8)
#define   MTK_PHY_RG_ZCAL_CTRL_MASK		GENMASK(5, 0)

#define MTK_PHY_RG_TX_FILTER			(0xfe)

#define MTK_PHY_RG_LPI_PCS_DSP_CTRL_REG120	(0x120)
#define   MTK_PHY_LPI_SIG_EN_LO_THRESH1000_MASK	GENMASK(12, 8)
#define   MTK_PHY_LPI_SIG_EN_HI_THRESH1000_MASK	GENMASK(4, 0)

#define MTK_PHY_RG_LPI_PCS_DSP_CTRL_REG122	(0x122)
#define   MTK_PHY_LPI_NORM_MSE_HI_THRESH1000_MASK	GENMASK(7, 0)

#define MTK_PHY_RG_TESTMUX_ADC_CTRL	(0x144)
#define   MTK_PHY_RG_TXEN_DIG_MASK		GENMASK(5, 5)

#define MTK_PHY_RG_CR_TX_AMP_OFFSET_A_B		(0x172)
#define   MTK_PHY_CR_TX_AMP_OFFSET_A_MASK	GENMASK(13, 8)
#define   MTK_PHY_CR_TX_AMP_OFFSET_B_MASK	GENMASK(6, 0)

#define MTK_PHY_RG_CR_TX_AMP_OFFSET_C_D		(0x173)
#define   MTK_PHY_CR_TX_AMP_OFFSET_C_MASK	GENMASK(13, 8)
#define   MTK_PHY_CR_TX_AMP_OFFSET_D_MASK	GENMASK(6, 0)

#define MTK_PHY_RG_AD_CAL_COMP			(0x17a)
#define   MTK_PHY_AD_CAL_COMP_OUT_SHIFT		(8)

#define MTK_PHY_RG_AD_CAL_CLK			(0x17b)
#define   MTK_PHY_DA_CAL_CLK			BIT(0)

#define MTK_PHY_RG_AD_CALIN			(0x17c)
#define   MTK_PHY_DA_CALIN_FLAG			BIT(0)

#define MTK_PHY_RG_DASN_DAC_IN0_A		(0x17d)
#define   MTK_PHY_DASN_DAC_IN0_A_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN0_B		(0x17e)
#define   MTK_PHY_DASN_DAC_IN0_B_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN0_C		(0x17f)
#define   MTK_PHY_DASN_DAC_IN0_C_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN0_D			(0x180)
#define   MTK_PHY_DASN_DAC_IN0_D_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN1_A			(0x181)
#define   MTK_PHY_DASN_DAC_IN1_A_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN1_B			(0x182)
#define   MTK_PHY_DASN_DAC_IN1_B_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN1_C			(0x183)
#define   MTK_PHY_DASN_DAC_IN1_C_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DASN_DAC_IN1_D			(0x184)
#define   MTK_PHY_DASN_DAC_IN1_D_MASK		GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG19b		(0x19b)
#define   MTK_PHY_BYPASS_DSP_LPI_READY	BIT(8)

#define MTK_PHY_RG_LP_IIR2_K1_L			(0x22a)
#define MTK_PHY_RG_LP_IIR2_K1_U			(0x22b)
#define MTK_PHY_RG_LP_IIR2_K2_L			(0x22c)
#define MTK_PHY_RG_LP_IIR2_K2_U			(0x22d)
#define MTK_PHY_RG_LP_IIR2_K3_L			(0x22e)
#define MTK_PHY_RG_LP_IIR2_K3_U			(0x22f)
#define MTK_PHY_RG_LP_IIR2_K4_L			(0x230)
#define MTK_PHY_RG_LP_IIR2_K4_U			(0x231)
#define MTK_PHY_RG_LP_IIR2_K5_L			(0x232)
#define MTK_PHY_RG_LP_IIR2_K5_U			(0x233)

#define MTK_PHY_RG_DEV1E_REG234			(0x234)
#define   MTK_PHY_TR_OPEN_LOOP_EN_MASK		GENMASK(0, 0)
#define   MTK_PHY_LPF_X_AVERAGE_MASK		GENMASK(7, 4)
#define   MTK_PHY_TR_LP_IIR_EEE_EN			BIT(12)

#define MTK_PHY_RG_LPF_CNT_VAL			(0x235)

#define MTK_PHY_RG_DEV1E_REG238		(0x238)
#define   MTK_PHY_LPI_SLV_SEND_TX_TIMER_MASK	GENMASK(8, 0)
#define   MTK_PHY_LPI_SLV_SEND_TX_EN			BIT(12)

#define MTK_PHY_RG_DEV1E_REG239		(0x239)
#define   MTK_PHY_LPI_SEND_LOC_TIMER_MASK	GENMASK(8, 0)
#define   MTK_PHY_LPI_TXPCS_LOC_RCV			BIT(12)

#define MTK_PHY_RG_DEV1E_REG27C			(0x27c)
#define   MTK_PHY_VGASTATE_FFE_THR_ST1_MASK	GENMASK(12, 8)
#define MTK_PHY_RG_DEV1E_REG27D			(0x27d)
#define   MTK_PHY_VGASTATE_FFE_THR_ST2_MASK	GENMASK(4, 0)

#define MTK_PHY_RG_DEV1E_REG2C7		(0x2c7)
#define   MTK_PHY_MAX_GAIN_MASK		GENMASK(4, 0)
#define   MTK_PHY_MIN_GAIN_MASK		GENMASK(12, 8)

#define MTK_PHY_RG_DEV1E_REG2D1		(0x2d1)
#define   MTK_PHY_VCO_SLICER_THRESH_BITS_HIGH_EEE_MASK	GENMASK(7, 0)
#define   MTK_PHY_LPI_SKIP_SD_SLV_TR					BIT(8)
#define   MTK_PHY_LPI_TR_READY							BIT(9)
#define   MTK_PHY_LPI_VCO_EEE_STG0_EN					BIT(10)

#define MTK_PHY_RG_DEV1E_REG323		(0x323)
#define   MTK_PHY_EEE_WAKE_MAS_INT_DC	BIT(0)
#define   MTK_PHY_EEE_WAKE_SLV_INT_DC	BIT(4)

#define MTK_PHY_RG_DEV1E_REG324		(0x324)
#define   MTK_PHY_SMI_DETCNT_MAX_MASK	GENMASK(5, 0)
#define   MTK_PHY_SMI_DET_MAX_EN		BIT(8)

#define MTK_PHY_RG_DEV1E_REG326		(0x326)
#define   MTK_PHY_LPI_MODE_SD_ON				BIT(0)
#define   MTK_PHY_RESET_RANDUPD_CNT				BIT(1)
#define   MTK_PHY_TREC_UPDATE_ENAB_CLR			BIT(2)
#define   MTK_PHY_LPI_QUIT_WAIT_DFE_SIG_DET_OFF	BIT(4)
#define   MTK_PHY_TR_READY_SKIP_AFE_WAKEUP		BIT(5)

#define MTK_PHY_LDO_PUMP_EN_PAIRAB		(0x502)
#define MTK_PHY_LDO_PUMP_EN_PAIRCD		(0x503)

#define MTK_PHY_DA_TX_R50_PAIR_A		(0x53d)
#define MTK_PHY_DA_TX_R50_PAIR_B		(0x53e)
#define MTK_PHY_DA_TX_R50_PAIR_C		(0x53f)
#define MTK_PHY_DA_TX_R50_PAIR_D		(0x540)

/* Registers on MDIO_MMD_VEND2 */
#define MTK_PHY_LED0_ON_CTRL			(0x24)
#define   MTK_PHY_LED0_ON_MASK			GENMASK(6, 0)
#define   MTK_PHY_LED0_ON_LINK1000		BIT(0)
#define   MTK_PHY_LED0_ON_LINK100		BIT(1)
#define   MTK_PHY_LED0_ON_LINK10		BIT(2)
#define   MTK_PHY_LED0_ON_LINKDOWN		BIT(3)
#define   MTK_PHY_LED0_ON_FDX			BIT(4) /* Full duplex */
#define   MTK_PHY_LED0_ON_HDX			BIT(5) /* Half duplex */
#define   MTK_PHY_LED0_FORCE_ON			BIT(6)
#define   MTK_PHY_LED0_POLARITY			BIT(14)
#define   MTK_PHY_LED0_ENABLE			BIT(15)

#define MTK_PHY_LED0_BLINK_CTRL			(0x25)
#define   MTK_PHY_LED0_1000TX			BIT(0)
#define   MTK_PHY_LED0_1000RX			BIT(1)
#define   MTK_PHY_LED0_100TX			BIT(2)
#define   MTK_PHY_LED0_100RX			BIT(3)
#define   MTK_PHY_LED0_10TX			BIT(4)
#define   MTK_PHY_LED0_10RX			BIT(5)
#define   MTK_PHY_LED0_COLLISION		BIT(6)
#define   MTK_PHY_LED0_RX_CRC_ERR		BIT(7)
#define   MTK_PHY_LED0_RX_IDLE_ERR		BIT(8)
#define   MTK_PHY_LED0_FORCE_BLINK		BIT(9)

#define MTK_PHY_ANA_TEST_BUS_CTRL_RG		(0x100)
#define   MTK_PHY_ANA_TEST_MODE_MASK		GENMASK(15, 8)

#define MTK_PHY_RG_DASN_TXT_DMY2		(0x110)
#define   MTK_PHY_TST_DMY2_MASK			GENMASK(5, 0)

#define MTK_PHY_RG_BG_RASEL			(0x115)
#define   MTK_PHY_RG_BG_RASEL_MASK		GENMASK(2, 0)

/* These macro privides efuse parsing for internal phy. */
#define EFS_DA_TX_I2MPB_A(x)			(((x) >> 0) & GENMASK(5, 0))
#define EFS_DA_TX_I2MPB_B(x)			(((x) >> 6) & GENMASK(5, 0))
#define EFS_DA_TX_I2MPB_C(x)			(((x) >> 12) & GENMASK(5, 0))
#define EFS_DA_TX_I2MPB_D(x)			(((x) >> 18) & GENMASK(5, 0))
#define EFS_DA_TX_AMP_OFFSET_A(x)		(((x) >> 24) & GENMASK(5, 0))

#define EFS_DA_TX_AMP_OFFSET_B(x)		(((x) >> 0) & GENMASK(5, 0))
#define EFS_DA_TX_AMP_OFFSET_C(x)		(((x) >> 6) & GENMASK(5, 0))
#define EFS_DA_TX_AMP_OFFSET_D(x)		(((x) >> 12) & GENMASK(5, 0))
#define EFS_DA_TX_R50_A(x)			(((x) >> 18) & GENMASK(5, 0))
#define EFS_DA_TX_R50_B(x)			(((x) >> 24) & GENMASK(5, 0))

#define EFS_DA_TX_R50_C(x)			(((x) >> 0) & GENMASK(5, 0))
#define EFS_DA_TX_R50_D(x)			(((x) >> 6) & GENMASK(5, 0))
#define EFS_DA_TX_R50_A_10M(x)			(((x) >> 12) & GENMASK(5, 0))
#define EFS_DA_TX_R50_B_10M(x)			(((x) >> 18) & GENMASK(5, 0))

#define EFS_RG_BG_RASEL(x)			(((x) >> 4) & GENMASK(2, 0))
#define EFS_RG_REXT_TRIM(x)			(((x) >> 7) & GENMASK(5, 0))

enum {
	NO_PAIR,
	PAIR_A,
	PAIR_B,
	PAIR_C,
	PAIR_D,
};

enum {
	GPHY_PORT0,
	GPHY_PORT1,
	GPHY_PORT2,
	GPHY_PORT3,
};

enum calibration_mode {
	EFUSE_K,
	SW_K
};

enum CAL_ITEM {
	REXT,
	TX_OFFSET,
	TX_AMP,
	TX_R50,
	TX_VCM
};

enum CAL_MODE {
	EFUSE_M,
	SW_M
};

const u8 mt798x_zcal_to_r50[64] = {
	7, 8, 9, 9, 10, 10, 11, 11,
	12, 13, 13, 14, 14, 15, 16, 16,
	17, 18, 18, 19, 20, 21, 21, 22,
	23, 24, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36, 37,
	38, 40, 41, 42, 43, 45, 46, 48,
	49, 51, 52, 54, 55, 57, 59, 61,
	62, 63, 63, 63, 63, 63, 63, 63
};

const char pair[4] = {'A', 'B', 'C', 'D'};

static int mtk_gephy_read_page(struct phy_device *phydev)
{
	return __phy_read(phydev, MTK_EXT_PAGE_ACCESS);
}

static int mtk_gephy_write_page(struct phy_device *phydev, int page)
{
	return __phy_write(phydev, MTK_EXT_PAGE_ACCESS, page);
}

static void mtk_gephy_config_init(struct phy_device *phydev)
{
	/* Disable EEE */
	phy_write_mmd(phydev, MDIO_MMD_AN, MDIO_AN_EEE_ADV, 0);

	/* Enable HW auto downshift */
	phy_modify_paged(phydev, MTK_PHY_PAGE_EXTENDED, 0x14, 0, BIT(4));

	/* Increase SlvDPSready time */
	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	__phy_write(phydev, 0x10, 0xafae);
	__phy_write(phydev, 0x12, 0x2f);
	__phy_write(phydev, 0x10, 0x8fae);
	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	/* Adjust 100_mse_threshold */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x123, 0xffff);

	/* Disable mcc */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0xa6, 0x300);
}

static int mt7530_phy_config_init(struct phy_device *phydev)
{
	mtk_gephy_config_init(phydev);

	/* Increase post_update_timer */
	phy_write_paged(phydev, MTK_PHY_PAGE_EXTENDED_3, 0x11, 0x4b);

	return 0;
}

static int mt7531_phy_config_init(struct phy_device *phydev)
{
	mtk_gephy_config_init(phydev);

	/* PHY link down power saving enable */
	phy_set_bits(phydev, 0x17, BIT(4));
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, 0xc6, 0x300);

	/* Set TX Pair delay selection */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x13, 0x404);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x14, 0x404);

	return 0;
}

#ifdef CONFIG_MEDIATEK_GE_PHY_SOC
/* One calibration cycle consists of:
 * 1.Set DA_CALIN_FLAG high to start calibration. Keep it high
 *   until AD_CAL_COMP is ready to output calibration result.
 * 2.Wait until DA_CAL_CLK is available.
 * 3.Fetch AD_CAL_COMP_OUT.
 */
static int cal_cycle(struct phy_device *phydev, int devad,
		     u32 regnum, u16 mask, u16 cal_val)
{
	unsigned long timeout;
	int reg_val;
	int ret;

	phy_modify_mmd(phydev, devad, regnum,
		       mask, cal_val);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_AD_CALIN,
			 MTK_PHY_DA_CALIN_FLAG);

	timeout = jiffies + usecs_to_jiffies(ANALOG_INTERNAL_OPERATION_MAX_US);
	do {
		reg_val = phy_read_mmd(phydev, MDIO_MMD_VEND1,
				       MTK_PHY_RG_AD_CAL_CLK);
	} while (time_before(jiffies, timeout) && !(reg_val & BIT(0)));

	if (!(reg_val & BIT(0))) {
		dev_err(&phydev->mdio.dev, "Calibration cycle timeout\n");
		return -ETIMEDOUT;
	}

	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_AD_CALIN,
			   MTK_PHY_DA_CALIN_FLAG);
	ret = phy_read_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_AD_CAL_COMP) >>
			   MTK_PHY_AD_CAL_COMP_OUT_SHIFT;
	dev_dbg(&phydev->mdio.dev, "cal_val: 0x%x, ret: %d\n", cal_val, ret);

	return ret;
}

static int rext_fill_result(struct phy_device *phydev, u16 *buf)
{
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
		       MTK_PHY_RG_REXT_TRIM_MASK, buf[0] << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_RG_BG_RASEL,
		       MTK_PHY_RG_BG_RASEL_MASK, buf[1]);

	return 0;
}

static int rext_cal_efuse(struct phy_device *phydev, u32 *buf)
{
	u16 rext_cal_val[2];

	rext_cal_val[0] = EFS_RG_REXT_TRIM(buf[3]);
	rext_cal_val[1] = EFS_RG_BG_RASEL(buf[3]);
	rext_fill_result(phydev, rext_cal_val);

	return 0;
}

static int tx_offset_fill_result(struct phy_device *phydev, u16 *buf)
{
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_CR_TX_AMP_OFFSET_A_B,
		       MTK_PHY_CR_TX_AMP_OFFSET_A_MASK, buf[0] << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_CR_TX_AMP_OFFSET_A_B,
		       MTK_PHY_CR_TX_AMP_OFFSET_B_MASK, buf[1]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_CR_TX_AMP_OFFSET_C_D,
		       MTK_PHY_CR_TX_AMP_OFFSET_C_MASK, buf[2] << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_CR_TX_AMP_OFFSET_C_D,
		       MTK_PHY_CR_TX_AMP_OFFSET_D_MASK, buf[3]);

	return 0;
}

static int tx_offset_cal_efuse(struct phy_device *phydev, u32 *buf)
{
	u16 tx_offset_cal_val[4];

	tx_offset_cal_val[0] = EFS_DA_TX_AMP_OFFSET_A(buf[0]);
	tx_offset_cal_val[1] = EFS_DA_TX_AMP_OFFSET_B(buf[1]);
	tx_offset_cal_val[2] = EFS_DA_TX_AMP_OFFSET_C(buf[1]);
	tx_offset_cal_val[3] = EFS_DA_TX_AMP_OFFSET_D(buf[1]);

	tx_offset_fill_result(phydev, tx_offset_cal_val);

	return 0;
}

static int tx_amp_fill_result(struct phy_device *phydev, u16 *buf)
{
	int i;
	int bias[16] = {0};
	const int vals_9461[16] = { 7, 1, 4, 7,
				    7, 1, 4, 7,
				    7, 1, 4, 7,
				    7, 1, 4, 7 };
	const int vals_9481[16] = { 10, 6, 6, 10,
				    10, 6, 6, 10,
				    10, 6, 6, 10,
				    10, 6, 6, 10 };

	switch (phydev->drv->phy_id) {
	case MTK_GPHY_ID_MT7981:
		/* We add some calibration to efuse values
		 * due to board level influence.
		 * GBE: +7, TBT: +1, HBT: +4, TST: +7
		 */
		memcpy(bias, (const void *)vals_9461, sizeof(bias));
		break;
	case MTK_GPHY_ID_MT7988:
		memcpy(bias, (const void *)vals_9481, sizeof(bias));
		break;
	default:
		break;
	}

	/* Prevent overflow */
	for (i = 0; i < 12; i++) {
		if (buf[i >> 2] + bias[i] > 63) {
			buf[i >> 2] = 63;
			bias[i] = 0;
		} else if (buf[i >> 2] + bias[i] < 0) {
			/* Bias caused by board design may change in the future.
			 * So check negative cases, too.
			 */
			buf[i >> 2] = 0;
			bias[i] = 0;
		}
	}

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TXVLD_DA_RG,
		       MTK_PHY_DA_TX_I2MPB_A_GBE_MASK, (buf[0] + bias[0]) << 10);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TXVLD_DA_RG,
		       MTK_PHY_DA_TX_I2MPB_A_TBT_MASK, buf[0] + bias[1]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_A2,
		       MTK_PHY_DA_TX_I2MPB_A_HBT_MASK, (buf[0] + bias[2]) << 10);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_A2,
		       MTK_PHY_DA_TX_I2MPB_A_TST_MASK, buf[0] + bias[3]);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B1,
		       MTK_PHY_DA_TX_I2MPB_B_GBE_MASK, (buf[1] + bias[4]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B1,
		       MTK_PHY_DA_TX_I2MPB_B_TBT_MASK, buf[1] + bias[5]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B2,
		       MTK_PHY_DA_TX_I2MPB_B_HBT_MASK, (buf[1] + bias[6]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B2,
		       MTK_PHY_DA_TX_I2MPB_B_TST_MASK, buf[1] + bias[7]);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C1,
		       MTK_PHY_DA_TX_I2MPB_C_GBE_MASK, (buf[2] + bias[8]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C1,
		       MTK_PHY_DA_TX_I2MPB_C_TBT_MASK, buf[2] + bias[9]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C2,
		       MTK_PHY_DA_TX_I2MPB_C_HBT_MASK, (buf[2] + bias[10]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C2,
		       MTK_PHY_DA_TX_I2MPB_C_TST_MASK, buf[2] + bias[11]);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D1,
		       MTK_PHY_DA_TX_I2MPB_D_GBE_MASK, (buf[3] + bias[12]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D1,
		       MTK_PHY_DA_TX_I2MPB_D_TBT_MASK, buf[3] + bias[13]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D2,
		       MTK_PHY_DA_TX_I2MPB_D_HBT_MASK, (buf[3] + bias[14]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D2,
		       MTK_PHY_DA_TX_I2MPB_D_TST_MASK, buf[3] + bias[15]);

	return 0;
}

static int tx_amp_cal_efuse(struct phy_device *phydev, u32 *buf)
{
	u16 tx_amp_cal_val[4];

	tx_amp_cal_val[0] = EFS_DA_TX_I2MPB_A(buf[0]);
	tx_amp_cal_val[1] = EFS_DA_TX_I2MPB_B(buf[0]);
	tx_amp_cal_val[2] = EFS_DA_TX_I2MPB_C(buf[0]);
	tx_amp_cal_val[3] = EFS_DA_TX_I2MPB_D(buf[0]);
	tx_amp_fill_result(phydev, tx_amp_cal_val);

	return 0;
}

static int tx_r50_fill_result(struct phy_device *phydev, u16 tx_r50_cal_val,
			      u8 txg_calen_x)
{
	int bias = 0;
	u16 reg, val;

	switch (phydev->drv->phy_id) {
	case MTK_GPHY_ID_MT7988:
	{
		bias = -2;
		break;
	}
	/* MTK_GPHY_ID_MT7981 enters default case */
	default:
		break;
	}

	val = clamp_val(bias + tx_r50_cal_val, 0, 63);

	switch (txg_calen_x) {
	case PAIR_A:
		reg = MTK_PHY_DA_TX_R50_PAIR_A;
		break;
	case PAIR_B:
		reg = MTK_PHY_DA_TX_R50_PAIR_B;
		break;
	case PAIR_C:
		reg = MTK_PHY_DA_TX_R50_PAIR_C;
		break;
	case PAIR_D:
		reg = MTK_PHY_DA_TX_R50_PAIR_D;
		break;
	}

	phy_write_mmd(phydev, MDIO_MMD_VEND1, reg, val | val << 8);

	return 0;
}

static int tx_r50_cal_efuse(struct phy_device *phydev, u32 *buf,
			    u8 txg_calen_x)
{
	u16 tx_r50_cal_val;

	switch (txg_calen_x) {
	case PAIR_A:
		tx_r50_cal_val = EFS_DA_TX_R50_A(buf[1]);
		break;
	case PAIR_B:
		tx_r50_cal_val = EFS_DA_TX_R50_B(buf[1]);
		break;
	case PAIR_C:
		tx_r50_cal_val = EFS_DA_TX_R50_C(buf[2]);
		break;
	case PAIR_D:
		tx_r50_cal_val = EFS_DA_TX_R50_D(buf[2]);
		break;
	}
	tx_r50_fill_result(phydev, tx_r50_cal_val, txg_calen_x);

	return 0;
}

static int tx_vcm_cal_sw(struct phy_device *phydev, u8 rg_txreserve_x)
{
	u8 lower_idx, upper_idx, txreserve_val;
	u8 lower_ret, upper_ret;
	int ret;

	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
			 MTK_PHY_RG_ANA_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
			   MTK_PHY_RG_CAL_CKINV);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
			 MTK_PHY_RG_TXVOS_CALEN);

	switch (rg_txreserve_x) {
	case PAIR_A:
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN0_A,
				   MTK_PHY_DASN_DAC_IN0_A_MASK);
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN1_A,
				   MTK_PHY_DASN_DAC_IN1_A_MASK);
		phy_set_bits_mmd(phydev, MDIO_MMD_VEND1,
				 MTK_PHY_RG_ANA_CAL_RG0,
				 MTK_PHY_RG_ZCALEN_A);
		break;
	case PAIR_B:
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN0_B,
				   MTK_PHY_DASN_DAC_IN0_B_MASK);
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN1_B,
				   MTK_PHY_DASN_DAC_IN1_B_MASK);
		phy_set_bits_mmd(phydev, MDIO_MMD_VEND1,
				 MTK_PHY_RG_ANA_CAL_RG1,
				 MTK_PHY_RG_ZCALEN_B);
		break;
	case PAIR_C:
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN0_C,
				   MTK_PHY_DASN_DAC_IN0_C_MASK);
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN1_C,
				   MTK_PHY_DASN_DAC_IN1_C_MASK);
		phy_set_bits_mmd(phydev, MDIO_MMD_VEND1,
				 MTK_PHY_RG_ANA_CAL_RG1,
				 MTK_PHY_RG_ZCALEN_C);
		break;
	case PAIR_D:
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN0_D,
				   MTK_PHY_DASN_DAC_IN0_D_MASK);
		phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
				   MTK_PHY_RG_DASN_DAC_IN1_D,
				   MTK_PHY_DASN_DAC_IN1_D_MASK);
		phy_set_bits_mmd(phydev, MDIO_MMD_VEND1,
				 MTK_PHY_RG_ANA_CAL_RG1,
				 MTK_PHY_RG_ZCALEN_D);
		break;
	default:
		ret = -EINVAL;
		goto restore;
	}

	lower_idx = TXRESERVE_MIN;
	upper_idx = TXRESERVE_MAX;

	dev_dbg(&phydev->mdio.dev, "Start TX-VCM SW cal.\n");
	while ((upper_idx - lower_idx) > 1) {
		txreserve_val = DIV_ROUND_CLOSEST(lower_idx + upper_idx, 2);
		ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
				MTK_PHY_DA_RX_PSBN_TBT_MASK |
				MTK_PHY_DA_RX_PSBN_HBT_MASK |
				MTK_PHY_DA_RX_PSBN_GBE_MASK |
				MTK_PHY_DA_RX_PSBN_LP_MASK,
				txreserve_val << 12 | txreserve_val << 8 |
				txreserve_val << 4 | txreserve_val);
		if (ret == 1) {
			upper_idx = txreserve_val;
			upper_ret = ret;
		} else if (ret == 0) {
			lower_idx = txreserve_val;
			lower_ret = ret;
		} else {
			goto restore;
		}
	}

	if (lower_idx == TXRESERVE_MIN) {
		lower_ret = cal_cycle(phydev, MDIO_MMD_VEND1,
				      MTK_PHY_RXADC_CTRL_RG9,
				      MTK_PHY_DA_RX_PSBN_TBT_MASK |
				      MTK_PHY_DA_RX_PSBN_HBT_MASK |
				      MTK_PHY_DA_RX_PSBN_GBE_MASK |
				      MTK_PHY_DA_RX_PSBN_LP_MASK,
				      lower_idx << 12 | lower_idx << 8 |
				      lower_idx << 4 | lower_idx);
		ret = lower_ret;
	} else if (upper_idx == TXRESERVE_MAX) {
		upper_ret = cal_cycle(phydev, MDIO_MMD_VEND1,
				      MTK_PHY_RXADC_CTRL_RG9,
				      MTK_PHY_DA_RX_PSBN_TBT_MASK |
				      MTK_PHY_DA_RX_PSBN_HBT_MASK |
				      MTK_PHY_DA_RX_PSBN_GBE_MASK |
				      MTK_PHY_DA_RX_PSBN_LP_MASK,
				      upper_idx << 12 | upper_idx << 8 |
				      upper_idx << 4 | upper_idx);
		ret = upper_ret;
	}
	if (ret < 0)
		goto restore;

	/* We calibrate TX-VCM in different logic. Check upper index and then
	 * lower index. If this calibration is valid, apply lower index's result.
	 */
	ret = upper_ret - lower_ret;
	if (ret == 1) {
		ret = 0;
		/* Make sure we use upper_idx in our calibration system */
		cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
			  MTK_PHY_DA_RX_PSBN_TBT_MASK |
			  MTK_PHY_DA_RX_PSBN_HBT_MASK |
			  MTK_PHY_DA_RX_PSBN_GBE_MASK |
			  MTK_PHY_DA_RX_PSBN_LP_MASK,
			  upper_idx << 12 | upper_idx << 8 |
			  upper_idx << 4 | upper_idx);
		dev_info(&phydev->mdio.dev, "TX-VCM SW cal result: 0x%x\n",
			 upper_idx);
	} else if (lower_idx == TXRESERVE_MIN && upper_ret == 1 &&
		   lower_ret == 1) {
		ret = 0;
		cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
			  MTK_PHY_DA_RX_PSBN_TBT_MASK |
			  MTK_PHY_DA_RX_PSBN_HBT_MASK |
			  MTK_PHY_DA_RX_PSBN_GBE_MASK |
			  MTK_PHY_DA_RX_PSBN_LP_MASK,
			  lower_idx << 12 | lower_idx << 8 |
			  lower_idx << 4 | lower_idx);
		dev_warn(&phydev->mdio.dev,
			 "TX-VCM SW cal result at low margin 0x%x\n",
			 lower_idx);
	} else if (upper_idx == TXRESERVE_MAX && upper_ret == 0 &&
		   lower_ret == 0) {
		ret = 0;
		dev_warn(&phydev->mdio.dev,
			 "TX-VCM SW cal result at high margin 0x%x\n",
			 upper_idx);
	} else {
		ret = -EINVAL;
	}

restore:
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
			   MTK_PHY_RG_ANA_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
			   MTK_PHY_RG_TXVOS_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
			   MTK_PHY_RG_ZCALEN_A);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
			   MTK_PHY_RG_ZCALEN_B | MTK_PHY_RG_ZCALEN_C |
			   MTK_PHY_RG_ZCALEN_D);

	return ret;
}

static inline void mt798x_phy_common_finetune(struct phy_device *phydev)
{
	u32 i;

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	/* EnabRandUpdTrig = 1 */
	__phy_write(phydev, 0x11, 0x2f00);
	__phy_write(phydev, 0x12, 0xe);
	__phy_write(phydev, 0x10, 0x8fb0);

	/* NormMseLoThresh = 85 */
	__phy_write(phydev, 0x11, 0x55a0);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x83aa);

	/* TrFreeze = 0 */
	__phy_write(phydev, 0x11, 0x0);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x9686);

	/* SSTrKp1000Slv = 5 */
	__phy_write(phydev, 0x11, 0xbaef);
	__phy_write(phydev, 0x12, 0x2e);
	__phy_write(phydev, 0x10, 0x968c);

	/* MrvlTrFix100Kp = 3, MrvlTrFix100Kf = 2,
	 * MrvlTrFix1000Kp = 3, MrvlTrFix1000Kf = 2
	 */
	__phy_write(phydev, 0x11, 0xd10a);
	__phy_write(phydev, 0x12, 0x34);
	__phy_write(phydev, 0x10, 0x8f82);

	/* VcoSlicerThreshBitsHigh */
	__phy_write(phydev, 0x11, 0x5555);
	__phy_write(phydev, 0x12, 0x55);
	__phy_write(phydev, 0x10, 0x8ec0);
	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	/* TR_OPEN_LOOP_EN = 1, lpf_x_average = 9*/
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG234,
		       MTK_PHY_TR_OPEN_LOOP_EN_MASK | MTK_PHY_LPF_X_AVERAGE_MASK,
		       BIT(0) | FIELD_PREP(MTK_PHY_LPF_X_AVERAGE_MASK, 0x9));

	/* rg_tr_lpf_cnt_val = 512 */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LPF_CNT_VAL, 0x200);

	/* IIR2 related */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K1_L, 0x82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K1_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K2_L, 0x103);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K2_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K3_L, 0x82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K3_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K4_L, 0xd177);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K4_U, 0x3);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K5_L, 0x2c82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K5_U, 0xe);

	/* FFE peaking */
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG27C,
		       MTK_PHY_VGASTATE_FFE_THR_ST1_MASK, 0x1b << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG27D,
		       MTK_PHY_VGASTATE_FFE_THR_ST2_MASK, 0x1e);

	/* Disable LDO pump */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_LDO_PUMP_EN_PAIRAB, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_LDO_PUMP_EN_PAIRCD, 0x0);
	/* Adjust LDO output voltage */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_LDO_OUTPUT_V, 0x2222);
}

static inline void mt7981_phy_finetune(struct phy_device *phydev)
{
	/* 100M eye finetune:
	 * Keep middle level of TX MLT3 shapper as default.
	 * Only change TX MLT3 overshoot level here.
	 */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_0TO1,
		      0x1ce);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_0TO1,
		      0x1c1);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_1TO0,
		      0x20f);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_1TO0,
		      0x202);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_0TON1,
		      0x3d0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_0TON1,
		      0x3c0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_N1TO0,
		      0x13);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_N1TO0,
		      0x5);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	/* SlvDSPreadyTime = 24, MasDSPreadyTime = 24 */
	__phy_write(phydev, 0x11, 0xc71);
	__phy_write(phydev, 0x12, 0xc);
	__phy_write(phydev, 0x10, 0x8fae);

	/* ResetSyncOffset = 6 */
	__phy_write(phydev, 0x11, 0x600);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8fc0);

	/* VgaDecRate = 1 */
	__phy_write(phydev, 0x11, 0x4c2a);
	__phy_write(phydev, 0x12, 0x3e);
	__phy_write(phydev, 0x10, 0x8fa4);

	/* FfeUpdGainForce = 4 */
	__phy_write(phydev, 0x11, 0x240);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x9680);

	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);
}

static inline void mt7988_phy_finetune(struct phy_device *phydev)
{
	u16 val[12] = { 0x0187, 0x01cd, 0x01c8, 0x0182,
			0x020d, 0x0206, 0x0384, 0x03d0,
			0x03c6, 0x030a, 0x0011, 0x0005 };
	int i;

	for (i = 0; i < MTK_PHY_TX_MLT3_END; i++)
		phy_write_mmd(phydev, MDIO_MMD_VEND1, i, val[i]);

	/* TCT finetune */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_TX_FILTER, 0x5);

	/* Disable TX power saving */
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG7,
		       MTK_PHY_DA_AD_BUF_BIAS_LP_MASK, 0x3 << 8);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);

	/* SlvDSPreadyTime = 24, MasDSPreadyTime = 12 */
	__phy_write(phydev, 0x11, 0x671);
	__phy_write(phydev, 0x12, 0xc);
	__phy_write(phydev, 0x10, 0x8fae);

	/* ResetSyncOffset = 5 */
	__phy_write(phydev, 0x11, 0x500);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8fc0);

	/* VgaDecRate is 1 at default on mt7988 */

	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_2A30);
	/* TxClkOffset = 2 */
	__phy_modify(phydev, MTK_PHY_ANARG_RG, MTK_PHY_TCLKOFFSET_MASK,
		     FIELD_PREP(MTK_PHY_TCLKOFFSET_MASK, 0x2));
	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);
}

static inline void mt798x_phy_eee(struct phy_device *phydev)
{
	phy_modify_mmd(phydev, MDIO_MMD_VEND1,
		       MTK_PHY_RG_LPI_PCS_DSP_CTRL_REG120,
		       MTK_PHY_LPI_SIG_EN_LO_THRESH1000_MASK |
		       MTK_PHY_LPI_SIG_EN_HI_THRESH1000_MASK,
		       FIELD_PREP(MTK_PHY_LPI_SIG_EN_LO_THRESH1000_MASK, 0x0) |
		       FIELD_PREP(MTK_PHY_LPI_SIG_EN_HI_THRESH1000_MASK, 0x14));

	phy_modify_mmd(phydev, MDIO_MMD_VEND1,
		       MTK_PHY_RG_LPI_PCS_DSP_CTRL_REG122,
		       MTK_PHY_LPI_NORM_MSE_HI_THRESH1000_MASK,
		       FIELD_PREP(MTK_PHY_LPI_NORM_MSE_HI_THRESH1000_MASK,
				  0xff));

	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
			   MTK_PHY_RG_TESTMUX_ADC_CTRL,
			   MTK_PHY_RG_TXEN_DIG_MASK);

	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1,
			 MTK_PHY_RG_DEV1E_REG19b, MTK_PHY_BYPASS_DSP_LPI_READY);

	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1,
			   MTK_PHY_RG_DEV1E_REG234, MTK_PHY_TR_LP_IIR_EEE_EN);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG238,
		       MTK_PHY_LPI_SLV_SEND_TX_TIMER_MASK |
		       MTK_PHY_LPI_SLV_SEND_TX_EN,
		       FIELD_PREP(MTK_PHY_LPI_SLV_SEND_TX_TIMER_MASK, 0x120));

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG239,
		       MTK_PHY_LPI_SEND_LOC_TIMER_MASK |
		       MTK_PHY_LPI_TXPCS_LOC_RCV,
		       FIELD_PREP(MTK_PHY_LPI_SEND_LOC_TIMER_MASK, 0x117));

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG2C7,
		       MTK_PHY_MAX_GAIN_MASK | MTK_PHY_MIN_GAIN_MASK,
		       FIELD_PREP(MTK_PHY_MAX_GAIN_MASK, 0x8) |
		       FIELD_PREP(MTK_PHY_MIN_GAIN_MASK, 0x13));

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG2D1,
		       MTK_PHY_VCO_SLICER_THRESH_BITS_HIGH_EEE_MASK,
		       FIELD_PREP(MTK_PHY_VCO_SLICER_THRESH_BITS_HIGH_EEE_MASK,
				  0x33) |
		       MTK_PHY_LPI_SKIP_SD_SLV_TR | MTK_PHY_LPI_TR_READY |
		       MTK_PHY_LPI_VCO_EEE_STG0_EN);

	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG323,
			 MTK_PHY_EEE_WAKE_MAS_INT_DC |
			 MTK_PHY_EEE_WAKE_SLV_INT_DC);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG324,
		       MTK_PHY_SMI_DETCNT_MAX_MASK,
		       FIELD_PREP(MTK_PHY_SMI_DETCNT_MAX_MASK, 0x3f) |
		       MTK_PHY_SMI_DET_MAX_EN);

	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG326,
			 MTK_PHY_LPI_MODE_SD_ON | MTK_PHY_RESET_RANDUPD_CNT |
			 MTK_PHY_TREC_UPDATE_ENAB_CLR |
			 MTK_PHY_LPI_QUIT_WAIT_DFE_SIG_DET_OFF |
			 MTK_PHY_TR_READY_SKIP_AFE_WAKEUP);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	/* Regsigdet_sel_1000 = 0 */
	__phy_write(phydev, 0x11, 0xb);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x9690);

	/* REG_EEE_st2TrKf1000 = 3 */
	__phy_write(phydev, 0x11, 0x114f);
	__phy_write(phydev, 0x12, 0x2);
	__phy_write(phydev, 0x10, 0x969a);

	/* RegEEE_slv_wake_tr_timer_tar = 6, RegEEE_slv_remtx_timer_tar = 20 */
	__phy_write(phydev, 0x11, 0x3028);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x969e);

	/* RegEEE_slv_wake_int_timer_tar = 8 */
	__phy_write(phydev, 0x11, 0x5010);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x96a0);

	/* RegEEE_trfreeze_timer2 = 586 */
	__phy_write(phydev, 0x11, 0x24a);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x96a8);

	/* RegEEE100Stg1_tar = 16 */
	__phy_write(phydev, 0x11, 0x3210);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x96b8);

	/* REGEEE_wake_slv_tr_wait_dfesigdet_en = 1 */
	__phy_write(phydev, 0x11, 0x1463);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x96ca);

	/* DfeTailEnableVgaThresh1000 = 27 */
	__phy_write(phydev, 0x11, 0x36);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8f80);
	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_3);
	__phy_modify(phydev, MTK_PHY_LPI_REG_14, MTK_PHY_LPI_WAKE_TIMER_1000_MASK,
		     FIELD_PREP(MTK_PHY_LPI_WAKE_TIMER_1000_MASK, 0x19c));

	__phy_modify(phydev, MTK_PHY_LPI_REG_1c, MTK_PHY_SMI_DET_ON_THRESH_MASK,
		     FIELD_PREP(MTK_PHY_SMI_DET_ON_THRESH_MASK, 0xc));
	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1,
		       MTK_PHY_RG_LPI_PCS_DSP_CTRL_REG122,
		       MTK_PHY_LPI_NORM_MSE_HI_THRESH1000_MASK,
		       FIELD_PREP(MTK_PHY_LPI_NORM_MSE_HI_THRESH1000_MASK, 0xff));
}

static inline int cal_sw(struct phy_device *phydev, enum CAL_ITEM cal_item,
			 u8 start_pair, u8 end_pair)
{
	u8 pair_n;
	int ret;

	for (pair_n = start_pair; pair_n <= end_pair; pair_n++) {
		/* TX_OFFSET & TX_AMP have no SW calibration. */
		switch (cal_item) {
		case TX_VCM:
			ret = tx_vcm_cal_sw(phydev, pair_n);
			break;
		default:
			return -EINVAL;
		}
		if (ret)
			return ret;
	}
	return 0;
}

static inline int cal_efuse(struct phy_device *phydev, enum CAL_ITEM cal_item,
			    u8 start_pair, u8 end_pair, u32 *buf)
{
	u8 pair_n;
	int ret;

	for (pair_n = start_pair; pair_n <= end_pair; pair_n++) {
		/* TX_VCM has no efuse calibration. */
		switch (cal_item) {
		case REXT:
			ret = rext_cal_efuse(phydev, buf);
			break;
		case TX_OFFSET:
			ret = tx_offset_cal_efuse(phydev, buf);
			break;
		case TX_AMP:
			ret = tx_amp_cal_efuse(phydev, buf);
			break;
		case TX_R50:
			ret = tx_r50_cal_efuse(phydev, buf, pair_n);
			break;
		default:
			return -EINVAL;
		}
		if (ret)
			return ret;
	}

	return 0;
}

static int start_cal(struct phy_device *phydev, enum CAL_ITEM cal_item,
		     enum CAL_MODE cal_mode, u8 start_pair,
		     u8 end_pair, u32 *buf)
{
	int ret;
	char cal_prop[5][20] = { "mediatek,rext", "mediatek,tx_offset",
				 "mediatek,tx_amp", "mediatek,tx_r50",
				 "mediatek,tx_vcm" };

	switch (cal_mode) {
	case EFUSE_M:
		ret = cal_efuse(phydev, cal_item, start_pair,
				end_pair, buf);
		break;
	case SW_M:
		ret = cal_sw(phydev, cal_item, start_pair, end_pair);
		break;
	default:
		return -EINVAL;
	}

	if (ret) {
		dev_err(&phydev->mdio.dev, "[%s]cal failed\n", cal_prop[cal_item]);
		return -EIO;
	}

	return 0;
}

static int mt798x_phy_calibration(struct phy_device *phydev)
{
	int ret = 0;
	u32 *buf;
	size_t len;
	struct nvmem_cell *cell;

	if (phydev->interface != PHY_INTERFACE_MODE_GMII)
		return -EINVAL;

	cell = nvmem_cell_get(&phydev->mdio.dev, "phy-cal-data");
	if (IS_ERR(cell)) {
		if (PTR_ERR(cell) == -EPROBE_DEFER)
			return PTR_ERR(cell);
		return 0;
	}

	buf = (u32 *)nvmem_cell_read(cell, &len);
	if (IS_ERR(buf))
		return PTR_ERR(buf);
	nvmem_cell_put(cell);

	if (!buf[0] || !buf[1] || !buf[2] || !buf[3] || len < 4 * sizeof(u32)) {
		dev_err(&phydev->mdio.dev, "invalid efuse data\n");
		ret = -EINVAL;
		goto out;
	}

	ret = start_cal(phydev, REXT, EFUSE_M, NO_PAIR, NO_PAIR, buf);
	if (ret)
		goto out;
	ret = start_cal(phydev, TX_OFFSET, EFUSE_M, NO_PAIR, NO_PAIR, buf);
	if (ret)
		goto out;
	ret = start_cal(phydev, TX_AMP, EFUSE_M, NO_PAIR, NO_PAIR, buf);
	if (ret)
		goto out;
	ret = start_cal(phydev, TX_R50, EFUSE_M, PAIR_A, PAIR_D, buf);
	if (ret)
		goto out;
	ret = start_cal(phydev, TX_VCM, SW_M, PAIR_A, PAIR_A, buf);
	if (ret)
		goto out;

out:
	kfree(buf);
	return ret;
}

static int mt7981_phy_probe(struct phy_device *phydev)
{
	mt798x_phy_common_finetune(phydev);
	mt7981_phy_finetune(phydev);
	mt798x_phy_eee(phydev);

	return mt798x_phy_calibration(phydev);
}

static int mt7988_phy_probe(struct phy_device *phydev)
{
	struct device_node *np;
	void __iomem *boottrap;
	u32 reg;
	int port;
	int ret;
	struct pinctrl *pinctrl;

	/* Setup LED polarity according to boottrap's polarity */
	np = of_find_compatible_node(NULL, NULL, "mediatek,boottrap");
	if (!np)
		return -ENOENT;
	boottrap = of_iomap(np, 0);
	if (!boottrap)
		return -ENOMEM;
	reg = readl(boottrap);
	port = phydev->mdio.addr;
	if ((port == GPHY_PORT0 && reg & BIT(8)) ||
	    (port == GPHY_PORT1 && reg & BIT(9)) ||
	    (port == GPHY_PORT2 && reg & BIT(10)) ||
	    (port == GPHY_PORT3 && reg & BIT(11))) {
		phy_write_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_ON_CTRL,
			      MTK_PHY_LED0_ENABLE | MTK_PHY_LED0_ON_LINK10 |
			      MTK_PHY_LED0_ON_LINK100 |
			      MTK_PHY_LED0_ON_LINK1000);
	} else {
		phy_write_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_ON_CTRL,
			      MTK_PHY_LED0_ENABLE | MTK_PHY_LED0_POLARITY |
			      MTK_PHY_LED0_ON_LINK10 |
			      MTK_PHY_LED0_ON_LINK100 |
			      MTK_PHY_LED0_ON_LINK1000);
	}
	phy_write_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_BLINK_CTRL,
		      MTK_PHY_LED0_1000TX | MTK_PHY_LED0_1000RX |
		      MTK_PHY_LED0_100TX  | MTK_PHY_LED0_100RX  |
		      MTK_PHY_LED0_10TX   | MTK_PHY_LED0_10RX);

	if (port == GPHY_PORT3) {
		pinctrl = devm_pinctrl_get_select_default(&phydev->mdio.bus->dev);
		if (IS_ERR(pinctrl)) {
			ret = PTR_ERR(pinctrl);
			dev_err(&phydev->mdio.dev, "Fail to set LED pins!\n");
			return -EINVAL;
		}
	}

	mt798x_phy_common_finetune(phydev);
	mt7988_phy_finetune(phydev);
	mt798x_phy_eee(phydev);

	return mt798x_phy_calibration(phydev);
}
#endif

static struct phy_driver mtk_gephy_driver[] = {
	{
		PHY_ID_MATCH_EXACT(MTK_GPHY_ID_MT7530),
		.name		= "MediaTek MT7530 PHY",
		.config_init	= mt7530_phy_config_init,
		/* Interrupts are handled by the switch, not the PHY
		 * itself.
		 */
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
	{
		PHY_ID_MATCH_EXACT(MTK_GPHY_ID_MT7531),
		.name		= "MediaTek MT7531 PHY",
		.config_init	= mt7531_phy_config_init,
		/* Interrupts are handled by the switch, not the PHY
		 * itself.
		 */
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
#ifdef CONFIG_MEDIATEK_GE_PHY_SOC
	{
		PHY_ID_MATCH_EXACT(MTK_GPHY_ID_MT7981),
		.name		= "MediaTek MT7981 PHY",
		.probe		= mt7981_phy_probe,
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
	{
		PHY_ID_MATCH_EXACT(MTK_GPHY_ID_MT7988),
		.name		= "MediaTek MT7988 PHY",
		.probe		= mt7988_phy_probe,
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
#endif
};

module_phy_driver(mtk_gephy_driver);

static struct mdio_device_id __maybe_unused mtk_gephy_tbl[] = {
	{ PHY_ID_MATCH_VENDOR(0x03a29400) },
	{ }
};

MODULE_DESCRIPTION("MediaTek Gigabit Ethernet PHY driver");
MODULE_AUTHOR("Daniel Golle <daniel@makrotopia.org>");
MODULE_AUTHOR("SkyLake Huang <SkyLake.Huang@mediatek.com>");
MODULE_AUTHOR("DENG, Qingfang <dqfext@gmail.com>");
MODULE_LICENSE("GPL");

MODULE_DEVICE_TABLE(mdio, mtk_gephy_tbl);
