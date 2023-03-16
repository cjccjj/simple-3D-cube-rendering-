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
     2-------7
    /|      /|
   3-------6 |
   | 5-----|-4
   |/      |/
   0-------1

  origin is at center of cube
  vertex 0 (-1,-1,1)
  vertex 7 (1,1,-1)
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
int cubesize = 15; //in pixels
int fl = 999; //distance of cammera, 80 looks fine, set to >=999 for orthographic projection
int delay_mils = 5; //the system need about 35 to work for my hardware (max fps 30), this is an addtion to that, fps = 1000/(35+delay_mils)
int zoom = 100;
int d = 1; //zoom in or out 
//for display fps
long lastframe_mil = 0;
long newframe_mil = 0;
int fps = 0;

//cube coordinate see top of page
float vertex[8][3] = {
  { -1, -1, 1},
  {1, -1, 1},
  { -1, 1, -1},
  { -1, 1, 1},
  {1, -1, -1},
  { -1, -1, -1},
  {1, 1, 1},
  {1, 1, -1}
};
//2D vertex coord for drawing
int vertex_2D[8][2];
//which vertex is at back most
int back_vertex = 0;

//draw line linking  vertex a to vertex b
void draw_line(int a, int b) {
  display.drawLine(vertex_2D[a][0], vertex_2D[a][1], vertex_2D[b][0], vertex_2D[b][1], WHITE);
}

//draw full frame if is perpective projection, hide back sizes if orthographic projection
void draw_frame(void) {
  if (fl < 999) {
    back_vertex = -1;
  }

  for (int i = 0; i < 8; i += 2) {
    for (int j = i + 1; j < i + 7; j += 2) {
      if (back_vertex != i && back_vertex != j % 8 ) {
        draw_line(i, j % 8 );
      }
    }
  }
}

//rotate in axel ax, x:ax=0, y:ax=1, z:az=2
//and then project to 2D
void rotate(int ax, int rot_degree) {
  float rot = rot_degree *  0.0174532; //0.0174532 = one degree
  for (int i = 0; i < 8; i++) {
    //rotate
    float temp = vertex[i][(ax + 1) % 3] * cos(rot) - vertex[i][(ax + 2) % 3] * sin(rot);
    vertex[i][(ax + 2) % 3] = vertex[i][(ax + 2) % 3] * cos(rot) + vertex[i][(ax + 1) % 3] * sin(rot);
    vertex[i][(ax + 1) % 3] = temp;
    //project
    vertex_2D[i][0] = (vertex[i][0] * cubesize * fl / (vertex[i][2] * cubesize + fl)) * zoom / 100 + SCREEN_WIDTH / 2;
    vertex_2D[i][1] = (vertex[i][1] * cubesize * fl / (vertex[i][2] * cubesize + fl)) * zoom / 100 + SCREEN_HEIGHT / 2;
    //looking for back most vertex number
    if (vertex[back_vertex][2] < vertex [i][2]) {
      back_vertex = i;
    }
  }
}

void display_fps() {
  //show fps
  display.setCursor(2, 2);
  newframe_mil = millis();
  fps = 1000 / (newframe_mil - lastframe_mil) ;
  lastframe_mil = newframe_mil;
  display.print("fps ");
  display.print(fps);
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
  while (true) { //loop
    display.clearDisplay();
    draw_frame();
    display_fps();
    display.display();
    delay(delay_mils); //to reduce fps

    //roate x,y,z each for 2 degree,
    rotate(0, 2);
    rotate(1, 2);
    rotate(2, 2);

    //zooming in and out
    zoom = zoom + 1 * d;
    if (zoom >= 150) {
      d = -1;
    }
    if (zoom <= 25) {
      d = 1;
    }
  }
}
