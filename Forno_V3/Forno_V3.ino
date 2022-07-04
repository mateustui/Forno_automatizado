/* FORNO MECAL - Versão 1 primeiro teste
  // forno somente com válvula solenoide para gás (sem centelhador)
  // envio de dados de tempo e temperatura em .csv para o USB
  // Inclusão de escrita e leitura da EEPROM para dados das temp Max e Min e tempo
  //Criado por Mateus Falcão 21/09/2021
*/


#include <TimerOne.h>
#include <Ch376msc.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "max6675.h"
#define GPIO_SO       A2
#define GPIO_CS       A3
#define GPIO_CLK      A4
#define led_ligado    A0      
#define led_desligado A1
#define centelhador   2 //A5
//#define sensor_chama  13
#define btn_l         4
#define btn_c         6
#define btn_r         5
#define solenoide     A5//2
#define motor         3
uint8_t opc = 1;
uint8_t flag_usb = 0;
uint8_t flag_tambor = 0;
uint8_t opc_2 = 0;
uint8_t l = 0, c = 0 , r = 0;
uint8_t estado = 0;
uint16_t count = 0;
uint16_t count_aux = 0;
uint16_t min_aux = 0;
byte tempo;
byte temp_max;
byte temp_min;
int endereco_max = 0;
int endereco_min = 2;
int endereco_tempo = 4;
float temperatura = 0.0;
String msg;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
//Important! First create a soft serial object, after create a Ch376 object
//SoftwareSerial mySerial(6, 5); // RX, TX pins on arduino
//..............................................................................................................................
Ch376msc flashDrive(Serial, 9600); // Ch376 object with software serial
MAX6675 termopar(GPIO_CLK, GPIO_CS, GPIO_SO);
void setup() {
  //Serial.begin(9600);
  // mySerial.begin(9600);// Important! First initialize soft serial object and after Ch376

  pinMode(led_ligado, OUTPUT);
  pinMode(led_desligado, OUTPUT);
  pinMode(centelhador, OUTPUT);
  pinMode(solenoide, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(btn_l, INPUT);
  pinMode(btn_c, INPUT);
  pinMode(btn_r, INPUT);
  digitalWrite(led_desligado, HIGH);
  digitalWrite(led_ligado, LOW);
  digitalWrite(centelhador, LOW);
  digitalWrite(solenoide, LOW);
  temp_max = EEPROM.read(endereco_max);
  temp_min = EEPROM.read(endereco_min);
  tempo = EEPROM.read(endereco_tempo);
  Timer1.initialize();
  Timer1.attachInterrupt(Leitura_sensor, 1000000);
  Timer1.stop();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   MECAL INOX   ");
  lcd.setCursor(0, 1);
  lcd.print("TEL:(27)33712230");
  delay(3000);
  flashDrive.init();
}

void loop() {
  if (flashDrive.checkIntMessage()) {
    if (flashDrive.getDeviceStatus()) {
      //Serial.println(F("Flash drive atached!"));
      flag_usb = 1;
    } else {
      //Serial.println(F("Flash drive detached!"));
      flag_usb = 0;
    }
  }
  l = digitalRead(btn_l);
  c = digitalRead(btn_c);
  r = digitalRead(btn_r);
  delayMicroseconds(15000);
  switch (estado)
  {
    case 0: //  desligado
      {
        flag_tambor = 0;
        Timer1.stop();
        lcd.setCursor(0, 0);
        lcd.print("<     LIGAR    >");
        if (flag_usb)
        {
          lcd.setCursor(0, 1);
          lcd.print("USB CONECTADO   ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("USB DESCONECTADO");
        }
        count = 0;
        count_aux = 0;
        min_aux = 0;
        digitalWrite(led_desligado, HIGH);
        digitalWrite(led_ligado, LOW);
        digitalWrite(centelhador, LOW);
        digitalWrite(solenoide, LOW);
        digitalWrite(motor, LOW);
        if (!c)
        {
          while (c == 0)
          {
            c = digitalRead(btn_c);
            delayMicroseconds(15000);
          }
          estado = 1;
          lcd.clear();
        }
        if (!r)
        {
          while (r == 0)
          {
            r = digitalRead(btn_r);
            delayMicroseconds(15000);
          }
          estado = 3;
          lcd.clear();
        }
        if (!l)
        {
          while (l == 0)
          {
            l = digitalRead(btn_l);
            delayMicroseconds(15000);
          }
          estado = 5;
          lcd.clear();
        }
        break;
      }
    case 1://ignicao
      {
        digitalWrite(solenoide, HIGH);
        lcd.setCursor(0, 0);
        lcd.print("ACENDA AS CHAMAS");
        if (flag_usb)
        {
          lcd.setCursor(0, 1);
          lcd.print("USB CONECTADO   ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("USB DESCONECTADO");
        }
        if (!l)
        {
          while (l == 0)
          {
            l = digitalRead(btn_l);
            delayMicroseconds(15000);
          }
          estado = 0;
          lcd.clear();
          Timer1.stop();
        }

        if (!c)
        {
          while (c == 0)
          {
            c = digitalRead(btn_c);
            delayMicroseconds(15000);
          }
          lcd.clear();
          estado = 2;
          Timer1.start();

        }
        if (!r)
        {
          while (r == 0)
          {
            r = digitalRead(btn_r);
            delayMicroseconds(15000);
          }

          lcd.clear();
          estado = 2;
          Timer1.start();
        }
        /*if (flag_igni == 0)
          {
          if (count < 5)
          {
            digitalWrite(solenoide, HIGH);
            if (digitalRead(sensor_chama))
            {
              cont_chama++;
            }
          }
          if (count > 2 && count < 5)
          {
            digitalWrite(centelhador, HIGH);
          }
          if (cont_chama >= 2 && count > 5)
          {
            cont_chama = 0;
            estado = 2;
            count = 0;
            digitalWrite(centelhador, LOW);
          }
          if (cont_chama < 2 && count > 5)
          {
            digitalWrite(solenoide, LOW);
            digitalWrite(centelhador, LOW);
          }
          if (count > 10)
          {
            count = 0;
            tentativas++;
          }
          if (tentativas > 2)
          {
            estado = 6;
            lcd.clear();
          }
          flag_igni = 1;
          }*/
        break;
      }
    case 2://ligado
      {
        digitalWrite(motor, HIGH);
        digitalWrite(led_ligado, HIGH);
        digitalWrite(led_desligado, LOW);
          if (temperatura > temp_max)
          {
            digitalWrite(solenoide, LOW);
            flag_tambor = 1;
          }
        
        
        if ((flag_tambor == 1 ) && (temperatura < temp_min))
        {
          estado = 0;
          lcd.clear();
        }
        if (!c)
        {
          while (c == 0)
          {
            c = digitalRead(btn_c);
            delayMicroseconds(15000);
          }
          estado = 0;
          lcd.clear();
        }
        /*  if (flag_igni == 0)
          {
            //Serial.println(cont_chama);
            if (!digitalRead(sensor_chama))
            {
              cont_chama++;
            }
            if (cont_chama > 4)
            {
              estado = 6;
              lcd.clear();
            }
            flag_igni = 1;
          }*/
        //Serial.println(temp);
        switch (opc)
        {
          case 0:
            {
              break;
            }
          case 1:
            {
              min_aux++;
              msg = (String)min_aux + "," + (String)temperatura + "\n";
              msg = (String)msg;
              char msg2[10];
              msg.toCharArray(msg2, msg.length() + 1);
              //Serial.print(msg2);
              flashDrive.setFileName("MECAL.CSV");  //set the file name
              //flashDrive.openFile();
              if (flashDrive.openFile() == ANSW_USB_INT_SUCCESS) {             //open the file
                flashDrive.moveCursor(CURSOREND);     //if the file exist, move the "virtual" cursor at end of the file, with CURSORBEGIN we actually rewrite our old file
                //flashDrive.moveCursor(flashDrive.getFileSize()); // is almost the same as CURSOREND, because we put our cursor at end of the file
              }
              if (flashDrive.getFreeSectors()) { //check the free space on the drive
                flashDrive.writeFile(msg2, msg.length()); //string, string length
              } else {
                //COLOCAR MSG DE PEN DRIVE CHEIO
              }
              flashDrive.closeFile();
              opc = 0;
              break;
            }
        }
        break;
      }
    case 3://temperatura max
      {
        switch (opc_2)
        {
          case 0:
            {
              lcd.setCursor(0, 0);
              lcd.print("< TEMP. MAXIMA >");
              lcd.setCursor(4, 1);
              lcd.print(temp_max);
              lcd.setCursor(7, 1);
              lcd.print(" GRAUS       ");
              if (!r)
              {
                while (r == 0)
                {
                  r = digitalRead(btn_r);
                  delayMicroseconds(15000);
                }
                estado = 4;
                lcd.clear();
              }
              if (!c)
              {
                while (c == 0)
                {
                  c = digitalRead(btn_c);
                  delayMicroseconds(15000);
                }
                opc_2 = 1;
              }
              if (!l)
              {
                while (l == 0)
                {
                  l = digitalRead(btn_l);
                  delayMicroseconds(15000);
                }
                estado = 0;
                lcd.clear();
              }
              break;
            }
          case 1:
            {
              if (temp_max < 100)
              {
                lcd.setCursor(6, 1);
                lcd.print(" ");
              }
              lcd.setCursor(0, 0);
              lcd.print("  TEMP. MAXIMA  ");
              lcd.setCursor(0, 1);
              lcd.print("<   ");
              lcd.setCursor(4, 1);
              lcd.print(temp_max);
              lcd.setCursor(7, 1);
              lcd.print(" GRAUS  >");
              if (!r)
              {
                while (r == 0)
                {
                  r = digitalRead(btn_r);
                  delayMicroseconds(15000);
                }
                if (temp_max >= 250)
                {
                  temp_max = 250;
                }
                else
                {
                  temp_max += 1;
                }
              }
              if (!c)
              {
                while (c == 0)
                {
                  c = digitalRead(btn_c);
                  delayMicroseconds(15000);
                }
                opc_2 = 0;
                lcd.clear();
                EEPROM.write(endereco_max, temp_max);
              }
              if (!l)
              {
                while (l == 0)
                {
                  l = digitalRead(btn_l);
                  delayMicroseconds(15000);
                }
                if (temp_max <= 30)
                {
                  temp_max = 30;
                }
                else
                {
                  temp_max -= 1 ;
                }
              }
              break;
            }
        }
        break;
      }

    case 4://temperatura min
      {
        switch (opc_2)
        {
          case 0:
            {
              lcd.setCursor(0, 0);
              lcd.print("< TEMP. MINIMA >");
              lcd.setCursor(4, 1);
              lcd.print(temp_min);
              lcd.setCursor(7, 1);
              lcd.print(" GRAUS       ");
              if (!r)
              {
                while (r == 0)
                {
                  r = digitalRead(btn_r);
                  delayMicroseconds(15000);
                }
                estado = 5;
                lcd.clear();
              }
              if (!c)
              {
                while (c == 0)
                {
                  c = digitalRead(btn_c);
                  delayMicroseconds(15000);
                }
                opc_2 = 1;
              }
              if (!l)
              {
                while (l == 0)
                {
                  l = digitalRead(btn_l);
                  delayMicroseconds(15000);
                }
                estado = 3;
                lcd.clear();
              }
              break;
            }
          case 1:
            {
              if (temp_min < 100)
              {
                lcd.setCursor(6, 1);
                lcd.print(" ");
              }
              lcd.setCursor(0, 0);
              lcd.print("  TEMP. MINIMA  ");
              lcd.setCursor(0, 1);
              lcd.print("<   ");
              lcd.setCursor(4, 1);
              lcd.print(temp_min);
              lcd.setCursor(7, 1);
              lcd.print(" GRAUS  >");
              if (!r)
              {
                while (r == 0)
                {
                  r = digitalRead(btn_r);
                  delayMicroseconds(15000);
                }
                if (temp_min >= temp_max)
                {
                  temp_min = temp_max;
                }
                else
                {
                  temp_min += 1;
                }
              }
              if (!c)
              {
                while (c == 0)
                {
                  c = digitalRead(btn_c);
                  delayMicroseconds(15000);
                }
                opc_2 = 0;
                lcd.clear();
                EEPROM.write(endereco_min, temp_min);
              }
              if (!l)
              {
                while (l == 0)
                {
                  l = digitalRead(btn_l);
                  delayMicroseconds(15000);
                }
                if (temp_min <= 30)
                {
                  temp_min = 30;
                }
                else
                {
                  temp_min -= 1;
                }
              }
              break;
            }
        }
        break;
      }
 
    case 5://gira tambor
      {

        switch (opc_2)
        {
          case 0:
            {
              digitalWrite(motor, LOW);
              lcd.setCursor(0, 0);
              lcd.print("<   CILINDRO   >");
              lcd.setCursor(0, 1);
              lcd.print("     LIGAR      ");
              if (!r)
              {
                while (r == 0)
                {
                  r = digitalRead(btn_r);
                  delayMicroseconds(15000);
                }
                digitalWrite(motor, LOW);
                estado = 0;
                lcd.clear();
                opc_2 = 0;
              }
              if (!c)
              {
                while (c == 0)
                {
                  c = digitalRead(btn_c);
                  delayMicroseconds(15000);
                }
                opc_2 = 1;
                lcd.clear();
              }
              if (!l)
              {
                while (l == 0)
                {
                  l = digitalRead(btn_l);
                  delayMicroseconds(15000);
                }
                digitalWrite(motor, LOW);
                estado = 4;
                lcd.clear();
                opc_2 = 0;
              }
              break;
            }
          case 1:
            {
              digitalWrite(motor, HIGH);
              lcd.setCursor(0, 0);
              lcd.print("<   CILINDRO   >");
              lcd.setCursor(0, 1);
              lcd.print("    DESLIGAR    ");
              if (!r)
              {
                while (r == 0)
                {
                  r = digitalRead(btn_r);
                  delayMicroseconds(15000);
                }
                digitalWrite(motor, LOW);
                estado = 0;
                lcd.clear();
                opc_2 = 0;
              }
              if (!c)
              {
                while (c == 0)
                {
                  c = digitalRead(btn_c);
                  delayMicroseconds(15000);
                }
                estado = 5;
                opc_2 = 0;
                lcd.clear();
              }
              if (!l)
              {
                while (l == 0)
                {
                  l = digitalRead(btn_l);
                  delayMicroseconds(15000);
                }
                digitalWrite(motor, LOW);
                estado = 4;
                lcd.clear();
                opc_2 = 0;
              }
            }
        }
        break;
        break;

        /*
                digitalWrite(centelhador, LOW);
                digitalWrite(solenoide, LOW);
                lcd.setCursor(0, 0);
                lcd.print("ALERTA....      ");
                lcd.setCursor(0, 1);
                lcd.print("SEM CHAMAS      ");
                tentativas = 0;
                if (!c)
                {
                  while (c == 0)
                  {
                    c = digitalRead(btn_c);
                    delayMicroseconds(15000);
                  }
                  estado = 0;
                  lcd.clear();
                }
                digitalWrite(led_ligado, HIGH);
                digitalWrite(led_desligado, HIGH);
                break;*/
      }
  }
}


void Leitura_sensor()
{
  temperatura = termopar.readCelsius();

  if (estado == 2)
  {
    displayTime(count);
  }
  if (count_aux == 60)
  {
    opc = 1;
    count_aux = 0;
  }
  count_aux++;
  count++;

}



void displayTime(int temp) {
  //Serial.println(temperatura);

  int  h, m, s, resto;
  h = temp / 3600;
  resto = temp % 3600;
  m = resto / 60;
  s = resto % 60;
  /*Serial.print(h);
    Serial.print(":");
    Serial.print(m);
    Serial.print(":");
    Serial.println(s);
  */

  lcd.setCursor(0, 0);
  lcd.print(h / 10 % 10);
  lcd.setCursor(1, 0);
  lcd.print(h  % 10);
  lcd.setCursor(2, 0);
  lcd.print(":");
  lcd.setCursor(3, 0);
  lcd.print(m / 10 % 10);
  lcd.setCursor(4, 0);
  lcd.print(m % 10);
  lcd.setCursor(5, 0);
  lcd.print(":");
  lcd.setCursor(6, 0);
  lcd.print(s / 10 % 10);
  lcd.setCursor(7, 0);
  lcd.print(s % 10);
  lcd.setCursor(8, 0);
  lcd.print(" ");
  lcd.setCursor(9, 0);
  lcd.print(temperatura, 1);
  lcd.setCursor(14, 0);
  lcd.print((char)223);
  lcd.setCursor(15, 0);
  lcd.print("C");
  if (flag_usb)
  {
    lcd.setCursor(0, 1);
    lcd.print("USB CONECTADO   ");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("USB DESCONECTADO");
  }

}
