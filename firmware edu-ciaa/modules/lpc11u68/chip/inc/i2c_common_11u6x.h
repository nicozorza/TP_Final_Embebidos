/*
 * @brief LPC11u6x I2C driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __I2C_COMMON_11U6X_H_
#define __I2C_COMMON_11U6X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup I2C_11U6X CHIP: LPC11u6x I2C driver
 * @ingroup CHIP_11U6X_Drivers
 * @{
 */

/**
 * @brief I2C register block structure
 */
typedef struct {				/* I2C0 Structure         */
	__IO uint32_t CONSET;		/*!< I2C Control Set Register. When a one is written to a bit of this register, the corresponding bit in the I2C control register is set. Writing a zero has no effect on the corresponding bit in the I2C control register. */
	__I  uint32_t STAT;			/*!< I2C Status Register. During I2C operation, this register provides detailed status codes that allow software to determine the next action needed. */
	__IO uint32_t DAT;			/*!< I2C Data Register. During master or slave transmit mode, data to be transmitted is written to this register. During master or slave receive mode, data that has been received may be read from this register. */
	__IO uint32_t ADR0;			/*!< I2C Slave Address Register 0. Contains the 7-bit slave address for operation of the I2C interface in slave mode, and is not used in master mode. The least significant bit determines whether a slave responds to the General Call address. */
	__IO uint32_t SCLH;			/*!< SCH Duty Cycle Register High Half Word. Determines the high time of the I2C clock. */
	__IO uint32_t SCLL;			/*!< SCL Duty Cycle Register Low Half Word. Determines the low time of the I2C clock. SCLL and SCLH together determine the clock frequency generated by an I2C master and certain times used in slave mode. */
	__O  uint32_t CONCLR;		/*!< I2C Control Clear Register. When a one is written to a bit of this register, the corresponding bit in the I2C control register is cleared. Writing a zero has no effect on the corresponding bit in the I2C control register. */
	__IO uint32_t MMCTRL;		/*!< Monitor mode control register. */
	__IO uint32_t ADR1;			/*!< I2C Slave Address Register. Contains the 7-bit slave address for operation of the I2C interface in slave mode, and is not used in master mode. The least significant bit determines whether a slave responds to the General Call address. */
	__IO uint32_t ADR2;			/*!< I2C Slave Address Register. Contains the 7-bit slave address for operation of the I2C interface in slave mode, and is not used in master mode. The least significant bit determines whether a slave responds to the General Call address. */
	__IO uint32_t ADR3;			/*!< I2C Slave Address Register. Contains the 7-bit slave address for operation of the I2C interface in slave mode, and is not used in master mode. The least significant bit determines whether a slave responds to the General Call address. */
	__I  uint32_t DATA_BUFFER;	/*!< Data buffer register. The contents of the 8 MSBs of the DAT shift register will be transferred to the DATA_BUFFER automatically after every nine bits (8 bits of data plus ACK or NACK) has been received on the bus. */
	__IO uint32_t MASK[4];		/*!< I2C Slave address mask register */
} LPC_I2C_T;

/*
 * @brief I2C Control Set register description
 */
#define I2C_I2CONSET_AA             ((0x04))/*!< Assert acknowledge flag */
#define I2C_I2CONSET_SI             ((0x08))/*!< I2C interrupt flag */
#define I2C_I2CONSET_STO            ((0x10))/*!< STOP flag */
#define I2C_I2CONSET_STA            ((0x20))/*!< START flag */
#define I2C_I2CONSET_I2EN           ((0x40))/*!< I2C interface enable */

/*
 * @brief I2C Control Clear register description
 */
#define I2C_I2CONCLR_AAC            ((1 << 2))	/*!< Assert acknowledge Clear bit */
#define I2C_I2CONCLR_SIC            ((1 << 3))	/*!< I2C interrupt Clear bit */
#define I2C_I2CONCLR_STOC           ((1 << 4))	/*!< I2C STOP Clear bit */
#define I2C_I2CONCLR_STAC           ((1 << 5))	/*!< START flag Clear bit */
#define I2C_I2CONCLR_I2ENC          ((1 << 6))	/*!< I2C interface Disable bit */

/*
 * @brief	I2C Common Control register description
 */
#define I2C_CON_AA            (1UL << 2)	/*!< Assert acknowledge bit */
#define I2C_CON_SI            (1UL << 3)	/*!< I2C interrupt bit */
#define I2C_CON_STO           (1UL << 4)	/*!< I2C STOP bit */
#define I2C_CON_STA           (1UL << 5)	/*!< START flag bit */
#define I2C_CON_I2EN          (1UL << 6)	/*!< I2C interface bit */

/*
 * @brief I2C Status Code definition (I2C Status register)
 */
#define I2C_STAT_CODE_BITMASK       ((0xF8))/*!< Return Code mask in I2C status register */
#define I2C_STAT_CODE_ERROR         ((0xFF))/*!< Return Code error mask in I2C status register */

/*
 * @brief I2C Master transmit mode
 */
#define I2C_I2STAT_M_TX_START                   ((0x08))/*!< A start condition has been transmitted */
#define I2C_I2STAT_M_TX_RESTART                 ((0x10))/*!< A repeat start condition has been transmitted */
#define I2C_I2STAT_M_TX_SLAW_ACK                ((0x18))/*!< SLA+W has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_SLAW_NACK               ((0x20))/*!< SLA+W has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_DAT_ACK                 ((0x28))/*!< Data has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_DAT_NACK                ((0x30))/*!< Data has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_ARB_LOST                ((0x38))/*!< Arbitration lost in SLA+R/W or Data bytes */

/*
 * @brief I2C Master receive mode
 */
#define I2C_I2STAT_M_RX_START                   ((0x08))/*!< A start condition has been transmitted */
#define I2C_I2STAT_M_RX_RESTART                 ((0x10))/*!< A repeat start condition has been transmitted */
#define I2C_I2STAT_M_RX_ARB_LOST                ((0x38))/*!< Arbitration lost */
#define I2C_I2STAT_M_RX_SLAR_ACK                ((0x40))/*!< SLA+R has been transmitted, ACK has been received */
#define I2C_I2STAT_M_RX_SLAR_NACK               ((0x48))/*!< SLA+R has been transmitted, NACK has been received */
#define I2C_I2STAT_M_RX_DAT_ACK                 ((0x50))/*!< Data has been received, ACK has been returned */
#define I2C_I2STAT_M_RX_DAT_NACK                ((0x58))/*!< Data has been received, NACK has been returned */

/*
 * @brief I2C Slave receive mode
 */
#define I2C_I2STAT_S_RX_SLAW_ACK                ((0x60))/*!< Own slave address has been received, ACK has been returned */
#define I2C_I2STAT_S_RX_ARB_LOST_M_SLA          ((0x68))/*!< Arbitration lost in SLA+R/W as master */
// #define I2C_I2STAT_S_RX_SLAW_ACK				((0x68)) /*!< Own SLA+W has been received, ACK returned */
#define I2C_I2STAT_S_RX_GENCALL_ACK             ((0x70))/*!< General call address has been received, ACK has been returned */
#define I2C_I2STAT_S_RX_ARB_LOST_M_GENCALL      ((0x78))/*!< Arbitration lost in SLA+R/W (GENERAL CALL) as master */
// #define I2C_I2STAT_S_RX_GENCALL_ACK				((0x78)) /*!< General call address has been received, ACK has been returned */
#define I2C_I2STAT_S_RX_PRE_SLA_DAT_ACK         ((0x80))/*!< Previously addressed with own SLA; Data has been received, ACK has been returned */
#define I2C_I2STAT_S_RX_PRE_SLA_DAT_NACK        ((0x88))/*!< Previously addressed with own SLA;Data has been received and NOT ACK has been returned */
#define I2C_I2STAT_S_RX_PRE_GENCALL_DAT_ACK     ((0x90))/*!< Previously addressed with General Call; Data has been received and ACK has been returned */
#define I2C_I2STAT_S_RX_PRE_GENCALL_DAT_NACK    ((0x98))/*!< Previously addressed with General Call; Data has been received and NOT ACK has been returned */
#define I2C_I2STAT_S_RX_STA_STO_SLVREC_SLVTRX   ((0xA0))/*!< A STOP condition or repeated START condition has been received while still addressed as SLV/REC (Slave Receive) or
														   SLV/TRX (Slave Transmit) */

/*
 * @brief I2C Slave transmit mode
 */
#define I2C_I2STAT_S_TX_SLAR_ACK                ((0xA8))/*!< Own SLA+R has been received, ACK has been returned */
#define I2C_I2STAT_S_TX_ARB_LOST_M_SLA          ((0xB0))/*!< Arbitration lost in SLA+R/W as master */
// #define I2C_I2STAT_S_TX_SLAR_ACK				((0xB0)) /*!< Own SLA+R has been received, ACK has been returned */
#define I2C_I2STAT_S_TX_DAT_ACK                 ((0xB8))/*!< Data has been transmitted, ACK has been received */
#define I2C_I2STAT_S_TX_DAT_NACK                ((0xC0))/*!< Data has been transmitted, NACK has been received */
#define I2C_I2STAT_S_TX_LAST_DAT_ACK            ((0xC8))/*!< Last data byte in I2DAT has been transmitted (AA = 0); ACK has been received */
#define I2C_SLAVE_TIME_OUT                      0x10000000UL/*!< Time out in case of using I2C slave mode */

/*
 * @brief I2C Data register definition
 */
#define I2C_I2DAT_BITMASK           ((0xFF))/*!< Mask for I2DAT register */
#define I2C_I2DAT_IDLE_CHAR         (0xFF)	/*!< Idle data value will be send out in slave mode in case of the actual expecting data requested from the master is greater than
											     its sending data length that can be supported */

/*
 * @brief I2C Monitor mode control register description
 */
#define I2C_I2MMCTRL_MM_ENA         ((1 << 0))			/**< Monitor mode enable */
#define I2C_I2MMCTRL_ENA_SCL        ((1 << 1))			/**< SCL output enable */
#define I2C_I2MMCTRL_MATCH_ALL      ((1 << 2))			/**< Select interrupt register match */
#define I2C_I2MMCTRL_BITMASK        ((0x07))		/**< Mask for I2MMCTRL register */

/*
 * @brief I2C Data buffer register description
 */
#define I2DATA_BUFFER_BITMASK       ((0xFF))/*!< I2C Data buffer register bit mask */

/*
 * @brief I2C Slave Address registers definition
 */
#define I2C_I2ADR_GC                ((1 << 0))	/*!< General Call enable bit */
#define I2C_I2ADR_BITMASK           ((0xFF))/*!< I2C Slave Address registers bit mask */

/*
 * @brief I2C Mask Register definition
 */
#define I2C_I2MASK_MASK(n)          ((n & 0xFE))/*!< I2C Mask Register mask field */

/*
 * @brief I2C SCL HIGH duty cycle Register definition
 */
#define I2C_I2SCLH_BITMASK          ((0xFFFF))	/*!< I2C SCL HIGH duty cycle Register bit mask */

/*
 * @brief I2C SCL LOW duty cycle Register definition
 */
#define I2C_I2SCLL_BITMASK          ((0xFFFF))	/*!< I2C SCL LOW duty cycle Register bit mask */

/*
 * @brief I2C monitor control configuration defines
 */
#define I2C_MONITOR_CFG_SCL_OUTPUT  I2C_I2MMCTRL_ENA_SCL		/**< SCL output enable */
#define I2C_MONITOR_CFG_MATCHALL    I2C_I2MMCTRL_MATCH_ALL		/**< Select interrupt register match */

/**
 * @}
 */

 #ifdef __cplusplus
}
#endif

#endif /* __I2C_COMMON_11U6X_H_ */
