/*
  Instrumentacao - UPE/Poli
  Turma ZH 2023.1 - Junho de 2023

  Projeto 01 - Termometro digital (Pt100)
*/

// Importacao de Bibliotecas
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Macros para controle de Debugs (Lembrete: Alguns dos debugs podem atrasar significativamente o processamento)
// #define DEBUG_TEMP_CALC       // Quando definida apresenta detalhes do calculo de temperatura
// #define DEBUG_AMOSTRAGEM      // Quando definida apresenta detalhes da aquisicao de dados
// #define DEBUG_DISPLAY_LCD     // Quando definida apresenta detalhes do Display LCD

// Mapeamento de Hardware
#define LCD_ADDR                        0x27
#define PT100_PIN                       A0

// Constantes globais
#define INTERVALO_ENTRE_AMOSTRAS_MS     10
#define NUMERO_DE_AMOSTRAS_PARA_MEDIA   10

#define V_IN                            5.0
#define AMP_GAIN                        20.08

// Declaracao de variaveis globais
unsigned long previous_time = 0;
float temperatura = 0, temperatura_anterior = 0;
float adc_value = 0, adc_sum = 0;
int average_counter = 0;

// Instanciacao de objetos
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

void inicializa_display() {
  lcd.init();
  lcd.backlight();
}

void apresentacao() {
  lcd.setCursor(0,0);
  lcd.print(" Instrumentacao ");
  lcd.setCursor(0,1);
  lcd.print("Termometro Pt100");
  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("Turma ZH 2023.1 ");
  lcd.setCursor(0,1);
  lcd.print("    UPE/Poli    ");
  delay(2000);
}

float calculate_temperature_10bits(int bits) {
  /*
    Considerando para os calculos de temperatura:
      ADC de 10 bits (2^10 - 1 = 1023)
      Va = V_IN / 2  -->  Va = 2.5 V
      R3 = 98 ohms
      alpha_pt100 = 0.3946
        R_pt100 a 0ºC = 100 ohms
        R_pt100 a 50ºC = 199.73 ohms
  */

  float amp_v_out = 0, v_ba = 0, v_b = 0, r_pt100 = 0, temp_celcius = 0;
  // float v_a = V_IN/2;
  float v_a = 2.5;

  amp_v_out = (V_IN / 1023) * bits;
  v_ba = amp_v_out / AMP_GAIN;
  v_b = v_a + v_ba;

  r_pt100 = v_b*(98/(V_IN-v_b));
  temp_celcius = (r_pt100 - 100) / 0.3946;

  #ifdef DEBUG_TEMP_CALC
    Serial.print("\nValor do ADC: ");
    Serial.println(bits);
    Serial.println("--------------------");
    Serial.print("amp_v_out: ");
    Serial.println(amp_v_out, 5);
    Serial.print("v_ba: ");
    Serial.println(v_ba, 5);
    Serial.print("v_b: ");
    Serial.println(v_b, 5);
    Serial.print("v_a: ");
    Serial.println(v_a, 5);
    Serial.println("--------------------");
    Serial.print("r_pt100: ");
    Serial.println(r_pt100, 5);
    Serial.print("temp_celcius: ");
    Serial.println(temp_celcius, 5);
  #endif

  return temp_celcius;
}

void setup() {
  inicializa_display();
  apresentacao();

  Serial.begin(9600);     // Para debug

  pinMode(PT100_PIN, INPUT);

  lcd.setCursor(0,0);
  lcd.print("  Temperatura:  ");
  lcd.setCursor(0,1);
  lcd.print("    --.--  C    ");
  lcd.setCursor(10,1);
  lcd.write(B11011111);
}

void loop() {
  if ((millis() - previous_time) >= INTERVALO_ENTRE_AMOSTRAS_MS) {
    previous_time = millis();

    int adc_instant_value = 0;

    adc_instant_value = analogRead(PT100_PIN);
    adc_sum += adc_instant_value;
    // adc_sum += analogRead(PT100_PIN);
    average_counter++;

    #ifdef DEBUG_AMOSTRAGEM
      Serial.print("\nmillis: ");
      Serial.println(previous_time);
      Serial.print("adc_instant_value: ");
      Serial.println(adc_instant_value);
      Serial.print("average_counter: ");
      Serial.print(average_counter);
      Serial.print(" de ");
      Serial.print(NUMERO_DE_AMOSTRAS_PARA_MEDIA);
      Serial.println(" amostras");
      Serial.print("adc_sum: ");
      Serial.println(adc_sum);
    #endif

    if (average_counter == NUMERO_DE_AMOSTRAS_PARA_MEDIA) {
      adc_value = adc_sum / NUMERO_DE_AMOSTRAS_PARA_MEDIA;
      temperatura = calculate_temperature_10bits(round(adc_value));

      #ifdef DEBUG_AMOSTRAGEM
        Serial.println("--------------------");
        Serial.print("adc_value: ");
        Serial.println(adc_value);
      #endif

      average_counter = 0;
      adc_sum = 0;
    }
  }

  if (temperatura != temperatura_anterior) {
    temperatura_anterior = temperatura;
  
    #ifdef DEBUG_DISPLAY_LCD
      Serial.print("\n Imprimindo nova temperatura no Display LCD: ");
      Serial.print(temperatura, 2);
      Serial.println(" ºC");
    #endif

    // Imprime nova temperatura
    lcd.setCursor(4,1);
    lcd.print(temperatura);
    lcd.setCursor(10,1);
    lcd.write(B11011111);
    lcd.setCursor(11,1);
    lcd.print("C");
  }
}
