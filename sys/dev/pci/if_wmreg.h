/*	$NetBSD: if_wmreg.h,v 1.128 2022/10/19 06:37:25 msaitoh Exp $	*/

/*
 * Copyright (c) 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/******************************************************************************

  Copyright (c) 2001-2012, Intel Corporation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of the Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

/*
 * Register description for the Intel i82542 (``Wiseman''),
 * i82543 (``Livengood''), and i82544 (``Cordova'') Gigabit
 * Ethernet chips.
 */

/*
 * The wiseman supports 64-bit PCI addressing.  This structure
 * describes the address in descriptors.
 */
typedef struct wiseman_addr {
	uint32_t	wa_low;		/* low-order 32 bits */
	uint32_t	wa_high;	/* high-order 32 bits */
} __packed wiseman_addr_t;

/*
 * The Wiseman receive descriptor.
 *
 * The receive descriptor ring must be aligned to a 4K boundary,
 * and there must be an even multiple of 8 descriptors in the ring.
 */
typedef struct wiseman_rxdesc {
	volatile wiseman_addr_t	wrx_addr;	/* buffer address */

	volatile uint16_t	wrx_len;	/* buffer length */
	volatile uint16_t	wrx_cksum;	/* checksum (starting at PCSS)*/

	volatile uint8_t	wrx_status;	/* Rx status */
	volatile uint8_t	wrx_errors;	/* Rx errors */
	volatile uint16_t	wrx_special;	/* special field (VLAN, etc.) */
} __packed wiseman_rxdesc_t;

/* wrx_status bits */
#define	WRX_ST_DD	__BIT(0)	/* descriptor done */
#define	WRX_ST_EOP	__BIT(1)	/* end of packet */
#define	WRX_ST_IXSM	__BIT(2)	/* ignore checksum indication */
#define	WRX_ST_VP	__BIT(3)	/* VLAN packet */
#define	WRX_ST_BPDU	__BIT(4)	/* ??? */
#define	WRX_ST_TCPCS	__BIT(5)	/* TCP checksum performed */
#define	WRX_ST_IPCS	__BIT(6)	/* IP checksum performed */
#define	WRX_ST_PIF	__BIT(7)	/* passed in-exact filter */

/* wrx_error bits */
#define	WRX_ER_CE	__BIT(0)	/* CRC error */
#define	WRX_ER_SE	__BIT(1)	/* symbol error */
#define	WRX_ER_SEQ	__BIT(2)	/* sequence error */
#define	WRX_ER_ICE	__BIT(3)	/* ??? */
#define	WRX_ER_CXE	__BIT(4)	/* carrier extension error */
#define	WRX_ER_TCPE	__BIT(5)	/* TCP checksum error */
#define	WRX_ER_IPE	__BIT(6)	/* IP checksum error */
#define	WRX_ER_RXE	__BIT(7)	/* Rx data error */

/* wrx_special field for VLAN packets */
#define	WRX_VLAN_ID(x)	((x) & 0x0fff)	/* VLAN identifier */
#define	WRX_VLAN_CFI	__BIT(12)	/* Canonical Form Indicator */
#define	WRX_VLAN_PRI(x)	(((x) >> 13) & 7)/* VLAN priority field */

/* extended RX descriptor for 82574 */
typedef union ext_rxdesc {
	struct {
		uint64_t erxd_addr;	/* Packet Buffer Address */
		uint64_t erxd_dd;	/* 63:1 reserved, 0 DD */
	} erx_data;
	struct {
		uint32_t erxc_mrq;	/*
					 * 31:13 reserved
					 * 12:8 Rx queue associated with the packet
					 * 7:4 reserved 3:0 RSS Type
					 */
		uint32_t erxc_rsshash;	/* RSS Hash or {Fragment Checksum, IP identification } */
		uint32_t erxc_err_stat;	/* 31:20 Extended Error, 19:0 Extened Status */
		uint16_t erxc_pktlen;	/* PKT_LEN */
		uint16_t erxc_vlan;	/* VLAN Tag */
	} erx_ctx;
} __packed ext_rxdesc_t;

#define	EXTRXD_DD_MASK		__BIT(0)

/*
 * erxc_rsshash is used for below 2 patterns
 *     (1) Fragment Checksum and IP identification
 *         - Fragment Checksum is valid
 *           when RXCSUM.PCSD cleared and RXCSUM.IPPCSE bit is set
 *         - IP identification is valid
 *           when RXCSUM.PCSD cleared and RXCSUM.IPPCSE bit is set
 *     (2) RSS Hash
 *         when RXCSUM.PCSD bit is set
 */
#define	EXTRXC_IP_ID_MASK	__BITS(15,0)
#define	EXTRXC_FRAG_CSUM_MASK	__BITS(31,16)
#define	EXTRXC_IP_ID(rsshash)	__SHIFTOUT(rsshash,ERXC_IP_ID_MASK)
#define	EXTRXC_FRAG_CSUM(rsshash) __SHIFTOUT(rsshash,ERXC_FRAG_CSUM_MASK)

/* macros for nrxc_mrq */
#define	EXTRXC_RSS_TYPE_MASK		__BITS(3,0)
/* __BITS(7,4) is reserved */
#define	EXTRXC_QUEUE_MASK		__BITS(12,8)
/* __BITS(31,13) is reserved */
#define	EXTRXC_RSS_TYPE(mrq)	__SHIFTOUT(mrq,EXTRXC_RSS_TYPE_MASK)
#define	EXTRXC_QUEUE(mrq)	__SHIFTOUT(mrq,EXTRXC_QUEUE_MASK)

#define	EXTRXC_RSS_TYPE_NONE		0x0 /* No hash computation done. */
#define	EXTRXC_RSS_TYPE_TCP_IPV4	0x1
#define	EXTRXC_RSS_TYPE_IPV4		0x2
#define	EXTRXC_RSS_TYPE_TCP_IPV6	0x3
#define	EXTRXC_RSS_TYPE_IPV6_EX		0x4
#define	EXTRXC_RSS_TYPE_IPV6		0x5
/*0x6:0xF is reserved. */

#define	EXTRXC_STATUS_MASK	__BITS(19,0)
#define	EXTRXC_ERROR_MASK	__BITS(31,20)
#define	EXTRXC_STATUS(err_stat)	__SHIFTOUT(err_stat,EXTRXC_STATUS_MASK)
#define	EXTRXC_ERROR(err_stat)	__SHIFTOUT(err_stat,EXTRXC_ERROR_MASK)

/* 3:0 is reserved. */
#define	EXTRXC_ERROR_CE		__BIT(4) /* The same as WRX_ER_CE. */
#define	EXTRXC_ERROR_SE		__BIT(5) /* The same as WRX_ER_SE. */
#define	EXTRXC_ERROR_SEQ	__BIT(6) /* The same as WRX_ER_SEQ. */
/* 7 is reserved. */
#define	EXTRXC_ERROR_CXE	__BIT(8) /* The same as WRX_ER_CXE. */
#define	EXTRXC_ERROR_TCPE	__BIT(9) /* The same as WRX_ER_TCPE. */
#define	EXTRXC_ERROR_IPE	__BIT(10) /* The same as WRX_ER_IPE. */
#define	EXTRXC_ERROR_RXE	__BIT(11) /* The same as WRX_ER_RXE. */

#define	EXTRXC_STATUS_DD		__BIT(0) /* The same as WRX_ST_DD. */
#define	EXTRXC_STATUS_EOP		__BIT(1) /* The same as WRX_ST_EOP. */
/* 2 is reserved. */
#define	EXTRXC_STATUS_VP		__BIT(3) /* The same as WRX_ST_VP. */
#define	EXTRXC_STATUS_UDPCS		__BIT(4) /* UDP checksum calculated on packet. */
#define	EXTRXC_STATUS_TCPCS		__BIT(5) /* The same as WRX_ST_TCPCS. */
#define	EXTRXC_STATUS_IPCS		__BIT(6) /* The same as WRX_ST_IPCS. */
/* 7 is reserved. */
#define	EXTRXC_STATUS_TST		__BIT(8) /* Time stamp taken. */
#define	EXTRXC_STATUS_IPIDV		__BIT(9) /* IP identification valid. */
#define	EXTRXC_STATUS_UDPV		__BIT(10) /* Valid UDP XSUM. */
/* 14:11 is reserved. */
#define	EXTRXC_STATUS_ACK		__BIT(15) /* ACK packet indication. */
#define	EXTRXC_STATUS_PKTTYPE_MASK	__BITS(19,16)
#define	EXTRXC_STATUS_PKTTYPE(status)	__SHIFTOUT(status,EXTRXC_STATUS_PKTTYPE_MASK)

/* advanced RX descriptor for 82575 and newer */
typedef union nq_rxdesc {
	struct {
		uint64_t nrxd_paddr;	/* 63:1 Packet Buffer Address, 0 A0/NSE */
		uint64_t nrxd_haddr;	/* 63:1 HEader Buffer Address, 0 DD */
	} nqrx_data;
	struct {
		uint32_t nrxc_misc;	/*
					 * 31: SPH, 30:21 HDR_LEN[9:0],
					 * 20:19 HDR_LEN[11:10], 18:17 RSV,
					 * 16:4 Packet Type 3:0 RSS Type
					 */
		uint32_t nrxc_rsshash;	/* RSS Hash or {Fragment Checksum, IP identification } */
		uint32_t nrxc_err_stat;	/* 31:20 Extended Error, 19:0 Extened Status */
		uint16_t nrxc_pktlen;	/* PKT_LEN */
		uint16_t nrxc_vlan;	/* VLAN Tag */
	} nqrx_ctx;
} __packed nq_rxdesc_t;

/* for nrxd_paddr macros */
#define	NQRXD_A0_MASK		__BIT(0)
#define	NQRXD_NSE_MASK		__BIT(0)
#define	NQRXD_ADDR_MASK		__BITS(63,1)
/* for nrxd_haddr macros */
#define	NQRXD_DD_MASK		__BIT(0)

/*
 * nrxc_rsshash is used for below 2 patterns
 *     (1) Fragment Checksum and IP identification
 *         - Fragment Checksum is valid
 *           when RXCSUM.PCSD cleared and RXCSUM.IPPCSE bit is set
 *         - IP identification is valid
 *           when RXCSUM.PCSD cleared and RXCSUM.IPPCSE bit is set
 *     (2) RSS Hash
 *         when RXCSUM.PCSD bit is set
 */
#define	NQRXC_IP_ID_MASK	__BITS(15,0)
#define	NQRXC_FRAG_CSUM_MASK	__BITS(31,16)
#define	NQRXC_IP_ID(rsshash)	__SHIFTOUT(rsshash,NRXC_IP_ID_MASK)
#define	NQRXC_FRAG_CSUM(rsshash) __SHIFTOUT(rsshash,NRXC_FRAG_CSUM_MASK)

/* macros for nrxc_misc */
#define	NQRXC_RSS_TYPE_MASK		__BITS(3,0)
#define	NQRXC_PKT_TYPE_ID_MASK		__BITS(11,4)
#define	NQRXC_PKT_TYPE_ETQF_INDEX_MASK	__BITS(11,4)
#define	NQRXC_PKT_TYPE_ETQF_VALID_MASK	__BIT(15)
#define	NQRXC_PKT_TYPE_VLAN_MASK 	__BIT(16)
#define	NQRXC_PKT_TYPE_MASK		__BITS(16,4)
/* __BITS(18,17) is reserved */
#define	NQRXC_HDRLEN_HIGH_MASK		__BITS(20,19)
#define	NQRXC_HDRLEN_LOW_MASK		__BITS(30,21)
#define	NQRXC_SPH_MASK			__BIT(31)

#define	NQRXC_RSS_TYPE(misc)	__SHIFTOUT(misc,NQRXC_RSS_TYPE_MASK)
#define	NQRXC_PKT_TYPE_ID(pkttype) \
		__SHIFTOUT(pkttype,NQRXC_PKT_TYPE_ID_MASK)
#define	NQRXC_PKT_TYPE(misc)	__SHIFTOUT(misc,NQRXC_PKT_TYPE_MASK)
#define	NQRXC_PKT_TYPE_ETQF_INDEX(pkttype) \
		__SHIFTOUT(pkttype,NQRXC_PKT_TYPE_ETQF_INDEX_MASK)
#define	NQRXC_PKT_TYPE_ETQF_VALID NQRXC_PKT_TYPE_ETQF_VALID_MASK
#define	NQRXC_PKT_TYPE_VLAN	NQRXC_PKT_TYPE_VLAN_MASK
#define	NQRXC_HEADER_LEN(misc)	(__SHIFTOUT(misc,NQRXC_HDRLEN_LOW_MASK) \
		| __SHIFTOUT(misc,NQRXC_HDRLEN_HIGH_MASK) << 10)
#define	NQRXC_SPH		NQRXC_SPH_MASK

#define	NQRXC_RSS_TYPE_NONE		0x0 /* No hash computation done. */
#define	NQRXC_RSS_TYPE_TCP_IPV4		0x1
#define	NQRXC_RSS_TYPE_IPV4		0x2
#define	NQRXC_RSS_TYPE_TCP_IPV6		0x3
#define	NQRXC_RSS_TYPE_IPV6_EX		0x4
#define	NQRXC_RSS_TYPE_IPV6		0x5
#define	NQRXC_RSS_TYPE_TCP_IPV6_EX	0x6
#define	NQRXC_RSS_TYPE_UDP_IPV4		0x7
#define	NQRXC_RSS_TYPE_UDP_IPV6		0x8
#define	NQRXC_RSS_TYPE_UDP_IPV6_EX	0x9
/*0xA:0xF is reserved. */

#define	NQRXC_PKT_TYPE_IPV4		__BIT(0)
#define	NQRXC_PKT_TYPE_IPV4E		__BIT(1)
#define	NQRXC_PKT_TYPE_IPV6		__BIT(2)
#define	NQRXC_PKT_TYPE_IPV6E		__BIT(3)
#define	NQRXC_PKT_TYPE_TCP		__BIT(4)
#define	NQRXC_PKT_TYPE_UDP		__BIT(5)
#define	NQRXC_PKT_TYPE_SCTP		__BIT(6)
#define	NQRXC_PKT_TYPE_NFS		__BIT(7)

#define	NQRXC_STATUS_MASK	__BITS(19,0)
#define	NQRXC_ERROR_MASK	__BITS(31,20)
#define	NQRXC_STATUS(err_stat)	__SHIFTOUT(err_stat,NQRXC_STATUS_MASK)
#define	NQRXC_ERROR(err_stat)	__SHIFTOUT(err_stat,NQRXC_ERROR_MASK)

/* 2:0 is reserved. */
#define	NQRXC_ERROR_HB0		__BIT(3) /* Header Buffer Overflow. */
/* 6:4 is reserved. */
/* 8:7 is reserved. */
#define	NQRXC_ERROR_L4E		__BIT(9) /* L4 error indication. */
#define	NQRXC_ERROR_IPE		__BIT(10) /* The same as WRX_ER_IPE. */
#define	NQRXC_ERROR_RXE		__BIT(11) /* The same as WRX_ER_RXE. */
/* XXX Where is WRX_ER_CE, WRX_ER_SE, WRX_ER_SEQ, WRX_ER_CXE error? */

#define	NQRXC_STATUS_DD		__BIT(0) /* The same as WRX_ST_DD. */
#define	NQRXC_STATUS_EOP	__BIT(1) /* The same as WRX_ST_EOP. */
/* 2 is reserved */
#define	NQRXC_STATUS_VP		__BIT(3) /* The same as WRX_ST_VP. */
#define	NQRXC_STATUS_UDPCS	__BIT(4) /* UDP checksum or IP payload checksum. */
					 /* XXX in I210 spec, this bit is the same as WRX_ST_BPDU(is "???" comment) */
#define	NQRXC_STATUS_L4I	__BIT(5) /* L4 integrity check was done. */
#define	NQRXC_STATUS_IPCS	__BIT(6) /* The same as WRX_ST_IPCS. */
#define	NQRXC_STATUS_PIF	__BIT(7) /* The same as WRX_ST_PIF. */
/* 8 is reserved */
#define	NQRXC_STATUS_VEXT	__BIT(9) /* First VLAN is found on a bouble VLAN packet. */
#define	NQRXC_STATUS_UDPV	__BIT(10) /* The packet contains a valid checksum field in a first fragment UDP IPv4 packet. */
#define	NQRXC_STATUS_LLINT	__BIT(11) /* The packet caused an immediate interrupt. */
#define	NQRXC_STATUS_STRIPCRC	__BIT(12) /* Ethernet CRC is stripped. */
/* 14:13 is reserved */
#define	NQRXC_STATUS_TSIP	__BIT(15) /* Timestamp in packet. */
#define	NQRXC_STATUS_TS		__BIT(16) /* Time stamped packet. */
/* 17 is reserved */
#define	NQRXC_STATUS_LB		__BIT(18) /* Sent by a local virtual machine (VM to VM switch indication). */
#define	NQRXC_STATUS_MC		__BIT(19) /* Packet received from Manageability Controller */
					  /* "MBC" in i350 spec */

/*
 * The Wiseman transmit descriptor.
 *
 * The transmit descriptor ring must be aligned to a 4K boundary,
 * and there must be an even multiple of 8 descriptors in the ring.
 */
typedef struct wiseman_tx_fields {
	uint8_t wtxu_status;		/* Tx status */
	uint8_t wtxu_options;		/* options */
	uint16_t wtxu_vlan;		/* VLAN info */
} __packed wiseman_txfields_t;
typedef struct wiseman_txdesc {
	wiseman_addr_t	wtx_addr;	/* buffer address */
	uint32_t	wtx_cmdlen;	/* command and length */
	wiseman_txfields_t wtx_fields;	/* fields; see below */
} __packed wiseman_txdesc_t;

/* Commands for wtx_cmdlen */
#define	WTX_CMD_EOP	__BIT(24)	/* end of packet */
#define	WTX_CMD_IFCS	__BIT(25)	/* insert FCS */
#define	WTX_CMD_RS	__BIT(27)	/* report status */
#define	WTX_CMD_RPS	__BIT(28)	/* report packet sent */
#define	WTX_CMD_DEXT	__BIT(29)	/* descriptor extension */
#define	WTX_CMD_VLE	__BIT(30)	/* VLAN enable */
#define	WTX_CMD_IDE	__BIT(31)	/* interrupt delay enable */

/* Descriptor types (if DEXT is set) */
#define	WTX_DTYP_MASK	__BIT(20)
#define	WTX_DTYP_C	__SHIFTIN(0, WTX_DTYP_MASK)	/* context */
#define	WTX_DTYP_D	__SHIFTIN(1, WTX_DTYP_MASK)	/* data */

/* wtx_fields status bits */
#define	WTX_ST_DD	__BIT(0)	/* descriptor done */
#define	WTX_ST_EC	__BIT(1)	/* excessive collisions */
#define	WTX_ST_LC	__BIT(2)	/* late collision */
#define	WTX_ST_TU	__BIT(3)	/* transmit underrun */

/* wtx_fields option bits for IP/TCP/UDP checksum offload */
#define	WTX_IXSM	__BIT(0)	/* IP checksum offload */
#define	WTX_TXSM	__BIT(1)	/* TCP/UDP checksum offload */

/* Maximum payload per Tx descriptor */
#define	WTX_MAX_LEN	4096

/*
 * The Livengood TCP/IP context descriptor.
 */
struct livengood_tcpip_ctxdesc {
	uint32_t	tcpip_ipcs;	/* IP checksum context */
	uint32_t	tcpip_tucs;	/* TCP/UDP checksum context */
	uint32_t	tcpip_cmdlen;
	uint32_t	tcpip_seg;	/* TCP segmentation context */
};

/* commands for context descriptors */
#define	WTX_TCPIP_CMD_TCP	__BIT(24)	/* 1 = TCP, 0 = UDP */
#define	WTX_TCPIP_CMD_IP	__BIT(25)	/* 1 = IPv4, 0 = IPv6 */
#define	WTX_TCPIP_CMD_TSE	__BIT(26)	/* segmentation context valid */

#define	WTX_TCPIP_IPCSS(x)	((x) << 0)	/* checksum start */
#define	WTX_TCPIP_IPCSO(x)	((x) << 8)	/* checksum value offset */
#define	WTX_TCPIP_IPCSE(x)	((x) << 16)	/* checksum end */

#define	WTX_TCPIP_TUCSS(x)	((x) << 0)	/* checksum start */
#define	WTX_TCPIP_TUCSO(x)	((x) << 8)	/* checksum value offset */
#define	WTX_TCPIP_TUCSE(x)	((x) << 16)	/* checksum end */

#define	WTX_TCPIP_SEG_STATUS(x)	((x) << 0)
#define	WTX_TCPIP_SEG_HDRLEN(x)	((x) << 8)
#define	WTX_TCPIP_SEG_MSS(x)	((x) << 16)

/*
 * PCI config registers used by the Wiseman.
 */
#define	WM_PCI_MMBA	PCI_MAPREG_START
/* registers for FLASH access on ICH8 */
#define	WM_ICH8_FLASH	0x0014

#define	WM_PCI_LTR_CAP_LPT	0xa8

/* XXX Only for PCH_SPT? */
#define	WM_PCI_DESCRING_STATUS	0xe4
#define	DESCRING_STATUS_FLUSH_REQ	__BIT(8)

/*
 * Wiseman Control/Status Registers.
 */
#define	WMREG_CTRL	0x0000	/* Device Control Register */
#define	CTRL_FD		__BIT(0)	/* full duplex */
#define	CTRL_BEM	__BIT(1)	/* big-endian mode */
#define	CTRL_PRIOR	__BIT(2)	/* 0 = receive, 1 = fair */
#define	CTRL_GIO_M_DIS	__BIT(2)	/* disabl PCI master access */
#define	CTRL_LRST	__BIT(3)	/* link reset */
#define	CTRL_ASDE	__BIT(5)	/* auto speed detect enable */
#define	CTRL_SLU	__BIT(6)	/* set link up */
#define	CTRL_ILOS	__BIT(7)	/* invert loss of signal */
#define	CTRL_SPEED(x)	((x) << 8)	/* speed (Livengood) */
#define	CTRL_SPEED_10	CTRL_SPEED(0)
#define	CTRL_SPEED_100	CTRL_SPEED(1)
#define	CTRL_SPEED_1000	CTRL_SPEED(2)
#define	CTRL_SPEED_MASK	CTRL_SPEED(3)
#define	CTRL_FRCSPD	__BIT(11)	/* force speed (Livengood) */
#define	CTRL_FRCFDX	__BIT(12)	/* force full-duplex (Livengood) */
#define	CTRL_D_UD_EN	__BIT(13)	/* Dock/Undock enable */
#define	CTRL_D_UD_POL	__BIT(14)	/* Defined polarity of Dock/Undock indication in SDP[0] */
#define	CTRL_F_PHY_R 	__BIT(15)	/* Reset both PHY ports, through PHYRST_N pin */
#define	CTRL_EXTLINK_EN __BIT(16)	/* enable link status from external LINK_0 and LINK_1 pins */
#define	CTRL_LANPHYPC_OVERRIDE __BIT(16) /* SW control of LANPHYPC */
#define	CTRL_LANPHYPC_VALUE __BIT(17)	/* SW value of LANPHYPC */
#define	CTRL_SWDPINS_SHIFT	18
#define	CTRL_SWDPINS_MASK	0x0f
#define	CTRL_SWDPIN(x)		(1U << (CTRL_SWDPINS_SHIFT + (x)))
#define	CTRL_SWDPIO_SHIFT	22
#define	CTRL_SWDPIO_MASK	0x0f
#define	CTRL_SWDPIO(x)		(1U << (CTRL_SWDPIO_SHIFT + (x)))
#define	CTRL_MEHE	__BIT(19)	/* Memory Error Handling Enable(I217)*/
#define	CTRL_RST	__BIT(26)	/* device reset */
#define	CTRL_RFCE	__BIT(27)	/* Rx flow control enable */
#define	CTRL_TFCE	__BIT(28)	/* Tx flow control enable */
#define	CTRL_VME	__BIT(30)	/* VLAN Mode Enable */
#define	CTRL_PHY_RESET	__BIT(31)	/* PHY reset (Cordova) */

#define	WMREG_CTRL_SHADOW 0x0004	/* Device Control Register (shadow) */

#define	WMREG_STATUS	0x0008	/* Device Status Register */
#define	STATUS_FD	__BIT(0)	/* full duplex */
#define	STATUS_LU	__BIT(1)	/* link up */
#define	STATUS_TCKOK	__BIT(2)	/* Tx clock running */
#define	STATUS_RBCOK	__BIT(3)	/* Rx clock running */
#define	STATUS_FUNCID_SHIFT 2		/* 82546 function ID */
#define	STATUS_FUNCID_MASK  3		/* ... */
#define	STATUS_TXOFF	__BIT(4)	/* Tx paused */
#define	STATUS_TBIMODE	__BIT(5)	/* fiber mode (Livengood) */
#define	STATUS_SPEED	__BITS(7, 6)	/* speed indication */
#define	STATUS_SPEED_10	  0
#define	STATUS_SPEED_100  1
#define	STATUS_SPEED_1000 2
#define	STATUS_ASDV(x)	((x) << 8)	/* auto speed det. val. (Livengood) */
#define	STATUS_LAN_INIT_DONE __BIT(9)	/* Lan Init Completion by NVM */
#define	STATUS_MTXCKOK	__BIT(10)	/* MTXD clock running */
#define	STATUS_PHYRA	__BIT(10)	/* PHY Reset Asserted (PCH) */
#define	STATUS_PCI66	__BIT(11)	/* 66MHz bus (Livengood) */
#define	STATUS_BUS64	__BIT(12)	/* 64-bit bus (Livengood) */
#define	STATUS_2P5_SKU	__BIT(12)	/* Value of the 2.5GBE SKU strap */
#define	STATUS_PCIX_MODE __BIT(13)	/* PCIX mode (Cordova) */
#define	STATUS_2P5_SKU_OVER __BIT(13)	/* Value of the 2.5GBE SKU override */
#define	STATUS_PCIXSPD(x) ((x) << 14)	/* PCIX speed indication (Cordova) */
#define	STATUS_PCIXSPD_50_66   STATUS_PCIXSPD(0)
#define	STATUS_PCIXSPD_66_100  STATUS_PCIXSPD(1)
#define	STATUS_PCIXSPD_100_133 STATUS_PCIXSPD(2)
#define	STATUS_PCIXSPD_MASK    STATUS_PCIXSPD(3)
#define	STATUS_GIO_M_ENA __BIT(19)	/* GIO master enable */
#define	STATUS_DEV_RST_SET __BIT(20)	/* Device Reset Set */

/* Strapping Option Register (PCH_SPT and newer) */
#define	WMREG_STRAP	0x000c
#define	STRAP_NVMSIZE	__BITS(1, 6)
#define	STRAP_FREQ	__BITS(12, 13)
#define	STRAP_SMBUSADDR	__BITS(17, 23)

#define	WMREG_EECD	0x0010	/* EEPROM Control Register */
#define	EECD_SK		__BIT(0)	/* clock */
#define	EECD_CS		__BIT(1)	/* chip select */
#define	EECD_DI		__BIT(2)	/* data in */
#define	EECD_DO		__BIT(3)	/* data out */
#define	EECD_FWE(x)	((x) << 4)	/* flash write enable control */
#define	EECD_FWE_DISABLED EECD_FWE(1)
#define	EECD_FWE_ENABLED  EECD_FWE(2)
#define	EECD_EE_REQ	__BIT(6)	/* (shared) EEPROM request */
#define	EECD_EE_GNT	__BIT(7)	/* (shared) EEPROM grant */
#define	EECD_EE_PRES	__BIT(8)	/* EEPROM present */
#define	EECD_EE_SIZE	__BIT(9)	/* EEPROM size
					   (0 = 64 word, 1 = 256 word) */
#define	EECD_EE_AUTORD	__BIT(9)	/* auto read done */
#define	EECD_EE_ABITS	__BIT(10)	/* EEPROM address bits
					   (based on type) */
#define	EECD_EE_SIZE_EX_MASK __BITS(14,11) /* EEPROM size for new devices */
#define	EECD_EE_TYPE	__BIT(13)	/* EEPROM type
					   (0 = Microwire, 1 = SPI) */
#define	EECD_SEC1VAL	__BIT(22)	/* Sector One Valid */
#define	EECD_SEC1VAL_VALMASK (EECD_EE_AUTORD | EECD_EE_PRES) /* Valid Mask */

#define	WMREG_FEXTNVM6	0x0010	/* Future Extended NVM 6 */
#define	FEXTNVM6_REQ_PLL_CLK	__BIT(8)
#define	FEXTNVM6_ENABLE_K1_ENTRY_CONDITION __BIT(9)
#define	FEXTNVM6_K1_OFF_ENABLE	__BIT(31)

#define	WMREG_EERD	0x0014	/* EEPROM read */
#define	EERD_DONE	0x02    /* done bit */
#define	EERD_START	0x01	/* First bit for telling part to start operation */
#define	EERD_ADDR_SHIFT	2	/* Shift to the address bits */
#define	EERD_DATA_SHIFT	16	/* Offset to data in EEPROM read/write registers */

#define	WMREG_CTRL_EXT	0x0018	/* Extended Device Control Register */
#define	CTRL_EXT_NSICR		__BIT(0) /* Non Interrupt clear on read */
#define	CTRL_EXT_GPI_EN(x)	(1U << (x)) /* gpin interrupt enable */
#define	CTRL_EXT_NVMVS		__BITS(0, 1) /* NVM valid sector */
#define	CTRL_EXT_LPCD		__BIT(2) /* LCD Power Cycle Done */
#define	CTRL_EXT_SWDPINS_SHIFT	4
#define	CTRL_EXT_SWDPINS_MASK	0x0d
/* The bit order of the SW Definable pin is not 6543 but 3654! */
#define	CTRL_EXT_SWDPIN(x)	(1U << (CTRL_EXT_SWDPINS_SHIFT \
		+ ((x) == 3 ? 3 : ((x) - 4))))
#define	CTRL_EXT_SWDPIO_SHIFT	8
#define	CTRL_EXT_SWDPIO_MASK	0x0d
#define	CTRL_EXT_SWDPIO(x)	(1U << (CTRL_EXT_SWDPIO_SHIFT \
		+ ((x) == 3 ? 3 : ((x) - 4))))
#define	CTRL_EXT_FORCE_SMBUS	__BIT(11)  /* Force SMBus mode */
#define	CTRL_EXT_ASDCHK		__BIT(12) /* ASD check */
#define	CTRL_EXT_EE_RST		__BIT(13) /* EEPROM reset */
#define	CTRL_EXT_IPS		__BIT(14) /* invert power state bit 0 */
#define	CTRL_EXT_SPD_BYPS	__BIT(15) /* speed select bypass */
#define	CTRL_EXT_IPS1		__BIT(16) /* invert power state bit 1 */
#define	CTRL_EXT_RO_DIS		__BIT(17) /* relaxed ordering disabled */
#define	CTRL_EXT_SDLPE		__BIT(18) /* SerDes Low Power Enable */
#define	CTRL_EXT_DMA_DYN_CLK	__BIT(19) /* DMA Dynamic Gating Enable */
#define	CTRL_EXT_PHYPDEN	__BIT(20)
#define	CTRL_EXT_LINK_MODE_MASK		0x00c00000
#define	CTRL_EXT_LINK_MODE_GMII		0x00000000
#define	CTRL_EXT_LINK_MODE_KMRN		0x00000000
#define	CTRL_EXT_LINK_MODE_1000KX	0x00400000
#define	CTRL_EXT_LINK_MODE_SGMII	0x00800000
#define	CTRL_EXT_LINK_MODE_PCIX_SERDES	0x00800000
#define	CTRL_EXT_LINK_MODE_TBI		0x00c00000
#define	CTRL_EXT_LINK_MODE_PCIE_SERDES	0x00c00000
#define	CTRL_EXT_EIAME		__BIT(24) /* Extended Interrupt Auto Mask En */
#define	CTRL_EXT_I2C_ENA	0x02000000  /* I2C enable */
#define	CTRL_EXT_DRV_LOAD	0x10000000
#define	CTRL_EXT_PBA		__BIT(31) /* PBA Support */

#define	WMREG_MDIC	0x0020	/* MDI Control Register */
#define	MDIC_DATA(x)	((x) & 0xffff)
#define	MDIC_REGADD(x)	((x) << 16)
#define	MDIC_PHY_SHIFT	21
#define	MDIC_PHY_MASK	__BITS(25, 21)
#define	MDIC_PHYADD(x)	((x) << 21)

#define	MDIC_OP_RW_MASK	__BITS(27, 26)
#define	MDIC_OP_WRITE	__SHIFTIN(1, MDIC_OP_RW_MASK)
#define	MDIC_OP_READ	__SHIFTIN(2, MDIC_OP_RW_MASK)
#define	MDIC_READY	__BIT(28)
#define	MDIC_I		__BIT(29)	/* interrupt on MDI complete */
#define	MDIC_E		__BIT(30)	/* MDI error */
#define	MDIC_DEST	__BIT(31)	/* Destination */

#define	WMREG_SCTL	0x0024	/* SerDes Control - RW */
/*
 * These 4 macros are also used for other 8bit control registers on the
 * 82575
 */
#define	SCTL_CTL_READY  __BIT(31)
#define	SCTL_CTL_DATA_MASK 0x000000ff
#define	SCTL_CTL_ADDR_SHIFT 8
#define	SCTL_CTL_POLL_TIMEOUT 640
#define	SCTL_DISABLE_SERDES_LOOPBACK 0x0400

#define	WMREG_FEXTNVM4	0x0024	/* Future Extended NVM 4 - RW */
#define	FEXTNVM4_BEACON_DURATION	__BITS(2, 0)
#define	FEXTNVM4_BEACON_DURATION_8US	0x7
#define	FEXTNVM4_BEACON_DURATION_16US	0x3

#define	WMREG_FCAL	0x0028	/* Flow Control Address Low */
#define	FCAL_CONST	0x00c28001	/* Flow Control MAC addr low */

#define	WMREG_FEXTNVM	0x0028	/* Future Extended NVM register */
#define	FEXTNVM_SW_CONFIG	__BIT(0)  /* SW PHY Config En (ICH8 B0) */
#define	FEXTNVM_SW_CONFIG_ICH8M	__BIT(27) /* SW PHY Config En (>= ICH8 B1) */

#define	WMREG_FCAH	0x002c	/* Flow Control Address High */
#define	FCAH_CONST	0x00000100	/* Flow Control MAC addr high */

#define	WMREG_FCT	0x0030	/* Flow Control Type */

#define	WMREG_KUMCTRLSTA 0x0034	/* MAC-PHY interface - RW */
#define	KUMCTRLSTA_MASK			0x0000ffff
#define	KUMCTRLSTA_OFFSET		0x001f0000
#define	KUMCTRLSTA_OFFSET_SHIFT		16
#define	KUMCTRLSTA_REN			0x00200000

#define	KUMCTRLSTA_OFFSET_FIFO_CTRL	0x00000000
#define	KUMCTRLSTA_OFFSET_CTRL		0x00000001
#define	KUMCTRLSTA_OFFSET_INB_CTRL	0x00000002
#define	KUMCTRLSTA_OFFSET_DIAG		0x00000003
#define	KUMCTRLSTA_OFFSET_TIMEOUTS	0x00000004
#define	KUMCTRLSTA_OFFSET_K1_CONFIG	0x00000007
#define	KUMCTRLSTA_OFFSET_INB_PARAM	0x00000009
#define	KUMCTRLSTA_OFFSET_HD_CTRL	0x00000010
#define	KUMCTRLSTA_OFFSET_M2P_SERDES	0x0000001e
#define	KUMCTRLSTA_OFFSET_M2P_MODES	0x0000001f

/* FIFO Control */
#define	KUMCTRLSTA_FIFO_CTRL_RX_BYPASS	0x0008
#define	KUMCTRLSTA_FIFO_CTRL_TX_BYPASS	0x0800

/* In-Band Control */
#define	KUMCTRLSTA_INB_CTRL_LINK_TMOUT_DFLT 0x0500
#define	KUMCTRLSTA_INB_CTRL_DIS_PADDING	0x0010

/* Diag */
#define	KUMCTRLSTA_DIAG_NELPBK	0x1000

/* K1 Config */
#define	KUMCTRLSTA_K1_ENABLE	0x0002

/* Half-Duplex Control */
#define	KUMCTRLSTA_HD_CTRL_10_100_DEFAULT 0x0004
#define	KUMCTRLSTA_HD_CTRL_1000_DEFAULT	0x0000

/* M2P Modes */
#define	KUMCTRLSTA_OPMODE_MASK	0x000c
#define	KUMCTRLSTA_OPMODE_INBAND_MDIO 0x0004

#define	WMREG_CONNSW	0x0034	/* Copper/Fiber Switch Control (>= 82575) */
#define	CONNSW_AUTOSENSE_EN	__BIT(0)	/* Auto Sense Enable */
#define	CONNSW_AUTOSENSE_CONF	__BIT(1)	/* Auto Sense Config Mode */
#define	CONNSW_ENRGSRC		__BIT(2)	/* SerDes Energy Detect Src */
#define	CONNSW_SERDESD		__BIT(9)	/* SerDes Signal Detect Ind. */
#define	CONNSW_PHYSD		__BIT(10)	/* PHY Signal Detect Ind. */
#define	CONNSW_PHY_PDN		__BIT(11)	/* Internal PHY in powerdown */

#define	WMREG_VET	0x0038	/* VLAN Ethertype */
#define	WMREG_MDPHYA	0x003c	/* PHY address - RW */

#define	WMREG_FEXTNVM3	0x003c	/* Future Extended NVM 3 */
#define	FEXTNVM3_PHY_CFG_COUNTER_MASK	__BITS(27, 26)
#define	FEXTNVM3_PHY_CFG_COUNTER_50MS	__BIT(27)

#define	WMREG_RAL(x)		(0x0040	+ ((x) * 8)) /* Receive Address List */
#define	WMREG_RAH(x)		(WMREG_RAL(x) + 4)
#define	WMREG_CORDOVA_RAL(x)	(((x) <= 15) ? (0x5400 + ((x) * 8)) : \
	    (0x54e0 + (((x) - 16) * 8)))
#define	WMREG_CORDOVA_RAH(x)	(WMREG_CORDOVA_RAL(x) + 4)
#define	WMREG_SHRAL(x)		(0x5438 + ((x) * 8))
#define	WMREG_SHRAH(x)		(WMREG_PCH_LPT_SHRAL(x) + 4)
#define	WMREG_PCH_LPT_SHRAL(x)	(0x5408 + ((x) * 8))
#define	WMREG_PCH_LPT_SHRAH(x)	(WMREG_PCH_LPT_SHRAL(x) + 4)
#define	WMREG_RAL_LO(b, x) ((b) + ((x) << 3))
#define	WMREG_RAL_HI(b, x) (WMREG_RAL_LO(b, x) + 4)
	/*
	 * Receive Address List: The LO part is the low-order 32-bits
	 * of the MAC address.  The HI part is the high-order 16-bits
	 * along with a few control bits.
	 */
#define	RAL_AS(x)	((x) << 16)	/* address select */
#define	RAL_AS_DEST	RAL_AS(0)	/* (cordova?) */
#define	RAL_AS_SOURCE	RAL_AS(1)	/* (cordova?) */
#define	RAL_RDR1	__BIT(30)	/* put packet in alt. rx ring */
#define	RAL_AV		__BIT(31)	/* entry is valid */

#define	WM_RAL_TABSIZE		15	/* RAL size for old devices */
#define	WM_RAL_TABSIZE_ICH8	7	/* RAL size for ICH* and PCH* */
#define	WM_RAL_TABSIZE_PCH2	5	/* RAL size for PCH2 */
#define	WM_RAL_TABSIZE_PCH_LPT	12	/* RAL size for PCH_LPT */
#define	WM_RAL_TABSIZE_82575	16	/* RAL size for 82575 */
#define	WM_RAL_TABSIZE_82576	24	/* RAL size for 82576 and 82580 */
#define	WM_RAL_TABSIZE_I350	32	/* RAL size for I350 */

#define	WMREG_ICR	0x00c0	/* Interrupt Cause Register */
#define	ICR_TXDW	__BIT(0)	/* Tx desc written back */
#define	ICR_TXQE	__BIT(1)	/* Tx queue empty */
#define	ICR_LSC		__BIT(2)	/* link status change */
#define	ICR_RXSEQ	__BIT(3)	/* receive sequence error */
#define	ICR_RXDMT0	__BIT(4)	/* Rx ring 0 nearly empty */
#define	ICR_RXO		__BIT(6)	/* Rx overrun */
#define	ICR_RXT0	__BIT(7)	/* Rx ring 0 timer */
#define	ICR_MDAC	__BIT(9)	/* MDIO access complete */
#define	ICR_RXCFG	__BIT(10)	/* Receiving /C/ */
#define	ICR_GPI(x)	__BIT(11+(x))	/* general purpose interrupts */
#define	ICR_RXQ(x)	__BIT(20+(x))	/* 82574: Rx queue x interrupt x=0,1 */
#define	ICR_TXQ(x)	__BIT(22+(x))	/* 82574: Tx queue x interrupt x=0,1 */
#define	ICR_OTHER	__BIT(24)	/* 82574: Other interrupt */
#define	ICR_INT		__BIT(31)	/* device generated an interrupt */

#define	WMREG_ITR	0x00c4	/* Interrupt Throttling Register */
#define	ITR_IVAL_MASK	0xffff		/* Interval mask */
#define	ITR_IVAL_SHIFT	0		/* Interval shift */

#define	WMREG_ICS	0x00c8	/* Interrupt Cause Set Register */
	/* See ICR bits. */

#define	WMREG_IMS	0x00d0	/* Interrupt Mask Set Register */
	/* See ICR bits. */

#define	WMREG_IMC	0x00d8	/* Interrupt Mask Clear Register */
	/* See ICR bits. */

#define	WMREG_EIAC_82574 0x00dc	/* Interrupt Auto Clear Register */
#define	WMREG_EIAC_82574_MSIX_MASK	(ICR_RXQ(0) | ICR_RXQ(1)	\
	    | ICR_TXQ(0) | ICR_TXQ(1) | ICR_OTHER)

#define	WMREG_FEXTNVM7	0x00e4  /* Future Extended NVM 7 */
#define	FEXTNVM7_SIDE_CLK_UNGATE __BIT(2)
#define	FEXTNVM7_DIS_SMB_PERST	__BIT(5)
#define	FEXTNVM7_DIS_PB_READ	__BIT(18)

#define	WMREG_IVAR	0x00e4  /* Interrupt Vector Allocation Register */
#define	WMREG_IVAR0	0x01700 /* Interrupt Vector Allocation */
#define	IVAR_ALLOC_MASK  __BITS(0, 6)	/* Bit 5 and 6 are reserved */
#define	IVAR_VALID       __BIT(7)
/* IVAR definitions for 82580 and newer */
#define	WMREG_IVAR_Q(x)	(WMREG_IVAR0 + ((x) / 2) * 4)
#define	IVAR_TX_MASK_Q(x) (0x000000ffUL << (((x) % 2) == 0 ? 8 : 24))
#define	IVAR_RX_MASK_Q(x) (0x000000ffUL << (((x) % 2) == 0 ? 0 : 16))
/* IVAR definitions for 82576 */
#define	WMREG_IVAR_Q_82576(x)	(WMREG_IVAR0 + ((x) & 0x7) * 4)
#define	IVAR_TX_MASK_Q_82576(x) (0x000000ffUL << (((x) / 8) == 0 ? 8 : 24))
#define	IVAR_RX_MASK_Q_82576(x) (0x000000ffUL << (((x) / 8) == 0 ? 0 : 16))
/* IVAR definitions for 82574 */
#define	IVAR_ALLOC_MASK_82574	__BITS(0, 2)
#define	IVAR_VALID_82574	__BIT(3)
#define	IVAR_TX_MASK_Q_82574(x) (0x0000000fUL << ((x) == 0 ? 8 : 12))
#define	IVAR_RX_MASK_Q_82574(x) (0x0000000fUL << ((x) == 0 ? 0 : 4))
#define	IVAR_OTHER_MASK		__BITS(16, 19)
#define	IVAR_INT_ON_ALL_WB	__BIT(31)

#define	WMREG_IVAR_MISC	0x01740 /* IVAR for other causes */
#define	IVAR_MISC_TCPTIMER __BITS(0, 7)
#define	IVAR_MISC_OTHER	__BITS(8, 15)

#define	WMREG_SVCR	0x00f0
#define	SVCR_OFF_EN		__BIT(0)
#define	SVCR_OFF_MASKINT	__BIT(12)

#define	WMREG_SVT	0x00f4
#define	SVT_OFF_HWM		__BITS(4, 0)

#define	WMREG_LTRV	0x00f8	/* Latency Tolerance Reporting */
#define	LTRV_VALUE	__BITS(9, 0)
#define	LTRV_SCALE	__BITS(12, 10)
#define	LTRV_SCALE_MAX	5
#define	LTRV_SNOOP_REQ	__BIT(15)
#define	LTRV_SEND	__BIT(30)
#define	LTRV_NONSNOOP	__BITS(31, 16)
#define	LTRV_NONSNOOP_REQ __BIT(31)

#define	WMREG_RCTL	0x0100	/* Receive Control */
#define	RCTL_EN		__BIT(1)	/* receiver enable */
#define	RCTL_SBP	__BIT(2)	/* store bad packets */
#define	RCTL_UPE	__BIT(3)	/* unicast promisc. enable */
#define	RCTL_MPE	__BIT(4)	/* multicast promisc. enable */
#define	RCTL_LPE	__BIT(5)	/* large packet enable */
#define	RCTL_LBM(x)	((x) << 6)	/* loopback mode */
#define	RCTL_LBM_NONE	RCTL_LBM(0)
#define	RCTL_LBM_PHY	RCTL_LBM(3)
#define	RCTL_RDMTS(x)	((x) << 8)	/* receive desc. min thresh size */
#define	RCTL_RDMTS_1_2	RCTL_RDMTS(0)
#define	RCTL_RDMTS_1_4	RCTL_RDMTS(1)
#define	RCTL_RDMTS_1_8	RCTL_RDMTS(2)
#define	RCTL_RDMTS_MASK	RCTL_RDMTS(3)
#define	RCTL_DTYP_MASK	__BITS(11,10)	/* descriptor type. 82574 only */
#define	RCTL_DTYP(x)	__SHIFTIN(x,RCTL_DTYP_MASK)
#define	RCTL_DTYP_ONEBUF RCTL_DTYP(0)	/* use one buffer(not split header). */
#define	RCTL_DTYP_SPH	RCTL_DTYP(1)	/* split header buffer. */
					/* RCTL_DTYP(2) and RCTL_DTYP(3) are reserved. */
#define	RCTL_MO		__BITS(13, 12)	/* multicast offset */
#define	RCTL_BAM	__BIT(15)	/* broadcast accept mode */
#define	RCTL_RDMTS_HEX	__BIT(16)
#define	RCTL_2k		(0 << 16)	/* 2k Rx buffers */
#define	RCTL_1k		(1 << 16)	/* 1k Rx buffers */
#define	RCTL_512	(2 << 16)	/* 512 byte Rx buffers */
#define	RCTL_256	(3 << 16)	/* 256 byte Rx buffers */
#define	RCTL_BSEX_16k	(1 << 16)	/* 16k Rx buffers (BSEX) */
#define	RCTL_BSEX_8k	(2 << 16)	/* 8k Rx buffers (BSEX) */
#define	RCTL_BSEX_4k	(3 << 16)	/* 4k Rx buffers (BSEX) */
#define	RCTL_DPF	__BIT(22)	/* discard pause frames */
#define	RCTL_PMCF	__BIT(23)	/* pass MAC control frames */
#define	RCTL_BSEX	__BIT(25)	/* buffer size extension (Livengood) */
#define	RCTL_SECRC	__BIT(26)	/* strip Ethernet CRC */

#define	WMREG_OLD_RDTR0	0x0108	/* Receive Delay Timer (ring 0) */
#define	WMREG_RDTR	0x2820
#define	RDTR_FPD	__BIT(31)	/* flush partial descriptor */

#define	WMREG_LTRC	0x01a0	/* Latency Tolerance Reportiong Control */

#define	WMREG_OLD_RDBAL0 0x0110	/* Receive Descriptor Base Low (ring 0) */
#define	WMREG_RDBAL(x) \
	((x) < 4 ? (0x02800 + ((x) * 0x100)) :	\
	    (0x0c000 + ((x) * 0x40)))

#define	WMREG_OLD_RDBAH0 0x0114	/* Receive Descriptor Base High (ring 0) */
#define	WMREG_RDBAH(x) \
	((x) < 4 ? (0x02804 + ((x) * 0x100)) :	\
	    (0x0c004 + ((x) * 0x40)))

#define	WMREG_OLD_RDLEN0 0x0118	/* Receive Descriptor Length (ring 0) */
#define	WMREG_RDLEN(x) \
	((x) < 4 ? (0x02808 + ((x) * 0x100)) :  \
	    (0x0c008 + ((x) * 0x40)))

#define	WMREG_SRRCTL(x) \
	((x) < 4 ? (0x0280c + ((x) * 0x100)) :	\
	    (0x0c00c + ((x) * 0x40)))	/* additional recv control used in 82575 ... */
#define	SRRCTL_BSIZEPKT_MASK		0x0000007f
#define	SRRCTL_BSIZEPKT_SHIFT		10	/* Shift _right_ */
#define	SRRCTL_BSIZEHDRSIZE_MASK	0x00000f00
#define	SRRCTL_BSIZEHDRSIZE_SHIFT	2	/* Shift _left_ */
#define	SRRCTL_DESCTYPE_LEGACY		0x00000000
#define	SRRCTL_DESCTYPE_ADV_ONEBUF	(1U << 25)
#define	SRRCTL_DESCTYPE_HDR_SPLIT	(2U << 25)
#define	SRRCTL_DESCTYPE_HDR_REPLICATION	(3U << 25)
#define	SRRCTL_DESCTYPE_HDR_REPLICATION_LARGE_PKT (4U << 25)
#define	SRRCTL_DESCTYPE_HDR_SPLIT_ALWAYS (5U << 25) /* 82575 only */
#define	SRRCTL_DESCTYPE_MASK		(7U << 25)
#define	SRRCTL_DROP_EN			0x80000000

#define	WMREG_OLD_RDH0	0x0120	/* Receive Descriptor Head (ring 0) */
#define	WMREG_RDH(x) \
	((x) < 4 ? (0x02810 + ((x) * 0x100)) :  \
	    (0x0c010 + ((x) * 0x40)))

#define	WMREG_OLD_RDT0	0x0128	/* Receive Descriptor Tail (ring 0) */
#define	WMREG_RDT(x) \
	((x) < 4 ? (0x02818 + ((x) * 0x100)) :	\
	    (0x0c018 + ((x) * 0x40)))

#define	WMREG_RXDCTL(x) \
	((x) < 4 ? (0x02828 + ((x) * 0x100)) :	\
	    (0x0c028 + ((x) * 0x40)))	/* Receive Descriptor Control */
#define	RXDCTL_PTHRESH(x) ((x) << 0)	/* prefetch threshold */
#define	RXDCTL_HTHRESH(x) ((x) << 8)	/* host threshold */
#define	RXDCTL_WTHRESH(x) ((x) << 16)	/* write back threshold */
#define	RXDCTL_GRAN	__BIT(24)	/* 0 = cacheline, 1 = descriptor */
/* flags used starting with 82575 ... */
#define	RXDCTL_QUEUE_ENABLE  0x02000000 /* Enable specific Tx Queue */
#define	RXDCTL_SWFLSH        0x04000000 /* Rx Desc. write-back flushing */

#define	WMREG_OLD_RDTR1	0x0130	/* Receive Delay Timer (ring 1) */
#define	WMREG_OLD_RDBA1_LO 0x0138 /* Receive Descriptor Base Low (ring 1) */
#define	WMREG_OLD_RDBA1_HI 0x013c /* Receive Descriptor Base High (ring 1) */
#define	WMREG_OLD_RDLEN1 0x0140	/* Receive Drscriptor Length (ring 1) */
#define	WMREG_OLD_RDH1	0x0148
#define	WMREG_OLD_RDT1	0x0150
#define	WMREG_OLD_FCRTH 0x0160	/* Flow Control Rx Threshold Hi (OLD) */
#define	WMREG_FCRTH	0x2168	/* Flow Control Rx Threhsold Hi */
#define	FCRTH_DFLT	0x00008000

#define	WMREG_OLD_FCRTL 0x0168	/* Flow Control Rx Threshold Lo (OLD) */
#define	WMREG_FCRTL	0x2160	/* Flow Control Rx Threshold Lo */
#define	FCRTL_DFLT	0x00004000
#define	FCRTL_XONE	0x80000000	/* Enable XON frame transmission */

#define	WMREG_FCTTV	0x0170	/* Flow Control Transmit Timer Value */
#define	FCTTV_DFLT	0x00000600

#define	WMREG_TXCW	0x0178	/* Transmit Configuration Word (TBI mode) */
	/* See MII ANAR_X bits. */
#define	TXCW_FD		__BIT(5)	/* Full Duplex */
#define	TXCW_HD		__BIT(6)	/* Half Duplex */
#define	TXCW_SYM_PAUSE	__BIT(7)	/* sym pause request */
#define	TXCW_ASYM_PAUSE	__BIT(8)	/* asym pause request */
#define	TXCW_TxConfig	__BIT(30)	/* Tx Config */
#define	TXCW_ANE	__BIT(31)	/* Autonegotiate */

#define	WMREG_RXCW	0x0180	/* Receive Configuration Word (TBI mode) */
	/* See MII ANLPAR_X bits. */
#define	RXCW_NC		__BIT(26)	/* no carrier */
#define	RXCW_IV		__BIT(27)	/* config invalid */
#define	RXCW_CC		__BIT(28)	/* config change */
#define	RXCW_C		__BIT(29)	/* /C/ reception */
#define	RXCW_SYNCH	__BIT(30)	/* synchronized */
#define	RXCW_ANC	__BIT(31)	/* autonegotiation complete */

#define	WMREG_MTA	0x0200	/* Multicast Table Array */
#define	WMREG_CORDOVA_MTA 0x5200

#define	WMREG_TCTL	0x0400	/* Transmit Control Register */
#define	TCTL_EN		__BIT(1)	/* transmitter enable */
#define	TCTL_PSP	__BIT(3)	/* pad short packets */
#define	TCTL_CT(x)	(((x) & 0xff) << 4)   /* 4:11 - collision threshold */
#define	TCTL_COLD(x)	(((x) & 0x3ff) << 12) /* 12:21 - collision distance */
#define	TCTL_SWXOFF	__BIT(22)	/* software XOFF */
#define	TCTL_RTLC	__BIT(24)	/* retransmit on late collision */
#define	TCTL_NRTU	__BIT(25)	/* no retransmit on underrun */
#define	TCTL_MULR	__BIT(28)	/* multiple request */

#define	TX_COLLISION_THRESHOLD		15
#define	TX_COLLISION_DISTANCE_HDX	512
#define	TX_COLLISION_DISTANCE_FDX	64

#define	WMREG_TCTL_EXT	0x0404	/* Transmit Control Register */
#define	TCTL_EXT_BST_MASK	0x000003ff /* Backoff Slot Time */
#define	TCTL_EXT_GCEX_MASK	0x000ffc00 /* Gigabit Carry Extend Padding */

#define	DEFAULT_80003ES2LAN_TCTL_EXT_GCEX 0x00010000

#define	WMREG_TIPG	0x0410	/* Transmit IPG Register */
#define	TIPG_IPGT_MASK	__BITS(0, 9)	/* IPG transmit time MASK */
#define	TIPG_IPGT(x)	__SHIFTIN((x), TIPG_IPGT_MASK) /* IPG transmit time */
#define	TIPG_IPGR1(x)	((x) << 10)	/* IPG receive time 1 */
#define	TIPG_IPGR2(x)	((x) << 20)	/* IPG receive time 2 */
#define	TIPG_WM_DFLT	(TIPG_IPGT(0x0a) | TIPG_IPGR1(0x02) | TIPG_IPGR2(0x0a))
#define	TIPG_LG_DFLT	(TIPG_IPGT(0x06) | TIPG_IPGR1(0x08) | TIPG_IPGR2(0x06))
#define	TIPG_1000T_DFLT	(TIPG_IPGT(0x08) | TIPG_IPGR1(0x08) | TIPG_IPGR2(0x06))
#define	TIPG_1000T_80003_DFLT \
    (TIPG_IPGT(0x08) | TIPG_IPGR1(0x02) | TIPG_IPGR2(0x07))
#define	TIPG_10_100_80003_DFLT \
    (TIPG_IPGT(0x09) | TIPG_IPGR1(0x02) | TIPG_IPGR2(0x07))

#define	WMREG_TQC	0x0418

#define	WMREG_OLD_TDBAL	0x0420	/* Transmit Descriptor Base Lo */
#define	WMREG_TDBAL(x) \
	((x) < 4 ? (0x03800 + ((x) * 0x100)) :	\
	    (0x0e000 + ((x) * 0x40)))

#define	WMREG_OLD_TDBAH	0x0424	/* Transmit Descriptor Base Hi */
#define	WMREG_TDBAH(x)\
	((x) < 4 ? (0x03804 + ((x) * 0x100)) :	\
	    (0x0e004 + ((x) * 0x40)))

#define	WMREG_OLD_TDLEN	0x0428	/* Transmit Descriptor Length */
#define	WMREG_TDLEN(x) \
	((x) < 4 ? (0x03808 + ((x) * 0x100)) :	\
	    (0x0e008 + ((x) * 0x40)))

#define	WMREG_OLD_TDH	0x0430	/* Transmit Descriptor Head */
#define	WMREG_TDH(x) \
	((x) < 4 ? (0x03810 + ((x) * 0x100)) :	\
	    (0x0e010 + ((x) * 0x40)))

#define	WMREG_OLD_TDT	0x0438	/* Transmit Descriptor Tail */
#define	WMREG_TDT(x) \
	((x) < 4 ? (0x03818 + ((x) * 0x100)) :	\
	    (0x0e018 + ((x) * 0x40)))

#define	WMREG_OLD_TIDV	0x0440	/* Transmit Delay Interrupt Value */
#define	WMREG_TIDV	0x3820

#define	WMREG_AIT	0x0458	/* Adaptive IFS Throttle */
#define	WMREG_VFTA	0x0600

#define	WMREG_LEDCTL	0x0e00	/* LED Control - RW */

#define	WMREG_MDICNFG	0x0e04	/* MDC/MDIO Configuration Register */
#define	MDICNFG_PHY_SHIFT	21
#define	MDICNFG_PHY_MASK	__BITS(25, 21)
#define	MDICNFG_COM_MDIO	__BIT(30)
#define	MDICNFG_DEST		__BIT(31)

#define	WM_MC_TABSIZE	128
#define	WM_ICH8_MC_TABSIZE 32
#define	WM_VLAN_TABSIZE	128

#define	WMREG_PHPM	0x0e14	/* PHY Power Management */
#define	PHPM_SPD_EN		__BIT(0)	/* Smart Power Down */
#define	PHPM_D0A_LPLU		__BIT(1)	/* D0 Low Power Link Up */
#define	PHPM_NOND0A_LPLU	__BIT(2)	/* Non-D0a LPLU */
#define	PHPM_NOND0A_GBE_DIS	__BIT(3)	/* Disable 1G in non-D0a */
#define	PHPM_GO_LINK_D		__BIT(5)	/* Go Link Disconnect */

#define	WMREG_EEER	0x0e30	/* Energy Efficiency Ethernet "EEE" */
#define	EEER_TX_LPI_EN		0x00010000 /* EEER Tx LPI Enable */
#define	EEER_RX_LPI_EN		0x00020000 /* EEER Rx LPI Enable */
#define	EEER_LPI_FC		0x00040000 /* EEER Ena on Flow Cntrl */
#define	EEER_EEER_NEG		0x20000000 /* EEER capability nego */
#define	EEER_EEER_RX_LPI_STATUS	0x40000000 /* EEER Rx in LPI state */
#define	EEER_EEER_TX_LPI_STATUS	0x80000000 /* EEER Tx in LPI state */
#define	WMREG_EEE_SU	0x0e34	/* EEE Setup */
#define	WMREG_IPCNFG	0x0e38	/* Internal PHY Configuration */
#define	IPCNFG_10BASE_TE	0x00000002 /* IPCNFG 10BASE-Te low power op. */
#define	IPCNFG_EEE_100M_AN	0x00000004 /* IPCNFG EEE Ena 100M AN */
#define	IPCNFG_EEE_1G_AN	0x00000008 /* IPCNFG EEE Ena 1G AN */

#define	WMREG_EXTCNFCTR	0x0f00  /* Extended Configuration Control */
#define	EXTCNFCTR_PCIE_WRITE_ENABLE	0x00000001
#define	EXTCNFCTR_OEM_WRITE_ENABLE	0x00000008
#define	EXTCNFCTR_MDIO_SW_OWNERSHIP	0x00000020
#define	EXTCNFCTR_MDIO_HW_OWNERSHIP	0x00000040
#define	EXTCNFCTR_GATE_PHY_CFG		0x00000080
#define	EXTCNFCTR_EXT_CNF_POINTER	0x0fff0000

#define	WMREG_EXTCNFSIZE 0x0f08  /* Extended Configuration Size */
#define	EXTCNFSIZE_LENGTH	__BITS(23, 16)

#define	WMREG_PHY_CTRL	0x0f10	/* PHY control */
#define	PHY_CTRL_SPD_EN		(1 << 0)
#define	PHY_CTRL_D0A_LPLU	(1 << 1)
#define	PHY_CTRL_NOND0A_LPLU	(1 << 2)
#define	PHY_CTRL_NOND0A_GBE_DIS	(1 << 3)
#define	PHY_CTRL_GBE_DIS	(1 << 6)

#define	WMREG_PCIEANACFG 0x0f18	/* PCIE Analog Config */

#define	WMREG_IOSFPC	0x0f28	/* Tx corrupted data */

#define	WMREG_PBA	0x1000	/* Packet Buffer Allocation */
#define	PBA_BYTE_SHIFT	10		/* KB -> bytes */
#define	PBA_ADDR_SHIFT	7		/* KB -> quadwords */
#define	PBA_8K		0x0008
#define	PBA_10K		0x000a
#define	PBA_12K		0x000c
#define	PBA_14K		0x000e
#define	PBA_16K		0x0010		/* 16K, default Tx allocation */
#define	PBA_20K		0x0014
#define	PBA_22K		0x0016
#define	PBA_24K		0x0018
#define	PBA_26K		0x001a
#define	PBA_30K		0x001e
#define	PBA_32K		0x0020
#define	PBA_34K		0x0022
#define	PBA_35K		0x0023
#define	PBA_40K		0x0028
#define	PBA_48K		0x0030		/* 48K, default Rx allocation */
#define	PBA_64K		0x0040
#define	PBA_RXA_MASK	__BITS(15, 0)

#define	WMREG_PBS	0x1008	/* Packet Buffer Size (ICH) */

#define	WMREG_PBECCSTS	0x100c	/* Packet Buffer ECC Status (PCH_LPT) */
#define	PBECCSTS_CORR_ERR_CNT_MASK	0x000000ff
#define	PBECCSTS_UNCORR_ERR_CNT_MASK	0x0000ff00
#define	PBECCSTS_UNCORR_ECC_ENABLE	0x00010000

#define	WMREG_EEMNGCTL	0x1010	/* MNG EEprom Control */
#define	EEMNGCTL_CFGDONE_0 0x040000	/* MNG config cycle done */
#define	EEMNGCTL_CFGDONE_1 0x080000	/*  2nd port */

#define	WMREG_I2CCMD	0x1028	/* SFPI2C Command Register - RW */
#define	I2CCMD_REG_ADDR_SHIFT	16
#define	I2CCMD_REG_ADDR		0x00ff0000
#define	I2CCMD_PHY_ADDR_SHIFT	24
#define	I2CCMD_PHY_ADDR		0x07000000
#define	I2CCMD_OPCODE_READ	0x08000000
#define	I2CCMD_OPCODE_WRITE	0x00000000
#define	I2CCMD_RESET		0x10000000
#define	I2CCMD_READY		0x20000000
#define	I2CCMD_INTERRUPT_ENA	0x40000000
#define	I2CCMD_ERROR		0x80000000
#define	MAX_SGMII_PHY_REG_ADDR	255
#define	I2CCMD_PHY_TIMEOUT	200

#define	WMREG_EEWR	0x102c	/* EEPROM write */

#define	WMREG_PBA_ECC	0x01100	/* PBA ECC */
#define	PBA_ECC_COUNTER_MASK	0xfff00000 /* ECC counter mask */
#define	PBA_ECC_COUNTER_SHIFT	20	   /* ECC counter shift value */
#define	PBA_ECC_CORR_EN		0x00000001 /* Enable ECC error correction */
#define	PBA_ECC_STAT_CLR	0x00000002 /* Clear ECC error counter */
#define	PBA_ECC_INT_EN		0x00000004 /* Enable ICR bit 5 on ECC error */

#define	WMREG_GPIE	0x01514 /* General Purpose Interrupt Enable */
#define	GPIE_NSICR	__BIT(0)	/* Non Selective Interrupt Clear */
#define	GPIE_MULTI_MSIX	__BIT(4)	/* Multiple MSIX */
#define	GPIE_EIAME	__BIT(30)	/* Extended Interrupt Auto Mask Ena. */
#define	GPIE_PBA	__BIT(31)	/* PBA support */

#define	WMREG_EICS	0x01520  /* Ext. Interrupt Cause Set - WO */
#define	WMREG_EIMS	0x01524  /* Ext. Interrupt Mask Set/Read - RW */
#define	WMREG_EIMC	0x01528  /* Ext. Interrupt Mask Clear - WO */
#define	WMREG_EIAC	0x0152c  /* Ext. Interrupt Auto Clear - RW */
#define	WMREG_EIAM	0x01530  /* Ext. Interrupt Ack Auto Clear Mask - RW */

#define	WMREG_EICR	0x01580  /* Ext. Interrupt Cause Read - R/clr */

#define	WMREG_MSIXBM(x)	(0x1600 + (x) * 4) /* MSI-X Allocation */

#define	EITR_RX_QUEUE(x)	__BIT(0+(x)) /* Rx Queue x Interrupt x=[0-3] */
#define	EITR_TX_QUEUE(x)	__BIT(8+(x)) /* Tx Queue x Interrupt x=[0-3] */
#define	EITR_TCP_TIMER	0x40000000 /* TCP Timer */
#define	EITR_OTHER	0x80000000 /* Interrupt Cause Active */

#define	WMREG_EITR(x)	(0x01680 + (0x4 * (x)))
#define	EITR_ITR_INT_MASK	__BITS(14,2)
#define	EITR_COUNTER_MASK_82575	__BITS(31,16)
#define	EITR_CNT_INGR		__BIT(31) /* does not overwrite counter */

#define	WMREG_EITR_82574(x)	(0x000e8 + (0x4 * (x)))
#define	EITR_ITR_INT_MASK_82574	__BITS(15, 0)

#define	WMREG_RXPBS	0x2404	/* Rx Packet Buffer Size  */
#define	RXPBS_SIZE_MASK_82576	0x0000007f

#define	WMREG_RDFH	0x2410	/* Receive Data FIFO Head */
#define	WMREG_RDFT	0x2418	/* Receive Data FIFO Tail */
#define	WMREG_RDFHS	0x2420	/* Receive Data FIFO Head Saved */
#define	WMREG_RDFTS	0x2428	/* Receive Data FIFO Tail Saved */
#define	WMREG_RADV	0x282c	/* Receive Interrupt Absolute Delay Timer */

#define	WMREG_TXDMAC	0x3000	/* Transfer DMA Control */
#define	TXDMAC_DPP	__BIT(0)	/* disable packet prefetch */

#define	WMREG_KABGTXD	0x3004	/* AFE and Gap Transmit Ref Data */
#define	KABGTXD_BGSQLBIAS 0x00050000

#define	WMREG_TDFH	0x3410	/* Transmit Data FIFO Head */
#define	WMREG_TDFT	0x3418	/* Transmit Data FIFO Tail */
#define	WMREG_TDFHS	0x3420	/* Transmit Data FIFO Head Saved */
#define	WMREG_TDFTS	0x3428	/* Transmit Data FIFO Tail Saved */
#define	WMREG_TDFPC	0x3430	/* Transmit Data FIFO Packet Count */

#define	WMREG_TXDCTL(n)		/* Trandmit Descriptor Control */ \
	(((n) < 4) ? (0x3828 + ((n) * 0x100)) : (0xe028 + ((n) * 0x40)))
#define	TXDCTL_PTHRESH(x) ((x) << 0)	/* prefetch threshold */
#define	TXDCTL_HTHRESH(x) ((x) << 8)	/* host threshold */
#define	TXDCTL_WTHRESH(x) ((x) << 16)	/* write back threshold */
/* flags used starting with 82575 ... */
#define	TXDCTL_COUNT_DESC	__BIT(22) /* Enable the counting of desc.
					   still to be processed. */
#define	TXDCTL_QUEUE_ENABLE  0x02000000 /* Enable specific Tx Queue */
#define	TXDCTL_SWFLSH        0x04000000 /* Tx Desc. write-back flushing */
#define	TXDCTL_PRIORITY      0x08000000

#define	WMREG_TADV	0x382c	/* Transmit Absolute Interrupt Delay Timer */
#define	WMREG_TSPMT	0x3830	/* TCP Segmentation Pad and Minimum
				   Threshold (Cordova) */
#define	TSPMT_TSMT(x)	(x)		/* TCP seg min transfer */
#define	TSPMT_TSPBP(x)	((x) << 16)	/* TCP seg pkt buf padding */

#define	WMREG_TARC0	0x3840	/* Tx arbitration count (0) */
#define	WMREG_TARC1	0x3940	/* Tx arbitration count (1) */

#define	WMREG_CRCERRS	0x4000	/* CRC Error Count */
#define	WMREG_ALGNERRC	0x4004	/* Alignment Error Count */
#define	WMREG_SYMERRC	0x4008	/* Symbol Error Count */
#define	WMREG_RXERRC	0x400c	/* Receive error Count - R/clr */
#define	WMREG_MPC	0x4010	/* Missed Packets Count - R/clr */
#define	WMREG_SCC	0x4014	/* Single Collision Count - R/clr */
#define	WMREG_ECOL	0x4018	/* Excessive Collisions Count - R/clr */
#define	WMREG_MCC	0x401c	/* Multiple Collision Count - R/clr */
#define	WMREG_LATECOL	0x4020	/* Late Collisions Count - R/clr */
#define	WMREG_COLC	0x4028	/* Collision Count - R/clr */
#define	WMREG_DC	0x4030	/* Defer Count - R/clr */
#define	WMREG_TNCRS	0x4034	/* Tx with No CRS - R/clr */
#define	WMREG_SEC	0x4038	/* Sequence Error Count */
#define	WMREG_CEXTERR	0x403c	/* Carrier Extension Error Count */
#define	WMREG_RLEC	0x4040	/* Receive Length Error Count */
#define	WMREG_XONRXC	0x4048	/* XON Rx Count - R/clr */
#define	WMREG_XONTXC	0x404c	/* XON Tx Count - R/clr */
#define	WMREG_XOFFRXC	0x4050	/* XOFF Rx Count - R/clr */
#define	WMREG_XOFFTXC	0x4054	/* XOFF Tx Count - R/clr */
#define	WMREG_FCRUC	0x4058	/* Flow Control Rx Unsupported Count - R/clr */
#define	WMREG_PRC64	0x405c	/* Packets Rx (64 bytes) - R/clr */
#define	WMREG_PRC127	0x4060	/* Packets Rx (65-127 bytes) - R/clr */
#define	WMREG_PRC255	0x4064	/* Packets Rx (128-255 bytes) - R/clr */
#define	WMREG_PRC511	0x4068	/* Packets Rx (255-511 bytes) - R/clr */
#define	WMREG_PRC1023	0x406c	/* Packets Rx (512-1023 bytes) - R/clr */
#define	WMREG_PRC1522	0x4070	/* Packets Rx (1024-1522 bytes) - R/clr */
#define	WMREG_GPRC	0x4074	/* Good Packets Rx Count - R/clr */
#define	WMREG_BPRC	0x4078	/* Broadcast Packets Rx Count - R/clr */
#define	WMREG_MPRC	0x407c	/* Multicast Packets Rx Count - R/clr */
#define	WMREG_GPTC	0x4080	/* Good Packets Tx Count - R/clr */
#define	WMREG_GORCL	0x4088	/* Good Octets Rx Count Low - R/clr */
#define	WMREG_GORCH	0x408c	/* Good Octets Rx Count High - R/clr */
#define	WMREG_GOTCL	0x4090	/* Good Octets Tx Count Low - R/clr */
#define	WMREG_GOTCH	0x4094	/* Good Octets Tx Count High - R/clr */
#define	WMREG_RNBC	0x40a0	/* Receive No Buffers Count */
#define	WMREG_RUC	0x40a4	/* Rx Undersize Count - R/clr */
#define	WMREG_RFC	0x40a8	/* Rx Fragment Count - R/clr */
#define	WMREG_ROC	0x40ac	/* Rx Oversize Count - R/clr */
#define	WMREG_RJC	0x40b0	/* Rx Jabber Count - R/clr */
#define	WMREG_MGTPRC	0x40b4	/* Management Packets RX Count - R/clr */
#define	WMREG_MGTPDC	0x40b8	/* Management Packets Dropped Count - R/clr */
#define	WMREG_MGTPTC	0x40bc	/* Management Packets TX Count - R/clr */
#define	WMREG_TORL	0x40c0	/* Total Octets Rx Low - R/clr */
#define	WMREG_TORH	0x40c4	/* Total Octets Rx High - R/clr */
#define	WMREG_TOTL	0x40c8	/* Total Octets Tx Low - R/clr */
#define	WMREG_TOTH	0x40cc	/* Total Octets Tx High - R/clr */
#define	WMREG_TPR	0x40d0	/* Total Packets Rx - R/clr */
#define	WMREG_TPT	0x40d4	/* Total Packets Tx - R/clr */
#define	WMREG_PTC64	0x40d8	/* Packets Tx (64 bytes) - R/clr */
#define	WMREG_PTC127	0x40dc	/* Packets Tx (65-127 bytes) - R/clr */
#define	WMREG_PTC255	0x40e0	/* Packets Tx (128-255 bytes) - R/clr */
#define	WMREG_PTC511	0x40e4	/* Packets Tx (256-511 bytes) - R/clr */
#define	WMREG_PTC1023	0x40e8	/* Packets Tx (512-1023 bytes) - R/clr */
#define	WMREG_PTC1522	0x40ec	/* Packets Tx (1024-1522 Bytes) - R/clr */
#define	WMREG_MPTC	0x40f0	/* Multicast Packets Tx Count - R/clr */
#define	WMREG_BPTC	0x40f4	/* Broadcast Packets Tx Count */
#define	WMREG_TSCTC	0x40f8	/* TCP Segmentation Context Tx */
#define	WMREG_TSCTFC	0x40fc	/* TCP Segmentation Context Tx Fail */
#define	WMREG_IAC	0x4100	/* Interrupt Assertion Count */
#define	WMREG_ICRXPTC	0x4104	/* Interrupt Cause Rx Pkt Timer Expire Count */
#define	WMREG_ICRXATC	0x4108	/* Interrupt Cause Rx Abs Timer Expire Count */
#define	WMREG_ICTXPTC	0x410c	/* Interrupt Cause Tx Pkt Timer Expire Count */
#define	WMREG_ICTXATC	0x4110	/* Interrupt Cause Tx Abs Timer Expire Count */
#define	WMREG_ICTXQEC	0x4118	/* Interrupt Cause Tx Queue Empty Count */
#define	WMREG_ICTXQMTC	0x411c	/* Interrupt Cause Tx Queue Min Thresh Count */
#define	WMREG_ICRXDMTC	0x4120	/* Interrupt Cause Rx Desc Min Thresh Count */
#define	WMREG_ICRXOC	0x4124	/* Interrupt Cause Receiver Overrun Count */
#define	WMREG_TLPIC	0x4148	/* EEE Tx LPI Count */
#define	WMREG_RLPIC	0x414c	/* EEE Rx LPI Count */
#define	WMREG_B2OGPRC	0x4158	/* BMC2OS packets received by host */
#define	WMREG_O2BSPC	0x415c	/* OS2BMC packets transmitted by host */

#define	WMREG_PCS_CFG	0x4200	/* PCS Configuration */
#define	PCS_CFG_PCS_EN	__BIT(3)

#define	WMREG_PCS_LCTL	0x4208	/* PCS Link Control */
#define	PCS_LCTL_FLV_LINK_UP	__BIT(0)	/* Forced Link Value */
#define	PCS_LCTL_FSV_MASK	__BITS(2, 1)	/* Forced Speed Value */
#define	PCS_LCTL_FSV_10			0		/* 10Mbps */
#define	PCS_LCTL_FSV_100		__BIT(1)	/* 100Mbps */
#define	PCS_LCTL_FSV_1000		__BIT(2)	/* 1Gpbs */
#define	PCS_LCTL_FDV_FULL	__BIT(3)	/* Force Duplex Value */
#define	PCS_LCTL_FSD		__BIT(4)	/* Force Speed and Duplex */
#define	PCS_LCTL_FORCE_LINK	__BIT(5)	/* Force Link */
#define	PCS_LCTL_LINK_LATCH_LOW	__BIT(6)	/* Link Latch Low */
#define	PCS_LCTL_FORCE_FC	__BIT(7)	/* Force Flow Control */
#define	PCS_LCTL_AN_ENABLE	__BIT(16)	/* AN enable */
#define	PCS_LCTL_AN_RESTART	__BIT(17)	/* AN restart */
#define	PCS_LCTL_AN_TIMEOUT	__BIT(18)	/* AN Timeout Enable */
#define	PCS_LCTL_AN_SGMII_BYP	__BIT(19)	/* AN SGMII Bypass */
#define	PCS_LCTL_AN_SGMII_TRIG	__BIT(20)	/* AN SGMII Trigger */
#define	PCS_LCTL_FAST_LINKTIMER	__BIT(24)	/* Fast Link Timer */
#define	PCS_LCTL_LINK_OK_FIX_EN	__BIT(25)	/* Link OK Fix Enable */

#define	WMREG_PCS_LSTS	0x420c	/* PCS Link Status */
#define	PCS_LSTS_LINKOK	__BIT(0)
#define	PCS_LSTS_SPEED	__BITS(2, 1)
#define	PCS_LSTS_SPEED_10	0
#define	PCS_LSTS_SPEED_100	1
#define	PCS_LSTS_SPEED_1000	2
#define	PCS_LSTS_FDX	__BIT(3)
#define	PCS_LSTS_AN_COMP __BIT(16)

#define	WMREG_PCS_ANADV	0x4218	/* AN Advertsement */
#define	WMREG_PCS_LPAB	0x421c	/* Link Partnet Ability */
#define	WMREG_PCS_NPTX	0x4220	/* Next Page Transmit */

#define	WMREG_RXCSUM	0x5000	/* Receive Checksum register */
#define	RXCSUM_PCSS	0x000000ff	/* Packet Checksum Start */
#define	RXCSUM_IPOFL	__BIT(8)	/* IP checksum offload */
#define	RXCSUM_TUOFL	__BIT(9)	/* TCP/UDP checksum offload */
#define	RXCSUM_IPV6OFL	__BIT(10)	/* IPv6 checksum offload */
#define	RXCSUM_CRCOFL	__BIT(11)	/* SCTP CRC32 checksum offload */
#define	RXCSUM_IPPCSE	__BIT(12)	/* IP payload checksum enable */
#define	RXCSUM_PCSD	__BIT(13)	/* packet checksum disabled */

#define	WMREG_RLPML	0x5004	/* Rx Long Packet Max Length */

#define	WMREG_RFCTL	0x5008	/* Receive Filter Control */
#define	WMREG_RFCTL_NFSWDIS	__BIT(6)  /* NFS Write Disable */
#define	WMREG_RFCTL_NFSRDIS	__BIT(7)  /* NFS Read Disable */
#define	WMREG_RFCTL_ACKDIS	__BIT(12) /* ACK Accelerate Disable */
#define	WMREG_RFCTL_ACKD_DIS	__BIT(13) /* ACK data Disable */
#define	WMREG_RFCTL_EXSTEN	__BIT(15) /* Extended status Enable. 82574 only. */
#define	WMREG_RFCTL_IPV6EXDIS	__BIT(16) /* IPv6 Extension Header Disable */
#define	WMREG_RFCTL_NEWIPV6EXDIS __BIT(17) /* New IPv6 Extension Header */

#define	WMREG_WUC	0x5800	/* Wakeup Control */
#define	WUC_APME		0x00000001 /* APM Enable */
#define	WUC_PME_EN		0x00000002 /* PME Enable */
#define	WUC_PME_STATUS		0x00000004 /* PME Status */
#define	WUC_APMPME		0x00000008 /* Assert PME on APM Wakeup */
#define	WUC_PHY_WAKE		0x00000100 /* if PHY supports wakeup */

#define	WMREG_WUFC	0x5808	/* Wakeup Filter Control */
#define	WUFC_LNKC	__BIT(0)	/* Link Status Change Wakeup Enable */
#define	WUFC_MAG	__BIT(1)	/* Magic Packet Wakeup Enable */
#define	WUFC_EX		__BIT(2)	/* Directed Exact Wakeup Enable */
#define	WUFC_MC		__BIT(3)	/* Directed Multicast Wakeup En */
#define	WUFC_BC		__BIT(4)	/* Broadcast Wakeup Enable */
#define	WUFC_ARPDIR	__BIT(5)	/* ARP Request Packet Wakeup En */
#define	WUFC_IPV4	__BIT(6)	/* Directed IPv4 Packet Wakeup En */
#define	WUFC_IPV6	__BIT(7)	/* Directed IPv6 Packet Wakeup En */
#define	WUFC_NS		__BIT(9)	/* NS Wakeup En */
#define	WUFC_NSDIR	__BIT(10)	/* NS Directed En */
#define	WUFC_ARP	__BIT(11)	/* ARP request En */
#define	WUFC_FLEX_HQ	__BIT(14)	/* Flex Filters Host Queueing En */
#define	WUFC_NOTCO	__BIT(15)	/* ? */
#define	WUFC_FLX	__BITS(23, 16)	/* Flexible Filter [0-7] En */
#define	WUFC_FLXACT	__BITS(27, 24)	/* Flexible Filter [0-3] Action */
#define	WUFC_FW_RST_WK	__BIT(31)	/* Wake on Firmware Reset Assert En */

#define	WMREG_WUS	0x5810	/* Wakeup Status (R/W1C) */
	/* Bit 30-24 and 15-12 are reserved */
#define	WUS_MNG		__BIT(8)	/* Manageability event */
#define	WUS_FLAGS	"\20"						\
	"\1LINKC"	"\2MAG"		"\3EX"		"\4MC"		\
	"\5BC"		"\6ARPDIR"	"\7IPV4"	"\10IPV6"	\
	"\11MNG"	"\12NS"		"\13NSDIR"	"\14ARP"	\
	"\21FLX0"	"\22FLX1"	"\23FLX2"	"\24FLX3"	\
	"\25FLX4"	"\26FLX5"	"\27FLX6"	"\30FLX7"	\
							"\40FW_RST_WK"

#define	WMREG_MRQC	0x5818	/* Multiple Receive Queues Command */
#define	MRQC_DISABLE_RSS	0x00000000
#define	MRQC_ENABLE_RSS_MQ_82574	__BIT(0) /* enable RSS for 82574 */
#define	MRQC_ENABLE_RSS_MQ	__BIT(1) /* enable hardware max RSS without VMDq */
#define	MRQC_ENABLE_RSS_VMDQ	__BITS(1, 0) /* enable RSS with VMDq */
#define	MRQC_DEFQ_MASK		__BITS(5, 3)
				/*
				 * Defines the default queue in non VMDq
				 * mode according to value of the Multiple Receive
				 * Queues Enable field.
				 */
#define	MRQC_DEFQ_NOT_RSS_FLT	__SHFTIN(__BIT(1), MRQC_DEFQ_MASK)
				/*
				 * the destination of all packets
				 * not forwarded by RSS or filters
				 */
#define	MRQC_DEFQ_NOT_MAC_ETH	__SHFTIN(__BITS(1, 0), MRQC_DEFQ_MASK)
				/*
				 * Def_Q field is ignored. Queueing
				 * decision of all packets not forwarded
				 * by MAC address and Ether-type filters
				 * is according to VT_CTL.DEF_PL field.
				 */
#define	MRQC_DEFQ_IGNORED1	__SHFTIN(__BIT(2), MRQC_DEFQ_MASK)
				/* Def_Q field is ignored */
#define	MRQC_DEFQ_IGNORED2	__SHFTIN(__BIT(2)|__BIT(0), MRQC_DEFQ_MASK)
				/* Def_Q field is ignored */
#define	MRQC_DEFQ_VMDQ		__SHFTIN(__BITS(2, 1), MRQC_DEFQ_MASK)
				/* for VMDq mode */
#define	MRQC_RSS_FIELD_IPV4_TCP		__BIT(16)
#define	MRQC_RSS_FIELD_IPV4		__BIT(17)
#define	MRQC_RSS_FIELD_IPV6_TCP_EX	__BIT(18)
#define	MRQC_RSS_FIELD_IPV6_EX		__BIT(19)
#define	MRQC_RSS_FIELD_IPV6		__BIT(20)
#define	MRQC_RSS_FIELD_IPV6_TCP		__BIT(21)
#define	MRQC_RSS_FIELD_IPV4_UDP		__BIT(22)
#define	MRQC_RSS_FIELD_IPV6_UDP		__BIT(23)
#define	MRQC_RSS_FIELD_IPV6_UDP_EX	__BIT(24)

#define	WMREG_RETA_Q(x)		(0x5c00 + ((x) >> 2) * 4) /* Redirection Table */
#define	RETA_NUM_ENTRIES	128
#define	RETA_ENTRY_MASK_Q(x)	(0x000000ffUL << (((x) % 4) * 8)) /* Redirection Table */
#define	RETA_ENT_QINDEX_MASK		__BITS(3,0) /*queue index for 82580 and newer */
#define	RETA_ENT_QINDEX0_MASK_82575	__BITS(3,2) /*queue index for pool0 */
#define	RETA_ENT_QINDEX1_MASK_82575	__BITS(7,6) /*queue index for pool1 and regular RSS */
#define	RETA_ENT_QINDEX_MASK_82574	__BIT(7) /*queue index for 82574 */

#define	WMREG_RSSRK(x)		(0x5c80 + (x) * 4) /* RSS Random Key Register */
#define	RSSRK_NUM_REGS		10

#define	WMREG_MANC	0x5820	/* Management Control */
#define	MANC_SMBUS_EN		__BIT(0)
#define	MANC_ASF_EN		__BIT(1)
#define	MANC_ARP_EN		__BIT(13)
#define	MANC_RECV_TCO_RESET	__BIT(16)
#define	MANC_RECV_TCO_EN	__BIT(17)
#define	MANC_BLK_PHY_RST_ON_IDE	__BIT(18)
#define	MANC_RECV_ALL		__BIT(19)
#define	MANC_EN_MAC_ADDR_FILTER	__BIT(20)
#define	MANC_EN_MNG2HOST	__BIT(21)
#define	MANC_EN_BMC2OS		__BIT(28)

#define	WMREG_MANC2H	0x5860	/* Management Control To Host - RW */
#define	MANC2H_PORT_623		(1 << 5)
#define	MANC2H_PORT_624		(1 << 6)

#define	WMREG_GCR	0x5b00	/* PCIe Control */
#define	GCR_RXD_NO_SNOOP	0x00000001
#define	GCR_RXDSCW_NO_SNOOP	0x00000002
#define	GCR_RXDSCR_NO_SNOOP	0x00000004
#define	GCR_TXD_NO_SNOOP	0x00000008
#define	GCR_TXDSCW_NO_SNOOP	0x00000010
#define	GCR_TXDSCR_NO_SNOOP	0x00000020
#define	GCR_CMPL_TMOUT_MASK	0x0000f000
#define	GCR_CMPL_TMOUT_10MS	0x00001000
#define	GCR_CMPL_TMOUT_RESEND	0x00010000
#define	GCR_CAP_VER2		0x00040000
#define	GCR_L1_ACT_WITHOUT_L0S_RX 0x08000000
#define	GCR_NO_SNOOP_ALL (GCR_RXD_NO_SNOOP | \
	    GCR_RXDSCW_NO_SNOOP |	     \
	    GCR_RXDSCR_NO_SNOOP |	     \
	    GCR_TXD_NO_SNOOP |		     \
	    GCR_TXDSCW_NO_SNOOP |	     \
	    GCR_TXDSCR_NO_SNOOP)

#define	WMREG_FACTPS	0x5b30	/* Function Active and Power State to MNG */
#define	FACTPS_MNGCG		0x20000000
#define	FACTPS_LFS		0x40000000	/* LAN Function Select */

#define	WMREG_GIOCTL	0x5b44	/* GIO Analog Control Register */
#define	WMREG_CCMCTL	0x5b48	/* CCM Control Register */
#define	WMREG_SCCTL	0x5b4c	/* PCIc PLL Configuration Register */

#define	WMREG_SWSM	0x5b50	/* SW Semaphore */
#define	SWSM_SMBI	0x00000001	/* Driver Semaphore bit */
#define	SWSM_SWESMBI	0x00000002	/* FW Semaphore bit */
#define	SWSM_WMNG	0x00000004	/* Wake MNG Clock */
#define	SWSM_DRV_LOAD	0x00000008	/* Driver Loaded Bit */
/* Intel driver defines H2ME register at 0x5b50 */
#define	WMREG_H2ME	0x5b50	/* SW Semaphore */
#define	H2ME_ULP		__BIT(11)
#define	H2ME_ENFORCE_SETTINGS	__BIT(12)

#define	WMREG_FWSM	0x5b54	/* FW Semaphore */
#define	FWSM_MODE		__BITS(1, 3)
#define	MNG_ICH_IAMT_MODE	0x2	/* PT mode? */
#define	MNG_IAMT_MODE		0x3
#define	FWSM_RSPCIPHY		__BIT(6)  /* Reset PHY on PCI reset */
#define	FWSM_WLOCK_MAC		__BITS(7, 9)
#define	FWSM_ULP_CFG_DONE	__BIT(10)
#define	FWSM_FW_VALID		__BIT(15) /* FW established a valid mode */

#define	WMREG_SWSM2	0x5b58	/* SW Semaphore 2 */
#define	SWSM2_LOCK		0x00000002 /* Secondary driver semaphore bit */

#define	WMREG_SW_FW_SYNC 0x5b5c	/* software-firmware semaphore */
#define	SWFW_EEP_SM		0x0001 /* eeprom access */
#define	SWFW_PHY0_SM		0x0002 /* first ctrl phy access */
#define	SWFW_PHY1_SM		0x0004 /* second ctrl phy access */
#define	SWFW_MAC_CSR_SM		0x0008
#define	SWFW_PHY2_SM		0x0020 /* first ctrl phy access */
#define	SWFW_PHY3_SM		0x0040 /* first ctrl phy access */
#define	SWFW_SOFT_SHIFT		0	/* software semaphores */
#define	SWFW_FIRM_SHIFT		16	/* firmware semaphores */

#define	WMREG_GCR2	0x5b64	/* 3GPIO Control Register 2 */
#define	WMREG_FEXTNVM9	0x5bb4	/* Future Extended NVM 9 */
#define	FEXTNVM9_IOSFSB_CLKGATE_DIS __BIT(11)
#define	FEXTNVM9_IOSFSB_CLKREQ_DIS __BIT(12)
#define	WMREG_FEXTNVM11	0x5bbc	/* Future Extended NVM 11 */
#define	FEXTNVM11_DIS_MULRFIX	__BIT(13)	/* Disable MULR fix */

#define	WMREG_FFLT_DBG	0x05F04 /* Debug Register */

#define	WMREG_CRC_OFFSET 0x5f50
#define	WMREG_PCH_RAICC(x)	(WMREG_CRC_OFFSET + (x) * 4)

#define	WMREG_B2OSPC	0x8fe0	/* BMC2OS packets sent by BMC */
#define	WMREG_O2BGPTC	0x8fe4	/* OS2BMC packets received by BMC */

#define	WMREG_EEC	0x12010
#define	EEC_FLASH_DETECTED __BIT(19)	/* FLASH */
#define	EEC_FLUPD	__BIT(23)	/* Update FLASH */

#define	WMREG_EEARBC_I210 0x12024

/*
 * NVM related values.
 *  Microwire, SPI, and flash
 */
#define	UWIRE_OPC_ERASE	0x04		/* MicroWire "erase" opcode */
#define	UWIRE_OPC_WRITE	0x05		/* MicroWire "write" opcode */
#define	UWIRE_OPC_READ	0x06		/* MicroWire "read" opcode */

#define	SPI_OPC_WRITE	0x02		/* SPI "write" opcode */
#define	SPI_OPC_READ	0x03		/* SPI "read" opcode */
#define	SPI_OPC_A8	0x08		/* opcode bit 3 == address bit 8 */
#define	SPI_OPC_WREN	0x06		/* SPI "set write enable" opcode */
#define	SPI_OPC_WRDI	0x04		/* SPI "clear write enable" opcode */
#define	SPI_OPC_RDSR	0x05		/* SPI "read status" opcode */
#define	SPI_OPC_WRSR	0x01		/* SPI "write status" opcode */
#define	SPI_MAX_RETRIES	5000		/* max wait of 5ms for RDY signal */

#define	SPI_SR_RDY	0x01
#define	SPI_SR_WEN	0x02
#define	SPI_SR_BP0	0x04
#define	SPI_SR_BP1	0x08
#define	SPI_SR_WPEN	0x80

#define	NVM_CHECKSUM		0xBABA
#define	NVM_SIZE		0x0040
#define	NVM_WORD_SIZE_BASE_SHIFT 6

#define	NVM_OFF_MACADDR		0x0000	/* MAC address offset 0 */
#define	NVM_OFF_MACADDR1	0x0001	/* MAC address offset 1 */
#define	NVM_OFF_MACADDR2	0x0002	/* MAC address offset 2 */
#define	NVM_OFF_COMPAT		0x0003
#define	NVM_OFF_ID_LED_SETTINGS	0x0004
#define	NVM_OFF_VERSION		0x0005
#define	NVM_OFF_CFG1		0x000a	/* config word 1 */
#define	NVM_OFF_CFG2		0x000f	/* config word 2 */
#define	NVM_OFF_EEPROM_SIZE	0x0012	/* NVM SIZE */
#define	NVM_OFF_CFG4		0x0013	/* config word 4 */
#define	NVM_OFF_CFG3_PORTB	0x0014	/* config word 3 */
#define	NVM_OFF_FUTURE_INIT_WORD1 0x0019
#define	NVM_OFF_INIT_3GIO_3	0x001a	/* PCIe Initial Configuration Word 3 */
#define	NVM_OFF_K1_CONFIG	0x001b	/* NVM K1 Config */
#define	NVM_OFF_LED_1_CFG	0x001c
#define	NVM_OFF_LED_0_2_CFG	0x001f
#define	NVM_OFF_SWDPIN		0x0020	/* SWD Pins (Cordova) */
#define	NVM_OFF_CFG3_PORTA	0x0024	/* config word 3 */
#define	NVM_OFF_ALT_MAC_ADDR_PTR 0x0037	/* to the alternative MAC addresses */
#define	NVM_OFF_COMB_VER_PTR	0x003d
#define	NVM_OFF_IMAGE_UID0	0x0042
#define	NVM_OFF_IMAGE_UID1	0x0043

#define	NVM_COMPAT_VALID_CHECKSUM	0x0001

#define	NVM_CFG1_LVDID		__BIT(0)
#define	NVM_CFG1_LSSID		__BIT(1)
#define	NVM_CFG1_PME_CLOCK	__BIT(2)
#define	NVM_CFG1_PM		__BIT(3)
#define	NVM_CFG1_ILOS		__BIT(4)	/* Invert loss of signal */
#define	NVM_CFG1_SWDPIO_SHIFT	5
#define	NVM_CFG1_SWDPIO_MASK	(0xf << NVM_CFG1_SWDPIO_SHIFT)
#define	NVM_CFG1_IPS1		__BIT(8)
#define	NVM_CFG1_LRST		__BIT(9)
#define	NVM_CFG1_FD		__BIT(10)
#define	NVM_CFG1_FRCSPD		__BIT(11)
#define	NVM_CFG1_IPS0		__BIT(12)
#define	NVM_CFG1_64_32_BAR	__BIT(13)

#define	NVM_CFG2_CSR_RD_SPLIT	__BIT(1)
#define	NVM_CFG2_82544_APM_EN	__BIT(2)
#define	NVM_CFG2_64_BIT		__BIT(3)
#define	NVM_CFG2_MAX_READ	__BIT(4)
#define	NVM_CFG2_DMCR_MAP	__BIT(5)
#define	NVM_CFG2_133_CAP	__BIT(6)
#define	NVM_CFG2_MSI_DIS	__BIT(7)
#define	NVM_CFG2_FLASH_DIS	__BIT(8)
#define	NVM_CFG2_FLASH_SIZE(x)	(((x) & 3) >> 9)
#define	NVM_CFG2_APM_EN		__BIT(10)
#define	NVM_CFG2_ANE		__BIT(11)
#define	NVM_CFG2_PAUSE(x)	(((x) & 3) >> 12)
#define	NVM_CFG2_ASDE		__BIT(14)
#define	NVM_CFG2_APM_PME	__BIT(15)
#define	NVM_CFG2_SWDPIO_SHIFT	4
#define	NVM_CFG2_SWDPIO_MASK	(0xf << NVM_CFG2_SWDPIO_SHIFT)
#define	NVM_CFG2_MNGM_SHIFT	13	/* Manageability Operation mode */
#define	NVM_CFG2_MNGM_MASK	(3U << NVM_CFG2_MNGM_SHIFT)
#define	NVM_CFG2_MNGM_DIS	0
#define	NVM_CFG2_MNGM_NCSI	1
#define	NVM_CFG2_MNGM_PT	2

#define	NVM_COMPAT_MAS_EN(x)		__BIT(x) /* Media Auto Sense Enable */
#define	NVM_COMPAT_SERDES_FORCE_MODE	__BIT(14) /* Don't use autonego */

#define	NVM_FUTURE_INIT_WORD1_VALID_CHECKSUM	0x0040

#define	NVM_K1_CONFIG_ENABLE	0x01

#define	NVM_SWDPIN_MASK		0xdf
#define	NVM_SWDPIN_SWDPIN_SHIFT 0
#define	NVM_SWDPIN_SWDPIO_SHIFT 8

#define	NVM_3GIO_3_ASPM_MASK	(0x3 << 2)	/* Active State PM Support */

#define	NVM_CFG3_PORTA_EXT_MDIO	__BIT(2)	/* External MDIO Interface */
#define	NVM_CFG3_PORTA_COM_MDIO	__BIT(3)	/* MDIO Interface is shared */
#define	NVM_CFG3_APME		__BIT(10)	/* APM Enable */
#define	NVM_CFG3_ILOS		__BIT(13)	/* Invert loss of signal */

#define	NVM_OFF_MACADDR_82571(x)	(3 * (x))

/*
 * EEPROM Partitioning. See Table 6-1, "EEPROM Top Level Partitioning"
 * in 82580's datasheet.
 */
#define	NVM_OFF_LAN_FUNC_82580(x)	((x) ? (0x40 + (0x40 * (x))) : 0)

#define	NVM_COMBO_VER_OFF	0x0083

#define	NVM_MAJOR_MASK		0xf000
#define	NVM_MAJOR_SHIFT		12
#define	NVM_MINOR_MASK		0x0ff0
#define	NVM_MINOR_SHIFT		4
#define	NVM_BUILD_MASK		0x000f
#define	NVM_UID_VALID		0x8000

/* iNVM Registers for i21[01] */
#define	WM_INVM_DATA_REG(reg)	(0x12120 + 4*(reg))
#define	INVM_SIZE			64 /* Number of INVM Data Registers */

/* iNVM default value */
#define	NVM_INIT_CTRL_2_DEFAULT_I211	0x7243
#define	NVM_INIT_CTRL_4_DEFAULT_I211	0x00c1
#define	NVM_LED_1_CFG_DEFAULT_I211	0x0184
#define	NVM_LED_0_2_CFG_DEFAULT_I211	0x200c
#define	NVM_RESERVED_WORD		0xffff

#define	INVM_DWORD_TO_RECORD_TYPE(dword)	((dword) & 0x7)
#define	INVM_DWORD_TO_WORD_ADDRESS(dword)	(((dword) & 0x0000FE00) >> 9)
#define	INVM_DWORD_TO_WORD_DATA(dword)		(((dword) & 0xFFFF0000) >> 16)

#define	INVM_UNINITIALIZED_STRUCTURE		0x0
#define	INVM_WORD_AUTOLOAD_STRUCTURE		0x1
#define	INVM_CSR_AUTOLOAD_STRUCTURE		0x2
#define	INVM_PHY_REGISTER_AUTOLOAD_STRUCTURE	0x3
#define	INVM_RSA_KEY_SHA256_STRUCTURE		0x4
#define	INVM_INVALIDATED_STRUCTURE		0xf

#define	INVM_RSA_KEY_SHA256_DATA_SIZE_IN_DWORDS	8
#define	INVM_CSR_AUTOLOAD_DATA_SIZE_IN_DWORDS	1

#define	INVM_DEFAULT_AL		0x202f
#define	INVM_AUTOLOAD		0x0a
#define	INVM_PLL_WO_VAL		0x0010

/* Version and Image Type field */
#define	INVM_VER_1	__BITS(12,3)
#define	INVM_VER_2	__BITS(22,13)
#define	INVM_IMGTYPE	__BITS(28,23)
#define	INVM_MINOR	__BITS(3,0)
#define	INVM_MAJOR	__BITS(9,4)

/* Word definitions for ID LED Settings */
#define	ID_LED_RESERVED_FFFF 0xffff

/* ich8 flash control */
#define	ICH_FLASH_COMMAND_TIMEOUT            5000    /* 5000 uSecs - adjusted */
#define	ICH_FLASH_ERASE_TIMEOUT              3000000 /* Up to 3 seconds - worst case */
#define	ICH_FLASH_CYCLE_REPEAT_COUNT         10      /* 10 cycles */
#define	ICH_FLASH_SEG_SIZE_256               256
#define	ICH_FLASH_SEG_SIZE_4K                4096
#define	ICH_FLASH_SEG_SIZE_64K               65536

#define	ICH_CYCLE_READ                       0x0
#define	ICH_CYCLE_RESERVED                   0x1
#define	ICH_CYCLE_WRITE                      0x2
#define	ICH_CYCLE_ERASE                      0x3

#define	ICH_FLASH_GFPREG   0x0000
#define	ICH_FLASH_HSFSTS   0x0004 /* Flash Status Register */
#define	HSFSTS_DONE		0x0001 /* Flash Cycle Done */
#define	HSFSTS_ERR		0x0002 /* Flash Cycle Error */
#define	HSFSTS_DAEL		0x0004 /* Direct Access error Log */
#define	HSFSTS_ERSZ_MASK	0x0018 /* Block/Sector Erase Size */
#define	HSFSTS_ERSZ_SHIFT	3
#define	HSFSTS_FLINPRO		0x0020 /* flash SPI cycle in Progress */
#define	HSFSTS_FLDVAL		0x4000 /* Flash Descriptor Valid */
#define	HSFSTS_FLLK		0x8000 /* Flash Configuration Lock-Down */
#define	ICH_FLASH_HSFCTL   0x0006 /* Flash control Register */
#define	HSFCTL_GO		0x0001 /* Flash Cycle Go */
#define	HSFCTL_CYCLE_MASK	0x0006 /* Flash Cycle */
#define	HSFCTL_CYCLE_SHIFT	1
#define	HSFCTL_BCOUNT_MASK	0x0300 /* Data Byte Count */
#define	HSFCTL_BCOUNT_SHIFT	8
#define	ICH_FLASH_FADDR    0x0008
#define	ICH_FLASH_FDATA0   0x0010
#define	ICH_FLASH_FRACC    0x0050
#define	ICH_FLASH_FREG0    0x0054
#define	ICH_FLASH_FREG1    0x0058
#define	ICH_FLASH_FREG2    0x005c
#define	ICH_FLASH_FREG3    0x0060
#define	ICH_FLASH_FPR0     0x0074
#define	ICH_FLASH_FPR1     0x0078
#define	ICH_FLASH_SSFSTS   0x0090
#define	ICH_FLASH_SSFCTL   0x0092
#define	ICH_FLASH_PREOP    0x0094
#define	ICH_FLASH_OPTYPE   0x0096
#define	ICH_FLASH_OPMENU   0x0098

#define	ICH_FLASH_REG_MAPSIZE      0x00a0
#define	ICH_FLASH_SECTOR_SIZE      4096
#define	ICH_GFPREG_BASE_MASK       0x1fff
#define	ICH_FLASH_LINEAR_ADDR_MASK 0x00ffffff

#define	ICH_NVM_SIG_WORD	0x13
#define	ICH_NVM_SIG_MASK	0xc000
#define	ICH_NVM_VALID_SIG_MASK	0xc0
#define	ICH_NVM_SIG_VALUE	0x80

#define	NVM_SIZE_MULTIPLIER 4096	/* multiplier for NVMS field */
#define	WM_PCH_SPT_FLASHOFFSET	0xe000	/* offset of NVM access regs(PCH_SPT)*/

/* for PCI express Capability registers */
#define	WM_PCIE_DCSR2_16MS	0x00000005

/* SFF SFP ROM data */
#define	SFF_SFP_ID_OFF		0x00
#define	SFF_SFP_ID_UNKNOWN	0x00	/* Unknown */
#define	SFF_SFP_ID_SFF		0x02	/* Module soldered to motherboard */
#define	SFF_SFP_ID_SFP		0x03	/* SFP transceiver */

#define	SFF_SFP_ETH_FLAGS_OFF	0x06
#define	SFF_SFP_ETH_FLAGS_1000SX	0x01
#define	SFF_SFP_ETH_FLAGS_1000LX	0x02
#define	SFF_SFP_ETH_FLAGS_1000CX	0x04
#define	SFF_SFP_ETH_FLAGS_1000T		0x08
#define	SFF_SFP_ETH_FLAGS_100LX		0x10
#define	SFF_SFP_ETH_FLAGS_100FX		0x20

/* I21[01] PHY related definitions */
#define	GS40G_PAGE_SELECT	0x16
#define	GS40G_PAGE_SHIFT	16
#define	GS40G_OFFSET_MASK	0xffff
#define	GS40G_PHY_PLL_FREQ_PAGE	0xfc0000
#define	GS40G_PHY_PLL_FREQ_REG	0x000e
#define	GS40G_PHY_PLL_UNCONF	0xff

/* advanced TX descriptor for 82575 and newer */
typedef union nq_txdesc {
	struct {
		uint64_t nqtxd_addr;
		uint32_t nqtxd_cmdlen;
		uint32_t nqtxd_fields;
	} nqtx_data;
	struct {
		uint32_t nqtxc_vl_len;
		uint32_t nqtxc_sn;
		uint32_t nqtxc_cmd;
		uint32_t nqtxc_mssidx;
	} nqtx_ctx;
} __packed nq_txdesc_t;


/* Commands for nqtxd_cmdlen and nqtxc_cmd */
#define	NQTX_CMD_EOP	__BIT(24)	/* end of packet */
#define	NQTX_CMD_IFCS	__BIT(25)	/* insert FCS */
#define	NQTX_CMD_RS	__BIT(27)	/* report status */
#define	NQTX_CMD_DEXT	__BIT(29)	/* descriptor extension */
#define	NQTX_CMD_VLE	__BIT(30)	/* VLAN enable */
#define	NQTX_CMD_TSE	__BIT(31)	/* TCP segmentation enable */

/* Descriptor types (if DEXT is set) */
#define	NQTX_DTYP_C	(2U << 20)	/* context */
#define	NQTX_DTYP_D	(3U << 20)	/* data */

#define	NQTXD_FIELDS_IDX_SHIFT		4	/* context index shift */
#define	NQTXD_FIELDS_IDX_MASK		0xf
#define	NQTXD_FIELDS_PAYLEN_SHIFT	14	/* payload len shift */
#define	NQTXD_FIELDS_PAYLEN_MASK	0x3ffff

#define	NQTXD_FIELDS_IXSM		__BIT(8) /* do IP checksum */
#define	NQTXD_FIELDS_TUXSM		__BIT(9) /* do TCP/UDP checksum */

#define	NQTXC_VLLEN_IPLEN_SHIFT		0	/* IP header len */
#define	NQTXC_VLLEN_IPLEN_MASK		0x1ff
#define	NQTXC_VLLEN_MACLEN_SHIFT	9	/* MAC header len */
#define	NQTXC_VLLEN_MACLEN_MASK		0x7f
#define	NQTXC_VLLEN_VLAN_SHIFT		16	/* vlan number */
#define	NQTXC_VLLEN_VLAN_MASK		0xffff

#define	NQTXC_CMD_MKRLOC_SHIFT		0	/* IP checksum offset */
#define	NQTXC_CMD_MKRLOC_MASK		0x1ff
#define	NQTXC_CMD_SNAP			__BIT(9)
#define	NQTXC_CMD_IPV_MASK		__BIT(10)
#define	NQTXC_CMD_IP4			__SHIFTIN(1, NQTXC_CMD_IPV_MASK)
#define	NQTXC_CMD_IP6			__SHIFTIN(0, NQTXC_CMD_IPV_MASK)
#define	NQTXC_CMD_TP_MASK		__BIT(11)
#define	NQTXC_CMD_TCP			__SHIFTIN(1, NQTXC_CMD_TP_MASK)
#define	NQTXC_CMD_UDP			__SHIFTIN(0, NQTXC_CMD_TP_MASK)
#define	NQTXC_MSSIDX_IDX_SHIFT		4	/* context index shift */
#define	NQTXC_MSSIDX_IDX_MASK		0xf
#define	NQTXC_MSSIDX_L4LEN_SHIFT	8	/* L4 header len shift */
#define	NQTXC_MSSIDX_L4LEN_MASK		0xff
#define	NQTXC_MSSIDX_MSS_SHIFT		16	/* MSS */
#define	NQTXC_MSSIDX_MSS_MASK		0xffff
