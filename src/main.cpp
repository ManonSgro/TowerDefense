#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cmath>
#include "map_reader.h"
#include "utils.h"
#include "monster.h"
#include "tower.h"
#include "player.h"
#include "installation.h"

#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <limits>

#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

#define MAX_TEXTURES 10
 
using namespace std;
 
//auto comp = [](const pair<int, int> &a, const pair<int, int> &b) {return a.second > b.second; };
 
 void draw_elem(GLuint texture_id, float x, float y, float coeff) { 
    glColor3d(255,255,255);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
    glPushMatrix();
        glBegin(GL_QUADS);
    
	    glTexCoord2f(0, 1);
        glVertex2f(-coeff, coeff);
        //glVertex2f(1, 1);
    
        glTexCoord2f(1, 1);
        glVertex2f(coeff, coeff);
        //glVertex2f(-1, 1);
    
        glTexCoord2f(1, 0);
        //glVertex2f(-1, -1);
        glVertex2f(coeff, -coeff);
    
        glTexCoord2f(0, 0);
        glVertex2f(-coeff, -coeff);
        //glVertex2f(1, -1);

	    glEnd();
    glPopMatrix();


    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
} 

 void draw_enemy(GLuint texture_id, float x, float y, float coeff) { 
    glColor3d(255,255,255);
    glEnable(GL_TEXTURE_2D);

	//coeff = coeff - 0.5f;
	//coeff = 0.07f;

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	glRotatef(-45.0f,0.0f,0.0f,1.0f);
    glPushMatrix();
        glBegin(GL_QUADS);
    
	    glTexCoord2f(0, 1);
        glVertex2f(coeff, coeff);
        //glVertex2f(1, 1);
    
        glTexCoord2f(1, 1);
        glVertex2f(-coeff, coeff);
        //glVertex2f(-1, 1);
    
        glTexCoord2f(1, 0);
        //glVertex2f(-1, -1);
        glVertex2f(-coeff, -coeff);
    
        glTexCoord2f(0, 0);
        glVertex2f(coeff, -coeff);
        //glVertex2f(1, -1);

	    glEnd();
    glPopMatrix();


    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
}

void draw_circle(float x, float y, float radius) { 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0.0f);
    static const int circle_points = 100;
    static const float angle = 2.0f * 3.1416f / circle_points;

    glBegin(GL_POLYGON);
    double angle1=0.0;
    glVertex2d(radius * cos(0.0) , radius * sin(0.0));
    int i;
    for (i=0; i<circle_points; i++)
    {       
        glVertex2d(radius * cos(angle1), radius *sin(angle1));
        angle1 += angle;
    }
    glEnd();
    glPopMatrix();
}

typedef struct {
	int screen_width, screen_height;
	int sdlflags;
	int screen_pixels;
	float scale;
	Map* map;
	GLuint textures[MAX_TEXTURES];
} App;

void setup_opengl( int width, int height )
{
	float ratio = (float) width / (float) height;

	/* Set the clear color. */
	glClearColor( 0, 0, 0, 0 );

	/* Setup our viewport. */
	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(-ratio,ratio,1,-1);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	SDL_GL_SwapBuffers();
}

bool handle_key_down(App* app, SDL_keysym* keysym )
{
	switch( keysym->sym ) {
	case SDLK_ESCAPE:
		return false;
	case SDLK_SPACE:
		break;
	case SDLK_PLUS:
	case SDLK_KP_PLUS:
		app->scale += 0.2f;
		break;
	case SDLK_MINUS:
	case SDLK_KP_MINUS:
		app->scale -= 0.2f;
		break;
	default:
		break;
	}
	return true;
}

void init_draw(App* app)
{
	glEnable(GL_BLEND); // enable blending
	glEnable(GL_TEXTURE_2D);
	// transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);

	glGenTextures(MAX_TEXTURES, app->textures);
	glBindTexture(GL_TEXTURE_2D, app->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB,
							 app->map->map_data->x, app->map->map_data->y, 0, GL_RGB,
							 GL_UNSIGNED_BYTE, app->map->map_data->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void draw_background(GLuint texture_id){
	glColor3d(255,255,255);
		glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, texture_id);

        glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

        glPushMatrix();
            glBegin(GL_QUADS);
        
		    glTexCoord2f(0, 1);
            glVertex2f(-1.66f, 1.0f);
        
            glTexCoord2f(1, 1);
            glVertex2f(1.66f, 1.0f);
        
            glTexCoord2f(1, 0);
            glVertex2f(1.66f, -1.0f);
        
            glTexCoord2f(0, 0);
            glVertex2f(-1.66f, -1.0f);

		    glEnd();
        glPopMatrix();


        glBindTexture(GL_TEXTURE_2D, 0);
        
        glDisable(GL_TEXTURE_2D);

}

void draw_screen(App* app)
{

	static GLfloat v0[] = { -1.0f, -1.0f,  1.0f };
	static GLfloat v1[] = {  1.0f, -1.0f,  1.0f };
	static GLfloat v2[] = {  1.0f,  1.0f,  1.0f };
	static GLfloat v3[] = { -1.0f,  1.0f,  1.0f };

	static GLfloat red[] = { 1.0f, 0.0f,  0.0f };
	static GLfloat white[] = {  1.0f, 1.0f,  1.0f };
	static GLfloat blue[] = {  0.2f,  0.3f,  0.8f };
	static GLfloat green[] = { 0.1f,  0.9f,  0.5f };
	/* Clear the color and depth buffers. */
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/* We don't want to modify the projection matrix. */
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	glScalef(app->scale+0.6, app->scale, 1);

	/* Send our triangle data to the pipeline. */
	glBindTexture(GL_TEXTURE_2D, app->textures[0]);
	glBegin( GL_QUADS );

	glColor3fv( white );
	glTexCoord2f(0, 0);
	glVertex3fv( v0 );
	glTexCoord2f(1, 0);
	glVertex3fv( v1 );
	glTexCoord2f(1, 1);
	glVertex3fv( v2 );
	glColor3fv( white );
	glTexCoord2f(0, 1);
	glVertex3fv( v3 );

	glEnd( );
	glBindTexture(GL_TEXTURE_2D, 0);

	//draw_circle(david.getPositionX(), david.getPositionY(), 100);

	//SDL_GL_SwapBuffers( );
}


bool process_events(App* app)
{
	SDL_Event event;
	bool stay = true;
	while( SDL_PollEvent( &event ) ) {

		switch( event.type ) {
		case SDL_KEYDOWN:
			/* Handle key presses. */
			stay = handle_key_down(app, &event.key.keysym );
			break;
		case SDL_QUIT:
			stay = false;
			break;
		case SDL_VIDEORESIZE:
			app->screen_width = event.resize.w;
			app->screen_height = event.resize.h;
			if( SDL_SetVideoMode( app->screen_width, app->screen_height,
														app->screen_pixels, app->sdlflags ) == 0 ) {
				error("Video mode set failed: %s", SDL_GetError( ) );
			}
			setup_opengl(event.resize.w, event.resize.h);
			break;
		}
		if (!stay)
			return false;
	}
	return true;
}

void MapFindNodeType(Map *map, int type, int *position){
	int nodes_count = map->nodes_count;
	for(int i=0; i<nodes_count;i++){
		draw_circle(map->nodes[i]->x,map->nodes[i]->y, 0.02);
		if(map->nodes[i]->type == type){
			position[0] = map->nodes[i]->x;
			position[1] = map->nodes[i]->y;
		}
	}
}

bool positionCheck(Map* map, int x, int y, float coeff){	
	const PPMPixel* current;
	for(int i=x-x*coeff; i<x+x*coeff; i=i+x*coeff){
		for(int j=y-y*coeff; j<y+y*coeff; j=j+y*coeff){
			current = MapGetPixel(map, i, j);
			if ((current->red != map->construction_color.red) || (current->green != map->construction_color.green) || (current->blue != map->construction_color.blue)){
				printf("CANNOT\n");
				return false;
			}
		}
	}
	return true;	
}

bool colisionCheck(int x1, int y1, float coeff1, int x2, int y2, float coeff2){
	if ((((x2-x2*coeff2>=x1-x1*coeff1) && (x2-x2*coeff2<=x1+x1*coeff1)) || ((x2+x2*coeff2>=x1-x1*coeff1) && (x2+x2*coeff2<=x1+x1*coeff1))) && (((y2-y2*coeff2>=y1-y1*coeff1) && (y2-y2*coeff2<=y1+y1*coeff1)) || ((y2+y2*coeff2>=y1+y1*coeff1) && (y2+y2*coeff2<=y1+y1*coeff1)))){
			printf("CANNOT %f %f %f %f ON %f %f %f %f !!!\n", x1-x1*coeff1, x1+x1*coeff1, y1-y1*coeff1, y1+y1*coeff1,x2-x2*coeff2,x2+x2*coeff2,y2-y2*coeff2,y2+y2*coeff2);
			return false;
		}
		printf("CANNNN %f %f %f %f ON %f %f %f %f !!!\n", x1-x1*coeff1, x1+x1*coeff1, y1-y1*coeff1, y1+y1*coeff1,x2-x2*coeff2,x2+x2*coeff2,y2-y2*coeff2,y2+y2*coeff2);
	return true;	
}

int getDistance(Node* origin, Node* destination){
	int distance = sqrt(pow(destination->x - origin->x, 2) + pow(destination->y - origin->y, 2));
	return distance;
};

int getDistanceConnected(Node* current, int* distances_i){
	NodeList* sibling = current->connected;
	int sumDistances=1000;
	int newDistance;
	Node* lastSibling;
	if(current->type == 2){
		distances_i[current->id] = 0;
		return 0;
	}else{
		if(sibling->node->type == 2){
			distances_i[sibling->node->id] = getDistance(current, sibling->node);
			return getDistance(current, sibling->node);
		}else{
			while(sibling){
				newDistance = getDistanceConnected(sibling->node, distances_i);
				if(newDistance<sumDistances){
					sumDistances = newDistance;
					lastSibling=sibling->node;
				}
				distances_i[lastSibling->id]=getDistance(current, lastSibling)+sumDistances;
				sibling = sibling->next;
			}
			return distances_i[lastSibling->id];

		}
	}
}

void plusCourtChemin(Map *map, int** distances){
	int nodes_count = map->nodes_count;
	for(int i =0;i<nodes_count;i++){
		for(int j=0;j<nodes_count;j++){
			distances[i][j] = -1;
		}
	}
	for(int i=0; i<nodes_count; i++){
		Node *myNode = map->nodes[i];
		getDistanceConnected(myNode, distances[i]);
	}
	for(int i =0;i<nodes_count;i++){
		for(int j=0;j<nodes_count;j++){
			printf("i=%d j=%d distance=%d\n", i, j, distances[i][j]);
		}
	}
}

int findFaster(Node* node, int** distances, int randNum){
	Node* faster = node->connected->node;
	NodeList* sibling = node->connected;
	while(sibling){
		if(distances[node->id][faster->id] > distances[node->id][sibling->node->id] && distances[node->id][sibling->node->id]>0){
			if(randNum <= 50){
				faster = sibling->node;
			}
		}
		sibling = sibling->next;
	}
	return faster->id;
}


float returnX(float pixels){
	return (pixels*3.2/500)-1.6;
}

float returnY(float pixels){
	return (pixels*2.0/300)-1.0;
}

float returnX2(float pixels,App app){
	return (pixels*3.2/app.screen_width)-1.6;
}

float returnY2(float pixels,App app){
	return (pixels*2.0/app.screen_height)-1.0;
}

Monster* createWave(TypeMonster type, int idWave){
	Monster* wave = new Monster[10];
	for(int i=0;i<10;i++){
		wave[i].init(1,idWave,type);
		wave[i].setPosition(10+i,20+i);
	}
	return wave;
}

Tower *createTower(TypeTower type){
	Tower *tower = new Tower(1,10,50,500,5,30,type);
	return tower;
}

Bullet *createBullet(Tower *tower, Monster monster, App app){
	Bullet *bullet = new Bullet(returnX2(tower->getPositionX(), app), returnY2(tower->getPositionY(), app), returnX(monster.getPositionX()), returnY(monster.getPositionY()));
	return bullet;
}

Installation *createInstallation(TypeInstallation type){
	Installation *installation = new Installation(type);
	return installation;
}

int main(int argc, char** argv){

	const SDL_VideoInfo* info = NULL;
	App app;
	GLenum err;
	

	if (argc != 2)
	{
		error("Wrong argument count:\nUsage:\n%s <idt_path>", argv[0]);
	}
	app.map = read_idt(argv[1]);
	check_map(app.map);
	

	int **distances = new int *[app.map->nodes_count];
	for(int i=0;i<app.map->nodes_count;i++){
		distances[i]= new int[app.map->nodes_count];
	}
	plusCourtChemin(app.map, distances);
	Player myPlayer(100);
	
	int position[2];
	TypeMonster fast(1, 3, weak, 5);
	TypeMonster resistant(2, 1, strong, 2);
	TypeMonster powerful(3, 2, medium, 10);
	std:vector<Monster*> waves;
	Monster* wave = createWave(fast, 0);
	waves.push_back(wave);

	vector<TypeMonster> typeMonsters;
	typeMonsters.push_back(fast);
	typeMonsters.push_back(resistant);
	typeMonsters.push_back(powerful);

	Node* myNode = app.map->nodes[4];
	NodeList* sibling = myNode->connected;
	while(sibling){
		printf("---Connected with %d\n", sibling->node->id);
		sibling = sibling->next;
	}


	vector<Bullet*> bullets;


	 //(int id, float coeffpuissance, float coeffportee, float coeffcadence);
	TypeTower typeTowerR(1, 3.0,1.0,0.3, 10);
	TypeTower typeTowerV(2, 1.0,0.5,1.3, 20);
	TypeTower typeTowerJ(3, 0.2,0.8,3.3, 30);
	TypeTower typeTowerB(4, 0.8,2.0,2.3, 40);


	vector<Tower*> tours;
	
	TypeInstallation radar(1, 1.25, 75, 10);
	TypeInstallation armement(2, 1.25, 75, 10);
	TypeInstallation munitions(3, 1.25, 75, 10);

	vector<Installation*> installations;

	/* First, initialize SDL's video subsystem. */
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
		error("Video initialization failed: %s", SDL_GetError( ) );
	}

	SDL_WM_SetCaption("Totowerdefense", "totowerdefense");
	/* Let's get some video information. */
	info = SDL_GetVideoInfo( );

	if( !info ) {
		error("Video query failed: %s", SDL_GetError( ) );
	}

	app.scale = 1.f;
	app.screen_width = app.map->map_data->x;
	app.screen_height = app.map->map_data->y;
	
	//app.screen_width = info->current_w / 2;
	//app.screen_height = info->current_h / 2;
	app.screen_pixels = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	app.sdlflags = SDL_OPENGL | SDL_DOUBLEBUF | SDL_RESIZABLE;

	/*
		 * Set the video mode
		 */
	if( SDL_SetVideoMode( app.screen_width, app.screen_height,
												app.screen_pixels, app.sdlflags ) == 0 ) {
		error("Video mode set failed: %s", SDL_GetError( ) );
	}

	/*
		 * At this point, we should have a properly setup
		 * double-buffered window for use with OpenGL.
		 */
	setup_opengl( app.screen_width, app.screen_height);

	/*
		 * Now we want to begin our normal app process--
		 * an event loop with a lot of redrawing.
	*/

	init_draw(&app);

	/* Chargement de l'image */
    char image_path[] = "images/bg_map_1.jpg";
    SDL_Surface* image = IMG_Load(image_path);
    if(NULL == image) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", image_path);
        exit(EXIT_FAILURE);
    }

    /* Initialisation de la texture */
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLenum format;
    switch(image->format->BytesPerPixel) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            fprintf(stderr, "Format des pixels de l'image %s non supporte.\n", image_path);
            return EXIT_FAILURE;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);




	/* Chargement de monster */
    char enemy1_path[] = "images/enemy1.jpg";
    SDL_Surface* enemy1 = IMG_Load(enemy1_path);
    if(NULL == enemy1) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", enemy1_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id2;
    glGenTextures(1, &texture_id2);
    glBindTexture(GL_TEXTURE_2D, texture_id2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, enemy1->w, enemy1->h, 0, format, GL_UNSIGNED_BYTE, enemy1->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);


	/* Chargement de monster */
    char enemy2_path[] = "images/enemy2.jpg";
    SDL_Surface* enemy2 = IMG_Load(enemy2_path);
    if(NULL == enemy2) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", enemy2_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id10;
    glGenTextures(1, &texture_id10);
    glBindTexture(GL_TEXTURE_2D, texture_id10);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, enemy2->w, enemy2->h, 0, format, GL_UNSIGNED_BYTE, enemy2->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);


	/* Chargement de monster */
    char enemy3_path[] = "images/enemy3.jpg";
    SDL_Surface* enemy3 = IMG_Load(enemy3_path);
    if(NULL == enemy3) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", enemy3_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id11;
    glGenTextures(1, &texture_id11);
    glBindTexture(GL_TEXTURE_2D, texture_id11);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, enemy3->w, enemy3->h, 0, format, GL_UNSIGNED_BYTE, enemy3->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);



    /* Chargement de tourRouge */
    char tourRouge_path[] = "images/whale1.jpg";
    SDL_Surface* tourRouge = IMG_Load(tourRouge_path);
    if(NULL == tourRouge) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", tourRouge_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id3;
    glGenTextures(1, &texture_id3);
    glBindTexture(GL_TEXTURE_2D, texture_id3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tourRouge->w, tourRouge->h, 0, format, GL_UNSIGNED_BYTE, tourRouge->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);


    /* Chargement de tourVert */
    char tourVert_path[] = "images/whale3.jpg";
    SDL_Surface* tourVert = IMG_Load(tourVert_path);
    if(NULL == tourVert) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", tourVert_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id4;
    glGenTextures(1, &texture_id4);
    glBindTexture(GL_TEXTURE_2D, texture_id4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tourVert->w, tourVert->h, 0, format, GL_UNSIGNED_BYTE, tourVert->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    
    /* Chargement de tourJaune */
    char tourJaune_path[] = "images/whale2.jpg";
    SDL_Surface* tourJaune = IMG_Load(tourJaune_path);
    if(NULL == tourJaune) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", tourJaune_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id5;
    glGenTextures(1, &texture_id5);
    glBindTexture(GL_TEXTURE_2D, texture_id5);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tourJaune->w, tourJaune->h, 0, format, GL_UNSIGNED_BYTE, tourJaune->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);



    /* Chargement de tourBleue */
    char tourBleue_path[] = "images/whale4.jpg";
    SDL_Surface* tourBleue = IMG_Load(tourBleue_path);
    if(NULL == tourBleue) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", tourBleue_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id6;
    glGenTextures(1, &texture_id6);
    glBindTexture(GL_TEXTURE_2D, texture_id6);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tourBleue->w, tourBleue->h, 0, format, GL_UNSIGNED_BYTE, tourBleue->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);



    /* Chargement de installation1 (radar) */
    char installation1_path[] = "images/installation3.jpg";
    SDL_Surface* installation1 = IMG_Load(installation1_path);
    if(NULL == installation1) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", installation1_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id7;
    glGenTextures(1, &texture_id7);
    glBindTexture(GL_TEXTURE_2D, texture_id7);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, installation1->w, installation1->h, 0, format, GL_UNSIGNED_BYTE, installation1->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);



    /* Chargement de installation2 (armement) */
    char installation2_path[] = "images/installation2.jpg";
    SDL_Surface* installation2 = IMG_Load(installation2_path);
    if(NULL == installation2) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", installation2_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id8;
    glGenTextures(1, &texture_id8);
    glBindTexture(GL_TEXTURE_2D, texture_id8);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, installation2->w, installation2->h, 0, format, GL_UNSIGNED_BYTE, installation2->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);



    /* Chargement de installation3 (munitions) */
    char installation3_path[] = "images/installation1.jpg";
    SDL_Surface* installation3 = IMG_Load(installation3_path);
    if(NULL == installation3) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", installation3_path);
        exit(EXIT_FAILURE);
    }
    /* Initialisation de la texture */
    GLuint texture_id9;
    glGenTextures(1, &texture_id9);
    glBindTexture(GL_TEXTURE_2D, texture_id9);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, installation3->w, installation3->h, 0, format, GL_UNSIGNED_BYTE, installation3->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

	/* Chargement de l'image */
    /*char image_path[] = "images/bg_map_1.jpg";
    SDL_Surface* image = IMG_Load(image_path);
    if(NULL == image) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", image_path);
        exit(EXIT_FAILURE);
    }

    /* Initialisation de la texture */
    /*GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLenum format;
    switch(image->format->BytesPerPixel) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            fprintf(stderr, "Format des pixels de l'image %s non supporte.\n", image_path);
            return EXIT_FAILURE;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);*/



    

	int newX;
	float y;
	Node* currentNode = app.map->nodes[0];
	Node* firstNode = app.map->nodes[0];
	Node* tmpNextNode;
	Node* nextNode;
	int randNum = rand()%(101);
	for(int i=0;i<waves.size();i++){
		for(int j=0;j<10;j++){
			waves[i][j].setCurrentNode(currentNode->id,currentNode->type,currentNode->x, currentNode->y);
			randNum = rand()*(101);
			nextNode = app.map->nodes[findFaster(currentNode, distances, randNum)];
			waves[i][j].setNextNode(nextNode->id, nextNode->x, nextNode->y);
		}
	}
	int time = 0;

	int loop = 2;
	int touche = 0;
	float xTour = 0;
	float yTour = 0;
	float xInstallation = 0;
	float yInstallation = 0;
	bool dessinTour;
	bool dessinInstallation;
	bool check = false;
	bool check2 = true;
	while( loop != 0 ) {

		
		dessinTour = false;
		dessinInstallation = false;
		/* Process incoming events. */
		if(!process_events(&app)) break;
		/* Draw the screen. */
		draw_screen(&app);

		if(loop==1){
			draw_background(texture_id);
			/*SDL_Event e;
			while (SDL_PollEvent(&e)){
				if (e.type == SDL_QUIT) {
					//SDL_Log("Program quit after %i ticks", e.quit.timestamp);
					break;
				}
				switch (e.type){
					case SDL_MOUSEBUTTONUP:
						if ((touche == SDLK_r)||(touche == SDLK_v)||(touche == SDLK_j)||(touche == SDLK_b)){
							dessinTour = true;
							dessinInstallation = false;
						}
						if ((touche == SDLK_p)||(touche == SDLK_a)||(touche == SDLK_c)){
							dessinInstallation = true;
							dessinTour = false;
						}
						//if ((e.button.x<app.map->map_data->x-50) && (e.button.x>50) && (e.button.y<app.map->map_data->y-50) && (e.button.y>50)){
							
							xTour = e.button.x;
							yTour = e.button.y;
							xInstallation = e.button.x;
							yInstallation = e.button.y;
						//}
						printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
						break;
					case SDL_KEYDOWN:
						touche = e.key.keysym.sym;
						printf("Touche %d\n", touche);
						printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						touche = e.key.keysym.sym;
						printf("Touche %d\n", touche);
						printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
						break;
					default : break;
				}
			}
			// Fermer le jeux
			if (touche == SDLK_q){
				loop = 0;
			}*/
			/*if ((xTour<app.map->map_data->x-10) && (xTour>10) && (yTour<app.map->map_data->y-10) && (yTour>10)){
				dessinTour = true;
			}else{
				dessinTour = false;
				//printf("fauxxxx Xtour=%f screen=%d\n", xTour, app.map->map_data->x);
			}*/

			// end the screen
			SDL_GL_SwapBuffers( );

			/*time += 10;
			if(time>=2000){
				time=0;
				if(waves.size()>=10){
					printf("You win !\n");
					loop = 0;
				}else{
					int randTypeMonster = rand()%(3);
					printf("----RAND TYPE MONSTER : %d\n", randTypeMonster);
					if(randTypeMonster==0){
						waves.push_back(createWave(fast, waves.size()));
					}else if(randTypeMonster==1){
						waves.push_back(createWave(resistant, waves.size()));
					}else if(randTypeMonster==2){
						waves.push_back(createWave(powerful, waves.size()));
					}
					randNum = rand()%(101);
					for(int j=0;j<10;j++){
						waves[waves.size()-1][j].setCurrentNode(firstNode->id,firstNode->type,firstNode->x,firstNode->y);
						randNum = rand()*(101);
						tmpNextNode = app.map->nodes[findFaster(firstNode, distances, randNum)];
						waves[waves.size()-1][j].setNextNode(tmpNextNode->id, tmpNextNode->x, tmpNextNode->y);
					}
				}
				
			}

			for(int i=0;i<tours.size();i++){
				tours[i]->setCounter(tours[i]->getCounter()+10);
			}*/

			SDL_Delay(10);

		}else{
		draw_background(texture_id);


		/* Draw the monster */
		//glColor3d(255,0,0);

	
		//SDL_GL_SwapBuffers();
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
		        //SDL_Log("Program quit after %i ticks", e.quit.timestamp);
		        break;
		    }
			switch (e.type){
				case SDL_MOUSEBUTTONUP:
					if ((touche == SDLK_r)||(touche == SDLK_v)||(touche == SDLK_j)||(touche == SDLK_b)){
						dessinTour = true;
						dessinInstallation = false;
					}
					if ((touche == SDLK_p)||(touche == SDLK_a)||(touche == SDLK_c)){
						dessinInstallation = true;
						dessinTour = false;
					}
					//if ((e.button.x<app.map->map_data->x-50) && (e.button.x>50) && (e.button.y<app.map->map_data->y-50) && (e.button.y>50)){
						
						xTour = e.button.x;
						yTour = e.button.y;
						xInstallation = e.button.x;
						yInstallation = e.button.y;
					//}
					printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
					break;
				case SDL_KEYDOWN:
					touche = e.key.keysym.sym;
					printf("Touche %d\n", touche);
					printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
					break;
				case SDL_KEYUP:
					touche = e.key.keysym.sym;
					printf("Touche %d\n", touche);
					printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
					break;
				default : break;
			}
		}
		// Fermer le jeux
		if (touche == SDLK_q){
			loop = 0;
		}
		/*if ((xTour<app.map->map_data->x-10) && (xTour>10) && (yTour<app.map->map_data->y-10) && (yTour>10)){
		 	dessinTour = true;
		}else{
			dessinTour = false;
		 	//printf("fauxxxx Xtour=%f screen=%d\n", xTour, app.map->map_data->x);
		}*/
		if(myPlayer.getMoney()<=0){
			dessinTour = false;
			dessinInstallation = false;
		}
		// Placer une tour
		if (dessinTour == true){
			check = positionCheck(app.map, xTour, yTour,0.12);
			
			for (int i=0; i<tours.size();i++){
				check2 = colisionCheck((int)xTour,(int)yTour,0.12,(int)tours[i]->getPositionX(),(int)tours[i]->getPositionY(), 0.12);
				if (check2 == false){
					break;
				}
			}	
			//check2 = colisionCheck(myTower.getPositionX(),myTower.getPositionY(), 0.05,myTower2.getPositionX(),myTower2.getPositionY(), 0.05);	
			if ((check == true)&&(check2 == true)){

				if (touche == SDLK_r){	
					tours.push_back(createTower(typeTowerR));
					myPlayer.setMoney(myPlayer.getMoney()-typeTowerR.getPrice());
				}
				if (touche == SDLK_v){		
					tours.push_back(createTower(typeTowerV));
					myPlayer.setMoney(myPlayer.getMoney()-typeTowerV.getPrice());
				}
				if (touche == SDLK_j){	
					tours.push_back(createTower(typeTowerJ));
					myPlayer.setMoney(myPlayer.getMoney()-typeTowerJ.getPrice());
				}
				if (touche == SDLK_b){
					tours.push_back(createTower(typeTowerB));
					myPlayer.setMoney(myPlayer.getMoney()-typeTowerB.getPrice());
				}
				//tours.push_back(createTower(typeTowerR));
				tours[tours.size()-1]->setPosition(xTour,yTour);

			}
		}	

		// Placer une installation
		/*if (dessinInstallation == true){
			check = positionCheck(app.map, xInstallation, yInstallation,0.07);
			
			for (int i=0; i<tours.size();i++){
				check2 = colisionCheck((int)xInstallation,(int)yInstallation,0.07,(int)tours[i]->getPositionX(),(int)tours[i]->getPositionY(), 0.1);
				if (check2 == false){
					break;
				}
			}
			for (int i=0; i<installations.size();i++){
				check2 = colisionCheck((int)xInstallation,(int)yInstallation,0.07,(int)installations[i]->getPositionX(),(int)installations[i]->getPositionY(), 0.1);
				if (check2 == false){
					break;
				}
			}	
			//check2 = colisionCheck(myTower.getPositionX(),myTower.getPositionY(), 0.05,myTower2.getPositionX(),myTower2.getPositionY(), 0.05);	
			if (check && check2){
				if (touche == SDLK_p){	
					installations.push_back(createInstallation(radar));
					myPlayer.setMoney(myPlayer.getMoney()-radar.getPrice());
					for(int i=0;i<tours.size();i++){
						if(installations[installations.size()-1]->isInShootingRange(tours[i]->getPositionX(), tours[i]->getPositionY())){
							tours[i]->setPortee(tours[i]->getPortee()*radar.getCoeff());
						}
						
					}
				}
				if (touche == SDLK_a){		
					installations.push_back(createInstallation(armement));
					myPlayer.setMoney(myPlayer.getMoney()-armement.getPrice());
					for(int i=0;i<tours.size();i++){
						if(installations[installations.size()-1]->isInShootingRange(tours[i]->getPositionX(), tours[i]->getPositionY())){
							tours[i]->setPuissance(tours[i]->getPuissance()*armement.getCoeff());
						}
					}
				}
				if (touche == SDLK_c){	
					installations.push_back(createInstallation(munitions));
					myPlayer.setMoney(myPlayer.getMoney()-munitions.getPrice());
					for(int i=0;i<tours.size();i++){

						if(installations[installations.size()-1]->isInShootingRange(tours[i]->getPositionX(), tours[i]->getPositionY())){
							tours[i]->setCadence(tours[i]->getCadence()*munitions.getCoeff());
						}
						
					}
				}
				//tours.push_back(createTower(typeTowerR));
				installations[installations.size()-1]->setPosition(xInstallation,yInstallation);

			}
		}*/


		for (int i=0; i<tours.size();i++){
			if (tours[i]->getType() == typeTowerR.getId()){
				//glColor3d(255,0,0);	
				draw_elem(texture_id3,returnX2(tours[i]->getPositionX(),app),returnY2(tours[i]->getPositionY(),app),0.1f);

			}
			if (tours[i]->getType() == typeTowerV.getId()){
				//glColor3d(0,255,0);	
				draw_elem(texture_id4,returnX2(tours[i]->getPositionX(),app),returnY2(tours[i]->getPositionY(),app),0.1f);
			}
			if (tours[i]->getType() == typeTowerJ.getId()){
				//glColor3d(240,195,0);	
				draw_elem(texture_id5,returnX2(tours[i]->getPositionX(),app),returnY2(tours[i]->getPositionY(),app),0.1f);
			}
			if (tours[i]->getType() == typeTowerB.getId()){
				//glColor3d(0,0,255);	
				draw_elem(texture_id6,returnX2(tours[i]->getPositionX(),app),returnY2(tours[i]->getPositionY(),app),0.1f);
			}
			draw_circle(returnX2(tours[i]->getPositionX(),app),returnY2(tours[i]->getPositionY(),app), 0.12);	
		}

		for (int i=0; i<installations.size();i++){
			//printf("after2\n");
			if (installations[i]->getType() == radar.getId()){
				//glColor3d(255,255,0);	
				draw_elem(texture_id7,returnX2(installations[i]->getPositionX(),app),returnY2(installations[i]->getPositionY(),app),0.07f);

			}
			if (installations[i]->getType() == armement.getId()){
				//glColor3d(0,255,255);	
				draw_elem(texture_id8,returnX2(installations[i]->getPositionX(),app),returnY2(installations[i]->getPositionY(),app),0.07f);
			}
			if (installations[i]->getType() == munitions.getId()){
				//glColor3d(255,0,255);	
				draw_elem(texture_id9,returnX2(installations[i]->getPositionX(),app),returnY2(installations[i]->getPositionY(),app),0.07f);	
			}
			//draw_circle(returnX2(installations[i]->getPositionX(),app),returnY2(installations[i]->getPositionY(),app), 0.05);	
		}
		
		for (int t=0; t<tours.size();t++){
			for(int i=0;i<waves.size();i++){
				for(int j=0;j<10;j++){
					if(waves[i][j].isAlive()){
						if(tours[t]->isInShootingRange(waves[i][j].getPositionX(), waves[i][j].getPositionY()) && (tours[t]->getCounter()>=(tours[t]->getCadence()*tours[t]->getCoeffCadence()) ) ){ //et compteur ok
							//dessiner une ligne
							/*glLineWidth(0.5);
							glColor3f(1.0, 0.0, 0.0);
							glBegin(GL_LINES);
							glVertex2f(returnX(waves[i][j].getPositionX()), returnY(waves[i][j].getPositionY()));
							glVertex2f(returnX2(tours[t]->getPositionX()),returnY2(tours[t]->getPositionY()));
							glEnd();*/
							bullets.push_back(createBullet(tours[t], waves[i][j], app));
							tours[t]->setCounter(0);
							//enlever les points
							waves[i][j].setPV(waves[i][j].getPV()-(tours[t]->getPuissance()*tours[t]->getCoeffPuissance()));
							i=10;
							j=10;
						}
					}
					
				}
			}
		}

		for(int i=0;i<bullets.size();i++){
			if(bullets[i]->moveToTarget()){
				draw_circle(bullets[i]->getPositionX(), bullets[i]->getPositionY(), 0.01);
			}else{
				bullets.erase(bullets.begin()+i);
			}
		}	

		// Move the monster
		for(int i=0;i<waves.size();i++){
			for(int j=0;j<10;j++){
				if(waves[i][j].isAlive()){
					if(waves[i][j].getIdType() == 1){
						draw_enemy(texture_id2,returnX(waves[i][j].getPositionX()),returnY(waves[i][j].getPositionY()),0.06f);
					}else if(waves[i][j].getIdType() == 2){
						draw_enemy(texture_id10,returnX(waves[i][j].getPositionX()),returnY(waves[i][j].getPositionY()),0.06f);
					}else{
						draw_enemy(texture_id11,returnX(waves[i][j].getPositionX()),returnY(waves[i][j].getPositionY()),0.06f);
					}
					//draw_circle(returnX(waves[i][j].getPositionX()),returnY(waves[i][j].getPositionY()), 0.02);
					if(waves[i][j].moveToNode(waves[i][j].getNextNodeX(), waves[i][j].getNextNodeY())){
						
						currentNode = app.map->nodes[waves[i][j].getIdNextNode()];
						waves[i][j].setCurrentNode(currentNode->id, currentNode->type, currentNode->x, currentNode->y);
						
						if(waves[i][j].getTypeCurrentNode() == 2){
							//waves[i][j].setPV(0);
							myPlayer.setPV(myPlayer.getPV()-waves[i][j].getDegats());
							if(!myPlayer.isAlive()){
								printf("You lose !\n");
								loop=0;
							}
						}else{
							randNum = rand()%(101);
							nextNode = app.map->nodes[findFaster(currentNode, distances, randNum)];
							waves[i][j].setNextNode(nextNode->id, nextNode->x, nextNode->y);

						}
					}

				}
				
			}
		}

		// end the screen
		SDL_GL_SwapBuffers( );

		time += 10;
		if(time>=2000){
			time=0;
			if(waves.size()>=10){
				printf("You win !\n");
				loop = 0;
			}else{
				int randTypeMonster = rand()%(3);
				printf("----RAND TYPE MONSTER : %d\n", randTypeMonster);
				if(randTypeMonster==0){
					waves.push_back(createWave(fast, waves.size()));
				}else if(randTypeMonster==1){
					waves.push_back(createWave(resistant, waves.size()));
				}else if(randTypeMonster==2){
					waves.push_back(createWave(powerful, waves.size()));
				}
				randNum = rand()%(101);
				for(int j=0;j<10;j++){
					waves[waves.size()-1][j].setCurrentNode(firstNode->id,firstNode->type,firstNode->x,firstNode->y);
					randNum = rand()*(101);
					tmpNextNode = app.map->nodes[findFaster(firstNode, distances, randNum)];
					waves[waves.size()-1][j].setNextNode(tmpNextNode->id, tmpNextNode->x, tmpNextNode->y);
				}
			}
			
		}

		for(int i=0;i<tours.size();i++){
			tours[i]->setCounter(tours[i]->getCounter()+10);
		}

		SDL_Delay(10);
	}
	}
	/* Liberation de la memoire allouee sur le GPU pour la texture */
    glDeleteTextures(1, &texture_id);

    /* Liberation de la mémoire occupee par img */ 
    SDL_FreeSurface(image);

	
	SDL_Quit();
	freeAllAllocations();
	return 0;
	
}
