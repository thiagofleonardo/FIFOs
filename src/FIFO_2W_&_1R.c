#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/mutex.h>
#include <string.h>

#define STACKSIZE 1024
#define PRIORITY 7

K_FIFO_DEFINE(my_fifo);
K_MUTEX_DEFINE(my_mutex); //mutex para controlar o acesso a variável cont

struct data_item_t {
    void* fifo_reserved; /* 1st word reserved for use by fifo */
    uint32_t value;
};

struct data_item_t tx1_data;
struct data_item_t tx2_data;
struct data_item_t *rx_data;
int cont = 0; //contador de elementos na fila, para evitar o esgotamento da memória

void escrita1(void)
{
    while (1) {
        tx1_data.value = 45;
        k_mutex_lock(&my_mutex, K_FOREVER);
        if (cont > 25) {
            k_mutex_unlock(&my_mutex);
            k_sleep(K_SECONDS(1));
            continue;
        }
        k_fifo_put(&my_fifo, &tx1_data);
        cont++;
        k_mutex_unlock(&my_mutex);
        k_sleep(K_SECONDS(1));
    }
}

void escrita2(void)
{
    while (1) {
        tx2_data.value = 24;
        k_mutex_lock(&my_mutex, K_FOREVER);
        if (cont > 25) {
            k_mutex_unlock(&my_mutex);
            k_sleep(K_SECONDS(1));
            continue;
        }
        k_fifo_put(&my_fifo, &tx2_data);
        cont++;
        k_mutex_unlock(&my_mutex);
        k_sleep(K_SECONDS(1));
    }
}

void leitura(void)
{
    while (1) {
        rx_data = k_fifo_get(&my_fifo, K_FOREVER);
        if (rx_data != NULL) {
            k_mutex_lock(&my_mutex, K_FOREVER);
            cont--;
            k_mutex_unlock(&my_mutex);
            printk("Valor lido %d\n", rx_data->value);
        }
        k_sleep(K_SECONDS(1));
    }
}

K_THREAD_DEFINE(thread1_id, STACKSIZE, escrita1, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
K_THREAD_DEFINE(thread2_id, STACKSIZE, escrita2, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
K_THREAD_DEFINE(thread3_id, STACKSIZE, leitura, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);