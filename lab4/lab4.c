// Demo of heavily simplified sprite engine
// by Ingemar Ragnemalm 2009
// used as base for lab 4 in TSBK03.
// OpenGL 3 conversion 2013.

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "MicroGlut.h"
#endif

#include <stdlib.h>
#include "LoadTGA.h"
#include "SpriteLight.h"
#include "GL_utilities.h"
#include <math.h>

// L�gg till egna globaler h�r efter behov.
float kMaxDistance = 50.0;
float kCohesionWeight = 0.001;
float kAlignmentWeight = 0.5;

float euclideanDistance(FPoint a, FPoint b)
{
    return sqrt(pow(a.h - b.h,2) + pow(a.v - b.v,2));
}

void SpriteBehavior() // Din kod!
{
// L�gg till din labbkod h�r. Det g�r bra att �ndra var som helst i
// koden i �vrigt, men mycket kan samlas h�r. Du kan utg� fr�n den
// globala listroten, gSpriteRoot, f�r att kontrollera alla sprites
// hastigheter och positioner, eller arbeta fr�n egna globaler.
    SpritePtr i,j;
    
    
    i = j = gSpriteRoot;
    
    while( i != NULL)
    {
        
        int count = 0;
        
        // alignment
        i->speedDiff = (FPoint){0.0, 0.0};
        // cohesion
        i->averagePosition = (FPoint){0.0, 0.0};
        // seperation
        i->avoidanceVector = (FPoint){0.0, 0.0};
        
        while ( j != NULL)
        {
            if (i != j)
            {
                
                if ( euclideanDistance(i->position, j->position) < kMaxDistance)
                {
                    // alignment
                    i->speedDiff.h += j->speed.h - i->speed.h;
                    i->speedDiff.h += j->speed.v - i->speed.v;
                    
                    // cohesion
                    i->averagePosition.h += j->position.h;
                    i->averagePosition.v += j->position.v;
                    count++;
                }
            }
            j = j->next;
        }
        //get average
        if (count > 0)
        {
            // alignment
            i->speedDiff.h /= count;
            i->speedDiff.v /= count;
            
            // cohesion
            i->averagePosition.h /= count;
            i->averagePosition.v /= count;
        }
        
        i = i->next;
    }
    i = gSpriteRoot;
    while ( i != NULL)
    {
        i->speed.h += i->speedDiff.h * kAlignmentWeight;
        i->speed.v += i->speedDiff.v * kAlignmentWeight;
        
        // for cohesion
        i->speed.h += (i->averagePosition.h - i->position.h) * kCohesionWeight;
        i->speed.v += (i->averagePosition.v - i->position.v) * kCohesionWeight;
        
        
        //i->position.h += i->speed.h;
        //i->position.v += i->speed.v;
        
        i = i->next;
    }
    
    
    
}

// Drawing routine
void Display()
{
	SpritePtr sp;
	
	glClearColor(0, 0, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	DrawBackground();
	
	SpriteBehavior(); // Din kod!
	
// Loop though all sprites. (Several loops in real engine.)
	sp = gSpriteRoot;
	do
	{
		HandleSprite(sp); // Callback in a real engine
		DrawSprite(sp);
		sp = sp->next;
	} while (sp != NULL);
	
	glutSwapBuffers();
}

void Reshape(int h, int v)
{
	glViewport(0, 0, h, v);
	gWidth = h;
	gHeight = v;
}

void Timer(int value)
{
	glutTimerFunc(20, Timer, 0);
	glutPostRedisplay();
}

// Example of user controllable parameter
float someValue = 0.0;

void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '+':
    	someValue += 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case '-':
    	someValue -= 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
	
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett f�r
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart f�r
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 100, 200, 1, 1);
	NewSprite(sheepFace, 200, 100, 1.5, -1);
	NewSprite(sheepFace, 250, 200, -1, 1.5);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("SpriteLight demo / Flocking");
	
	glutDisplayFunc(Display);
	glutTimerFunc(20, Timer, 0); // Should match the screen synch
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	
	InitSpriteLight();
	Init();
	
	glutMainLoop();
	return 0;
}
