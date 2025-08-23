#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

// These are the variables used to track the degree of rotation on the x, y, and z axes respectively
float A, B, C;

// set the screen 
int width = 100;
int height = 60;

// constants for circular calculations
const int R1 = 10; // Radius for the circle made by the cross-section of the torus
const int R2 = 20; // Radius for the circle made by the rotation of the centre point of the circle made by the cross-section of the torus around the y-axis
float theta; // responsible for the rotation of R1 that makes up the circle in the cross-section of the torus
float phi; // responsible for the rotation of the circle that makes up the torus

// set constants for projection later
const int distanceToScreen = 50;
const int distanceFromCam = 120;

// zbuffer to store depth values and fbuffer to store rendered frame before pushing to display
float zbuffer[100 * 60];
char fbuffer[100 * 60];

// the character used to fill in the background
int backgroundASCII = ' ';

// Ambient light term to keep dark sides slightly visible
const float AMBIENT = 0.15f;

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
    - float L, which represents the Luminance of the point 
* The function will fill in the rotated coordinate with the character given in ch
*/
void rotatedPoint(float x, float y, float z, float L) {
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
        /* Note the possible range of values of L.
         * L is the dot product of the rotated surface normal vector, and the light direction vector (0, 1, -1).
         * The dot product of two vectors a and b is geometrically |a||b|cos(theta) where theta is the angle btwn a and b.
         * Now, range of cos(theta) is [-1, 1].
         * surface normal vector is a normal vector, so magnitude is 1.
         * magnitude of (0, 1, -1) is sqrt(2).
         * Therefore, range of values of L is [-sqrt(2), sqrt(2)].*/
        // Original strict backface culling based on L > 0:
        // if (L > 0) {
        //     if (ooz > zbuffer[screenIndex]) {
        //         zbuffer[screenIndex] = ooz;
        //         int LchIndex = L * 8; // ranges ~[0,11]
        //         fbuffer[screenIndex] = ".,-~:;=!*#$@"[LchIndex];
        //     }
        // }

        // New approach: add ambient and don’t cull strictly on L.
        if (ooz > zbuffer[screenIndex]) {
            zbuffer[screenIndex] = ooz;

            // Add ambient and clamp. Use only the diffuse part when L > 0.
            float shade = AMBIENT + fmaxf(0.0f, L);
            // Map shade to character ramp indices [0..11]. Clamp to be safe.
            int LchIndex = (int)(shade * 8.0f);
            if (LchIndex < 0) LchIndex = 0;
            if (LchIndex > 11) LchIndex = 11;
            fbuffer[screenIndex] = ".,-~:;=!*#$@"[LchIndex];
        }
    }
    }


int main(void) {
    printf("\x1b[2J"); // ANSI escape sequence to clear the terminal for a nice view of the rendering
    
    while(1) {
        // fill the zbuffer with 0s and frame buffer with "background"
        memset(fbuffer, backgroundASCII, width * height);
        memset(zbuffer, 0, width * height * 4);

        for (theta = 0; theta < 2*M_PI; theta += 0.03) {
            for (phi = 0; phi < 2*M_PI; phi += 0.01) {
                // calculate surface normal
                float nx = cos(theta) * cos(phi);
                float ny = sin(theta);
                float nz = -cos(theta) * sin(phi);

                // rotate normal
                float rny = rotatedY(nx, ny, nz);
                float rnz = rotatedZ(nx, ny, nz);
                
                // luminance is the dot product of the rotated normal and the light vector (0,1,-1)
                float luminance = -rny -rnz;
                // luminance calculated using (0, 1, -1) vector as light direction
                //float luminance = (cos(phi)*cos(theta)*sin(C)) - (cos(A)*cos(theta)*sin(phi)) - (sin(A)*sin(theta)) + cos(C)*(cos(A)*sin(theta) - cos(theta)*sin(A)*sin(phi));
                rotatedPoint((R2 + R1*cos(theta)) * cos(phi), R1*sin(theta), -(R2 + R1*cos(theta))*sin(phi), luminance);
            }
        }

        // ANSI escape code to set cursor to Home position
        printf("\x1b[H");
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? fbuffer[k] : 10);
        }

        // spin effect
        A += 0.05;
        //B += 0.03;
        C += 0.02;
        usleep(10000);

    }
    return 0;
}
