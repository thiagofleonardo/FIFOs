#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#define STACKSIZE 1024
#define PRIORITY 7

K_FIFO_DEFINE(my_fifo);

struct data_item_t {
    void* fifo_reserved; /* 1st word reserved for use by fifo */
    uint32_t value;
};

struct data_item_t tx_data;
struct data_item_t *rx_data;

void escrita(void)
{
    while (1) {
        tx_data.value = 45;
        k_fifo_put(&my_fifo, &tx_data);
        k_sleep(K_SECONDS(1));
    }
}

void leitura(void)
{
    while (1) {
        rx_data = k_fifo_get(&my_fifo, K_FOREVER);
        if (rx_data != NULL) {
            printk("Valor lido %d\n", rx_data->value);
        }
        k_sleep(K_SECONDS(1));
    }
}

K_THREAD_DEFINE(thread1_id, STACKSIZE, escrita, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
K_THREAD_DEFINE(thread2_id, STACKSIZE, leitura, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);