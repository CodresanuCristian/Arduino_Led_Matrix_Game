
// ---------------- SLAVE ---------------
//
// Placa SLAVE controleaza:
//
// - matricea
// - butoanele de gameplay
// - cele trei leduri
// - difuzorul  

#include "LedControl.h";
#include <Wire.h>
#include "pitches.h";          // Librarie in care sunt definite frecventele pentru functia tone()   -->   https://gist.github.com/mikeputnam/2820675

LedControl lc = LedControl(11,13,12,1);

#define LED1 10
#define LED2 9    
#define LED3 8

#define BTN1 7
#define BTN2 6    // Cele 3 butoane pentru gameplay
#define BTN3 5

#define SPEAKER 4

byte matrix[9][8] = {0};   // Linia 9 suplimentara este pentru a face verificari

int currentMatrixTime = 0;
int previousMatrixTime = 0;   
int delayMatrixTime = 140;       //(easy -200 | medium - 140 | hard - 100)

int currentSongTime = 0;
int previousSongTime = 0;
int delaySongTime = 0; 

int btnState1, btnState2, btnState3;

float difficultyLevel = 1.4;    //(easy - 1.8 | medium - 1.4 | hard - 1.1)
int score = 0;

int thisNote;, i = 0; 
byte song, maxNotes = 0, difficulty, matrixBrightness;
bool matrixTurnOn = false, finalSong = false;
char scoreChr[7];


//===================================== PIRATES OF THE CARIBBEAN ========================================================================================================

byte keyNotesSong1[] = { 
  5,7,1,1,0,1,2,3,3,0,3,4,2,2,0,1,7,1,0,5,7,1,1,0,1,2,3,3,0,3,4,2,2,0,1,7,1,0,5,7,1,1,0,          // Convertesc notele din notesPirates[] pentru a sti 
  1,3,4,4,0,4,5,6,6,0,5,4,5,1,0,1,2,3,3,0,4,5,1,0,1,3,2,2,0,3,1,2,0,1,1,1,2,3,3,0,                // pe care coloana a matricei trebuie trimisa fiecare nota 
  3,1,2,0,1,1,1,2,3,3,0,3,4,2,2,0,1,7,1,0,5,7,1,1,0,1,2,3,3,0,3,4,2,2,0,1,7,1,0,                  
  1,2,3,3,0,3,4,2,2,0,1,7,1,0,5,7,1,1,0,1,3,4,4,0,4,5,6,6,0,5,4,5,1,0,1,2,3,3,0,4,5,1,0,          // Conversia se face in felul urmator: A -> 1 | B -> 2 | ... | G -> 7
  5,4,5,1,0,1,2,3,3,0,4,5,1,0,1,3,2,2,0,3,1,2,0,5,0,0,6,0,0,5,5,0,7,0,5,4,0,0,
};

int notesSong1[] = { 
   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
   NOTE_A4, NOTE_G4, NOTE_A4, 0, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0, 
   NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0, NOTE_A4, NOTE_G4, NOTE_A4, 0, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0, 
   NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,         // notele din <pithces.h>
   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0, NOTE_D5, NOTE_E5, NOTE_A4, 0, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
   NOTE_C5, NOTE_A4, NOTE_B4, 0, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0, 
   NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0, NOTE_A4, NOTE_G4, NOTE_A4, 0, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0, 
   NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0, NOTE_A4, NOTE_G4, NOTE_A4, 0,
   NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0, NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
   NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0, NOTE_D5, NOTE_E5, NOTE_A4, 0, 
   NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0, NOTE_C5, NOTE_A4, NOTE_B4, 0, NOTE_E5, 0, 0, NOTE_F5, 0, 0,
   NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0, NOTE_D5, 0, 0, NOTE_C5, 0, 0,NOTE_B4, NOTE_C5, 0,
   NOTE_B4, 0, NOTE_A4, NOTE_E5, 0, 0, NOTE_F5, 0, 0, NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
   NOTE_D5, 0, 0, NOTE_C5, 0, 0, NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4
};
   
byte lengthNotesSong1[203] = {
  8,8,4,8,8,8,8,4,8,8,8,8,4,8,8,8,8,3,8,8,8,4,8,8,8,8,4,8,8,8,8,4,8,8,8,8,3,8,
  8,8,4,8,8,8,8,4,8,8,8,8,4,8,8,8,8,8,2,8,8,8,4,8,8,4,8,4,8,8,8,4,8,8,8,8,3,3,2,8,                          // Lungimea notei - adica cat de mult sa dureze nota respectiva
  8,8,4,8,8,8,8,4,8,8,8,8,3,8,8,8,4,8,8,8,8,4,8,8,8,8,4,8,8,8,8,3,8,8,8,4,8,8,
  8,8,4,8,8,8,8,4,8,8,8,8,8,4,8,8,8,2,8,8,4,8,4,8,8,8,4,8,8,8,8,3,3,4,8,3,4,8,3,
  8,8,8,8,8,8,8,8,3,4,8,3,4,8,3,8,8,8,8,8,2,4,8,3,4,8,3,8,8,8,8,8,8,8,8,3,4,8,3,2,8,3,8,8,8,8,8,2
};



//===================================== MARIO THEME SONG ========================================================================================================

byte keyNotesSong2[] = {
  5, 5, 0, 5, 0, 3, 5, 0, 7, 0, 0, 0, 7, 0, 0, 0, 3, 0, 0, 7, 0, 0, 5, 0, 0, 1, 0, 2, 0, 1, 1, 0,
  7, 5, 7, 1, 0, 6, 7, 0, 5, 0, 3, 4, 2, 0, 0, 3, 0, 0, 7, 0, 0, 5, 0, 0, 1, 0, 7, 5, 7, 1, 0, 6,
  7, 0, 5, 0, 3, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0 
};

int notesSong2[] = {

  NOTE_E7, NOTE_E7, 0, NOTE_E7, 0, NOTE_C7, NOTE_E7, 0, NOTE_G7, 0, 0, 0, NOTE_G6, 0, 0, 0,
  NOTE_C7, 0, 0, NOTE_G6, 0, 0, NOTE_E6, 0, 0, NOTE_A6, 0, NOTE_B6, 0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7, NOTE_A7, 0, NOTE_F7, NOTE_G7, 0, NOTE_E7, 0, NOTE_C7, NOTE_D7, NOTE_B6, 0, 0,
  NOTE_C7, 0, 0, NOTE_G6, 0, 0, NOTE_E6, 0, 0, NOTE_A6, 0, NOTE_B6, 0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7, NOTE_A7, 0, NOTE_F7, NOTE_G7, 0, NOTE_E7, 0, NOTE_C7, NOTE_D7, NOTE_B6, 0, 0 
};

byte lengthNotesSong2[78] = {
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  9, 9, 9, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0, 12, 12,
  9, 9, 9, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12
};





//===================================== SETUP ========================================================================================================


void setup() 
{
    lc.shutdown(0,false);
    lc.setIntensity(0,3);
    lc.clearDisplay(0);

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    pinMode(BTN3, INPUT_PULLUP);

    Serial.begin(9600);
    Wire.begin(10);
    Wire.onReceive(receiveEvent);
}


//===================================== LOOP ========================================================================================================

void loop() 
{
    currentSongTime = millis();
    currentMatrixTime = millis();
    
    btnState1 = digitalRead(BTN1);
    btnState2 = digitalRead(BTN2);
    btnState3 = digitalRead(BTN3);
    
    if ((currentSongTime - previousSongTime) >= delaySongTime)
    {        
        if (thisNote < maxNotes)                                         
        {                                                                
            noTone(SPEAKER);  
            finalSong = false;                       // MASTER citeste prin variabila finalSong ca sunt in timpul jocului pentru a bloca accesul la alte optiuni ale meniului.
            Wire.onRequest(requestEvent);                                     
            if (song == 1)
            {                                              
                delaySongTime = (1000 / lengthNotesSong1[thisNote]) * difficultyLevel;     // setez un timp pentru fiecare nota  
                tone(SPEAKER, notesSong1[thisNote], delaySongTime);        
            }   
            
            if (song == 2)
            {                                              
                delaySongTime = (1000 / lengthNotesSong1[thisNote]) * difficultyLevel; 
                tone(SPEAKER, notesSong2[thisNote], delaySongTime);        
            }                                                                                                                           
            thisNote++;   
        }               
        else 
            finalSong = true;    
                                                   
        previousSongTime = currentSongTime;                               
    }                                                                  

    
  
    if ((currentMatrixTime - previousMatrixTime) >= delayMatrixTime)   
    {                                                                  
        if (song == 1) //and (thisNote < maxNotes))                      
            AdvanceALine(keyNotesSong1[thisNote]);        

        if (song == 2) //and (thisNote < maxNotes))                      
            AdvanceALine(keyNotesSong2[thisNote]); 
                                                            
        previousMatrixTime = currentMatrixTime;                          
    }      

}




//===================================== FUNCTII ========================================================================================================

void receiveEvent(int howMany)
{
      matrixTurnOn = Wire.read();
      matrixBrightness = Wire.read();
      song = Wire.read();                                     // SLAVE primeste informatiile de la MASTER
      maxNotes = Wire.read();                                     
      difficulty = Wire.read();                               // Daca sa aprinda sau sa stinga matricea, melodia, numarul maxim de note, etc ...

      if (matrixTurnOn == true)
      {
        for (int i = 0; i < 8; i++)
          for (int j = 0; j < 8; j++) 
             lc.setLed(0,i,j,true);
        lc.setIntensity(0,matrixBrightness);
      }
      
      if (matrixTurnOn == false){
          for (int i = 0; i < 8; i++)
             for (int j = 0; j < 8; j++) 
                lc.setLed(0,i,j,false);
                
          switch (difficulty)
          {
              case 1:{
                  difficultyLevel = 1.6;      
                  delayMatrixTime = 160;      
                  break;
              }
              case 2:{
                  difficultyLevel = 1.4;      
                  delayMatrixTime = 140;
                  break;
              }
              case 3:{
                  difficultyLevel = 1.1;      
                  delayMatrixTime = 110;
                  break;
              }
          }
      }

  thisNote = 0;
}


void requestEvent()
{
  Wire.write(finalSong);                              // Informatiile pe care le trimite SLAVE catre MASTER
  dtostrf(score, 5, 0, scoreChr);
  Wire.write(scoreChr);                               // Daca joc sau nu joc si scorul.
  Serial.print("FinalSong: ");
  Serial.println(finalSong);
  Serial.print("Score: ");
  Serial.println(scoreChr);
}



void AdvanceALine(int displayColumn)
{    
    if ((displayColumn == 1) or (displayColumn == 2) or (displayColumn == 3))        // 
    {                                                                                //
        matrix[0][0] = 1;                                                            //
        matrix[0][1] = 1;                                                            //   Functia primeste ca parametru una din cele 7 note convertite in vectorul keyNotesSong[]
    }                                                                                //   si le distribuie in felul urmator:
                                                                                     //   
    if ((displayColumn == 4) or (displayColumn == 5))                                // 
    {                                                                                //
        matrix[0][3] = 1;                                                            //   primele 3 note corespund coloanelor 1 si 2 ale matricei 
        matrix[0][4] = 1;                                                            //   notele 4 si 5 corespund coloanelor 4 si 5 
    }                                                                                //   notele 6 si 7 corespund coloanelor 7 si 8
                                                                                     //
    if ((displayColumn == 6) or (displayColumn == 7))                                // 
    {                                                                                //
        matrix[0][6] = 1;                                                            //
        matrix[0][7] = 1;                                                            //
    }                                                                                //

  

    if ((btnState1 == 0) and (matrix[8][0] == 0)){             //
      score = score - 10;                                      //
    }                                                          //     Daca unul din cele 3 butoane de gameplay este apasat 
                                                               //     dar pe ultima linie nu exista nici o nota
    if ((btnState2 == 0) and (matrix[8][3] == 0)){             //     atunci scorul scade cu -10.
      score = score - 10;                                      //   
    }                                                          //
                                                               //
    if ((btnState3 == 0) and (matrix[8][6] == 0)){             // 
      score = score - 10;                                      //
    }                                                          //
    
    
    if ((btnState1 == 0) and (matrix[8][0]==1))                //
    {                                                          // 
        score = score + 50;                                    //  
        matrix[8][0]=0;                                        //
        matrix[8][1]=0;                                        //   Daca unul din cele 3 butoane este apasat iar pe ultima linie
                                                               //   adica este nota (are valoarea 1)
    }                                                          //   atunci linia de test va primi valoarea 0
                                                               //                      
    if ((btnState2 == 0) and (matrix[8][3]==1))                //
    {                                                          //     
        score = score + 50;                                    //                                
        matrix[8][3]=0;                                        //   Simulam "prinderea" notelor muzicale
        matrix[8][4]=0;                                        //
    }                                                          //
                                                               //   OBS: Mai intai mutam virtual apoi fizic. 
    if ((btnState3 == 0) and (matrix[8][6]==1))                //
    {                                                          //   De exemplu, daca avem valoarea 1 pe linia 7, aprindem ledul matricei si mutam valoarea 1 pe linia 8 (cea de test).
        score = score + 50;                                    //   pentru a se face verificarea. Daca linia 8 are valoarea 1, inseamna ca nu am apasat butonul la timp
        matrix[8][6]=0;                                        //  
        matrix[8][7]=0;                                        //
                                                               //
    }                                                          //
       



    for (int i = 0; i < 9; i++)
       for (int j = 0; j < 8; j++)                
       {
          if ((matrix[i][j] == 1) and (i < 8))      //  unde exista valoarea 1 aprindem ledul matricei
             lc.setLed(0, i, j, true);              //

                                                  
          if ((matrix[i][j] == 0) and (i < 8))      //  unde exista valoarea 0 stingem ledul matricei
             lc.setLed(0, i, j, false);             //

             
          if ((matrix[i][j] == 0) and (i == 8))     //
          {                                         //
              if ((j == 0) or (j == 1))             //                                     
                  digitalWrite(LED1, LOW);          //  daca linia 8, cea de test, are valoarea 0, inseaemna ca nu am        
                                                    //  pierdut nici o nota si deci singem ledul corespunzator liniei
              if ((j == 3) or (j == 4))             //
                  digitalWrite(LED2, LOW);          //  
                                                    //  
              if ((j == 6) or (j == 7))             //
                  digitalWrite(LED3, LOW);          //  
          }                                         //
          
   
          if ((matrix[i][j] == 1) and (i == 8))     //
          {                                         //
              if ((j == 0) or (j == 1)){            //  daca linia 8, cea de test, are valoarea 1, inseaemna ca am pierdut nota.                  
                  digitalWrite(LED1, HIGH);         //  drept urmare aprindem ledul rosu corespunzator liniei si scadem -15 din scor
                  score = score - 15;               //   
              }                                     // 
                   
              if ((j == 3) or (j == 4)){            //
                  digitalWrite(LED2, HIGH);         //
                  score = score - 15;               //
              }                                     //
              
              if ((j == 6) or (j == 7)){            //    
                  digitalWrite(LED3, HIGH);         //
                  score = score - 15;               //
              }                                     //
          }                                         //  
      }  



      for (int i = 7; i >= 0; i--)                 //
       for (int j = 0; j < 8; j++)                 //   Mutam toate elementele pe linia urmatoare
       {                                           //
           matrix[i+1][j] = matrix[i][j];          //
           matrix[i][j] = 0;                       //
       }                                           //
}
