//////////////////////////////////////////////////////////////////
// Project: ESP32 Bluetooth LTI Board & RTC3231 No.2           ///
//////////////////////////////////////////////////////////////////
#define TITLE           "BLUETOOTH LTI BOARD2"
#include <SPI.h>
#include <Wire.h>
#include <DMD32.h>
#include <BluetoothSerial.h>
#include <EEPROM.h>
#include "RTClib.h"
#include "fonts/SystemFont5x7.h"
#include "fonts/SystemFont3x5.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"
#include "fonts/Font_6x14.h"

#define DISPLAYS_ACROSS 8 //-> Number of P10 panels used, side to side.
#define DISPLAYS_DOWN 1
#define EEPROM_SIZE 20

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dateStringBuff[50], timeStringBuff[50], timeDife[50], data, array[10];
String t, t_day, t_month, t_year, t_hour, t_min, t_sec, t_date;
int wowl, smh, fac, rc, pbb, day, month, year, day2, month2, year2, flag, count , count2, count3, total, x, y, z, change;

int aSMH = 0, bSMH = 0, cSMH = 0, dSMH = 0;
int aFAC = 0, bFAC = 0, cFAC = 0, dFAC = 0;
int aRC  = 0, bRC  = 0, cRC  = 0, dRC  = 0;
int aPBB = 0, bPBB = 0, cPBB = 0, dPBB = 0;
int aWOWL = 0, bWOWL, cWOWL = 0, dWOWL = 0;

unsigned long previousMillis = 0;   
const long interval = 7000;

void setup(void) {

  dmd.clearScreen( true );
  Serial.begin(115200);
  SerialBT.begin(TITLE);
  EEPROM.begin(EEPROM_SIZE);

  delay(1000);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("OK");

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    GetDate();
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // January 21, 2014 at 3am you would call:
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //-> Set the time and date based on your computer time and date. Use this line of code if it doesn't work in "if (! rtc.isrunning())"
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // If the time and date are successfully set, then deactivate the code line (make the code a comment), then re-upload the code.

  //read the last data from flash memory
  flag = EEPROM.read(8);
  if (flag == 1) { //-------------------data available
    Serial.println("Data available//");
    cWOWL = EEPROM.read(0); aWOWL = EEPROM.read(10); dWOWL = EEPROM.read(16);
    cWOWL = cWOWL + dWOWL;
    
    cSMH = EEPROM.read(1); aSMH = EEPROM.read(9);  dSMH = EEPROM.read(14); 
    cSMH = cSMH + dSMH;
    
    cFAC = EEPROM.read(2); aFAC = EEPROM.read(11); dFAC = EEPROM.read(17);
    cFAC = cFAC + dFAC;
    
    cRC = EEPROM.read(3); aRC = EEPROM.read(12); dRC = EEPROM.read(18);
    cRC = cRC + dRC;
    
    cPBB = EEPROM.read(4); aPBB = EEPROM.read(13); dPBB = EEPROM.read(15);
    cPBB = cPBB + dPBB;
    
    day = EEPROM.read(5); month = EEPROM.read(6); year = EEPROM.read(7); 
    
    smh = fac = rc = pbb = 0; 
    
    smh = (53824 * dSMH) + (cSMH * 232) + aSMH;
          
    fac = (53824 * dFAC) + (cFAC * 232) + aFAC; 
    
    rc  = (53824 * dRC) + (cRC * 232) + aRC; 
    
    pbb = (53824 * dPBB) + (cPBB * 232) + aPBB;
    
    wowl = (53824 * dWOWL) + (cWOWL * 232) + aWOWL;  
  }
  else { //-----------------------------first time
    wowl = 0; smh = 0; fac = 0; rc = 0; day = 1; month = 2; year = 20; day2 = 1; month2 = 2; year2 = 20; Serial.println(flag);
  }

  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen(true);
}

void GetDate() {
  DateTime now = rtc.now();
  t_day = now.day(), DEC;
  t_month = now.month(), DEC;
  t_year = now.year(), DEC;
  t_hour = now.hour(), DEC;
  t_min = now.minute(), DEC;
  t_sec = now.second(), DEC;
  t_date = daysOfTheWeek[now.dayOfTheWeek()];

  count = t_day.toInt(); // hari ni
  count2 = t_month.toInt();// bulan ni
  count3 = t_year.toInt();
  count3 = count3 - 2000;// tahun ni

  //delay(5000);
 // Serial.println("Done");
}

void CountDayDiff() {
  //kira2 untuk wdwl - panel 5
  z = year % 4;
  if (year > count3) { //lebih tahun
    total = 0;
  }
  else if (year == count3 && month > count2) { //tahun sama tpi lebih bulan
    total = 0;
  }
  else if (year == count3 && month == count2 && day > count) { //tahun sama, bulan sama tapi hari lebih
    total = 0;
  }
  else {
    if (count2 == month && year == count3) {
      total = count - day;
    }
    
    //--------1. buat hari -> 1
    else { //dh sebulan
      int g = year;
      if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12 ) { //bulan ganjil
        x = 31 - day + 1;
      }
      else if (month == 2) {  //bulan genap
        if (z == 0) { //z == 20 || z == 16 || z == 24 || z == 28
          x = 29 - day + 1;
        } //tahun ganjil
        else {
          x = 28 - day + 1;
        }
      }
      else {
        x = 30 - day + 1;
      }

      if (month == 12) {
        y = 1;
        g++;
      }
      else {
        y = month + 1;
      }
      
      //--------2. buat bulan -> 1
      ////loop
      while (count2 != y || count3 != g) {
        if (y == 1 || y == 3 || y == 5 || y == 7 || y == 8 || y == 10 || y == 12 ) { //bulan ganjil
          x = x + 31;
        }
        else if (y == 2) {  //februari
          if (z == 0) {
            x = x + 29;
          } //tahun ganjil
          else {
            x = x + 28;
          }
        }
        else { //bulan genap
          x =  x + 30;
        }
        
        //--------3. buat tahun sama
        if (y == 12) {
          y = 1;
          g++;              
          while (count3 != g){
            int y2 = g;
            y2 = y2 % 4; 
            if (y2 == 0){ //tahun ganjil
              x = x + 366;
            }
            else{
              x = x + 365;
            }
            g++;
          }
        }

        //--------4. buat bulan sama
        else {
          y = y + 1;
        } 
      }
       
      //--------5. tambah baki
      total = x + count - 1;
    }
  }

  if (year == 20) {
      total--;
  }
  SaveData();
}

void PrintData() {
//  ========================================================================== change display for panel 1
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (change == 1) { change = 0; }
    else { change = 1; }
  }
  
  if (change == 1) {
  //========================================================================== panel 1 - Last date of incident
    String xday = Convert(day) ;
    xday.toCharArray(array, xday.length() + 1);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString( 1, 4, array, xday.length(), GRAPHICS_NORMAL);  //1 +12
    dmd.drawString( 13, 4, ".", 1, GRAPHICS_NORMAL); //13 +5
    
    String xmonth = Convert(month) ;
    xmonth.toCharArray(array, xmonth.length() + 1);
    dmd.drawString( 18, 4, array, xmonth.length(), GRAPHICS_NORMAL); //18 +12
    dmd.drawString( 30, 4, ".20", 3, GRAPHICS_NORMAL);  //30 +18
    
    String xyear = Convert(year) ;
    xyear.toCharArray(array, xyear.length() + 1);
    dmd.drawString( 48, 4, array, xyear.length(), GRAPHICS_NORMAL); //48
  }
  else {
  //========================================================================== panel 1 - previous best performance
    String xpbb = Convert(pbb) ;
    xpbb.toCharArray(array, xpbb.length() + 1);
    dmd.selectFont(Font_6x14);
    dmd.drawString( 1, 0, array, xpbb.length(), GRAPHICS_NORMAL); //1
  }
  
  //========================================================================== panel 2 - work days without LTI
  CountDayDiff(); //kira2
  String xtotal = Convert(total);
  xtotal.toCharArray(timeDife, xtotal.length() + 1);
  dmd.selectFont(Font_6x14);
  dmd.drawString( 66, 0, timeDife, xtotal.length(), GRAPHICS_NORMAL); //66

  //========================================================================== panel 3 - smart man hours
  String xsmh = Convert(smh) ;
  xsmh.toCharArray(array, xsmh.length() + 1);
  dmd.drawString( 130, 0, array, xsmh.length(), GRAPHICS_NORMAL); //130

  //========================================================================== panel 4 - first aid cases
  String xfac = Convert(fac) ;
  xfac.toCharArray(array, xfac.length() + 1);
  dmd.drawString( 194, 0, array, xfac.length(), GRAPHICS_NORMAL); //194

  //========================================================================== panel 5 - recordable cases
  String xrc = Convert(rc) ;
  xrc.toCharArray(array, xrc.length() + 1);
  dmd.drawString( 226, 0, array, xrc.length(), GRAPHICS_NORMAL);

  //========================================================================== print in Serial for prof
  Serial.print(day); Serial.print("/"); Serial.print(month); Serial.print("/"); Serial.print(year); Serial.print(" ");
  Serial.print(t_day); Serial.print("/"); Serial.print(t_month); Serial.print("/"); Serial.print(t_year);
  Serial.print(" "); Serial.print(t_hour); Serial.print(":"); Serial.print(t_min); Serial.print(":"); Serial.print(t_sec);
  Serial.print(" "); Serial.print(t_date); Serial.print(" WDWL:"); Serial.print(total); Serial.print(" SMH:");
  Serial.print(smh); Serial.print(" FAC:"); Serial.print(fac); Serial.print(" RC:"); Serial.print(rc); Serial.print(" PBB:"); Serial.println(pbb);
  
}

String Convert(int sInput) {
  if (sInput < 10)
  {
    return"0" + String(sInput);
  }
  else
  {
    return String(sInput);
  }
}

void loop(void) {
  dmd.scanDisplayBySPI();
  dmd.clearScreen(true);

  GetDate();
  PrintData();

  if (SerialBT.available())
  {
    data = SerialBT.read();
    SerialBT.write(Serial.read());
    Serial.write(SerialBT.read());
    switch (data)
    {
      case 'A': { //===================================================== tambah smh - panel 4
          if (smh < 99999999){
            smh++;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'B': { //===================================================== tolak smh
          if (smh > 0){
            smh--;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'C': { //===================================================== tambah smh 10 
          if (smh <= 99999990){
            smh = smh + 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'D': { //===================================================== tolak smh 10
          if (smh >= 10){
            smh = smh - 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'E': { //===================================================== tambah smh 100
          if (smh <= 99999900){
            smh = smh + 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'F': { //===================================================== tolak smh 100
          if (smh >= 100){
            smh = smh - 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'G': { //===================================================== tambah smh 1000
          if (smh <= 99999000){
            smh = smh + 1000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'H': { //===================================================== tolak smh 1000
          if (smh >= 1000){
            smh = smh - 1000;
            SaveData();
          }
          data = 0;
          break;
        } 
      case 'p': { //===================================================== tambah smh 10k
          if (smh <= 99990000){
            smh = smh + 10000;
            SaveData();
          }
          data = 0;
          break;
        }
       case 'q': { //===================================================== tolak smh 10k
          if (smh >= 10000){
            smh = smh - 10000;
            SaveData();
          }
          data = 0;
          break;
        } 
       case 'r': { //===================================================== tambah smh 100k
          if (smh <= 99900000){
            smh = smh + 100000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 's': { //===================================================== tolak smh 100k
          if (smh >= 100000){
            smh = smh - 100000;
            SaveData();
          }
          data = 0;
          break;
        } 
      case 'I': { //===================================================== tambah fac - panel 5
          if (fac < 9999){
            fac++;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'J': { //===================================================== tolak fac
          if (fac > 0) {
            fac--;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'K': { //===================================================== tambah fac 10 
          if (fac < 9990){
            fac = fac + 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'L': { //===================================================== tolak fac 10
          if (fac >= 10) {
            fac = fac - 10;
            SaveData();
          }
          data = 0;
          break;
        }
       case 'M': { //===================================================== tambah fac 100
          if (fac < 9900){
            fac = fac + 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'N': { //===================================================== tolak fac 100
          if (fac >= 100) {
            fac = fac - 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'O': { //===================================================== tambah fac 1000
          if (fac < 9000){
            fac = fac + 1000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'P': { //===================================================== tolak fac 1000
          if (fac >= 1000){
            fac = fac - 1000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'Q': { //===================================================== tambah rc - panel 6
          if (rc < 9999){
            rc++;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'R': { //===================================================== tolak rc
          if (rc > 0){
            rc--;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'S': { //===================================================== tambah rc 10 
          if (rc < 9990){
            rc = rc + 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'T': { //===================================================== tolak rc 10
          if (rc >= 10){
            rc = rc - 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'U': { //===================================================== tambah rc 100
          if (rc < 9900){
            rc = rc + 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'V': { //===================================================== tolak rc 100
          if (rc >= 100){
            rc = rc - 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'W': { //===================================================== tambah rc 1000
          if (rc < 9000){
            rc = rc + 1000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'X': { //===================================================== tolak rc 1000
          if (rc >= 1000){
            rc = rc - 1000;          
            SaveData();
          }
          data = 0;
          break;
        } 
       case 'Y': { //===================================================== tambah pbb - panel 
          if (pbb < 99999999){
            pbb++;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'Z': { //===================================================== tolak pbb
          if (pbb > 0){
            pbb--;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'a': { //===================================================== tambah pbb 10 
          if (pbb <= 99999990){
            pbb = pbb + 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'b': { //===================================================== tolak pbb 10
          if (pbb >= 10){
            pbb = pbb - 10;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'c': { //===================================================== tambah pbb 100
          if (pbb <= 99999900){
            pbb = pbb + 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'd': { //===================================================== tolak pbb 100
          if (pbb >= 100){
            pbb = pbb - 100;
            SaveData();
          }
          data = 0;
          break;
        }
      case 't': { //===================================================== tambah pbb 1000
          if (pbb <= 99999000){
            pbb = pbb + 1000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'u': { //===================================================== tolak pbb 1000
          if (pbb >= 1000){
            pbb = pbb - 1000;
            SaveData();
          }
          data = 0;
          break;
        } 
      case 'v': { //===================================================== tambah pbb 10k
          if (pbb <= 99990000){
            pbb = pbb + 10000;
            SaveData();
          }
          data = 0;
          break;
        }
       case 'w': { //===================================================== tolak pbb 10k
          if (pbb >= 10000){
            pbb = pbb - 10000;
            SaveData();
          }
          data = 0;
          break;
        } 
       case 'x': { //===================================================== tambah pbb 100k
          if (pbb <= 99900000){
            pbb = pbb + 100000;
            SaveData();
          }
          data = 0;
          break;
        }
      case 'y': { //===================================================== tolak pbb 100k
          if (pbb >= 100000){
            pbb = pbb - 100000;
            SaveData();
          }
          data = 0;
          break;
        } 
      case 'e': { //===================================================== tambah day - panel 2
          day++;
          SaveData();
          data = 0;
          break;
        }
      case 'f': { //===================================================== tolak day
          day--;
          SaveData();
          data = 0;
          break;
        }
      case 'g': { //===================================================== tambah month
          month++;
          SaveData();
          data = 0;
          break;
        }
      case 'h': { //===================================================== tolak month
          month--;
          SaveData();
          data = 0;
          break;
        }
      case 'i': { //===================================================== tambah year
          year++;
          SaveData();
          data = 0;
          break;
        }
      case 'j': { //===================================================== tolak year
          year--;
          SaveData();
          data = 0;
          break;
        }
      case 'k': { //===================================================== reset smh
          smh = aSMH = cSMH = dSMH = 0;
          
          SaveData();
          data = 0;
          break;
        }
       case 'l': { //===================================================== reset fac
          fac = aFAC = cFAC = dFAC = 0;
          
          SaveData();
          data = 0;
          break;
        }
       case 'm': { //===================================================== reset rc
          rc = aRC = cRC = dRC = 0;
          
          SaveData();
          data = 0;
          break;
        } 
       case 'n': { //===================================================== reset pbb
          pbb = aPBB = cPBB = dPBB = 0;
          
          SaveData();
          data = 0;
          break;
        }
        case 'o': { //===================================================== reset wowl
          wowl = 0; aWOWL = cWOWL = 0; 
          day = 1; month = 2; year = 20;
          //Serial.print(" reset ");
          
          SaveData();
          data = 0;
          break;
        }
    }
  }
}

void SaveData(){
 // Serial.print("Data saved "); 
  //Serial.print(" "); Serial.println(smh); Serial.print(" "); Serial.println(fac); Serial.print(" "); Serial.println(rc); Serial.print(" "); Serial.println(pbb);
  bWOWL = total; cWOWL = 0; dWOWL = 0;
   if (bWOWL > 232){
     while ( bWOWL > 0){
       if ( bWOWL <= 232){
         aWOWL = bWOWL;
         bWOWL = 0;
       }
       else{
        if (cWOWL <= 231){
         cWOWL++;
        }
        else {
          dWOWL++;
          cWOWL = 0;
        }
         bWOWL = bWOWL - 232;
       }
     }
    }
    else{
      bWOWL = bWOWL;
      cWOWL = dWOWL = 0;
    }
    
  bSMH = smh;  cSMH = 0; dSMH = 0;
  if (bSMH > 232){
   while ( bSMH > 0){
      if ( bSMH <= 232){
         aSMH = bSMH;
          bSMH = 0;
       }
      else{
         if (cSMH <= 231){
          cSMH++;
        }
        else{
          dSMH++;
          cSMH = 0;
        }
         bSMH = bSMH - 232;
      }
    }
   }
    else{
       aSMH = bSMH;
       cSMH = 0; dSMH = 0;
    } 
          
  bFAC = fac;  cFAC = 0; dFAC = 0;
  if (bFAC > 232){
   while ( bFAC > 0){
      if ( bFAC <= 232){
         aFAC = bFAC;
          bFAC = 0;
       }
      else{
         if (cFAC <= 231){
          cFAC++;
        }
        else{
          dFAC++;
          cFAC = 0;
        }
         bFAC = bFAC - 232;
      }
    }
   }
    else{
       aFAC = bFAC;
       cFAC = 0; dFAC = 0;
    } 
          
   bRC = rc; cRC = 0; dRC = 0;
   if (bRC > 232){
     while ( bRC > 0){
      if ( bRC <= 232){
          aRC = bRC;
          bRC = 0;
      }
      else{
        if (cRC <= 231){
         cRC++;
        }
        else{
          dRC++;
          cRC = 0;
        }
          bRC = bRC - 232;
      }
     }
   }
    else{
       aRC = bRC;
       cRC = 0; dRC = 0;
   }
  
   bPBB = pbb; cPBB = 0; dPBB = 0;     
  if (bPBB > 232){
    while ( bPBB > 0){
      if ( bPBB <= 232){
         aPBB = bPBB;
         bPBB = 0;
       }
      else{
        if (cPBB <= 231){
         cPBB++;
        }
        else{
          dPBB++;
          cPBB = 0;
        }
         bPBB = bPBB - 232;
       }
      }
   }
   else{
     aPBB = bPBB;
     cPBB = 0; dPBB = 0; 
   }
    
  EEPROM.write(10, aWOWL); 
  EEPROM.write( 0, cWOWL);
  EEPROM.write( 16, dWOWL);
  
  EEPROM.write( 9, aSMH); 
  EEPROM.write( 1, cSMH);
  EEPROM.write( 14, dSMH);
  
  EEPROM.write(11, aFAC); 
  EEPROM.write( 2, cFAC);
  EEPROM.write( 17, dFAC);
  
  EEPROM.write(12, aRC); 
  EEPROM.write( 3, cRC);
  EEPROM.write( 18, dRC);
  
  EEPROM.write(13, aPBB); 
  EEPROM.write(4, cPBB);
  EEPROM.write(15, dPBB);

  EEPROM.write(5, day);
  EEPROM.write(6, month);  
  EEPROM.write(7, year);   flag = 1;
  EEPROM.write(8, flag);
  EEPROM.commit();

  /*Serial.print("( "); Serial.print(eSMH); Serial.print(" * 12487168) + ("); 
  Serial.print("( "); Serial.print(dSMH); Serial.print(" * 53824) + ("); 
  Serial.print(cSMH); Serial.print(" * 232) + "); 
  Serial.print(aSMH); Serial.print("  = "); Serial.println(smh); */
}
