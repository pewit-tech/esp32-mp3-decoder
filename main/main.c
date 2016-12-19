#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "soc/gpio_sig_map.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

#include "rom/ets_sys.h"
#include "rom/lldesc.h"
#include "rom/gpio.h"

#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_reg.h"
#include "soc/i2s_reg.h"
#include "driver/periph_ctrl.h"
#include "i2s_freertos.h"
#include "esp_intr.h"
#include <math.h>

#define S_RATE  I2S_DEFAULT_SAMPLE_RATE

static void tskLcd(void *pvParameters)
{
    //sbuf = malloc(DMALEN * 2);
    //uint16_t *rbuf = malloc(DMALEN * 2);
    uint16_t xorval = 0;

    int x;
    int y = 0;
    // lcdIfaceInit();
    i2sInit();

    float amplitude = 20000;
            float freq_Hz = 440;
            float phase=0;
        
            float freq_radians_per_sample = freq_Hz*2*M_PI/S_RATE;

//  lcdFlush();
    //while (1)
    {
        // for (x = 0; x < DMALEN; x++) {
        //     //rbuf[x] = x ^ xorval;
        //     rbuf[x] = 0x0;    //clear rbuf
        // }

        //for (int cycle = 0; cycle < 30; cycle++)
        {

            //sendRecvBufDma(sbuf, rbuf, DMALEN);
            // sendRecvBufDma(sbuf, sbuf, DMALEN);
            vTaskDelay(0 / portTICK_PERIOD_MS);
            //finishDma();
        }

        int sampleNumber = 0;

        while (1) {
                //sbuf[x] = x ^ xorval;
                //sbuf[x+1] = 0x0;
                //sbuf[x] = 0xFF00;

                phase += freq_radians_per_sample;
                unsigned int sample = (uint16_t)(amplitude * sin(phase));
                // sbuf[x] = sample;

                sample = (sample << 16) | sample;

                
                // printf("pushing sample %d\n", sampleNumber);
                i2sPushSample(sample);
                // printf("sample %d PUSHED\n", sampleNumber);

                sampleNumber++;

                // if (sampleNumber == S_RATE) {
                //     sampleNumber = 0;
                //     phase = 0;
                // }
            }

        //vTaskDelay(500 / portTICK_PERIOD_MS);
        vTaskDelay(0);
        // for (x = 0; x < DMALEN; x++) if (rbuf[x] != (x ^ xorval)) {
        //         printf("Rxbuf err! pos %d val %x xor %x", x, (int)rbuf[x], (int)xorval);
        //     }
        printf(".");
        fflush(stdout);
        xorval++;
        y+=0x100;
        y = y % 0x10000;
    }
}


void wifiTestTask(void *pvParameters)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = AP_NAME,
            .password = AP_PASS,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    // while(1) {
    //     vTaskDelay(20000 / portTICK_PERIOD_MS);
    // }

    // vTaskDelete(NULL);
}


void printTestTask(void *pvParameters)
{
    printf("HELLO FROM CORE");
    fflush(stdout);
    vTaskDelete(NULL);
}

void user_init(void);

void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    //vTaskDelay(1000 / portTICK_PERIOD_MS);

    // PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);

    // // esp32_configgpio(GPIO_NUM_21, OUTPUT_FUNCTION_2);
    // gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
    // gpio_matrix_out(GPIO_NUM_21, I2S1O_DATA_OUT0_IDX, 0, 0);

    // // esp32_configgpio(GPIO_NUM_20, OUTPUT_FUNCTION_2);
    // gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
    // gpio_matrix_out(GPIO_NUM_18, I2S1I_BCK_OUT_IDX, 0, 0);

    // // esp32_configgpio(GPIO_NUM_19, OUTPUT_FUNCTION_2);
    // gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
    // gpio_matrix_out(GPIO_NUM_18, I2S1I_WS_OUT_IDX, 0, 0);

    //vTaskDelay(3000/portTICK_RATE_MS);

    //xTaskCreatePinnedToCore(wifiTestTask, "wifiTestTask", 10000, NULL, 20, NULL, 0);

    wifiTestTask(NULL);

    // vTaskDelay(5000 / portTICK_PERIOD_MS);

    // tskLcd(NULL);

    // xTaskCreatePinnedToCore(tskLcd, "tskLcd", 20000, NULL, 20, NULL, 1);
    // xTaskCreatePinnedToCore(printTestTask, "printTestTask", 2048, NULL, 3, NULL, 1);
    // xTaskCreate(printTestTask, "printTestTask", 10000, NULL, 3, NULL);

    // printf("HELLO FROM CORE1");
    // fflush(stdout);

    // while(1) {
    //     vTaskDelay(20000 / portTICK_PERIOD_MS);
    // }

    user_init();

    // gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    // int level = 0;
    // while (true) {
    //     gpio_set_level(GPIO_NUM_4, level);
    //     level = !level;
    //     vTaskDelay(300 / portTICK_PERIOD_MS);
    // }
}
