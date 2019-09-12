#include <stdlib.h>
#include <stdbool.h>

extern short BATTLESHIP[240][320];
extern short LOSE[240][320];
extern short WIN[240][320];
volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void draw_square(int i, int j, short int color);
typedef struct cell {
  int ship;
  bool shot;
}
Cell;

typedef struct ship {
  int x1, y1, x2, y2, x3, y3;
  int shot;
  bool sunk;
}
Ship;

int main(void) {

  volatile char * addr_jp1port = (char * ) 0xFF200060;
  *(addr_jp1port + 4) = 0x00;

  volatile int * pixel_ctrl_ptr = (int * ) 0xFF203020;

  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = * pixel_ctrl_ptr; // pixel_buffer_start points to the pixel buffer

  Cell pgrid[7][7];
  Cell cgrid[7][7];
  Ship p1, p2, p3, c1, c2, c3;
  int input;
  srand(11);

  while (true) {
    //Initialize blank board
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        pgrid[i][j].ship = 0;
        pgrid[i][j].shot = false;
        cgrid[i][j].ship = 0;
        cgrid[i][j].shot = false;
      }
    }

    p1.sunk = false;
    p1.shot = 0;
    c1.sunk = false;
    c1.shot = 0;

    p2.sunk = false;
    p2.shot = 0;
    p3.sunk = false;
    p3.shot = 0;

    c2.sunk = false;
    c2.shot = 0;
    c3.sunk = false;
    c3.shot = 0;

    //Generate Computer Ship Locations
    {
      int rot = rand() % 2;
      if (rot == 0) {
        do {
          c1.x1 = rand() % 7;
        } while (c1.x1 > 4);

        c1.x2 = c1.x1 + 1;
        c1.x3 = c1.x2 + 1;
        c1.y1 = rand() % 7;
        c1.y2 = c1.y1;
        c1.y3 = c1.y1;

      } else {
        do {
          c1.y1 = rand() % 7;
        } while (c1.y1 > 4);
        c1.y2 = c1.y1 + 1;
        c1.y3 = c1.y2 + 1;
        c1.x1 = rand() % 7;
        c1.x2 = c1.x1;
        c1.x3 = c1.x2;

      }
      cgrid[c1.x1][c1.y1].ship = 1;
      cgrid[c1.x2][c1.y2].ship = 1;
      cgrid[c1.x3][c1.y3].ship = 1;

      do {
        rot = rand() % 2;
        if (rot == 0) {
          do {
            c2.x1 = rand() % 7;
          } while (c2.x1 > 5);

          c2.x2 = c2.x1 + 1;

          c2.y1 = rand() % 7;
          c2.y2 = c2.y1;
        } else {
          do {
            c2.y1 = rand() % 7;
          } while (c2.y1 > 5);
          c2.y2 = c2.y1 + 1;

          c2.x1 = rand() % 7;
          c2.x2 = c2.x1;
        }
      } while (cgrid[c2.x1][c2.y1].ship != 0 || cgrid[c2.x2][c2.y2].ship != 0);

      cgrid[c2.x1][c2.y1].ship = 2;
      cgrid[c2.x2][c2.y2].ship = 2;

      do {
        rot = rand() % 2;
        if (rot == 0) {
          do {
            c3.x1 = rand() % 7;
          } while (c3.x1 > 5);

          c3.x2 = c3.x1 + 1;

          c3.y1 = rand() % 7;
          c3.y2 = c3.y1;
        } else {
          do {
            c3.y1 = rand() % 7;
          } while (c3.y1 > 5);

          c3.y2 = c3.y1 + 1;

          c3.x1 = rand() % 7;
          c3.x2 = c3.x1;
        }
      } while (cgrid[c3.x1][c3.y1].ship != 0 || cgrid[c3.x2][c3.y2].ship != 0);

      cgrid[c3.x1][c3.y1].ship = 3;
      cgrid[c3.x2][c3.y2].ship = 3;
    }

    //Draw Start Screen
    clear_screen();
    for (int i = 0; i < 240; i++)
      for (int j = 0; j < 320; j++)
        *
        (short int * )(pixel_buffer_start + (j << 1) + (i << 10)) = BATTLESHIP[i][j];

    do {
      input = * addr_jp1port;
    } while ((input & 0x3F) == 0);
    do {
      input = * addr_jp1port;
    } while ((input & 0x3F) != 0);
	for(int d=0;d<100;d++);
    clear_screen();
    
    int mode;
    
    do {
      input = * addr_jp1port;
      
    } while ((input & 0x20) == 0&&(input & 0x10)==0);
   do {
      
    } while (((* addr_jp1port) & 0x3F) != 0);
    for(int d=0;d<100;d++);
    if(input & 0x20){
        mode=1;
    }else{
    	mode=0;    
	}
    
    
    //Draw Grid
    for (int i = 0; i < 8; i++) {
      draw_line(20 * i + 10, 50, 20 * i + 10, 190, 0xFFFF);
      draw_line(170 + 20 * i, 50, 170 + 20 * i, 190, 0xFFFF);
      draw_line(10, 50 + i * 20, 150, 50 + i * 20, 0xFFFF);
      draw_line(170, 50 + i * 20, 310, 50 + i * 20, 0xFFFF);
    }

    //Place Ship 1
    {
      int i1 = 0, j1 = 0, i2 = 1, j2 = 0, i3 = 2, j3 = 0;
      bool rot = false;
      while (pgrid[i1][j1].ship == true || pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true) {
        i1++;
        i2++;
        i3++;
      }
      draw_square(i1, j1, 0x8433);
      draw_square(i2, j2, 0x8433);
      draw_square(i3, j3, 0x8433);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i3+1) + 10, 20 * j1 + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * (j3+1) + 50, 20 * (i3+1) + 10, 20 * (j3+1) + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j3+1) + 50, 0xBBBB);
      draw_line(20 * (i3+1) + 10, 20 * j1 + 50, 20 * (i3+1) + 10, 20 * (j3+1) + 50, 0xBBBB);
      while (true) {

        do {
          input = * addr_jp1port;
        } while ((input & 0x3F) == 0);
        
        do {
          
        } while (((* addr_jp1port)& 0x3F) != 0);
        for(int d=0;d<100;d++);
        draw_square(i1, j1, 0x0000);
        draw_square(i2, j2, 0x0000);
        draw_square(i3, j3, 0x0000);
        draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i3+1) + 10, 20 * j1 + 50, 0xFFFF);
      draw_line(20 * i1 + 10, 20 * (j3+1) + 50, 20 * (i3+1) + 10, 20 * (j3+1) + 50, 0xFFFF);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j3+1) + 50, 0xFFFF);
      draw_line(20 * (i3+1) + 10, 20 * j1 + 50, 20 * (i3+1) + 10, 20 * (j3+1) + 50, 0xFFFF);
        //Right
        if (input & 0x01) {
          do {
            if (rot) {
              i1 = (i1 + 1) % 7;
              i2 = i1;
              i3 = i2;
            } else {
              i1 = (i1 + 1) % 5;
              i2 = i1 + 1;
              i3 = i2 + 1;
            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true || pgrid[i3][j3].ship == true);
          //Left
        } else if (input & 0x02) {
          do {
            if (rot) {
              i1 = i1 - 1;
              if (i1 < 0)
                i1 = 6;
              i2 = i1;
              i3 = i2;
            } else {
              i1 = i1 - 1;
              if (i1 < 0)
                i1 = 4;
              i2 = i1 + 1;
              i3 = i2 + 1;
            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true || pgrid[i3][j3].ship == true);
          //Up
        } else if (input & 0x04) {
          do {
            if (rot) {
              j1 = j1 - 1;
              if (j1 < 0)
                j1 = 4;
              j2 = j1 + 1;
              j3 = j2 + 1;
            } else {
              j1 = j1 - 1;
              if (j1 < 0)
                j1 = 6;
              j2 = j1;
              j3 = j1;
            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true || pgrid[i3][j3].ship == true);
          //Down
        } else if (input & 0x08) {
          do {
            if (rot) {
              j1 = j1 + 1;
              if (j1 > 4)
                j1 = 0;
              j2 = j1 + 1;
              j3 = j2 + 1;
            } else {
              j1 = j1 + 1;
              if (j1 > 6)
                j1 = 0;
              j2 = j1;
              j3 = j1;
            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true || pgrid[i3][j3].ship == true);
          //Rotate
        } else if (input & 0x10) {
          if (rot) {
            if (!((i1 + 2) > 6 || pgrid[i1 + 1][j1].ship == true || pgrid[i1 + 2][j1].ship == true)) {
              i2 = i1 + 1;
              i3 = i2 + 1;
              j2 = j1;
              j3 = j1;
              rot = false;
            }
          } else {
            if (!((j1 + 2) > 6 || pgrid[i1][j1 + 1].ship == true || pgrid[i1][j1 + 2].ship == true)) {

              j2 = j1 + 1;
              j3 = j2 + 1;
              i2 = i1;
              i3 = i1;
              rot = true;
            }
          }
          //Place
        } else if (input & 0x20) {
          draw_square(i1, j1, 0x8433);
          draw_square(i2, j2, 0x8433);
          draw_square(i3, j3, 0x8433);
          
          break;
        }
        draw_square(i1, j1, 0x8433);
        draw_square(i2, j2, 0x8433);
        draw_square(i3, j3, 0x8433);
        draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i3+1) + 10, 20 * j1 + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * (j3+1) + 50, 20 * (i3+1) + 10, 20 * (j3+1) + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j3+1) + 50, 0xBBBB);
      draw_line(20 * (i3+1) + 10, 20 * j1 + 50, 20 * (i3+1) + 10, 20 * (j3+1) + 50, 0xBBBB);

      }
      p1.x1 = i1;
      p1.y1 = j1;
      p1.x2 = i2;
      p1.y2 = j2;
      p1.x3 = i3;
      p1.y3 = j3;
      pgrid[i1][j1].ship = true;
      pgrid[i2][j2].ship = true;
      pgrid[i3][j3].ship = true;

    }

    //Place Ship 2
    {
      int i1 = 0, j1 = 0, i2 = 1, j2 = 0;
      bool rot = false;
      while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true) {
        i1++;
        i2++;
        if (i1 == 6) {
          i1 = 0;
          i2 = 1;
          j1++;
          j2++;
        }

      }
      draw_square(i1, j1, 0x8433);
      draw_square(i2, j2, 0x8433);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * j1 + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * (j2+1) + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * (i2+1) + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);
      while (true) {

        do {
          input = * addr_jp1port;
        } while ((input & 0x3F) == 0);
        
        do {
          
        } while (((* addr_jp1port)& 0x3F) != 0);
        for(int d=0;d<100;d++);
        draw_square(i1, j1, 0x0000);
        draw_square(i2, j2, 0x0000);
        draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * j1 + 50, 0xFFFF);
      draw_line(20 * i1 + 10, 20 * (j2+1) + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xFFFF);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j2+1) + 50, 0xFFFF);
      draw_line(20 * (i2+1) + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xFFFF);

        //Right
        if (input & 0x01) {
          do {
            if (rot) {
              i1 = (i1 + 1) % 7;
              i2 = i1;

            } else {
              i1 = (i1 + 1) % 6;
              i2 = i1 + 1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Left
        } else if (input & 0x02) {
          do {
            if (rot) {
              i1 = i1 - 1;
              if (i1 < 0)
                i1 = 6;
              i2 = i1;

            } else {
              i1 = i1 - 1;
              if (i1 < 0)
                i1 = 5;
              i2 = i1 + 1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Up
        } else if (input & 0x04) {
          do {
            if (rot) {
              j1 = j1 - 1;
              if (j1 < 0)
                j1 = 5;
              j2 = j1 + 1;

            } else {
              j1 = j1 - 1;
              if (j1 < 0)
                j1 = 6;
              j2 = j1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Down
        } else if (input & 0x08) {
          do {
            if (rot) {
              j1 = j1 + 1;
              if (j1 > 5)
                j1 = 0;
              j2 = j1 + 1;

            } else {
              j1 = j1 + 1;
              if (j1 > 6)
                j1 = 0;
              j2 = j1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Rotate
        } else if (input & 0x10) {
          if (rot) {
            if (!((i1 + 1) > 6 || pgrid[i1 + 1][j1].ship == true)) {
              i2 = i1 + 1;

              j2 = j1;

              rot = false;
            }
          } else {
            if (!((j1 + 1) > 6 || pgrid[i1][j1 + 1].ship == true)) {

              j2 = j1 + 1;

              i2 = i1;

              rot = true;
            }
          }
          //Place
        } else if (input & 0x20) {
          draw_square(i1, j1, 0x8433);
          draw_square(i2, j2, 0x8433);

          break;
        }
        draw_square(i1, j1, 0x8433);
        draw_square(i2, j2, 0x8433);
		draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * j1 + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * (j2+1) + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * (i2+1) + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);


      }
      p2.x1 = i1;
      p2.y1 = j1;
      p2.x2 = i2;
      p2.y2 = j2;
      pgrid[i1][j1].ship = true;
      pgrid[i2][j2].ship = true;

    }
    //Place Ship 3
    {
      int i1 = 0, j1 = 0, i2 = 1, j2 = 0;
      bool rot = false;
      while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true) {
        i1++;
        i2++;
        if (i1 == 6) {
          i1 = 0;
          i2 = 1;
          j1++;
          j2++;
        }

      }
      draw_square(i1, j1, 0x8433);
      draw_square(i2, j2, 0x8433);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * j1 + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * (j2+1) + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * (i2+1) + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);
      while (true) {

        do {
          input = * addr_jp1port;
        } while ((input & 0x3F) == 0);
        do {
          
        } while (((* addr_jp1port)& 0x3F) != 0);
        for(int d=0;d<100;d++);
        draw_square(i1, j1, 0x0000);
        draw_square(i2, j2, 0x0000);
		draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * j1 + 50, 0xFFFF);
      draw_line(20 * i1 + 10, 20 * (j2+1) + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xFFFF);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j2+1) + 50, 0xFFFF);
      draw_line(20 * (i2+1) + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xFFFF);
        //Right
        if (input & 0x01) {
          do {
            if (rot) {
              i1 = (i1 + 1) % 7;
              i2 = i1;

            } else {
              i1 = (i1 + 1) % 6;
              i2 = i1 + 1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Left
        } else if (input & 0x02) {
          do {
            if (rot) {
              i1 = i1 - 1;
              if (i1 < 0)
                i1 = 6;
              i2 = i1;

            } else {
              i1 = i1 - 1;
              if (i1 < 0)
                i1 = 5;
              i2 = i1 + 1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Up
        } else if (input & 0x04) {
          do {
            if (rot) {
              j1 = j1 - 1;
              if (j1 < 0)
                j1 = 5;
              j2 = j1 + 1;

            } else {
              j1 = j1 - 1;
              if (j1 < 0)
                j1 = 6;
              j2 = j1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Down
        } else if (input & 0x08) {
          do {
            if (rot) {
              j1 = j1 + 1;
              if (j1 > 5)
                j1 = 0;
              j2 = j1 + 1;

            } else {
              j1 = j1 + 1;
              if (j1 > 6)
                j1 = 0;
              j2 = j1;

            }
          } while (pgrid[i1][j1].ship == true || pgrid[i2][j2].ship == true);
          //Rotate
        } else if (input & 0x10) {
          if (rot) {
            if (!((i1 + 1) > 6 || pgrid[i1 + 1][j1].ship == true)) {
              i2 = i1 + 1;

              j2 = j1;

              rot = false;
            }
          } else {
            if (!((j1 + 1) > 6 || pgrid[i1][j1 + 1].ship == true)) {

              j2 = j1 + 1;

              i2 = i1;

              rot = true;
            }
          }
          //Place
        } else if (input & 0x20) {
          draw_square(i1, j1, 0x8433);
          draw_square(i2, j2, 0x8433);

          break;
        }
        draw_square(i1, j1, 0x8433);
        draw_square(i2, j2, 0x8433);
		draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * j1 + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * (j2+1) + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * i1 + 10, 20 * j1 + 50, 20 * i1 + 10, 20 * (j2+1) + 50, 0xBBBB);
      draw_line(20 * (i2+1) + 10, 20 * j1 + 50, 20 * (i2+1) + 10, 20 * (j2+1) + 50, 0xBBBB);

      }
      p3.x1 = i1;
      p3.y1 = j1;
      p3.x2 = i2;
      p3.y2 = j2;
      pgrid[i1][j1].ship = true;
      pgrid[i2][j2].ship = true;
    }

    //REMOVE AFTER COMPLETE
    //Draw Opponent Ship
    for (int k = 1; k < 20; k++) {
      draw_line(20 * c1.x1 + 171, 20 * c1.y1 + 50 + k, 20 * (c1.x1 + 1) + 169, 20 * c1.y1 + 50 + k, 0x8433);
      draw_line(20 * c1.x2 + 171, 20 * c1.y2 + 50 + k, 20 * (c1.x2 + 1) + 169, 20 * c1.y2 + 50 + k, 0x8433);
      draw_line(20 * c1.x3 + 171, 20 * c1.y3 + 50 + k, 20 * (c1.x3 + 1) + 169, 20 * c1.y3 + 50 + k, 0x8433);
      draw_line(20 * c2.x1 + 171, 20 * c2.y1 + 50 + k, 20 * (c2.x1 + 1) + 169, 20 * c2.y1 + 50 + k, 0x8433);
      draw_line(20 * c2.x2 + 171, 20 * c2.y2 + 50 + k, 20 * (c2.x2 + 1) + 169, 20 * c2.y2 + 50 + k, 0x8433);
      draw_line(20 * c3.x1 + 171, 20 * c3.y1 + 50 + k, 20 * (c3.x1 + 1) + 169, 20 * c3.y1 + 50 + k, 0x8433);
      draw_line(20 * c3.x2 + 171, 20 * c3.y2 + 50 + k, 20 * (c3.x2 + 1) + 169, 20 * c3.y2 + 50 + k, 0x8433);
    }


    //Take turns guessing
    while (true) {
      //REPLACE WITH USER INPUT
      //User Guess
      int x, y;
      int ux = 0, uy = 0;

      while (cgrid[ux][uy].shot == true) {
        ux++;

        if (ux == 7) {
          ux = 0;
          uy++;
        }

      }
      draw_line(20 * ux + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * uy + 50, 0xBBBB);
      draw_line(20 * ux + 170, 20 * (uy + 1) + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xBBBB);
      draw_line(20 * ux + 170, 20 * uy + 50, 20 * ux + 170, 20 * (uy + 1) + 50, 0xBBBB);
      draw_line(20 * (ux + 1) + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xBBBB);
      while (true) {

        do {
          input = * addr_jp1port;
        } while ((input & 0x3F) == 0);
        
        do {
          
        } while (((*addr_jp1port) & 0x3F) != 0);
        for(int d=0;d<100;d++);
        draw_line(20 * ux + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * uy + 50, 0xFFFF);
        draw_line(20 * ux + 170, 20 * (uy + 1) + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xFFFF);
        draw_line(20 * ux + 170, 20 * uy + 50, 20 * ux + 170, 20 * (uy + 1) + 50, 0xFFFF);
        draw_line(20 * (ux + 1) + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xFFFF);
        //Right
        if (input & 0x01) {
          do {
            ux = (ux + 1) % 7;
          } while (cgrid[ux][uy].shot == true);
          //Left
        } else if (input & 0x02) {
          do {
            ux = ux - 1;
            if (ux < 0)
              ux = 6;
          } while (cgrid[ux][uy].shot == true);
          //Up
        } else if (input & 0x04) {
          do {
            uy = (uy + 1) % 7;
          } while (cgrid[ux][uy].shot == true);
          //Rotate
        } else if (input & 0x08) {
          do {
            uy = uy - 1;
            if (uy < 0)
              uy = 6;
          } while (cgrid[ux][uy].shot == true);
        } else if (input & 0x20) {

          break;
        }


		for(int d=0;d<100;d++);
        draw_line(20 * ux + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * uy + 50, 0xBBBB);
        draw_line(20 * ux + 170, 20 * (uy + 1) + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xBBBB);
        draw_line(20 * ux + 170, 20 * uy + 50, 20 * ux + 170, 20 * (uy + 1) + 50, 0xBBBB);
        draw_line(20 * (ux + 1) + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xBBBB);
      }
      draw_line(20 * ux + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * uy + 50, 0xFFFF);
      draw_line(20 * ux + 170, 20 * (uy + 1) + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xFFFF);
      draw_line(20 * ux + 170, 20 * uy + 50, 20 * ux + 170, 20 * (uy + 1) + 50, 0xFFFF);
      draw_line(20 * (ux + 1) + 170, 20 * uy + 50, 20 * (ux + 1) + 170, 20 * (uy + 1) + 50, 0xFFFF);

      cgrid[ux][uy].shot = true;
      if (cgrid[ux][uy].ship == 1) {
        draw_line(20 * ux + 171, 51 + 20 * uy, 20 * (ux + 1) + 169, 49 + 20 * (uy + 1), 0xAAAA);
        draw_line(20 * (ux + 1) + 169, 51 + 20 * uy, 20 * ux + 171, 49 + 20 * (uy + 1), 0xAAAA);
        c1.shot++;
        if (c1.shot == 3)
          c1.sunk = true;
      } else if (cgrid[ux][uy].ship == 2) {
        draw_line(20 * ux + 171, 51 + 20 * uy, 20 * (ux + 1) + 169, 49 + 20 * (uy + 1), 0xAAAA);
        draw_line(20 * (ux + 1) + 169, 51 + 20 * uy, 20 * ux + 171, 49 + 20 * (uy + 1), 0xAAAA);
        c2.shot++;
        if (c2.shot == 2)
          c2.sunk = true;
      } else if (cgrid[ux][uy].ship == 3) {
        draw_line(20 * ux + 171, 51 + 20 * uy, 20 * (ux + 1) + 169, 49 + 20 * (uy + 1), 0xAAAA);
        draw_line(20 * (ux + 1) + 169, 51 + 20 * uy, 20 * ux + 171, 49 + 20 * (uy + 1), 0xAAAA);
        c3.shot++;
        if (c3.shot == 2)
          c3.sunk = true;
      } else {
        draw_line(20 * ux + 171, 51 + 20 * uy, 20 * (ux + 1) + 169, 49 + 20 * (uy + 1), 0x199F);
        draw_line(20 * (ux + 1) + 169, 51 + 20 * uy, 20 * ux + 171, 49 + 20 * (uy + 1), 0x199F);
      }
      if (c1.sunk == true && c2.sunk == true && c3.sunk == true)
        break;

      for (int i = 0; i < 1000000000; i++);

      //Computer turn
      if (mode == 0) {
        do {
          x = rand() % 7;
          y = rand() % 7;
        } while (pgrid[x][y].shot != false);
      } else {
        if (p1.shot != 0 && p1.sunk == false) {
          int d1, d2;
          if (pgrid[p1.x1][p1.y1].shot == true && pgrid[p1.x2][p1.y2].shot == true) {
            d1 = p1.x2 - p1.x1;
            d2 = p1.y2 - p1.y1;
            do {
              if (rand() % 2 == 1) {
                x = p1.x1 - d1;
                y = p1.y1 - d2;
              } else {
                x = p1.x2 + d1;
                y = p1.y2 + d2;
              }
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          } else if (pgrid[p1.x2][p1.y2].shot == true && pgrid[p1.x3][p1.y3].shot == true) {
            d1 = p1.x3 - p1.x2;
            d2 = p1.y3 - p1.y2;
            do {
              if (rand() % 2 == 1) {
                x = p1.x2 - d1;
                y = p1.y2 - d2;
              } else {
                x = p1.x3 + d1;
                y = p1.y3 + d2;
              }
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          } else if (pgrid[p1.x1][p1.y1].shot == true) {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p1.x1 + d1;
              y = p1.y1 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          } else if (pgrid[p1.x2][p1.y2].shot == true) {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p1.x2 + d1;
              y = p1.y2 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          } else if (pgrid[p1.x3][p1.y3].shot == true) {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p1.x3 + d1;
              y = p1.y3 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          }
        } else if (p2.shot != 0 && p2.sunk == false) {
          int d1, d2;
          if (pgrid[p2.x1][p2.y1].shot == true) {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p2.x1 + d1;
              y = p2.y1 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          } else {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p2.x2 + d1;
              y = p2.y2 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          }
        } else if (p3.shot != 0 && p3.sunk == false) {
          int d1, d2;
          if (pgrid[p3.x1][p3.y1].shot == true) {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p3.x1 + d1;
              y = p3.y1 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          } else {
            do {
              d1 = rand() % 3 - 1;
              if (d1 == 0)
                d2 = rand() % 2 * 2 - 1;
              else
                d2 = 0;
              x = p3.x2 + d1;
              y = p3.y2 + d2;
            } while (x < 0 || x > 6 || y < 0 || y > 6 || pgrid[x][y].shot == true);
          }
        } else {
          do {
            x = rand() % 7;
            y = rand() % 7;
          } while (pgrid[x][y].shot != false);
        }
      }

      pgrid[x][y].shot = true;
      if (pgrid[x][y].ship == 1) {
        draw_line(20 * x + 11, 51 + 20 * y, 20 * (x + 1) + 9, 49 + 20 * (y + 1), 0xAAAA);
        draw_line(20 * (x + 1) + 9, 51 + 20 * y, 20 * x + 11, 49 + 20 * (y + 1), 0xAAAA);
        p1.shot++;
        if (p1.shot == 3)
          p1.sunk = true;
      } else if (pgrid[x][y].ship == 2) {
        draw_line(20 * x + 11, 51 + 20 * y, 20 * (x + 1) + 9, 49 + 20 * (y + 1), 0xAAAA);
        draw_line(20 * (x + 1) + 9, 51 + 20 * y, 20 * x + 11, 49 + 20 * (y + 1), 0xAAAA);
        p2.shot++;
        if (p2.shot == 2)
          p2.sunk = true;
      } else if (pgrid[x][y].ship == 3) {
        draw_line(20 * x + 11, 51 + 20 * y, 20 * (x + 1) + 9, 49 + 20 * (y + 1), 0xAAAA);
        draw_line(20 * (x + 1) + 9, 51 + 20 * y, 20 * x + 11, 49 + 20 * (y + 1), 0xAAAA);
        p3.shot++;
        if (p3.shot == 2)
          p3.sunk = true;
      } else {
        draw_line(20 * x + 11, 51 + 20 * y, 20 * (x + 1) + 9, 49 + 20 * (y + 1), 0x199F);
        draw_line(20 * (x + 1) + 9, 51 + 20 * y, 20 * x + 11, 49 + 20 * (y + 1), 0x199F);
      }
      if (p1.sunk == true && p2.sunk == true && p3.sunk == true)
        break;
      
    }

    for (int i = 0; i < 100000000; i++);

    //Draw Win/Lose
    if (p1.sunk == true && p2.sunk == true && p3.sunk == true)
      for (int i = 0; i < 240; i++)
        for (int j = 0; j < 320; j++)
          *
          (short int * )(pixel_buffer_start + (j << 1) + (i << 10)) = LOSE[i][j];
    else
      for (int i = 0; i < 240; i++)
        for (int j = 0; j < 320; j++)
          *
          (short int * )(pixel_buffer_start + (j << 1) + (i << 10)) = WIN[i][j];

    do {
      input = * addr_jp1port;
    } while ((input & 0x3F) == 0);
    do {
      input = * addr_jp1port;
    } while ((input & 0x3F) != 0);
	for(int d=0;d<100;d++);
  }

  return 0;
}

// code not shown for clear_screen() and draw_line() subroutines
void clear_screen() {
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      plot_pixel(x, y, 0x0);
    }
  }
}
void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
  bool is_steep = abs(y1 - y0) > abs(x1 - x0);
  if (is_steep) {
    x0 = x0 ^ y0;
    y0 = x0 ^ y0;
    x0 = x0 ^ y0;
    x1 = x1 ^ y1;
    y1 = x1 ^ y1;
    x1 = x1 ^ y1;
  }
  if (x0 > x1) {
    x0 = x0 ^ x1;
    x1 = x0 ^ x1;
    x0 = x0 ^ x1;
    y0 = y0 ^ y1;
    y1 = y0 ^ y1;
    y0 = y0 ^ y1;
  }
  int deltax = abs(x1 - x0);
  int deltay = abs(y1 - y0);
  int error = -(deltax / 2);
  int y = y0;
  int y_step;
  if (y0 < y1) {
    y_step = 1;
  } else {
    y_step = -1;
  }
  int x_step;
  if (x0 < x1) {
    x_step = 1;
  } else {
    x_step = -1;
  }
  for (int x = x0; x <= x1; x += x_step) {
    if (is_steep) {
      plot_pixel(y, x, line_color);
    } else {
      plot_pixel(x, y, line_color);
    }
    error = error + deltay;
    if (error >= 0) {
      y = y + y_step;
      error = error - deltax;
    }
  }
}

void draw_square(int i, int j, short int color) {
  for (int k = 1; k < 20; k++) {
    draw_line(20 * i + 11, 20 * j + 50 + k, 20 * (i + 1) + 9, 20 * j + 50 + k, color);

  }
}

void plot_pixel(int x, int y, short int line_color) {
  *(short int * )(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}
// code for subroutines (not shown)
void wait_for_vsync() {
  volatile int * pixel_ctrl_ptr = (int * ) 0xFF203020;
  register int status;
  * pixel_ctrl_ptr = 1;
  status = * (pixel_ctrl_ptr + 3);
  while ((status & 0x01) != 0) {
    status = * (pixel_ctrl_ptr + 3);
  }
}