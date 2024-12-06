#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include <string.h>

#define STACKSIZE 1024
#define PRIORITY 1 
int cont; //contador

struct estrutura_fifo {
    void* fifo_reserved;
    uint32_t valor;//Define os valores que serão armazenados na FiFo como inteiros positivos de 32bits
};

K_FIFO_DEFINE(fifo);//Define FiFo

struct estrutura_fifo transmissao1;//Objetos transimitidos para FiFo
struct estrutura_fifo transmissao2;
struct estrutura_fifo* leitura;//Ponteiro usado para leitura da FiFo
struct estrutura_fifo* leitura2;//Ponteiro usado para leitura da FiFo

void Transmissor(void)//Thread que transmite valor para FiFo
{
    while (1) {
        transmissao1.valor = 13;//Define o valor do item transmitido
        k_fifo_put(&fifo, &transmissao1);//Coloca o item na FIFO
		cont++;
        k_msleep(1000);
    }
}

void Transmissor2(void)//Thread que transmite valor para FiFo
{
    while (1) {
        transmissao2.valor = 15;//Define o valor do item transmitido
        k_fifo_put(&fifo, &transmissao2);//Coloca o item na FIFO
        k_msleep(1000);         
		cont++;

    }
}

void leitor(void)//Thread que le e imprime os valores contidos na FiFo
{
    while (1) {
        leitura = k_fifo_get(&fifo, K_FOREVER);//Espera ate que tenha um item na FiFo para ser lido, em seguida realiza a leitura
        k_msleep(1000); 
		cont--;                        
        printk("Valor lido %d; o tamanho da FiFo é %d\n\n", leitura->valor, cont);//Printa o valor lido
    }
}

void leitor2(void)//Thread que le e imprime os valores contidos na FiFo
{
    while (1) {
        leitura2 = k_fifo_get(&fifo, K_FOREVER);//Espera ate que tenha um item na FiFo para ser lido, em seguida realiza a leitura
        k_msleep(1000); 
		cont--;                        
        printk("Valor lido %d; o tamanho da FiFo é %d\n\n", leitura->valor, cont);//Printa o valor lido
    }
}

K_THREAD_DEFINE(Transmissornome, STACKSIZE, Transmissor, NULL, NULL, NULL,
    PRIORITY, 0, 0);//Thread de transmissao
K_THREAD_DEFINE(Transmissor2nome, STACKSIZE, Transmissor2, NULL, NULL, NULL,
    PRIORITY, 0, 0);//Thread de transmissao2
K_THREAD_DEFINE(leitornome, STACKSIZE, leitor, NULL, NULL, NULL,
    PRIORITY, 0, 0);//Thread de leitura
/*K_THREAD_DEFINE(leitor2nome, STACKSIZE, leitor2, NULL, NULL, NULL,
    PRIORITY, 0, 0);//Thread de leitura2*/