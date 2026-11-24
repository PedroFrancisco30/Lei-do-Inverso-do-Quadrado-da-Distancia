# Fotometria e Caracterização de Fotossensor (LDR) via ESP32

Projeto desenvolvido para a disciplina de **Laboratório de Física Experimental** (Engenharia da Computação — 4º Período, CEFET-MG Campus V – Divinópolis).

**Integrantes:** Felipe Silva, Pedro Francisco, Pedro Costa e Samuel.

---

## 📌 Objetivo
Verificar experimentalmente a **Lei do Inverso do Quadrado da Distância** ($E \propto 1/d^2$) para a propagação luminosa e caracterizar a resposta elétrica não linear de um fotorresistor (**LDR** de sulfeto de cádmio – CdS), determinando sua constante física de sensibilidade ($\gamma$) por meio de **linearização logarítmica** utilizando um sistema de aquisição de dados (DAQ) microcontrolado com **ESP32**.

---

## 🛠️ Hardware Utilizado
* **Microcontrolador:** ESP32 (ADC de 12 bits, $0$ a $4095$ para $0$ a $3,3\text{ V}$)
* **Sensor:** Fotorresistor (LDR) de CdS
* **Resistor de referência:** $10\ \text{k}\Omega$ (em série com o LDR formando um divisor de tensão)
* **Display:** OLED 0,96" I2C (SSD1306)
* **Fonte luminosa:** Lanterna de smartphone (intensidade constante)
* **Auxiliares:** Protoboard de 830 pontos, cabos DuPont e régua milimetrada de $30\text{ cm}$

### Esquema de Ligações (Pinagem)

| Componente | Pino | Ligação no ESP32 / Protoboard |
| :--- | :--- | :--- |
| **Display OLED** | `VCC` / `GND` | `3V3` ($3,3\text{ V}$) / `GND` |
| **Display OLED** | `SCL` / `SDA` | `GPIO 22` / `GPIO 21` (I2C) |
| **Divisor de Tensão (LDR)** | Terminal 1 do LDR | `3V3` ($3,3\text{ V}$) |
| **Divisor de Tensão (LDR)** | Terminal 2 do LDR + Resistor $10\text{ k}\Omega$ | **`GPIO 34`** (nó central de leitura) |
| **Resistor $10\text{ k}\Omega$** | Outro terminal | `GND` |

---

## 💻 Funcionamento do Firmware (`projeto_Optica_Fisica.ino`)
1. **Filtragem Digital (*Oversampling*):** A função `lerADCFiltrado()` realiza **200 leituras consecutivas** no pino `GPIO 34` (intervaladas em $500\ \mu\text{s}$) e calcula a média aritmética, reduzindo ruídos elétricos da leitura analógica.
2. **Cálculo Físico em Tempo Real:** A partir do valor médio do ADC, o ESP32 calcula a tensão $V_{out}$ sobre o resistor de $10\text{ k}\Omega$ e a resistência instantânea do LDR:
   $$V_{out} = 3,3 \cdot \left(\frac{ADC}{4095}\right) \qquad R_{LDR} = 10000 \cdot \left(\frac{4095 - ADC}{ADC}\right)$$
3. **Interface:** Exibe em tempo real no display OLED a distância alvo, o valor filtrado de `ADC`, a tensão em Volts e a resistência $R_{LDR}$ em Ohms. Ao enviar um comando via Monitor Serial (`115200 baud`), exporta a linha formatada em CSV com $\ln(d)$ e $\ln(R_{LDR})$ calculados.

### Bibliotecas Necessárias (Arduino IDE)
* `Wire.h` e `math.h` (nativas do pacote `esp32 by Espressif Systems`)
* `Adafruit GFX Library`
* `Adafruit SSD1306`

---

## 📊 Resultados Obtidos (Resumo)
* **Faixa de medição:** $2,0\text{ cm}$ a $15,0\text{ cm}$ ($R_{LDR}$ variou de $7.689\ \Omega$ a $34.868\ \Omega$).
* **Regressão Linear no SciDAVis** ($\ln(R_{LDR}) = A \cdot \ln(d) + B$):
  * $A = (0,737 \pm 0,017)$
  * $B = (8,46 \pm 0,03)$
  * $R^2 \approx 0,99999$ | $\chi^2 \approx 0,0075$
* **Constante de Sensibilidade:** $\gamma_{exp} = \frac{A}{2} = (0,369 \pm 0,009)$
