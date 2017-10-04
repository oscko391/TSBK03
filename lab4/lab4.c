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

// Lägg till egna globaler här efter behov.
float kMaxDistance = 200.0;
float kCohesionWeight = 0.001;
float kAlignmentWeight = 0.01;
float kAvoidanceWeight = 0.3;
float randomWeight = 0.4;

SpritePtr bSheep;

float euclideanDistance(FPoint a, FPoint b)
{
    return sqrt(pow(a.h - b.h,2) + pow(a.v - b.v,2));
}

FPoint calcAvoidance(SpritePtr i, SpritePtr j)
{
    FPoint avoidance = (FPoint){i->position.h - j->position.h, i->position.v - j->position.v};
    float distance = sqrt(pow(avoidance.h,2) + pow(avoidance.v, 2));
    
    avoidance.h *= (1.0 - distance/kMaxDistance) / distance;
    avoidance.v *= (1.0 - distance/kMaxDistance) / distance;
    
    return avoidance;
}

void SpriteBehavior() // Din kod!
{
// Lägg till din labbkod här. Det går bra att ändra var som helst i
// koden i övrigt, men mycket kan samlas här. Du kan utgå från den
// globala listroten, gSpriteRoot, för att kontrollera alla sprites
// hastigheter och positioner, eller arbeta från egna globaler.
    SpritePtr i,j;
      
      
      
    i = gSpriteRoot;
    
    while( i != NULL)
    {
       
        int count = 0;
        // alignment
        i->speedDiff = (FPoint){0.0, 0.0};
        // cohesion
        i->averagePosition = (FPoint){0.0, 0.0};
        // seperation
        i->avoidanceVector = (FPoint){0.0, 0.0};
        i->speedSetter = (FPoint){0.0, 0.0};
        j = gSpriteRoot;
        
        while ( j != NULL)
        {
	   
            if (i != j)
            {
                if ( euclideanDistance(j->position, i->position) < kMaxDistance)
                {
                    // alignment
                    i->speedDiff.h += j->speed.h - i->speed.h;
                    i->speedDiff.v += j->speed.v - i->speed.v;
                    
                    // cohesion
                    i->averagePosition.h += j->position.h;
                    i->averagePosition.v += j->position.v;
		    
                    // separation
                    FPoint a = calcAvoidance(i,j);
                    i->avoidanceVector.h += a.h;
                    i->avoidanceVector.v += a.v;
                    
                    count+=1;
                }
            }
            j = j->next;
        }
        //printf("%i\n",count);
        //get average
        if (count > 0)
        {
            // alignment
            i->speedDiff.h /= count;
            i->speedDiff.v /= count;
            
            // cohesion
            i->averagePosition.h /= count;
            i->averagePosition.v /= count;
            
            //separation
            i->avoidanceVector.h /= count;
            i->avoidanceVector.v /= count;
            
            i->speedSetter.h = (i->averagePosition.h - i->position.h) * kCohesionWeight
                                + i->speedDiff.h * kAlignmentWeight
                                + i->avoidanceVector.h * kAvoidanceWeight;
            i->speedSetter.v = (i->averagePosition.v - i->position.v) * kCohesionWeight
                                + i->speedDiff.v * kAlignmentWeight
                                + i->avoidanceVector.v * kAvoidanceWeight;
        }
        
        i = i->next;
    }
    i = gSpriteRoot;
    while ( i != NULL)
    {
        // for cohesion
        i->speed.h += i->speedSetter.h;
        i->speed.v += i->speedSetter.v;
        
        i = i->next;
    }
    
    bSheep->speed.h += fmodf(rand(), randomWeight) - randomWeight/2.0;
    bSheep->speed.v += fmodf(rand(), randomWeight) - randomWeight/2.0;
   
    
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
    	kCohesionWeight += 0.0005;
    	printf("kCohesionWeight = %f\n", kCohesionWeight);
    	break;
    case '-':
    	kCohesionWeight -= 0.0005;
    	printf("kCohesionWeight = %f\n", kCohesionWeight);
    	break;
    case 'w':
    	kAlignmentWeight += 0.005;
    	printf("kAlignmentWeight = %f\n", kAlignmentWeight);
    	break;
    case 's':
    	kAlignmentWeight -= 0.005;
    	printf("kAlignmentWeight = %f\n", kAlignmentWeight);
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
	
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett får
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart får
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 500, 300, 0.5, 0.5);
	NewSprite(sheepFace, 300, 500, -0.5, -0.5);
	NewSprite(sheepFace, 250, 200, -1, 1.5);
	NewSprite(sheepFace, 500, 200, 0.0, 1.0);
	NewSprite(sheepFace, 600, 200, -1.5, -0.5);
	NewSprite(sheepFace, 700, 200, 1.0, -1.0);
        bSheep = NewSprite(blackFace, 400, 300, 1.0, -1.0);
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
