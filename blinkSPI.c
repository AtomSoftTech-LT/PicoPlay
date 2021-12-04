#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"


//FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#define B_LED 10
#define R_LED 11
#define G_LED 12
#define Y_LED 13

#define rst_btn 19
#define add_btn 20

#define HIGH 1
#define LOW 0

#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

//setup our queue length and handle
 static const uint8_t rateQ_len = 5;
 static QueueHandle_t rateQ_h;

void GreenLEDTask (void *param)
{
    while(1) {
        gpio_put(G_LED,HIGH);
        vTaskDelay(400);
        gpio_put(G_LED,LOW);
        vTaskDelay(400);
    }
}
void RedLEDTask (void *param)
{
    while(1) {
        gpio_put(R_LED,HIGH);
        vTaskDelay(700);
        gpio_put(R_LED,LOW);
        vTaskDelay(700);
    }
}

void AddTimeTask (void *param)
{
    while(1) {
        int btnData = gpio_get(add_btn);

        if(btnData == LOW)
        {
            int data = 1;
            if(xQueueSend(rateQ_h,(void *)&data,( TickType_t ) 0) != pdTRUE)
            {
                //add to queue failed
            }

            //debounce
            vTaskDelay(333);
        }
    }
}

void RstTimeTask (void *param)
{
    while(1){
        int btnData = gpio_get(rst_btn);
        
        if(btnData == LOW)
        {
            int data = 0;
            if(xQueueSend(rateQ_h,(void *)&data,( TickType_t ) 0) != pdTRUE)
            {
                //add to queue failed 
            }

            //debounce
            vTaskDelay(333);
        }
    }
}

void BlueLEDTask (void *param)
{
    int myTodo;
    int delay_time = 100;

    while(1) {
        
        if(xQueueReceive(rateQ_h,(void *)&myTodo,( TickType_t ) 0) == pdTRUE)
        {
            //New data update last delay for future use
            if(myTodo == 0){
                delay_time = 100;
            } else {
                delay_time += 100;
            }
        }

        gpio_put(B_LED,HIGH);
        vTaskDelay(delay_time);
        gpio_put(B_LED,LOW);
        vTaskDelay(delay_time);
    }
}
void YellowLEDTask (void *param)
{
    while(1) {
        gpio_put(Y_LED,HIGH);
        vTaskDelay(100);
        gpio_put(Y_LED,LOW);
        vTaskDelay(100);
    }
}

void SendSPITask(void *param){
    const uint8_t temp = 0xAB;

    while(1){
        gpio_put(PIN_CS,LOW);
        spi_write_blocking(SPI_PORT, &temp,1);
        gpio_put(PIN_CS,HIGH);
        vTaskDelay(1000);
    }
}

int main()
{
    stdio_init_all();
    
    //SPI Stuff
    spi_init(SPI_PORT, 1000000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);  //enabled I/O and set func to GPIO_FUNC_SIO
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    //LED Stuff
    gpio_init(G_LED);
    gpio_init(R_LED);
    gpio_init(B_LED);
    gpio_init(Y_LED);

    gpio_init(rst_btn);
    gpio_init(add_btn);

    //Set buttons as inputs
    //gpio_set_input_enabled(rst_btn, true);
    //gpio_set_input_enabled(add_btn, true);
    gpio_set_dir(rst_btn, GPIO_IN);
    gpio_set_dir(add_btn, GPIO_IN);

    gpio_pull_up(rst_btn);
    gpio_pull_up(add_btn);

    gpio_set_dir(G_LED, GPIO_OUT);
    gpio_set_dir(Y_LED, GPIO_OUT);
    gpio_set_dir(R_LED, GPIO_OUT);
    gpio_set_dir(B_LED, GPIO_OUT);
    
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    //create our queue
    rateQ_h = xQueueCreate(rateQ_len, sizeof(int));


    TaskHandle_t gLEDtask = NULL;
    TaskHandle_t rLEDtask = NULL;
    TaskHandle_t bLEDtask = NULL;
    TaskHandle_t yLEDtask = NULL;
    TaskHandle_t spiTask = NULL;

    TaskHandle_t AddTimeTask_h = NULL;
    TaskHandle_t RstTimeTask_h = NULL;

    uint32_t statusRstTime = xTaskCreate(
                    RstTimeTask,
                    "Reset Time",
                    1000,
                    NULL,
                    1,
                    &RstTimeTask_h
    );

    uint32_t statuAddTime = xTaskCreate(
                    AddTimeTask,
                    "Add Time",
                    1000,
                    NULL,
                    1,
                    &AddTimeTask_h
    );


    uint32_t statusSpi = xTaskCreate(
                    SendSPITask,
                    "SPI Task",
                    1000,
                    NULL,
                    tskIDLE_PRIORITY,
                    &spiTask
    );
    

    uint32_t status = xTaskCreate(
                    GreenLEDTask,
                    "Green LED",
                    500,
                    NULL,
                    tskIDLE_PRIORITY,
                    &gLEDtask
    );
    
    uint32_t statusR = xTaskCreate(
                    RedLEDTask,
                    "Red LED",
                    500,
                    NULL,
                    tskIDLE_PRIORITY,
                    &rLEDtask
    );

    uint32_t statusB = xTaskCreate(
                    BlueLEDTask,
                    "Blue LED",
                    1000,
                    NULL,
                    tskIDLE_PRIORITY,
                    &bLEDtask
    );

    uint32_t statusY = xTaskCreate(
                    YellowLEDTask,
                    "Yellow LED",
                    500,
                    NULL,
                    tskIDLE_PRIORITY,
                    &yLEDtask
    );

    vTaskStartScheduler();
    
    while(1){
        
    }
}
