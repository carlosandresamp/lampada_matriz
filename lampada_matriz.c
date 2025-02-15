#include <stdio.h> // Inclui a biblioteca padrão de entrada e saída
#include <math.h>  // Inclui a biblioteca matemática para operações como raiz quadrada
#include <string.h> // Inclui a biblioteca de manipulação de strings
#include "pico/stdlib.h" // Inclui a biblioteca padrão para o microcontrolador Pico
#include "hardware/adc.h" // Inclui a biblioteca para manipulação do ADC (Conversor Analógico-Digital)
#include "hardware/dma.h" // Inclui a biblioteca para manipulação do DMA (Acesso Direto à Memória)
#include "inc/neopixel.c" // Inclui o arquivo de implementação para controle de LED's NeoPixel
#include "inc/ssd1306.h" // Inclui o arquivo de implementação para controle do display OLED SSD1306
#include "hardware/i2c.h" // Inclui a biblioteca para comunicação I2C

// Definições de pinos
#define MIC_CHANNEL 2 // Define o canal do microfone
#define MIC_PIN (26 + MIC_CHANNEL) // Define o pino do microfone com base no canal
#define LED_PIN 7 // Define o pino para os LED's
#define LED_COUNT 25 // Define a quantidade de LED's
#define I2C_SDA 14 // Define o pino SDA para I2C
#define I2C_SCL 15 // Define o pino SCL para I2C

// Parâmetros do ADC
#define ADC_CLOCK_DIV 96.f // Define o divisor de clock para o ADC
#define SAMPLES 200 // Define o número de amostras a serem coletadas
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajusta o valor lido do ADC para a faixa de 0 a 3.3V
#define ADC_MAX 3.3f // Define o valor máximo do ADC
#define ADC_STEP (3.3f / 5.f) // Define o passo de intensidade do ADC

// Variáveis globais
uint dma_channel; // Variável para armazenar o canal DMA
dma_channel_config dma_cfg; // Variável para armazenar a configuração do canal DMA
uint16_t adc_buffer[SAMPLES]; // Buffer para armazenar as amostras do ADC
int clap_count = 0; // Contador de comandos (palmas)
bool led_on = false; // Estado da matriz de LED's (ligado ou desligado)
uint8_t brightness = 32; // Brilho dos LED's (0 apaga, 32 é baixo CONFORTÁVEL À VISÃO, 255 é alto CUIDADO!!! ⚠)

// Funções
void sample_mic(); // Declaração da função para amostrar o microfone
float mic_power(); // Declaração da função para calcular a potência do microfone
uint8_t get_intensity(float v); // Declaração da função para calcular a intensidade do som
void display_status(); // Declaração da função para atualizar o display OLED
void set_brightness(uint8_t brightness); // Declaração da função para definir o brilho dos LED's

int main() {
    stdio_init_all(); // Inicializa a entrada e saída padrão
    sleep_ms(5000); // Aguarda 5 segundos antes de continuar

    // Inicialização do I2C e OLED
    i2c_init(i2c1, 400000); // Inicializa a comunicação I2C com uma taxa de 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA para função I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL para função I2C
    gpio_pull_up(I2C_SDA); // Ativa o resistor de pull-up no pino SDA
    gpio_pull_up(I2C_SCL); // Ativa o resistor de pull-up no pino SCL
    ssd1306_init(); // Inicializa o display OLED SSD1306

    // Inicialização da matriz de LED's
    npInit(LED_PIN, LED_COUNT); // Inicializa a matriz de LED's NeoPixel

    // Configuração do ADC
    adc_gpio_init(MIC_PIN); // Inicializa o pino do microfone para uso com ADC
    adc_init(); // Inicializa o ADC
    adc_select_input(MIC_CHANNEL); // Seleciona o canal do microfone para leitura
    adc_fifo_setup(true, true, 1, false, false); // Configura o FIFO do ADC
    adc_set_clkdiv(ADC_CLOCK_DIV); // Define o divisor de clock do ADC

    // Configuração do DMA
    dma_channel = dma_claim_unused_channel(true); // Reivindica um canal DMA não utilizado
    dma_cfg = dma_channel_get_default_config(dma_channel); // Obtém a configuração padrão do canal DMA
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Define o tamanho dos dados transferidos como 16 bits
    channel_config_set_read_increment(&dma_cfg, false); // Não incrementa o endereço de leitura
    channel_config_set_write_increment(&dma_cfg, true); // Incrementa o endereço de escrita
    channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Define a requisição de dados para o ADC

    while (true) { // Loop infinito
        sample_mic(); // Chama a função para amostrar o microfone
        float avg = mic_power(); // Calcula a potência média do sinal do microfone
        avg = 2.f * fabs(ADC_ADJUST(avg)); // Ajusta e normaliza o valor da potência
        uint intensity = get_intensity(avg); // Obtém a intensidade do som detectado

        // Verifica se a intensidade é suficiente para contar uma palma
        if (intensity >= 3) { // Ajuste o valor conforme necessário
            clap_count++; // Incrementa o contador de palmas
            led_on = !led_on; // Alterna o estado da matriz de LED's
            if (led_on) {
                set_brightness(brightness); // Acende a matriz com o brilho atual
            } else {
                npClear(); // Apaga a matriz de LED's
                npWrite(); // Atualiza a matriz de LED's
            }
            display_status(); // Atualiza o display OLED com o estado atual
            sleep_ms(500); // Aguarda 500 ms para evitar múltiplas contagens
        }

        // Se a matriz estiver acesa, mantenha-a acesa
        if (led_on) {
            set_brightness(brightness); // Mantém o brilho da matriz de LED's
        }
    }
}

// Função para capturar o áudio do microfone usando DMA
void sample_mic() {
    adc_fifo_drain(); // Drena o FIFO do ADC
    adc_run(false); // Para a conversão do ADC
    dma_channel_configure(dma_channel, &dma_cfg, adc_buffer, &(adc_hw->fifo), SAMPLES, true); // Configura o DMA para transferir dados do ADC para o buffer
    adc_run(true); // Inicia a conversão do ADC
    dma_channel_wait_for_finish_blocking(dma_channel); // Aguarda a conclusão da transferência DMA
    adc_run(false); // Para a conversão do ADC
}

// Calcula a potência do sinal do microfone
float mic_power() {
    float avg = 0.f; // Inicializa a média
    for (uint i = 0; i < SAMPLES; ++i) // Loop para somar os quadrados das amostras
        avg += adc_buffer[i] * adc_buffer[i]; // Soma o quadrado de cada amostra
    avg /= SAMPLES; // Calcula a média
    return sqrt(avg); // Retorna a raiz quadrada da média
}

// Calcula a intensidade do som detectado
uint8_t get_intensity(float v) {
    uint count = 0; // Inicializa o contador de intensidade
    while ((v -= ADC_STEP / 5) > 0.f) // Enquanto o valor for maior que zero, decrementa e conta
        ++count; // Incrementa o contador
    return count; // Retorna a contagem de intensidade
}

// Atualiza o display OLED com o estado do LED e a contagem de comandos
void display_status() {
    // Buffer de exibição
    uint8_t ssd[ssd1306_buffer_length]; // Cria um buffer para o display
    memset(ssd, 0, ssd1306_buffer_length); // Zera o buffer para evitar lixo de dados

    // Converte o número de comandos para string
    char buffer[20]; // Cria um buffer para armazenar a string de comandos
    sprintf(buffer, "Comandos: %d", clap_count); // Formata a string com a contagem de comandos

    // Exibe estado do LED
    char led_status[10]; // Cria um buffer para armazenar o estado do LED
    if (led_on) {
        sprintf(led_status, "LED ON"); // Se os LED's estiverem acesos, define a string como "LED ON"
    } else {
        sprintf(led_status, "LED OFF"); // Se os LED's estiverem apagados, define a string como "LED OFF"
    }

    // Escreve os textos no buffer
    ssd1306_draw_string(ssd, 5, 20, buffer);    // Exibe "Comandos: X" no buffer
    ssd1306_draw_string(ssd, 5, 5, led_status); // Exibe "LED ON" ou "LED OFF" no buffer

    // Define a área de renderização corretamente
    struct render_area frame_area; // Cria uma estrutura para definir a área de renderização
    frame_area.start_column = 0; // Define a coluna inicial
    frame_area.end_column = ssd1306_width - 1; // Define a coluna final
    frame_area.start_page = 0; // Define a página inicial
    frame_area.end_page = ssd1306_n_pages - 1; // Define a página final
    frame_area.buffer_length = ssd1306_buffer_length; // Define o comprimento do buffer

    // Renderiza no display
    render_on_display(ssd, &frame_area); // Chama a função para renderizar o conteúdo no display OLED
}

// Define o brilho da matriz de LED's
void set_brightness(uint8_t brightness) {
    for (int i = 0; i < LED_COUNT; i++) { // Loop para cada LED na matriz
        npSetLED(i, (255 * brightness) / 255, (255 * brightness) / 255, (255 * brightness) / 255); // Define a cor do LED como branco com o brilho especificado
    }
    npWrite(); // Atualiza a matriz de LED's com as novas configurações de brilho
}