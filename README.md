simple-3D-cube-rendering

This is to show a different way to draw a cube in a routine. I have searched different codes to draw a cube, but they are all hard coded, drawing 12 edges one by one. Because a cube is 3D, it has 8 vertices, but 12 edges, so, you can't simply create a 8 steps loop and draw 12 edges to connect them as a cube. I tried to change the order of vertices, including some logic to avoid repeating a line, but they don't work or are too complicated that are meaningless.
```
     7-------6
    /|      /|
   4-------5 |
   | 3-----|-2
   |/      |/
   0-------1
```

Until I found the way below, that you group the vertices into 4 even and 4 odd-vertex, and an even vertex only connect to 3 odd vertex, and wise verse.
     2-------7
    /|      /|
   3-------6 |
   | 5-----|-4
   |/      |/
   0-------1

In this coordination, the 12 edges can be drawn in a loop of 4, which I think is clean and nice. Also it reflects the fundamental math of a cube.

for (int i = 0; i < 8; i += 2) {
    for (int j = i + 1; j < i + 7; j += 2) {
          draw_line(i, j % 8 ); 
    }
}
  

Hardware:
  - Arduino Nano
  - 0.96 inch 12864 OLED

Libs:
  - Adafruite GFX
  - Adafruite ssd1306

Video demo here:
  - https://youtu.be/ccC5uqc9CUA

