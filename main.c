#include <stdio.h>
#include "pico/stdlib.h"

#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

#define ds1_pin 28
#define shcp1_pin 27
#define stcp1_pin 26
#define ds0_pin 20
#define shcp0_pin 21
#define stcp0_pin 22

//    a
//  f   b
//    g
//  e   c
//    d   h
int bits[10] = {
        //abcdefgh
        0b11111100,  // 0
        0b01100000,  // 1
        0b11011010,  // 2
        0b11110010,  // 3
        0b01100110,  // 4
        0b10110110,  // 5
        0b10111110,  // 6
        0b11100000,  // 7
        0b11111110,  // 8
        0b11110110   // 9
};

void shift_led(bool value){
    gpio_put(stcp1_pin, false);
    gpio_put(shcp1_pin, false);
    gpio_put(ds1_pin, value);
    gpio_put(shcp1_pin, true);
    gpio_put(stcp1_pin, true);
    gpio_put(shcp1_pin, false);
    
}

void shift_bit(bool value){
    gpio_put(ds0_pin, value);
    gpio_put(shcp0_pin, false);
    gpio_put(shcp0_pin, true);
    gpio_put(shcp0_pin, false);
}
void shift_word(int32_t value){
    gpio_put(stcp0_pin, false);
    for (size_t i = 0; i < 32; i++)
    {
        shift_bit((value) & (1<<(i)));
    }
    gpio_put(stcp0_pin, true);
}

static void writeCharToCli(EmbeddedCli *embeddedCli, char c) {
    putchar_raw(c);
}

int main() {
    // Initialize LED pin
    gpio_init(ds1_pin);
    gpio_set_dir(ds1_pin, GPIO_OUT);
    gpio_init(shcp1_pin);
    gpio_set_dir(shcp1_pin, GPIO_OUT);
    gpio_init(stcp1_pin);
    gpio_set_dir(stcp1_pin, GPIO_OUT);

    gpio_init(ds0_pin);
    gpio_set_dir(ds0_pin, GPIO_OUT);
    gpio_init(shcp0_pin);
    gpio_set_dir(shcp0_pin, GPIO_OUT);
    gpio_init(stcp0_pin);
    gpio_set_dir(stcp0_pin, GPIO_OUT);

    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->maxBindingCount = 16;
    // EmbeddedCli *cli = embeddedCliNew(config);
    EmbeddedCli *cli = embeddedCliNewDefault();
    void writeChar(EmbeddedCli *embeddedCli, char c);
    cli->writeChar = writeCharToCli;
    char c = (char)getchar_timeout_us(100);
    embeddedCliReceiveChar(cli, c);

    // Initialize chosen serial port
    stdio_init_all();

    // Loop forever
    size_t count = 0;
    size_t count1 = 0;
    shift_led(true);
    while (true) {
        char c = (char)getchar_timeout_us(10);
        embeddedCliReceiveChar(cli, c);
        embeddedCliProcess(cli);

        if(count1==100){
            shift_led(true);
            count1=0;
        }
        else{
            shift_led(false);
        }
        count1++;
        int32_t buf = 0;
        buf = (bits[count / 1000 % 10] << 24) | (bits[count / 100 % 10] << 16) | (bits[count / 10 % 10]<<8) | bits[count % 10];
        shift_word(buf);
        count++;
        sleep_ms(10);
    }
}
