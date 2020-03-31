#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

#define ESC (27)
#define TIMER_ID (0)
#define TIMER_ID_2 (1)
#define TIMER_INTERVAL (20)
#define DEFAULT (5)
#define UP (1)
#define DOWN (2)
#define LEFT (3)
#define RIGTH (4)

/* Coordinates help. */
//#define CS (1)

/* Light spots on the map. */
typedef struct _LightSpot {
    GLfloat x;
    GLfloat y;
    int isActive;
} LightSpot;

/* Global variables. */
FILE* dat;
char **walls=NULL,**start=NULL,**theEnd=NULL;
int size,i,j,arrow=DEFAULT,help=0,begin=0,level=0;
long int score=0;
GLfloat startX,startY,endX,endY;
LightSpot lightSpots[2];

/* Functions declarations. */
void onKeyboard(unsigned char key, int x, int y);
void onReshape(int width, int height);
void onDisplay(void);
void onTimer(int value);
void initLight(void);
void checkEnd(void);
void loadData(char* data);

/* Error check function. */
void greska(char* errMsg) {
    printf("Error in function: %s\n",errMsg);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    /* Initialize GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Initialize/Create window. */
    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("lavirint");
    
    /* Callback functions. */
    glutKeyboardFunc(onKeyboard);
    glutReshapeFunc(onReshape);
    glutDisplayFunc(onDisplay);
    
    /* Define clear color. */
    glClearColor(0.00f, 0.00f, 0.00f, 0.00f);

    /* Enable Z buffer. */
    glEnable(GL_DEPTH_TEST);

    /* Main loop. */
    glutMainLoop();
    
    /* Close file. */
    fclose(dat);
    
    exit(EXIT_SUCCESS);
}

void onKeyboard(unsigned char key, int x, int y) {
    switch (key) {
        /* Close game. */
        case ESC:
            /* Free memory. */
            if(walls!=NULL) {
                for(i=0;i<25;i++)
                    free(walls[i]);
                free(walls);
            }
            if(start!=NULL) {
                for(i=0;i<7;i++)
                    free(start[i]);
                free(start);
            }
            if(theEnd!=NULL) {
                for (i=0;i<7;i++)
                    free(theEnd[i]);
                free(theEnd);
            }
            exit(EXIT_SUCCESS);
            break;
        /* Start game animation. */
        case 'g':
        case 'G':
            begin=1;
            checkEnd();
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID_2);
            break;
        /* Translate ball down. */
        case 's':
        case 'S':
            arrow=DOWN;
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID);
            score++;
            break;
        /* Translate ball up. */
        case 'w':
        case 'W':
            arrow=UP;
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID);
            score++;
            break;
        /* Translate ball left. */
        case 'a':
        case 'A':
            arrow=LEFT;
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID);
            score++;
            break;
        /* Translate ball right. */
        case 'd':
        case 'D':
            arrow=RIGTH;
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID);
            score++;
            break;
    }
}

void onReshape(int width, int height) {
    /* Initialize viewport. */
    glViewport(0,0,width,height);
    
    /* Initialize projection. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,(float)width/height,1,100);
}

void onDisplay(void){
    /* Clear buffers. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* Set point of view. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            0, 0, 25,
            0, 0, 0,  
            0, 1, 0
    );
    
    /* Draw coordinates beginning. */
    #ifdef CS
        glBegin(GL_LINES);
            glColor3f(0.00f, 0.00f, 1.00f); //blue -x
            glVertex3f(0.00f, 0.00f, 0.00f);
            glVertex3f(100.00f, 0.00f, 0.00f);
            
            glColor3f(1.00f, 0.00f, 0.00f); //red -y
            glVertex3f(0.00f, 0.00f, 0.00f);
            glVertex3f(0.00f, 100.00f, 0.00f);
            
            glColor3f(0.00f, 1.00f, 0.00f); //green -z
            glVertex3f(0.00f, 0.00, 0.00f);
            glVertex3f(0.00f, 0.00f,100.00f);
         glEnd();
    #endif
     
    if(begin==1) {
        /* Translate whole picture. */
        glTranslatef(-size/2,size/2,0);
        
        initLight();

        /* Draw light spots. */
        for(i=0;i<2;i++){
        
            if(lightSpots[i].isActive == 1) {
                glPushMatrix();
                    /* Material property. */
                    GLfloat diffuseCoeffsBall[] = {0.878f, 0.968f, 0.168f, 1.00f};
                    /* Apply material property. */
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffsBall);
                    glTranslatef(lightSpots[i].y,-lightSpots[i].x,0);
                    glScalef(0.30f, 0.30f, 0.30f);
                    glutSolidCube(1);
                glPopMatrix(); 
            }
        }
        /* Draw ball. */
        /* Material property. */
        GLfloat diffuseCoeffsBall[] = {0.980f, 0.364f, 0.341f, 1.00f};
        /* Apply material property. */
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffsBall);
        glPushMatrix();
            glTranslatef(startX,startY,0);
            glutSolidSphere((GLdouble)0.4, 50, 50);
        glPopMatrix();
        
        /* Draw labyrinth. */
        /* Material property. */
        GLfloat diffuseCoeffsLabyrinth[] = {0.121f, 1.0f, 1.0f, 1.00f};
        /* Apply material property. */
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffsLabyrinth);
        for(i=0;i<size;i++)
            for(j=0;j<size;j++)
            {
                if(walls[i][j]=='#'){
                    glPushMatrix();
                        glColor3f(0,1,0);
                        glTranslatef(j,-i,0);
                        glutSolidCube(1);
                    glPopMatrix();
                }
            }
        
        /* Translate to coordinates beginning. */
        glTranslatef(size/2,-size/2,0);
        
        /* Draw terrain. */
        /* Material property. */
        GLfloat diffuseCoeffsTerrain[] = {1.0f, 0.0f, 0.0f, 1.00f};
        /* Apply material property. */
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffsTerrain);
        glTranslatef(0,0,-0.5);
        glBegin(GL_POLYGON);
            glVertex3f((float)size/2.0f, (float)size/2.0f,0);
            glVertex3f(-(float)size/2.0f, (float)size/2.0f, 0);
            glVertex3f(-(float)size/2.0f, -(float)size/2.0f, 0);
            glVertex3f((float)size/2.0f, -(float)size/2.0f, 0);
        glEnd();
        glTranslatef(0,0,0.5);
    } else if (begin==0) {
        
        /* Initialize file. */
        dat=fopen("start.txt","r");
        
        /* Initialize labyrinth matrix. */
        start=(char**) malloc(sizeof(char*)*7);
        if(start==NULL)
            greska("main -> first malloc");
        for(i=0;i<7;i++) {
            start[i]=(char*) malloc(sizeof(char)*25);
        if(start[i]==NULL)
            greska("main -> second malloc");
        }
    
        /* Load labyrinth. */
        for(i=0;i<7;i++)
            for(j=0;j<25;j++) 
                fscanf(dat,"%c",&start[i][j]);
        
        initLight();
        /* Material property. */
        GLfloat diffuseCoeffsLabyrinth[] = {0.121f, 1.0f, 1.0f, 1.00f};
        /* Apply material property. */
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffsLabyrinth);
        glTranslatef(-12,8,0);
        for(i=0;i<7;i++)
            for(j=0;j<25;j++){
                if(start[i][j]=='#'){
                    glPushMatrix();
                        glColor3f(0.349, 0.650, 0.356);
                        glTranslatef(j,-i,0);
                        glutSolidCube(1);
                    glPopMatrix();
                }
            }
        glTranslatef(12,-8,0);
    } else if(begin==-1) {
        /* Initialize file. */
        dat=fopen("end.txt","r");
        
        /* Initialize labyrinth matrix. */
        theEnd=(char**) malloc(sizeof(char*)*7);
        if(theEnd==NULL)
            greska("main -> first malloc");
        for(i=0;i<7;i++) {
            theEnd[i]=(char*) malloc(sizeof(char)*30);
            if(theEnd[i]==NULL)
                greska("main -> second malloc");
        }
    
        /* Load labyrinth. */
        for(i=0;i<7;i++)
            for(j=0;j<30;j++) 
                fscanf(dat,"%c",&theEnd[i][j]);
        
        startX=0;
        startY=0;
        
        initLight();
        
        /* Material property. */
        GLfloat diffuseCoeffsLabyrinth[] = {0.121f, 1.0f, 1.0f, 1.00f};
        /* Apply material property. */
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffsLabyrinth);
        glTranslatef(-15,3,0);
        for(i=0;i<7;i++)
            for(j=0;j<30;j++){
                if(theEnd[i][j]=='#'){
                    glPushMatrix();
                        glColor3f(0.349, 0.650, 0.356);
                        glTranslatef(j,-i,0);
                        glutSolidCube(1);
                    glPopMatrix();
                }
            }
        glTranslatef(15,-3,0);
    }
    /* Swap buffers. */
    glutSwapBuffers();
}

void onTimer(int value) {
    /* Timer for main game animation. */
    if(value==TIMER_ID) {
        /* Change coordinates of the ball. */
        if(arrow==DOWN) {
            if(-(startY-1)>=0 && -(startY-1)<size)
            if(walls[abs((int)startY-1)][(int)startX]!='#') {
                startY=startY-1;
            }
        }
        else if(arrow==LEFT) {
            if((startX-1)>=0 && (startX-1)<size)
            if(walls[abs((int)startY)][(int)startX-1]!='#') {
                startX=startX-1;
            }
        }
        else if(arrow==UP) {
            if(-(startY+1)>=0 && -(startY+1)<size)
            if(walls[abs((int)startY+1)][(int)startX]!='#') {
                startY=startY+1;
            }
        }
        else if(arrow==RIGTH) {
            if((startX+1)>=0 && (startX+1)<size)
            if(walls[abs((int)startY)][(int)startX+1]!='#') {
                startX=startX+1;
            }
        }
        
        for (i = 0; i < 2; i++) {
            if(fabs(startX) == lightSpots[i].y) {
                if (fabs(startY) == lightSpots[i].x) {
                    lightSpots[i].isActive = 0;
                    help=0;
                    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,180.00f);
                    glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID_2);
                }
            }
        }
        checkEnd();
    }
    
    /* Lighting on. */
    if(value==TIMER_ID_2) {
        if(help<=90 ) {
            help++;
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID_2);
        }
    }
    glutPostRedisplay();
}


void initLight(void){
    /* Light properties. */
    GLfloat lightPosition[] = {startX, startY, 3.00f, 1.00f};
    GLfloat lightAmbient[] = {0.00f, 0.00f, 0.00f, 1.00f};
    GLfloat lightDiffuse[] = {1.00f, 1.00f, 1.00f, 1.00f};
    GLfloat lightSpecular[] = {1.00f, 1.00f, 1.00f, 1.00f};

    /* Material properties. */
    GLfloat ambientCoeffs[] = {0.0f, 0.0f, 0.0f, 0.00f};
    GLfloat specularCoeffs[] = {0.00f, 0.00f, 0.00f, 1.00f};
    GLfloat shininess = 20.00f;

    /* Smooth shading. */
    glShadeModel(GL_SMOOTH);

    /* Enable lighting. */
    glEnable(GL_LIGHTING);

    /* Enable first light. */
    glEnable(GL_LIGHT0);

    /* Apply light properties. */
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    /* Apply material properties. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularCoeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
    /* Light cutoff .*/
    if(help>90) {
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.00f);
        GLfloat spotDirection[] = {0.00f, 0.00f, -1.00f};
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
    } 
}

void checkEnd(void) {
    if(abs(startX)==endX && abs(startY)==endY) {
        if(level==0){
            help=0;
            level++;
            
            /* Initialize labyrinth matrix. */
            walls=(char**) malloc(sizeof(char*)*25);
            if(walls==NULL)
                greska("main -> first malloc");
            for(i=0;i<25;i++) {
                walls[i]=(char*) malloc(sizeof(char)*25);
                if(walls[i]==NULL)
                    greska("main -> second malloc");
            }
    
            loadData("sketch15.txt");
                
        } else if(level==1) {
            level++;
            help=0;
            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,180.00f);
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID_2);
            
            loadData("sketch19.txt");
            
            printf("Your score for level 1 is %ld\n",score);
            score=0;
        } else if(level==2) {
            level++;
            help=0;
            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,180.00f);
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID_2);
            
            loadData("sketch21.txt");
            
            printf("Your score for level 2 is %ld\n",score);
            score=0;
        } else if(level==3) {
            level++;
            help=0;
            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,180.00f);
            glutTimerFunc(TIMER_INTERVAL,onTimer,TIMER_ID_2);
            
            loadData("sketch25.txt");
            
            printf("Your score for level 3 is %ld\n",score);
            score=0;
        } else if(level==4) {
            level++;
            begin=-1;
            help=0;
            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,180.00f);
            glutPostRedisplay();
            
            printf("Your score for level 4 is %ld\n",score);
            score=0;
        }
    }
}

void loadData(char* data) {
    /* Initialize file. */
    dat=fopen(data,"r");
    
    /* Scan file. */
    fscanf(dat, "%d", &size);
    fscanf(dat, "%f", &startY);
    fscanf(dat, "%f", &startX);
    fscanf(dat, "%f", &endY);
    fscanf(dat, "%f", &endX);
    
    /* Load light spots. */
    for (i = 0; i < 2; i ++) {
        fscanf(dat, "%f", &lightSpots[i].x);
        fscanf(dat, "%f", &lightSpots[i].y);
        lightSpots[i].isActive = 1;
    }
    
    /* Load labyrinth. */
    for(i=0;i<size;i++)
        for(j=0;j<size;j++) 
            fscanf(dat,"%c",&walls[i][j]);
}
