#include "Chip8.h"

#include <glfw3.h>

GLFWwindow* window;

void drawScreen(Chip8& chip8) {
    glClear(GL_COLOR_BUFFER_BIT);

    //for loop that looks through the chip8's screen and checks whether a pixel is set, if the pixel is set then we draw
    for (int x = 0; x < 64; x++) {//rows
        glBegin(GL_QUADS); //let gl know we are drawing some quadrilaterals

        for (int y = 0; y < 32; y++) {//columns
            if (chip8.screen[x][y]){ //if pixel is set we draw
                glVertex2f(x*10, y*10); //make the square
                glVertex2f(x*10+10, y*10);
                glVertex2f(x*10+10, y*10+10);
                glVertex2f(x*10, y*10+10);
            }
        }
        glEnd(); //make gl end our drawing
    }

    glfwSwapBuffers(window); //swap out our buffers on the window
    glfwPollEvents(); //now let the events take place
}

int initWindow() {
    if (!glfwInit()) return -1; //failsafe for if we cannot make our window for some reason

    window = glfwCreateWindow(640, 320, "CChip8", nullptr, nullptr); //create our window and set the window variable equal to it
    if (!window) return -1; //if the window is broken for some reason return from the method

    //set up the glfw window and set it to 640*320
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 640, 320);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 320, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);

    return 0;
}
