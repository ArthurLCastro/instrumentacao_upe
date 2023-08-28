/*
  Instrumentacao - UPE/Poli
  Turma ZH 2023.1 - Agosto de 2023

  Projeto 02 - Medidor de tensao e corrente continua, potencia e resistencia eletrica
*/

#define DEBUG_MEDICOES

// Importacao de Bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Mapeamento de Hardware
#define LCD_ADDR    0x27
#define V_PIN       A0
#define I_PIN       A1
#define R_REF_PIN   A2
#define RX_PIN      A3

// Constantes globais
#define VCC         4.9

#define R1          100000.0
#define R2          22000.0
#define R3          0.28
#define R4          4700.0
#define R5          5000.0
#define R6          1000.0

#define CTE_V   (R1+R2)/R2      // Constante para calculo de tensao
#define CTE_R   (R4+R6)/R6      // Constante para calculo de resistencia

// Declaracao de variaveis globais
float tensao_V, corrente_mA, potencia_mW, resistencia_Ohms;

// Instanciacao de objetos
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// Funcoes auxiliares
void inicializa_display() {
  lcd.init();
  lcd.backlight();
}

void apresentacao() {
  lcd.setCursor(0,0);
  lcd.print(" Instrumentacao ");
  lcd.setCursor(0,1);
  lcd.print(" Multimetro  DC ");
  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("Turma ZH 2023.1 ");
  lcd.setCursor(0,1);
  lcd.print("    UPE/Poli    ");
  delay(2000);
}

void atualiza_display(float voltage, float current, float power, float resistance){
  lcd.clear();

  // -------------------- Voltimetro --------------------
  lcd.setCursor(0, 0);
  lcd.print(voltage, 2);
  lcd.setCursor(6, 0);
  lcd.print("V");

  // -------------------- Amperimetro --------------------
  lcd.setCursor(10, 0);
  lcd.print(current, 2);
  lcd.setCursor(14, 0);
  lcd.print("mA");

  // -------------------- Wattimetro --------------------
  lcd.setCursor(0, 1);

  // Exibe valores em Mili Watts
  if (power < 1000) {
    lcd.print(power, 1);
    lcd.setCursor(5, 1);
    lcd.print("m");

  // Exibe valores em Watts
  } else {
    lcd.print(power/1000.0);
  }

  lcd.setCursor(6, 1);
  lcd.print("W");

  // -------------------- Ohmimetro --------------------
  lcd.setCursor(10, 1);

  // Fora do intervalo de medicao do equipamento (100 a 10k Ohms)
  if (resistance <= 50 || resistance >= 15000) {
    lcd.print("-----");

  // Dentro do intervalo de medicao do equipamento
  } else {

    // Exibe valores em Ohms
    if (resistance < 1000) {
      lcd.print(resistance, 1);

    // Exibe valores em Kilo Ohms
    } else {
      lcd.print(resistance/1000.0);
      lcd.setCursor(14, 1);
      lcd.print("k");
    }
  }

  // Exibe o simbolo de Ohm (https://www.squids.com.br/arduino/index.php/tabelas/216-tabela-de-caracteres-para-display-lcd)
  lcd.setCursor(15, 1);
  lcd.write(B11110100);
}

float get_instant_voltage(unsigned int analog_pin) {
  return analogRead(analog_pin)*(VCC/1023.0);
}

void setup() {
  inicializa_display();
  apresentacao();

  Serial.begin(9600);     // Para debug

  pinMode(V_PIN, INPUT);
  pinMode(I_PIN, INPUT);
  pinMode(R_REF_PIN, INPUT);
  pinMode(RX_PIN, INPUT);
}

void loop() {
  tensao_V = get_instant_voltage(V_PIN)*CTE_V;

  corrente_mA = get_instant_voltage(I_PIN)/R3*1000;

  potencia_mW = corrente_mA * tensao_V;

  float rx_pin_voltage = get_instant_voltage(RX_PIN);
  float rref_pin_voltage = get_instant_voltage(R_REF_PIN);

  resistencia_Ohms = (rx_pin_voltage*R5)/(rref_pin_voltage*CTE_R-rx_pin_voltage);

  #ifdef DEBUG_MEDICOES
    Serial.print("Tensão: ");
    Serial.print(tensao_V);
    Serial.print(" V");

    Serial.print(" | Corrente: ");
    Serial.print(corrente_mA);
    Serial.print(" mA");

    Serial.print(" | Potência: ");
    Serial.print(potencia_mW);
    Serial.print(" mW");

    Serial.print(" | Resistência: ");
    Serial.print(resistencia_Ohms);
    Serial.println(" Ohms");
  #endif

  atualiza_display(tensao_V, corrente_mA, potencia_mW, resistencia_Ohms);

  delay(100);
}
