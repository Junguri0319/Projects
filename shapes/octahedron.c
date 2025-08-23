#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

// These are the variables used to track the degree of rotation on the x, y, and z axes respectively
float A, B, C;

// set the screen and cube dimensions
int width = 100;
int height = 60;

// set constants for projection later
const int distanceToScreen = 20;
const int distanceFromCam = 130;

const int pyramidBaseSize = 30;

// zbuffer to store depth values and fbuffer to store rendered frame before pushing to display
float zbuffer[100 * 60];
char fbuffer[100 * 60];

// the character used to fill in the background
int backgroundASCII = ' ';

// calculates the x coord after rotation
float rotatedX(float x, float y, float z) {
    return x * (cos(C) * cos(B)) + y * (cos(C) * sin(B) * sin(A) - sin(C) * cos(A)) + z * (cos(C) * sin (B) * cos(A) + sin(C) * sin(A));
}

// calculates the y coord after rotation
float rotatedY(float x, float y, float z) {
    return x * (sin(C) * cos (B)) + y * (sin(C) * sin(B) * sin(A) + cos(C) * cos(A)) + z * (sin(C) * sin(B) * cos(A) - cos(C) * sin(A));
}

// calculates the z coord after rotation
float rotatedZ(float x, float y, float z) {
    return x * (-sin(B)) + y * (cos(B) * sin(A)) + z * (cos(B) * cos(A));
}

/*
* This function takes 4 inputs:
    - floats x,y,z which represent the coordinates of a point (x,y,z)
    - int ch, which represents the ASCII code for the character we want to fill the coordinate with
* The function will fill in the rotated coordinate with the character given in ch
*/
void rotatedPoint(float x, float y, float z, int ch) {
    float rx = rotatedX(x, y, z); // rx for rotated x and so on
    float ry = rotatedY(x, y, z);
    float rz = rotatedZ(x, y, z) + distanceFromCam;

    float ooz = 1 / rz; // "one over z" to be used for zbuffer
    
    int projx = (int)(width / 2 + distanceToScreen * rx * ooz * 2); // projx for projected x
    int projy = (int)(height / 2 + distanceToScreen * ry * ooz); // projy for pojected y
    
    int screenIndex = projx + projy * width; // used to update frame and z buffers

    // this check is added to make sure a memory access error is not made
    // sometimes screenIndex goes over width * height for example
    if (screenIndex >= 0 && screenIndex < width * height) {
        if (ooz > zbuffer[screenIndex]) {
            zbuffer[screenIndex] = ooz;
            fbuffer[screenIndex] = ch;
        }
    }
}


int main(void) {
    printf("\x1b[2J"); // ANSI escape sequence to clear the terminal for a nice view of the rendering
    
    while(1) {
        // fill the zbuffer with 0s and frame buffer with "background"
        memset(fbuffer, backgroundASCII, width * height);
        memset(zbuffer, 0, width * height * 4);

        // loops for drawing triangular sides of pyramid 
        float pyramidApexY = -41.66;
        float pyramidBaseY = 30;

        float pyramidHeight = pyramidBaseY - pyramidApexY;

        float triangleTilt = 0;
        for (float t = pyramidBaseSize; t >= pyramidApexY; t -= 0.5) {
            float triangleWidthMult = (t - pyramidApexY) / pyramidHeight;
            float triangleWidth = triangleWidthMult * pyramidBaseSize;
            
            for (float u = -triangleWidth; u <= triangleWidth; u += 0.5) {
                rotatedPoint(pyramidBaseSize - triangleTilt, t + (pyramidApexY / 2), u, '@');
                rotatedPoint(-pyramidBaseSize + triangleTilt, t + (pyramidApexY / 2), u, '!');
                rotatedPoint(u, t + (pyramidApexY / 2), pyramidBaseSize - triangleTilt, ';');
                rotatedPoint(u, t + (pyramidApexY / 2), -pyramidBaseSize + triangleTilt, '*');
                rotatedPoint(pyramidBaseSize - triangleTilt, -t - pyramidApexY, -u, '-');
                rotatedPoint(-pyramidBaseSize + triangleTilt, -t - pyramidApexY, -u, '#');
                rotatedPoint(-u, -t - pyramidApexY, pyramidBaseSize - triangleTilt, '$');
                rotatedPoint(-u, -t - pyramidApexY, -pyramidBaseSize + triangleTilt, '.');
            }
            triangleTilt += 0.2;
        }

        // ANSI escape code to set cursor to Home position
        printf("\x1b[H");
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? fbuffer[k] : 10);
        }

        // spin effect
        A += 0.05;
        B += 0.05;
        C += 0.05;
        usleep(8000 * 2);

    }
    return 0;
}
