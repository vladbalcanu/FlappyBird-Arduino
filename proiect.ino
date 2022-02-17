#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#define DC           9     // DC
#define RST          8     // RST
#define CS           10     // CS
static Adafruit_ST7735 screen = Adafruit_ST7735(CS,  DC, RST);

// CONSTANTE

#define BirdWidth 14 // Latimea pasarii
#define BirdHeight 12 // Inaltimea passarii
#define TowerWidth 15 // Latimea turnurilor
#define birdXPosition 50 // Pozitia constanta pe orizontala a pasarii
#define TowersGap 40 // Spatiul dintre turnul de jos si cel de sus
#define CeilingHeight 10  //inaltime tavan
#define GroundHeight 10 //inaltime pamant
#define GroundStartPosition 138 //Pozitia pe verticala de la care incepe pamantul
#define BlackBarHeight  12//inaltime bara neagra pentru scor
#define birdJumpHeight 16//inaltimea sariturii pasarii


//Culori pasare
#define C0 ST77XX_BLUE
#define C1 ST77XX_YELLOW
#define C2 ST77XX_WHITE
#define C3 ST77XX_RED
#define C4 ST77XX_BLACK

//PASARE
const unsigned int bird[] = {
  C0, C0, C2, C2, C2, C0, C1, C1, C1, C0, C0, C0, C0, C0,
  C0, C2, C2, C2, C2, C2, C1, C1, C1, C1, C0, C0, C0, C0,
  C2, C2, C2, C2, C2, C2, C2, C1, C2, C2, C2, C0, C0, C0,
  C2, C2, C2, C2, C2, C2, C1, C2, C2, C2, C2, C2, C0, C0,
  C2, C2, C2, C2, C2, C2, C1, C2, C4, C4, C4, C2, C1, C0,
  C2, C2, C2, C2, C2, C1, C1, C2, C4, C4, C4, C2, C1, C3,
  C2, C2, C2, C2, C1, C1, C1, C2, C4, C4, C4, C2, C3, C3,
  C2, C2, C1, C1, C1, C1, C1, C2, C2, C2, C2, C2, C3, C3,
  C0, C1, C1, C1, C1, C1, C1, C1, C2, C2, C2, C1, C1, C3,
  C0, C0, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C0, C0,
  C0, C0, C0, C1, C1, C1, C1, C1, C1, C1, C1, C0, C0, C0,
  C0, C0, C0, C0, C0, C1, C1, C1, C1, C0, C0, C0, C0, C0
};

//Varori globale ce se modifica
int score ; // scor
int towerPosX ; //Pozitia pe orizontala a turnurilor
int heightUpper ; //Inaltimea turnului de sus
int heightBottom ; // Inaltimea turnului de jos
int GameSpeed ; // Viteza/Dificultatea jocului
int birdPosition ; //Pozitia pasarii pe verticala
int oldBirdPosition; // Vechea pozitie a pasarii pe verticala
int butonTapped ; // Variabila ce semnaleaza daca butonul a fost apasat
int interPos ; // Pozitie intermediara folosita in cazul in care pazarea zboara in sus
int currentTime; // Timpul curent
int buton; // Variabila in care citim valoarea pinului 7
int pointAdded; //Variabila ce semnaleaza daca s-a adaugat punctul
int UpSpeed; //Variabila ce semnaleaza daca s-a crescut viteza jocului


void setup() {
  screen.initR(INITR_BLACKTAB);
  pinMode(7, INPUT_PULLUP);
  Serial.begin(9600);

}
// Initializarea jocului la start
void Init() {
  screen.fillRect(0, 0, 128, CeilingHeight, ST77XX_GREEN); // Tavan
  screen.fillRect(0, GroundStartPosition, 128, GroundHeight, ST77XX_GREEN); // Pamant
  screen.fillRect(0, GroundStartPosition + GroundHeight, 128, BlackBarHeight, ST77XX_BLACK); // Linie neagra sub pamant pentru scor
  screen.fillRect(0, CeilingHeight, 128, GroundStartPosition - CeilingHeight, ST77XX_BLUE); // Cer
  screen.setTextColor(ST77XX_RED);
  screen.setCursor(5, 150);
  screen.print("Score:");
  towerPosX = 128; //Pozitia initiala de plecare a turnurilor ( finalul ecranului in dreapta)
  heightUpper = random(20, 80);
  heightBottom = GroundStartPosition - CeilingHeight - heightUpper - TowersGap ; 
  GameSpeed = 1;
  birdPosition = 50; //Pozitia pasarii pe verticala de start
  oldBirdPosition = 49; // Vechea pozitie a pasarii pe verticala de start
  butonTapped = 0;
  interPos = 0;
  currentTime = 0;
  score = 0;
  pointAdded=0;
  UpSpeed=0;
}

//Miscarea/Desenarea pasarii
void birdMovement() {
  if (butonTapped == 1) { //daca pasarea merge in sus
    screen.drawFastHLine(birdXPosition, interPos + BirdHeight, BirdWidth, ST77XX_BLUE);
    screen.drawFastHLine(birdXPosition, interPos + BirdHeight + 1, BirdWidth, ST77XX_BLUE);
    for (int py = 0; py < BirdHeight ; py++) {
      for (int px = 0; px < BirdWidth; px++) {
        screen.drawPixel(birdXPosition + px, interPos + py, bird[px + py * BirdWidth]);
      }
    }
    interPos = interPos - 2;
    oldBirdPosition = interPos;
  } else { //daca pasarea merge in jos
    screen.drawFastHLine(birdXPosition, oldBirdPosition, BirdWidth, ST77XX_BLUE);
    for (int py = 0; py < BirdHeight ; py++) {
      for (int px = 0; px < BirdWidth; px++) {
        screen.drawPixel(birdXPosition + px, birdPosition + py, bird[px + py * BirdWidth]);
      }
    }
  }
}

//Miscarea/Desenarea turnurilor din joc
void towerMovement() {
  for (int px = 0 ; px < TowerWidth ; px++) {

    screen.drawFastVLine(towerPosX + px, CeilingHeight, heightUpper, ST77XX_GREEN );
    screen.drawFastVLine(towerPosX + px, CeilingHeight + heightUpper + TowersGap, heightBottom, ST77XX_GREEN);
  }
  for (int backDel = 0 ; backDel < GameSpeed; backDel++) {
    screen.drawFastVLine(towerPosX + TowerWidth + backDel, CeilingHeight, heightUpper, ST77XX_BLUE );
    screen.drawFastVLine(towerPosX + TowerWidth + backDel, CeilingHeight + heightUpper + TowersGap, heightBottom, ST77XX_BLUE);
  }
  towerPosX = towerPosX - GameSpeed;
  if (towerPosX < birdXPosition-TowerWidth && pointAdded==0){
    score++;
    pointAdded=1;
    UpSpeed=0;
  }
  if (towerPosX < -TowerWidth-GameSpeed) {
    towerPosX = 128;
    pointAdded=0;
    heightUpper = random(10, 80); //Height of upper tower
    heightBottom = GroundStartPosition - CeilingHeight - heightUpper - TowersGap ; // Height of lower tower
  }
}

//Coliziunile cu turnuri/pamant/tavan
int colissions() {
  if (butonTapped == 1) {
    if (interPos < CeilingHeight || interPos + BirdHeight >= GroundStartPosition) {
      return 1; //Atinge tavanul sau podeaua
    }
  } else {
    if (birdPosition < CeilingHeight || birdPosition + BirdHeight >= GroundStartPosition) {
      return 1; //Atinge tavanul sau podeaua
    }
  }

  if (towerPosX >= birdXPosition - TowerWidth && towerPosX <= birdXPosition + TowerWidth) {
    if (butonTapped == 1) {
      if (interPos < CeilingHeight + heightUpper || interPos + BirdWidth > CeilingHeight + heightUpper + TowersGap)
        return 1;
    } else {
      if (birdPosition < CeilingHeight + heightUpper || birdPosition + BirdWidth > CeilingHeight + heightUpper + TowersGap) {
        return 1;
      }
    }
  }
  return 0;
}

//Logica suplimentara legata de apasarea butonului/declansarea urcarii pasarii/scrierea scorului
void buttonLogic() {
  if (butonTapped == 0) {
    oldBirdPosition = birdPosition;
    birdPosition = birdPosition + 1;
  }

  if (butonTapped == 1 && interPos <= birdPosition ) {
    oldBirdPosition = birdPosition;
    birdPosition = birdPosition + 1;
    butonTapped = 0;
  }
  int x = millis();
  if (buton == 0 && abs(x - currentTime) > 250 ) {
    currentTime = millis();
    interPos = oldBirdPosition;
    birdPosition = birdPosition - birdJumpHeight;
    butonTapped = 1;
  }
  for (int py = 150 ; py < 160; py++) {
    screen.drawFastHLine(40, py, 50, ST77XX_BLACK);
  }
  screen.setCursor(40, 150);
  screen.print(score);
  if (score > 0 && score % 5 == 0 && GameSpeed < 5 && towerPosX<birdXPosition-TowerWidth && UpSpeed==0 ){
    GameSpeed++;
    UpSpeed=1;
  }
}

//Functie pentru terminarea jocului
void lose() {
  screen.fillRect(0, 0, 128, 160, ST77XX_BLACK);
  screen.setTextColor(ST77XX_RED);
  screen.setCursor(40, 50);
  screen.print("GAME OVER");
  screen.setCursor(40, 70);
  screen.print("Score: ");
  screen.print(score);
  screen.setCursor(10, 100);
  screen.setTextColor(ST77XX_WHITE);
  screen.print("Press the Button to");
  screen.setCursor(37, 110);
  screen.print("play again");
  while (1) {
    int buton = digitalRead(7);
    if (buton == 0)
      break;
  }
  delay(200);
}

//Jocul
void game() {
  Init();
  while (1) { // loop pentru joc
    buton = digitalRead(7); //citire comanda
    birdMovement();
    towerMovement();
    if (colissions() == 1)
      break;
    buttonLogic();
  }
  delay(200);
  lose();
}


void loop() {
  game();
}
