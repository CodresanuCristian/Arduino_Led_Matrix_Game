//  ---------- MASTER -------
//
// Placa MASTER are controlul asupra:
// - eranului LCD
// - butonului On / Off
// - ledului On / OFF
// - butoanelor de navigare in meniu

#include <LiquidCrystal.h>
#include <Wire.h>
#include <EEPROM.h>
                             
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#define LCD 9           // Contrast LCD
#define BACKLIGHT 13    // Luminozitae LCD
#define POWERBTN 0      // Buton On/Off
#define POWERLED A3     // Led On/Off

#define BACK 12
#define RETURN 8
#define SELECT 11
#define NEXT 10

int maxNotes = 0, score=0;
byte option1 = 1, option2 = 1, option3 = 1, select = 0;
byte matrixBrightness = 1, lcdBrightness = 90, difficulty = 2, song = 0;
bool backBtnState, selectBtnState, nextBtnState, returnBtnState,
     powerBtnState, lastPowerBtnState = LOW, powerLedState = HIGH, matrixTurnOn = false;

unsigned long currentTime = 0, previousTime = 0, delayTime = 200, debounceTime = 0;

char scoreChr[7]={};


//============================================== SETUP =====================================================================

void setup() 
{
  pinMode(POWERLED, OUTPUT);
  pinMode(POWERBTN, INPUT_PULLUP);
  pinMode(BACKLIGHT, OUTPUT);
  pinMode(BACK, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(RETURN, INPUT_PULLUP);
  pinMode(NEXT, INPUT_PULLUP);
  
  pinMode(LCD, OUTPUT); 
  lcd.begin(16, 2);
  analogWrite(LCD, lcdBrightness); 
  
  Serial.begin(9600);
  Wire.begin();
}



//============================================== LOOP =====================================================================

void loop() 
{
   bool readBtn = digitalRead(POWERBTN);            
   if (readBtn != lastPowerBtnState)
   {
      currentTime = millis();                          // DEBOUNCE BUTON ON/OFF
   } 
   if ((millis() - currentTime) < 50)
   {                                                        // Daca e apasat se aprinde ledul
      if (readBtn != powerBtnState)
      {
          powerBtnState = readBtn;
          if (powerBtnState == 1){
              powerLedState = !powerLedState;
              digitalWrite(POWERLED,powerLedState);
          }
      }
   }


        
   if (powerLedState == HIGH)
   { 
      currentTime = millis(); 
      
      digitalWrite(BACKLIGHT, HIGH);                                         // Daca ledul este aprins, atunci porneste LCD-ul
      if ((currentTime - previousTime) >= delayTime){        
         backBtnState = digitalRead(BACK);
         selectBtnState = digitalRead(SELECT);
         returnBtnState = digitalRead(RETURN);
         nextBtnState = digitalRead(NEXT);
      
         previousTime = currentTime;
         TapTapMain();                                                      // Se apeleaza functia de acces a meniului
         
         if ((masterRead() == 0) and (option1 == 1) and (select == 2)){     // Daca ma joc, LCD-ul afisaza "Level " si "Score"
             lcd.clear();
             lcdWrite(0,0,1,"Level: ");
             lcd.print(option2);
             lcdWrite(1,0,0,"Score: ");
             lcd.print(scoreChr);
         }
      }
   }   
   else                                                                     // Daca ledul este stins, "oprim" LCD-ul
   {   
      lcd.clear();                  
      digitalWrite(BACKLIGHT, LOW);                                         // LCD-ul nu este fizic oprit, doar curatam ecranul si ii stingem lumina.
   }

}


//======================================= FUNCTII DEFINITE ==============================================================


void lcdWrite(byte rand, byte coloana, bool clear, char text[])
{
    if (clear == true)                                                  // Functie pentru scrierea textelor pe LCD.
       lcd.clear();                                                     
    lcd.setCursor(coloana,rand);                                        // Scriu o singura functie in loc de 3.
    lcd.print(text);   
}



bool masterRead()
{                                                                       // Verifica daca SLAVE (- a doua placa), a inceput jocul pe matrice.
   Wire.requestFrom(10,6); 
   bool finalSong = Wire.read();
   int i = 0;                                                           // Daca primeste valoarea true (adica jocul este in desfasurare), atunci citesc scorul.
   while (Wire.available()){                                            // Scorul il primesc de tip char deoarece Master nu poate citi de la Slave decat valori cuprinse 
       scoreChr[i] = Wire.read();                                       // intre 0 si 255. Drept urmare, se face conversia din integer in char (in codul SLAVE).
       i++;
   } 
  
  return finalSong;
}


void masterToSlave()                                                           // Trimitem catre Slave urmatoarele informatii:
{
   Wire.beginTransmission(10);                                                    // - Cand sa porneasca / opreasca matricea. Daca selectez optiunea MatrixBrightness, SLAVE trebuie sa porneasca matricea
   Wire.write(matrixTurnOn);                                                      //   pentru ca utilizatorul sa vada exact ce valoarea trebuie setatata. Daca ies din aceasta optiune, ledurile matricei trebuie stinse.
   Wire.write(matrixBrightness);                                                  // - Nivelul de luminozitate (0..5) pentru matrice. 
   Wire.write(song);                                                              // - Song (1..5) adica ce melodie a fost aleasa   
   Wire.write(maxNotes);                                                          // - Cate note are melodia.
   Wire.write(difficulty);                                                        // - Nivelul de dificultate ales.
   Wire.endTransmission();
}



// --------------------------   SCHEMA MENIULUI   -------------------------------
//
//    1.PLAY TAP-TAP   ->  1.SONG 1
//                     ->    ...
//                     ->  5.SONG 5
//                   
//
//    2.HIGHSCORE      ->  1.(afisare) 
//
//
//    3.OPTIONS        ->  1.DFFICULTY          ->  1.EASY
//                                              ->  2.MEDIUM
//                                              ->  3.HARD
//                                              
//                     ->  2.MATRIX BRIGHTNESS  ->  1.LEVEL 1
//                                              ->    ...
//                                              ->  5.LEVEL 5
//
//                     ->  3.LCD BRIGHTNESS     ->  1.LEVEL 1
//                                              ->    ...
//                                              ->  2.LEVEL 5

void TapTapMain(void)
{
    if (selectBtnState == 0)                                                            // 
    {                                                                                   //
       if ((option1 == 1) and (select < 2)){                                            // 
          select++;                                                                     //   Contolul pentru butoanele SELECT si RETURN  
          if (select == 2){                                                             //
            masterToSlave();                                                            //
          }                                                                             //          option1 - poate lua valori intre 1 si 3. Reprezinta meniul principal - PLAY , HIGHSCORE , OPTIONS  
       }                                                                                //          option2 - poate lua valori intre 1 si 5. Daca option1 == 1 (adica PLAY), option2 reprezinta submeniul sau (SONG1 ... SONG5), s.a.m.d. 
       if (option1 == 2)                                                                //          option3 - poate lua valori intre 1 si 5. Este un submeniu al submeniului si este valabil doar daca option1 == 3 (adica OPTIONS)
          select = 1;                                                                   //
       if ((option1 == 3) and (option2 == 1) and (select < 3))                          //
          select++;                                                                     //        
       if ((option1 == 3) and (option2 != 1) and (select < 3))                          // 
          select++;                                                                     //  
    }                                                                                   // 
                                                                                        //
    if ((returnBtnState == 0) and (select > 0))                                         //    
    {                                                                                   //
       if ((option1 == 1) && (select == 2)){                                            //
          if (masterRead() == 1){                                                       //
            select--;                                                                   //  
          }                                                                             //   
       }                                                                                //
       else if ((option1 ==3) and ((option2 == 2) or (option2 ==3 ))and (select == 2))  //
       {                                                                                //                                                   
          select--;                                                                     //  
          option3 = 1;                                                                  //  
          option2 = 1;                                                                  //
       }                                                                                //
       else                                                                             //
          select--;                                                                     //
    }                                                                                   // 
//-------------------------------------------------------------------------------------------------------------------------------

    if ((nextBtnState == 0) and (select == 0)){               //
       if (option1 < 3)                                       //
          option1++;                                          //    Controlul pentru butoanele NEXT si BACK in 
    }                                                         //    meniul principal (Start, HighScore, Options)
    if ((backBtnState == 0) and (select == 0)){               //
       if (option1 > 1)                                       //
          option1--;                                          //
    }                                                         //
    
//---------------------------------------------------------------------------------------------------

    if ((nextBtnState == 0) and (select == 1) && (option1 == 1)){  //
       if (option2 < 5)                                            //
          option2++;                                               //    Controlul pentru NEXT si BACK in 
    }                                                              //    submeniul Start (Song1 , ... , Song5)
    if ((backBtnState == 0) and (select == 1) && (option1 == 1)){  //
       if (option2 > 1)                                            //
          option2--;                                               //
    }                                                              //
    
//---------------------------------------------------------------------------------------------------------------

    if ((nextBtnState == 0) and (select == 1) and (option1 == 3)){  //
       if (option2 < 3)                                             //
          option2++;                                                //    Controlul pentru NEXT si BACK in 
    }                                                               //    submeniul Options 
    if ((backBtnState == 0) and (select == 1) and (option1 == 3)){  //    (Difficulty, Matrix Brightness, LCD Brightness)
       if (option2 > 1)                                             //
          option2--;                                                //
    }                                                               //

//-------------------------------------------------------------------------------------------------------------------
                                                                        //  
    if ((selectBtnState == 0) and (select == 3) and (option3 == 1)){    // 
       lcdWrite(0,7,0,"__");                                            //
       difficulty = 1;                                                  //   
       select = 2;                                                      //    
    }                                                                   // 
    if ((selectBtnState == 0) and (select == 3) and (option3 == 2)){    //    Marcarea selectiei facute se face prin adaugarea
       lcdWrite(0,7,0,"__");                                            //    sirului de caractere "___" deasupra textului.
       difficulty = 2;                                                  //
       select = 2;                                                      //
    }                                                                   // 
    if ((selectBtnState == 0) and (select == 3) and (option3 == 3)){    // 
       lcdWrite(0,7,0,"__");                                            //
       difficulty = 3;                                                  //
       select = 2;                                                      //
    }                                                                   //    
    if ((nextBtnState == 0) and (select == 2) and (option2 == 1)){      //
       if (option3 < 3)                                                 //
          option3++;                                                    //    Controlul pentru NEXT, BACK si SELECT in 
                                                                        //    submeniul Difficulty (Easy, Medium, Hard)   
    }                                                                   //
    if ((backBtnState == 0) and (select == 2) and (option2 == 1)){      //    
       if (option3 > 1)                                                 //
          option3--;                                                    //
       }         
//-------------------------------------------------------------------------------------------------------------------

    if ((nextBtnState == 0) and (select == 2) and (option2 == 2)){    //
       if (option3 < 5)                                               //
          option3++;                                                  //  Controlul pentru NEXT, BACK si SELECT in                                                                    // 
    }                                                                 //  submeniul Matrix Brightness (Level 1, ... , Level 5)
                                                                      //
    if ((backBtnState == 0) and (select == 2) and (option2 == 2)){    //    
       if (option3 > 1)                                               //
          option3--;                                                  //
    }                                                                 //
    if ((selectBtnState == 0) and (select == 2) and (option2 == 2))   //    
    select = 2;                                                       //
//-----------------------------------------------------------------------------------------------------------------------------

    if ((nextBtnState == 0) and (select == 2) and (option2 == 3)){    //
       if (option3 < 5){                                              //
          option3++;                                                  //
          analogWrite(LCD, option3);                                  //    Controlul pentru NEXT, BACK si SELECT in 
       }                                                              //    submeniul LCD Brightness (Level 1, ... , Level 5)
    }                                                                 //  
                                                                      //
    if ((backBtnState == 0) and (select == 2) and (option2 == 3)){    //    
       if (option3 > 1){                                              //
          option3--;                                                  //
          analogWrite(LCD, option3);                                  //
       }                                                              //
    }                                                                 //
                                                                      //
    if ((selectBtnState == 0) and (select == 2) and (option2 == 3))   //    
    select = 2;                                                       //
//-----------------------------------------------------------------------------------------------------------------------------

// ---------  CE SE AFISAZA PE LCD IN FUNCTIE DE VALORILE PE CARE LE-AU LUAT VARIABILELE DE MAI SUS ---------------------------



    if (option1 == 1)                                   // *** PLAY TAP-TAP ***
    {                            
       if (select != 2){
          lcdWrite(1,2,1,"PLAY TAPTAP");
          lcdWrite(1,14,0,">>");
       }
       if (select == 1)
       {  
          matrixTurnOn = false;
          switch (option2)
          {
             case 1:{                                                //- Song 1
                lcdWrite(0,1,1,"The Pirates of");
                lcdWrite(1,3,0,"Caribbean");
                lcdWrite(1,14,0,">>");    
                song = 1;
                maxNotes = 203;
                break;    
             }
             case 2:{                                                //- Song 2
                lcdWrite(1,0,1,"<<");
                lcdWrite(1,3,0,"Mario Theme");
                lcdWrite(1,14,0,">>");
                song = 2;  
                maxNotes = 78;
                break;
             }
             case 3:{                                                //- Song 3
                lcdWrite(1,0,1,"<<");         
                lcdWrite(1,5,0,"SONG 3");
                lcdWrite(1,14,0,">>");
                song = 3;
                break;
             }
             case 4:{                                                //- Song 4
                lcdWrite(1,0,1,"<<");
                lcdWrite(1,5,0,"SONG 4");
                lcdWrite(1,14,0,">>");  
                song = 4;
                break;
             }
             case 5:{                                                //- Song 5
                lcdWrite(1,0,1,"<<");
                lcdWrite(1,5,0,"SONG 5");
                song = 5;
                break;
             }
          } // end switch
       }
       
    }  //end if (option1 == 1)

    if (option1 == 2)                                  // *** HIGHSCORE ***
    {                          
       lcdWrite(1,0,1,"<<");
       lcdWrite(1,3,0,"HIGH SCORE");
       lcdWrite(1,14,0,">>");
       if (select == 1)
       {
          score = atoi(scoreChr);
          int highScore = EEPROM.read(0);
          Serial.print("Score: ");
          Serial.println(score);
          Serial.print("HighScore: ");
          Serial.println(highScore);
          if(score > highScore){
              EEPROM.write(0,score);
              Serial.println("Score mai mare ca HighScore");
          }
          else
              EEPROM.write(0,0);

          highScore = EEPROM.read(0);
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("High Score is: ");
          lcd.setCursor(5,1);
          lcd.print(highScore);
       }  
    }

    if (option1 == 3){                                 // *** OPTIONS ***
       lcdWrite(1,0,1,"<<");
       lcdWrite(1,5,0,"OPTIONS");
       if (select != 0)
       {                                                             
          switch (option2)
          {
             case 1:{                                             // ** Difficulty
                lcdWrite(1,3,1,"DIFFICULTY");
                lcdWrite(1,14,0,">>"); 
                matrixTurnOn = false;    
                masterToSlave();                                            
                if (select == 2)
                {
                   switch (option3)
                   {
                      case 1:{                                         // - Easy
                         lcdWrite(1,6,1,"EASY");
                         lcdWrite(1,14,0,">>");
                         if (difficulty != 1)
                            lcdWrite(0,5,0,"      "); 
                         if (difficulty == 1)
                            lcdWrite(0,6,0,"____"); 
                         break; 
                      }
                      case 2:{                                         // - Medium
                         lcdWrite(1,0,1,"<<");
                         lcdWrite(1,5,0,"MEDIUM");
                         lcdWrite(1,14,0,">>");
                         if (difficulty != 2)
                            lcdWrite(0,5,0,"      "); 
                         if (difficulty == 2)
                            lcdWrite(0,5,0,"______"); 
                         break; 
                      }
                      case 3:{                                         //- Hard
                         lcdWrite(1,0,1,"<<");
                         lcdWrite(1,6,0,"HARD");
                         if (difficulty != 3)
                            lcdWrite(0, 5, 0, "      "); 
                         if (difficulty == 3)
                            lcdWrite(0,6,0,"____"); 
                         break; 
                      }
                   }    // end switch 
                  // masterToSlave();
                } 
                break;
             }        // end difficulty
             case 2:{                                      //** Matrix Brightness
                lcdWrite(1, 0, 1,"<<");
                lcdWrite(0, 5, 0,"MATRIX");
                lcdWrite(1, 3, 0,"BRIGHTNESS");
                lcdWrite(1, 14, 0,">>");
                if (select == 2)
                {
                   lcdWrite(1, 0, 1,"BRIGHTNESS: ");
                   lcd.print(option3);
                   matrixBrightness = option3 * 3;
                   matrixTurnOn = true;
                   masterToSlave();
                }
                break;
             }  // end matrix brightness
             case 3:{                                     //** LCD Brightness
                lcdWrite(1,0,1,"<<");
                lcdWrite(0,6,0,"LCD");
                lcdWrite(1,3,0,"BRIGHTNESS");
                if (select == 2)
                {
                   lcdWrite(1, 0, 1,"BRIGHTNESS: ");
                   lcd.print(option3); 
                   lcdBrightness = option3 * 20 + 10;
                   analogWrite(LCD, lcdBrightness);  
                } 
                break;     
             }  // end lcd brightness
          }    
       }    // end options   
    }     
    
}
     
