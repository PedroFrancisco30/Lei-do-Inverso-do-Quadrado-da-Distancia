#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int PINO_LDR = 34;

const int num_medidas = 9;
float distancias[num_medidas] = {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0, 12.0, 15.0};
int indice_atual = 0;

const float R_FIXO = 10000.0; // Resistor fixo de 10k Ohms no divisor de tensão

void imprimirCabecalho() {
  Serial.println();
  Serial.println("=========================================================");
  Serial.println("EXPERIMENTO DE FOTOMETRIA E CARACTERIZACAO DO SENSOR LDR");
  Serial.println("Aperte ENTER p/ medir | Digite 'r' + ENTER p/ reiniciar");
  Serial.println("=========================================================");
  Serial.println("Distancia_cm,ADC_Medio,Tensao_V,Resistencia_Ohms,ln_d,ln_R");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  analogReadResolution(12);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Erro ao iniciar o display OLED!"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  imprimirCabecalho();
}

// Filtro de Média Móvel (Oversampling de 200 amostras para remover ruído)
float lerADCFiltrado() {
  long soma = 0;
  for(int i = 0; i < 200; i++) {
    soma += analogRead(PINO_LDR);
    delayMicroseconds(500);
  }
  return (float)soma / 200.0;
}

void loop() {
  float adc = lerADCFiltrado();
  if (adc >= 4094.0) adc = 4094.0;
  if (adc <= 1.0) adc = 1.0;

  float tensao = (adc / 4095.0) * 3.3;
  // Fórmula para LDR ligado no 3.3V e Resistor fixo de 10k ligado no GND:
  float r_ldr = R_FIXO * ((4095.0 - adc) / adc);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("LAB FISICA - OPTICA");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  if (indice_atual < num_medidas) {
    display.setCursor(0, 16);
    display.print("Posicione em: ");
    display.print(distancias[indice_atual], 0);
    display.println(" cm");
    
    display.setCursor(0, 30);
    display.print("ADC: ");
    display.print(adc, 0);
    display.print(" (");
    display.print(tensao, 2);
    display.println("V)");

    display.setCursor(0, 42);
    display.print("R_LDR: ");
    display.print(r_ldr, 0);
    display.println(" Ohms");

    display.setCursor(0, 55);
    display.print("[Aperte ENTER no PC]");
  } else {
    display.setCursor(0, 22);
    display.println("COLETA FINALIZADA!");
    display.setCursor(0, 38);
    display.println("Digite 'r' + ENTER");
    display.setCursor(0, 48);
    display.println("para medir de novo.");
  }
  display.display();

  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "r" || comando == "R") {
      indice_atual = 0;
      imprimirCabecalho();
    } else if (indice_atual < num_medidas) {
      float d = distancias[indice_atual];
      float ln_d = log(d);
      float ln_R = log(r_ldr);

      Serial.printf("%.1f,%.1f,%.3f,%.1f,%.4f,%.4f\n", d, adc, tensao, r_ldr, ln_d, ln_R);
      Serial.flush();

      indice_atual++;
    }
  }
  
  delay(50);
}
