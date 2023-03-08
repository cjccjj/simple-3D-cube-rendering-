/**************************************************************************
  3D cube demo by CJ Zhang
  Wiring:
  OLED12864   Arduino Nano
  SDA         7
  SCL         10
  DC          11
  RES         13
  CS - N.A.

  Cube 3d vertices numbering and coordinates
     7-------6
    /|      /|
   4-------5 |
   | 3-----|-2
   |/      |/
   0-------1

  origin is at center of cube
  vertex 0 (-1,-1,1)
  vertex 1 (1,-1,1)
  ...
**************************************************************************/

#include <SPI.h>
//#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   7 //SDA
#define OLED_CLK   10 //SCL
#define OLED_DC    11
#define OLED_CS    12 //N.A.
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//customization
int cubesize = 30; //in pixels
int fl = 999; //distance of cammera, 80 looks fine, set to >=999 for orthographic projection
int angle = 0; //starting angle
int zoom = 100; //zoom starting zoom
int d = 1; //zoom in or out first
int delay_mils = 0; //the system need about 35 to work for my hardware (max fps 30), this is an addtion to that, fps = 1000/(35+delay_mils)

//for fps display
long lastframe_mil = 0;
long newframe_mil = 0;
int fps = 0;

//2D vertex coord for drawing
int vertex_2D[8][3];
//which vertex is at back most
int back_vertex = 0;

void draw_frame(void) {
  //draw full frame if is perpective projection, hide back sizes if orthographic projection
  if (fl < 999) {
    back_vertex = -1;
  }

  //link 0123
  if (!(back_vertex == 0 || back_vertex == 1)) {    display.drawLine(vertex_2D[0][0], vertex_2D[0][1], vertex_2D[1][0], vertex_2D[1][1], WHITE);  }
  if (!(back_vertex == 1 || back_vertex == 2)) {    display.drawLine(vertex_2D[1][0], vertex_2D[1][1], vertex_2D[2][0], vertex_2D[2][1], WHITE);  }
  if (!(back_vertex == 2 || back_vertex == 3)) {    display.drawLine(vertex_2D[2][0], vertex_2D[2][1], vertex_2D[3][0], vertex_2D[3][1], WHITE);  }
  if (!(back_vertex == 3 || back_vertex == 0)) {    display.drawLine(vertex_2D[3][0], vertex_2D[3][1], vertex_2D[0][0], vertex_2D[0][1], WHITE);  }

  //link 4567
  if (!(back_vertex == 4 || back_vertex == 5)) {    display.drawLine(vertex_2D[4][0], vertex_2D[4][1], vertex_2D[5][0], vertex_2D[5][1], WHITE);  }
  if (!(back_vertex == 5 || back_vertex == 6)) {    display.drawLine(vertex_2D[5][0], vertex_2D[5][1], vertex_2D[6][0], vertex_2D[6][1], WHITE);  }
  if (!(back_vertex == 6 || back_vertex == 7)) {    display.drawLine(vertex_2D[6][0], vertex_2D[6][1], vertex_2D[7][0], vertex_2D[7][1], WHITE);  }
  if (!(back_vertex == 7 || back_vertex == 4)) {    display.drawLine(vertex_2D[7][0], vertex_2D[7][1], vertex_2D[4][0], vertex_2D[4][1], WHITE);  }

  //link 04 15 26 37
  if (!(back_vertex == 0 || back_vertex == 4)) {    display.drawLine(vertex_2D[0][0], vertex_2D[0][1], vertex_2D[4][0], vertex_2D[4][1], WHITE);  }
  if (!(back_vertex == 1 || back_vertex == 5)) {    display.drawLine(vertex_2D[1][0], vertex_2D[1][1], vertex_2D[5][0], vertex_2D[5][1], WHITE);  }
  if (!(back_vertex == 2 || back_vertex == 6)) {    display.drawLine(vertex_2D[2][0], vertex_2D[2][1], vertex_2D[6][0], vertex_2D[6][1], WHITE);  }
  if (!(back_vertex == 3 || back_vertex == 7)) {    display.drawLine(vertex_2D[3][0], vertex_2D[3][1], vertex_2D[7][0], vertex_2D[7][1], WHITE);  }
}

void cubeloop(void) {

  float rot, rotx, roty, rotz, rotxx, rotyy, rotzz;

  //initail unit cube position
  int vertex[8][3] = {
    { -1, -1, 1}, //vertex 0
    {1, -1, 1},   //vertex 1
    {1, 1, 1},    //...
    { -1, 1, 1},
    { -1, -1, -1},
    {1, -1, -1},
    {1, 1, -1},
    { -1, 1, -1} //vertex 7
  };

  //calculate vertex coord
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {
      vertex[i][j] = vertex[i][j] * cubesize / 2;
    }
  }

  while (true) { //loop
    //calculate new vertex coord
    for (int i = 0; i < 8; i++) {
      rot = angle * 0.0174532; //0.0174532 = one degree
      //rotateY
      rotz = vertex[i][2] * cos(rot) - vertex[i][0] * sin(rot);
      rotx = vertex[i][2] * sin(rot) + vertex[i][0] * cos(rot);
      roty = vertex[i][1];
      //rotateX
      rotyy = roty * cos(rot) - rotz * sin(rot);
      rotzz = roty * sin(rot) + rotz * cos(rot);
      rotxx = rotx;
      //rotateZ
      rotx = rotxx * cos(rot) - rotyy * sin(rot);
      roty = rotxx * sin(rot) + rotyy * cos(rot);
      rotz = rotzz;

      //3D to 2D  projection
      rotx = rotx * fl / (fl + rotz);
      roty = roty * fl / (fl + rotz);

      //zoom and move to display coord center
      vertex_2D[i][0] = rotx * zoom / 100 + SCREEN_WIDTH / 2 ;
      vertex_2D[i][1] = roty * zoom  / 100 + SCREEN_HEIGHT / 2;
      vertex_2D[i][2] = rotz;

      //look for which vertex is at back most
      if (vertex_2D[back_vertex][2] < vertex_2D [i][2]) {
        back_vertex = i;
      }
    }

    display.clearDisplay();
    draw_frame();

    //show fps
    display.setCursor(2, 2);
    newframe_mil = millis();
    fps = 1000 / (newframe_mil - lastframe_mil) ;
    lastframe_mil = newframe_mil;
    display.print("fps ");
    display.print(fps);
    display.display();
    delay(delay_mils); //to reduce fps

    angle = (angle + 3) % 360;  //rotate step at 3 degrees
    zoom = zoom + 1 * d; //zooming step at 1%
    if (zoom >= 200) { //zooming range 1.25 to 2.0
      d = -1;
    }
    if (zoom <= 25) {
      d = 1;
    }
  }
}

void setup() {
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK); 
  display.display();
  display.clearDisplay();
}

void loop() {
  cubeloop();
}
