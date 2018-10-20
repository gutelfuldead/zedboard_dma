/**
 * @brief  Provides API for abstracting control for handling bare-metal
 *         AXI-Stream DMA Engine configured in Scatter Gather (SG) Mode
 *         with Dynamic ReAlignment Engine (DRE) enabled  
 * @author Jason Gutel (github.com/gutelfuldead)
 */

#ifndef AXIS_DMA_CONTROLLER_H
#define AXIS_DMA_CONTROLLER_H

#include "xaxidma.h"
#include "xscugic.h"

// #define AXISDMA_ENABLE_PRINTS /**< Define to enable debug printing in API */

#define E_NOBDS -2 /**< returned when not enough bds can be allocated from axisDmaCtrl_sendPackets */

/**
 * RX Callback Type
 * @param buf_addr provides memory offset of returned buffer
 * @param buf_len  provides length of returned buffer in bytes
 */
typedef void (*rx_cb_t)(uint32_t buf_addr, uint32_t buf_len);

/**
 * TX Callback Type
 */
typedef void (*tx_cb_t)(void);

/**
 * @brief      Structure used to pass all configuration parameters 
 *             to the api
 */
struct axisDmaCtrl_params{
	/* memory parameters */
	size_t rx_bd_space_base; /**< base address for rx bds */
	size_t rx_bd_space_high; /**< high address for rx bds */
	size_t tx_bd_space_base; /**< base address for tx bds */
	size_t tx_bd_space_high; /**< high address for tx bds */
	size_t tx_buffer_base;   /**< base address for tx data buffer */
	size_t tx_buffer_high;   /**< high address for tx data buffer */
	size_t rx_buffer_base;   /**< base address for rx data buffer */
	size_t rx_buffer_high;   /**< high address for rx data buffer */
	size_t bd_buf_size;      /**< number of bytes in a bd buffer */
	uint8_t coalesce_count;	 /**< number of bds that must be ready for an irq to fire */
	uint8_t axisDma_txIrqPriority; /**< priority level for the tx dma irq */
	uint8_t axisDma_rxIrqPriority; /**< priority level for the rx dma irq */
	uint8_t axisDma_txIrqId; /**< Interrupt ID for the tx dma from xparameters */
	uint8_t axisDma_rxIrqId; /**< Interrupt ID for the rx dma from xparameters */
	uint8_t axisDma_dmaDevId; /**< ID for the dma Device from xparameters */
	uint8_t axisDma_XscuGic_DevId; /**< ID for the XscuGic from xparameters */
};

/**
 * @brief      Changes the TX Callback on the fly
 *
 * @param[in]  cb    function for tx callback
 *
 * @return     success on valid memory address (not null)
 */
int axisDmaCtrl_register_tx_cb(tx_cb_t cb);

/**
 * @brief      Changes the RX Callback on the fly
 *
 * @param[in]  cb    function for rx callback
 *
 * @return     success on valid memory address (not null)
 */
int axisDmaCtrl_register_rx_cb(rx_cb_t cb);

/**
 * @brief      Initializes DMA system. Registers TX and RX callback functions.
 *             Sets up memory area to be registered as noncacheable by the tlb. 
 *
 * @param      params           Initialized axisDmaCtrl_params struct
 * @param      IntcInstancePtr  Pointer to the XscuGic instance
 * @param[in]  rxCb             The receive callback function
 * @param[in]  txCb             The transmit callback function
 *
 * @return     { description_of_the_return_value }
 */
int axisDmaCtrl_init(struct axisDmaCtrl_params *params, 
	XScuGic * IntcInstancePtr,
	rx_cb_t rxCb,
	tx_cb_t txCb);

/**
 * @brief      Removes DMA interrupts from the GIC. Clears specified memory
 *             provided in the init. Unregisters callback functions.	 
 *
 * @param      IntcInstancePtr  Pointer to the XscuGic instance
 */
void axisDmaCtrl_disable(XScuGic * IntcInstancePtr);

/**
 * @brief      Handles sending a memory region as a single packet. Must be
 *             cognisant of how the AXI4-Stream system in the FPGA works.
 *             End of memory region will assert a TLAST flag.
 *
 * @param      packetBuf   The packet buffer
 * @param[in]  packetSize  The packet size
 *
 * @return     { description_of_the_return_value }
 */
int axisDmaCtrl_sendPackets(uint8_t * packetBuf, size_t packetSize);

/**
 * @brief      Debug function to print the elements of the axisDmaCtrl_params struct
 *
 * @param      in    Struct to print from
 */
void axisDmaCtrl_printParams(struct axisDmaCtrl_params *in);

#endif // AXIS_DMA_CONTROLLER_H
