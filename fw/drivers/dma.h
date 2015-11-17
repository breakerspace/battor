#ifndef DMA_H
#define DMA_H

void dma_init();
void dma_start(void* adcb_samples0, void* adcb_samples1, uint16_t samples_len);
void dma_stop();

void dma_uart_tx(const void* data, uint16_t len);
uint8_t dma_uart_tx_ready();
void dma_uart_tx_pause(uint8_t on_off);
void dma_sram_txrx(const void* txd, void* rxd, uint16_t len);

#endif
