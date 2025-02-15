# 🔥 Lâmpada Matriz - Projeto Final EmbarcaTech

## 🚀 Sobre o Projeto
Lâmpada Matriz é um sistema embarcado interativo que detecta comandos sonoros e acende uma matriz de LED's com base na intensidade do som. O projeto foi desenvolvido como parte do programa **EmbarcaTech**, um programa de capacitação profissional técnica destinado a alunos de nível superior em **Tecnologias da Informação e Comunicação (TIC)** e áreas correlatas, com foco em **Sistemas Embarcados**.

## 🎯 Funcionalidades
✅ Detecta sons altos e os converte em comandos
✅ Acende e apaga a matriz de LED's conforme a detecção de som
✅ Exibe a contagem de comandos no display OLED SSD1306
✅ Ajuste de brilho dos LED's (no código)
✅ Consumo de energia adaptável, de acordo com a regulagem do brilho

## 🛠️ Hardware Utilizado
- 🔹 **Placa BitDogLab** (RP2040)
- 🎤 **Microfone de eletreto (ADC)**
- 💡 **Matriz de LED's Neopixel (25 LED's)**
- 🖥️ **Display OLED SSD1306 (I2C)**
- 🔌 Resistores e fiação para conexão

## 📜 Bibliotecas Utilizadas
- `pico/stdlib.h` - Funções básicas para o RP2040
- `hardware/adc.h` - Leitura de valores do microfone via ADC
- `hardware/dma.h` - Manipulação do acesso direto à memória
- `hardware/i2c.h` - Comunicação com o display OLED
- `inc/neopixel.c` - Controle dos LED's Neopixel
- `inc/ssd1306.h` - Controle do display OLED

## ⚙️ Instalação e Configuração
### 1️⃣ Configuração do Ambiente
Certifique-se de ter o SDK do **Raspberry Pi Pico** instalado. Siga os passos abaixo:
```sh
sudo apt update && sudo apt install cmake gcc-arm-none-eabi build-essential
```

### 2️⃣ Clonando o Repositório
```sh
git clone https://github.com/seuusuario/lampada_matriz.git
cd lampada_matriz
```

### 3️⃣ Compilação do Código
```sh
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 4️⃣ Envio para a Placa
Conecte o **RP2040** no modo **BOOTSEL** e envie o arquivo `.uf2`:
```sh
cp lampada_matriz.uf2 /media/$USER/RPI-RP2/
```

## 📸 Demonstração
✨ **Aguardando imagens ou vídeos do projeto em ação!**

## 🧑‍💻 Desenvolvimento
👤 **Carlos André** - Desenvolvedor principal

🚀 _Desenvolvido no contexto do programa EmbarcaTech._

