#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926
#define P2 PI/2
#define P3 3*PI/2
//#define DEG 0.0174533


#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024

#define FOV (PI / 3)  // 60 degrees FOV

#define NUM_OF_RAYS 960 //30 //60 //960 //480 //240 //120
#define RAY_ANGLE_INC (FOV / NUM_OF_RAYS)
#define MAP_SIZE 225//should be a multiple of MAP_ARRAY
#define MAP_ARRAY 15//the size of our map in cells, maps always square
#define MAP_CELL_SIZE MAP_SIZE/MAP_ARRAY
#define PLAYER_SIZE    MAP_CELL_SIZE/3
#define VIEW_STRIP    WINDOW_WIDTH/NUM_OF_RAYS
#define TURNING_ANGLE 0.07
#define SPEED    PLAYER_SIZE/7

#define MOUSE_SENSITIVITY 0.00015

//puts them in the middle of cell 1,1 facing right

float playerX = MAP_CELL_SIZE*1.5;
float playerY = MAP_CELL_SIZE*0.5;

float playerAngle = 1.5;

int keyStates[256] = {0};


int map[] = 
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
     1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
     1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
     1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1,
     1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1,
     1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1,
     1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,
     1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
     1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1,
     1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1,
     1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
     1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
     1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
     

struct Ray{
    float distance;
    float angle;
    bool vertical;
    int wallType;
};

Ray rays[NUM_OF_RAYS];


void drawMinimap(){
    //to put the map in the bottm right
    float translateY = WINDOW_HEIGHT - MAP_SIZE;

    //draw the maps border
    glColor3f(0,0,0);
    glBegin(GL_QUADS);
        glVertex2f(0, translateY);
        glVertex2f(MAP_SIZE, translateY);
        glVertex2f(MAP_SIZE, translateY + MAP_SIZE);
        glVertex2f(0, translateY + MAP_SIZE);
    glEnd();

    //loop through map
    for(int i=0 ; i<MAP_ARRAY ; i++){
        for(int j=0 ; j<MAP_ARRAY; j++){

            //pick colour based on wall type
            switch(map[i*MAP_ARRAY+j]){
                case 0: 
                    glColor3f(0,0,0);
                    break;
                case 1: 
                    glColor3f(255,255,255);
                    break;
            }

            //draw           
            glBegin(GL_QUADS);
                    glVertex2f(MAP_CELL_SIZE*j+1, translateY + MAP_CELL_SIZE*i+1);
                    glVertex2f(MAP_CELL_SIZE*j+MAP_CELL_SIZE-1,translateY + MAP_CELL_SIZE*i+1);
                    glVertex2f(MAP_CELL_SIZE*j+MAP_CELL_SIZE-1,translateY + MAP_CELL_SIZE*i+MAP_CELL_SIZE-1);
                    glVertex2f(MAP_CELL_SIZE*j+1,translateY + MAP_CELL_SIZE*i+MAP_CELL_SIZE-1);
            glEnd();
            
        }

    }


    
    glPointSize(PLAYER_SIZE);
    glColor3f(0,255,0);

    glBegin(GL_POINTS);
        glVertex2f(playerX, translateY + playerY);
    glEnd();
    
}






void drawView(){

    //draw the floor
    glColor3f(0,0,0);
    glBegin(GL_QUADS);
        glVertex2f(0, WINDOW_HEIGHT/2);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT/2);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
        glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    //draw each part of the wall
    float lineHeight;
    float fixFishEye;
    float intensity;
    for(int i=0 ; i<sizeof(rays)/sizeof(Ray) ; i++){

        // fix distortion (fish-eye effect)
        fixFishEye = playerAngle - rays[i].angle;
        if(fixFishEye < 0){
            fixFishEye += 2 * PI;
        }else if(fixFishEye > 2 * PI){
            fixFishEye -= 2 * PI;
        }
        rays[i].distance = rays[i].distance * cos(fixFishEye);

        intensity = 1.0 / (1.0 + 0.03 * rays[i].distance); // Adjust 0.01 for stronger/weaker effect

        switch(rays[i].wallType){
            case 1: 
                if(rays[i].vertical){
                    glColor3f(0.9 * intensity, 0.9 * intensity, 0.9 * intensity);
                }else{
                    glColor3f(0.7 * intensity, 0.7 * intensity, 0.7 * intensity);
                }
                break;
        }

        // set line height and draw
        lineHeight = (MAP_CELL_SIZE * WINDOW_HEIGHT) / rays[i].distance;
        glBegin(GL_QUADS);
            glVertex2f(i * VIEW_STRIP, WINDOW_HEIGHT/2 - lineHeight/2);
            glVertex2f((i + 1) * VIEW_STRIP, WINDOW_HEIGHT/2 - lineHeight/2);
            glVertex2f((i + 1) * VIEW_STRIP, WINDOW_HEIGHT/2 + lineHeight/2);
            glVertex2f(i * VIEW_STRIP, WINDOW_HEIGHT/2 + lineHeight/2);
        glEnd();
    }
}





//helper function for casting rays
float distance(float ax, float bx, float ay, float by){
    return sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));
}



//casts a bunch o rays
void castRays(){
    float rayY, rayX, xOffset, yOffset;
    int mapX, mapY,mapIndex, hWallType, vWallType;
    int depth, maxDepth=MAP_ARRAY; //how many times we loop before giving up

    float rayAngle = playerAngle - FOV / 2;

    if(rayAngle<0){//constrain angle
            rayAngle += 2*PI;
    }else if(rayAngle > 2*PI){
        rayAngle -= 2*PI;
    }
    

    for(int i=0  ;i<NUM_OF_RAYS ; i++){        
        //HORIZONTAL RAY CHECK
        float hX, hY, hDistance=9999999999;
        depth=0;
        float aTan = -1/tan(rayAngle);

        //initial maths to find the ray cords and offsets
        if(rayAngle==PI || rayAngle==0){//looking straight left or right, never gonna collide
            rayX = playerX;
            rayY = playerY;
            depth = 8;
        }else{
            if(rayAngle > PI){//looking up
                rayY = ((int)playerY)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE)-0.0001;
                yOffset = -MAP_CELL_SIZE;
            }else{//looking down
                rayY = (((int)playerY)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE))+MAP_CELL_SIZE;
                yOffset = MAP_CELL_SIZE;
            }
            rayX = (playerY-rayY)*aTan+playerX;
            xOffset = -yOffset*aTan;
        }

        //figure out when we hit a wall
        while(depth < maxDepth){
            mapX = ((int)rayX)/((int)MAP_CELL_SIZE);
            mapY = rayY/((int)MAP_CELL_SIZE);
            mapIndex = mapY*MAP_ARRAY+mapX;
            if(mapIndex >= 0 && mapIndex < MAP_ARRAY*MAP_ARRAY && map[mapIndex]!=0){//hit a wall
                depth = maxDepth;
                hWallType =  map[mapIndex];
                hX = rayX;
                hY = rayY;
                hDistance = distance(playerX, hX, playerY, hY);
            }else{
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        

        //VERTICAL RAY CHECK
        //basically identidentical to horizontal check
        float vX, vY, vDistance=9999999999;
        depth=0;
        float nTan = -tan(rayAngle);


        //initial maths to find the ray cords and offsets
        if(rayAngle==P2 || rayAngle==P3){//looking straight up or down, never gonna collide
            rayX = playerX;
            rayY = playerY;
            depth = 8;
        }else{
            if(rayAngle > P2 && rayAngle < P3){//looking up
                rayX = ((int)playerX)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE)-0.0001;
                xOffset = -MAP_CELL_SIZE;
            }else{//looking down
                rayX = (((int)playerX)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE))+MAP_CELL_SIZE;
                xOffset = MAP_CELL_SIZE;
                
            }
            rayY = (playerX-rayX)*nTan+playerY;
            yOffset = -xOffset*nTan;
        }

        //figure out when we hit a wall
        while(depth < maxDepth){
            mapX = ((int)rayX)/((int)MAP_CELL_SIZE);
            mapY = rayY/((int)MAP_CELL_SIZE);
            mapIndex = mapY*MAP_ARRAY+mapX;
            if(mapIndex >= 0 && mapIndex < MAP_ARRAY*MAP_ARRAY && map[mapIndex]!=0){//hit a wall
                depth = maxDepth;
                vWallType = map[mapIndex];
                vX = rayX;
                vY = rayY;
                vDistance = distance(playerX, vX, playerY, vY);
            }else{
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        //pick the smallest of the two
        if(hDistance < vDistance){
            rayX = hX;
            rayY = hY;
            rays[i].distance = hDistance;
            rays[i].vertical = false;
            rays[i].wallType = hWallType;
            rays[i].angle = rayAngle;
        }else{
            rays[i].distance = vDistance;
            rays[i].vertical = true;
            rays[i].wallType = vWallType;
            rays[i].angle = rayAngle;
        }

        //increase angle for next ray
        rayAngle +=RAY_ANGLE_INC;
        if(rayAngle<0){//constrain angle
            rayAngle += 2*PI;
        }else if(rayAngle > 2*PI){
            rayAngle -= 2*PI;
        }
    }
}


//TODO collision detection
//tank controls
//not sure why we need int x and y as args
void updateMovement() {
    float nextX_w, nextY_w, nextX_s, nextY_s, nextX_a, nextY_a, nextX_d, nextY_d;
    int nextMapIndex_w, nextMapIndex_s, nextMapIndex_a, nextMapIndex_d;

    if (keyStates['w']) { // Move forward
        nextX_w = playerX + cos(playerAngle) * SPEED;
        nextY_w = playerY + sin(playerAngle) * SPEED;
        nextMapIndex_w = ((int)nextY_w / ((int)MAP_CELL_SIZE)) * MAP_ARRAY + ((int)nextX_w / ((int)MAP_CELL_SIZE));
        if (nextMapIndex_w >= 0 && nextMapIndex_w < MAP_ARRAY * MAP_ARRAY && map[nextMapIndex_w] == 0) {
            playerX = nextX_w;
            playerY = nextY_w;  
        }
    }
    if (keyStates['s']) { // Move backward
        nextX_s = playerX - cos(playerAngle) * SPEED;
        nextY_s = playerY - sin(playerAngle) * SPEED;
        nextMapIndex_s = ((int)nextY_s / ((int)MAP_CELL_SIZE)) * MAP_ARRAY + ((int)nextX_s / ((int)MAP_CELL_SIZE));
        if (nextMapIndex_s >= 0 && nextMapIndex_s < MAP_ARRAY * MAP_ARRAY && map[nextMapIndex_s] == 0) {
            playerX = nextX_s;
            playerY = nextY_s;  
        }
    }
    if (keyStates['a']) { // Move left
        nextX_a = playerX + sin(playerAngle) * SPEED; // Adjusting for left turn
        nextY_a = playerY - cos(playerAngle) * SPEED; // Adjusting for left turn
        nextMapIndex_a = ((int)nextY_a / MAP_CELL_SIZE) * MAP_ARRAY + ((int)nextX_a / MAP_CELL_SIZE);
        nextMapIndex_a = ((int)nextY_a / ((int)MAP_CELL_SIZE)) * MAP_ARRAY + ((int)nextX_a / ((int)MAP_CELL_SIZE));
        if (nextMapIndex_a >= 0 && nextMapIndex_a < MAP_ARRAY * MAP_ARRAY && map[nextMapIndex_a] == 0) {
            playerX = nextX_a;
            playerY = nextY_a;  
        }
    }
    if (keyStates['d']) { // Move right
        nextX_d = playerX - sin(playerAngle) * SPEED; // Adjusting for right turn
        nextY_d = playerY + cos(playerAngle) * SPEED; // Adjusting for right turn
        nextMapIndex_d = ((int)nextY_d / ((int)MAP_CELL_SIZE)) * MAP_ARRAY + ((int)nextX_d / ((int)MAP_CELL_SIZE));
        if (nextMapIndex_d >= 0 && nextMapIndex_d < MAP_ARRAY * MAP_ARRAY && map[nextMapIndex_d] == 0) {
            playerX = nextX_d;
            playerY = nextY_d;  
        }
    }
    
    glutPostRedisplay(); // Request a redraw
}

void buttons(unsigned char key, int x, int y) {
    keyStates[key] = 1; // Mark the key as pressed

    // Handle any actions that should happen only once (e.g., exit on ESC)
    if (key == 27) {
        exit(0);
    }
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = 0; // Mark the key as released
}


void mouseMotion(int x, int y) {
    static int lastX = -1;
    static bool isWarping = false;

    int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

    // If we're in the middle of a warp, ignore this event
    if (isWarping) {
        isWarping = false;  // Reset flag to resume regular motion
        return;
    }

    if (lastX == -1) {  
        lastX = x;  // Initialize lastX to the current x on the first call
    }

    // Calculate the change in x position
    int deltaX = x - lastX;
    playerAngle += deltaX * MOUSE_SENSITIVITY;

    // Warp the pointer to the center to allow for continuous movement
    isWarping = true;  // Set flag to indicate warping (avoiding looping)
    glutWarpPointer(centerX, centerY);
    lastX = centerX;  // Set lastX to center to keep deltaX calculation consistent

    glutPostRedisplay();  // Update the display
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw elements
    castRays();
    drawView();
    drawMinimap();

    glutSwapBuffers(); 
}

void init(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glEnable(GL_MULTISAMPLE);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Dungeon");
    glClearColor(0, 0, 0, 0); 
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0); 
    glutSetCursor(GLUT_CURSOR_NONE); // Hide the cursor
    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2); // Center the cursor
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
}

void timer(int value) {
    updateMovement();  // Update movement on each timer tick
    glutTimerFunc(16, timer, 0);  // Set the timer to call every 16 ms (60 FPS)
}

int main(int argc, char** argv){	
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutKeyboardUpFunc(keyUp);  // Register key release handler
    glutTimerFunc(0, timer, 0); // Start the timer
    glutMainLoop();
    return 0;
}

