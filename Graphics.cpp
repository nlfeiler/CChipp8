#include "Chip8.h"

#include <glfw3.h>

GLFWwindow* window;

void drawScreen(Chip8& chip8) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {

            if (chip8.screen[x][y] != 0){
                glBegin(GL_QUADS);
                float px = x * 10.0f;
                float py = y * 10.0f;

                glVertex2f(px, py);
                glVertex2f(px + 10, py);
                glVertex2f(px + 10, py + 10);
                glVertex2f(px, py + 10);
                glEnd();
            }
        }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
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
