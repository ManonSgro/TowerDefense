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
#include "button.h"

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

// App struct
typedef struct {
	int screen_width, screen_height;
	int sdlflags;
	int screen_pixels;
	float scale;
	Map* map;
	GLuint textures[MAX_TEXTURES];
} App;

// Setup 2d space
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

// Zoom
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

// Setup drawing textures
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

// Draw textured screen
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

// Draw screen (ppm texture)
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
}

// Events for keydown, quit and resize
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

// Find node according to type
void MapFindNodeType(Map *map, int type, int *position){
	int nodes_count = map->nodes_count;
	for(int i=0; i<nodes_count;i++){
		if(map->nodes[i]->type == type){
			position[0] = map->nodes[i]->x;
			position[1] = map->nodes[i]->y;
		}
	}
}

// Check building zone
bool positionCheck(Map* map, int x, int y, float coeff){	
	const PPMPixel* current;
	for(int i=x-x*coeff; i<x+x*coeff; i=i+x*coeff){
		for(int j=y-y*coeff; j<y+y*coeff; j=j+y*coeff){
			current = MapGetPixel(map, i, j);
			if ((current->red != map->construction_color.red) || (current->green != map->construction_color.green) || (current->blue != map->construction_color.blue)){
				return false;
			}
		}
	}
	return true;	
}

// Check collision between two elements
bool colisionCheck(float x1, float y1, float coeff1, float x2, float y2, float coeff2){
	if ((((x2-coeff2>=x1-coeff1) && (x2-coeff2<=x1+coeff1)) || ((x2+coeff2>=x1-coeff1) && (x2+coeff2<=x1+coeff1))) && (((y2-coeff2>=y1-coeff1) && (y2-coeff2<=y1+coeff1)) || ((y2+coeff2>=y1-coeff1) && (y2+coeff2<=y1+coeff1)))){
			return false;
		}
	return true;	
}


// Draw textured square
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
    
        glTexCoord2f(1, 1);
        glVertex2f(coeff, coeff);
    
        glTexCoord2f(1, 0);
        glVertex2f(coeff, -coeff);
    
        glTexCoord2f(0, 0);
        glVertex2f(-coeff, -coeff);

	    glEnd();
    glPopMatrix();


    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
} 

// Draw textured square (rotate)
 void draw_enemy(GLuint texture_id, float x, float y, float coeff) { 
    glColor3d(255,255,255);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    glPushMatrix();
        glBegin(GL_QUADS);
    
	    glTexCoord2f(0, 1);
        glVertex2f(-coeff, coeff);
    
        glTexCoord2f(1, 1);
        glVertex2f(coeff, coeff);
    
        glTexCoord2f(1, 0);
        glVertex2f(coeff, -coeff);
    
        glTexCoord2f(0, 0);
        glVertex2f(-coeff, -coeff);

	    glEnd();
    glPopMatrix();


    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
} 

// Draw a circle around x,y
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

// Get distance bextween two points
int getDistance(Node* origin, Node* destination){
	int distance = sqrt(pow(destination->x - origin->x, 2) + pow(destination->y - origin->y, 2));
	return distance;
};

// Get sum of distances until the exit
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

// Create matrix with distances
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
			//printf("i=%d j=%d distance=%d\n", i, j, distances[i][j]);
		}
	}
}

// Find faster way according to matrix
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

// Return vertex x and y positions
float returnX(float pixels, App app){
	return (pixels*3.2/app.screen_width)-1.6;
}
float returnY(float pixels, App app){
	return (pixels*2.0/app.screen_height)-1.0;
}

// Create wave of monsters
Monster* createWave(TypeMonster type, int idWave){
	Monster* wave = new Monster[10];
	for(int i=0;i<10;i++){
		wave[i].init(1,idWave,type);
		wave[i].setPosition(10+i,20+i);
	}
	return wave;
}

// Create new tower
Tower *createTower(TypeTower type){
	Tower *tower = new Tower(1,10,50,500,5,30,type);
	return tower;
}

// create new bullet
Bullet *createBullet(Tower *tower, Monster monster, App app){
	Bullet *bullet = new Bullet(returnX(tower->getPositionX(), app), returnY(tower->getPositionY(), app), returnX(monster.getPositionX(), app), returnY(monster.getPositionY(), app));
	return bullet;
}

// Create new installation
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
		error("Wrong argument count:\nUsage:\n%s <itd_path>", argv[0]);
	}
	app.map = read_itd(argv[1]);
	check_map(app.map);
	

	int **distances = new int *[app.map->nodes_count];
	for(int i=0;i<app.map->nodes_count;i++){
		distances[i]= new int[app.map->nodes_count];
	}
	plusCourtChemin(app.map, distances);
	Player myPlayer(200);
	
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
		sibling = sibling->next;
	}


	vector<Bullet*> bullets;


	// Set types of towers
	TypeTower typeTowerR(1, 3.0,1.0,0.3, 10);
	TypeTower typeTowerV(2, 1.0,0.5,1.3, 20);
	TypeTower typeTowerJ(3, 0.2,0.8,3.3, 30);
	TypeTower typeTowerB(4, 0.8,2.0,2.3, 40);

	// Set towers
	vector<Tower*> tours;
	
	// Set types of installations
	TypeInstallation radar(1, 1.25, 75, 10);
	TypeInstallation armement(2, 1.25, 75, 10);
	TypeInstallation munitions(3, 1.25, 75, 10);

	// Set installations
	vector<Installation*> installations;

	// Set buttons
	Button start(200,160,95,30);
	Button help(200,196,95,30);
	Button quit(200,232,95,30);

	/* Initialize SDL's video subsystem. */
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
		error("Video initialization failed: %s", SDL_GetError( ) );
	}

	// Set caption
	SDL_WM_SetCaption("Willy", "Willy");

	/* Get some video information. */
	info = SDL_GetVideoInfo( );

	if( !info ) {
		error("Video query failed: %s", SDL_GetError( ) );
	}

	// Set app infos
	app.scale = 1.f;
	app.screen_width = app.map->map_data->x;
	app.screen_height = app.map->map_data->y;
	app.screen_pixels = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	app.sdlflags = SDL_OPENGL | SDL_DOUBLEBUF | SDL_RESIZABLE;

	// Set the Video mode
	if( SDL_SetVideoMode( app.screen_width, app.screen_height,
												app.screen_pixels, app.sdlflags ) == 0 ) {
		error("Video mode set failed: %s", SDL_GetError( ) );
	}

	// Setup double-buffered window to use with OpenGL
	setup_opengl( app.screen_width, app.screen_height);

	init_draw(&app);

	/*** Load textures ***/
	vector<char*> textures_path;
    textures_path.push_back("images/bg_map_1.jpg");
    textures_path.push_back("images/bg_map_2.jpg");
    textures_path.push_back("images/bg_map_3.jpg");
    textures_path.push_back("images/bg_map_4.jpg");
    textures_path.push_back("images/enemy1.jpg");
    textures_path.push_back("images/enemy2.jpg");
    textures_path.push_back("images/enemy3.jpg");
    textures_path.push_back("images/whale1.jpg");
    textures_path.push_back("images/whale2.jpg");
    textures_path.push_back("images/whale3.jpg");
    textures_path.push_back("images/whale4.jpg");
    textures_path.push_back("images/installation1.jpg");
    textures_path.push_back("images/installation2.jpg");
    textures_path.push_back("images/installation3.jpg");
    textures_path.push_back("images/bg_menu_1.jpg");
    textures_path.push_back("images/bg_menu_2.jpg");
    textures_path.push_back("images/bg_menu_3.jpg");
    textures_path.push_back("images/bg_menu_4.jpg");
    textures_path.push_back("images/bg_help.jpg");
    textures_path.push_back("images/win.jpg");
    textures_path.push_back("images/bg_gameOver.jpg");
    textures_path.push_back("images/0.jpg");
    textures_path.push_back("images/1.jpg");
    textures_path.push_back("images/2.jpg");
    textures_path.push_back("images/3.jpg");
    textures_path.push_back("images/4.jpg");
    textures_path.push_back("images/5.jpg");
    textures_path.push_back("images/6.jpg");
    textures_path.push_back("images/7.jpg");
    textures_path.push_back("images/8.jpg");
    textures_path.push_back("images/9.jpg");
    textures_path.push_back("images/dollar.jpg");

	SDL_Surface* textures_surfaces[33];

	GLuint texture_id;
	GLuint texture_id30;
	GLuint texture_id31;
	GLuint texture_id32;
	GLuint texture_id2;
	GLuint texture_id10;
	GLuint texture_id11;
	GLuint texture_id3;
	GLuint texture_id4;
	GLuint texture_id5;
	GLuint texture_id6;
	GLuint texture_id7;
	GLuint texture_id8;
	GLuint texture_id9;
	GLuint texture_id12;
	GLuint texture_id13;
	GLuint texture_id14;
	GLuint texture_id15;
	GLuint texture_id16;
	GLuint texture_id17;
	GLuint texture_id18;
	GLuint texture_id19;
	GLuint texture_id20;
	GLuint texture_id21;
	GLuint texture_id22;
	GLuint texture_id23;
	GLuint texture_id24;
	GLuint texture_id25;
	GLuint texture_id26;
	GLuint texture_id27;
	GLuint texture_id28;
	GLuint texture_id29;
	GLuint textures_id[] = {texture_id, texture_id30, texture_id31, texture_id32, texture_id2, texture_id10, texture_id11, texture_id3,
	texture_id4, texture_id5, texture_id6, texture_id7, texture_id8, texture_id9, texture_id12, texture_id13, texture_id14, texture_id15, texture_id16,
	 texture_id17, texture_id18, texture_id19, texture_id20, texture_id21, texture_id22, texture_id23, texture_id24, texture_id25, texture_id26,
	 texture_id27, texture_id28, texture_id29};

	 /* Test image */
    char image_path[] = "images/bg_map_1.jpg";
    SDL_Surface* image = IMG_Load(image_path);
    if(NULL == image) {
        fprintf(stderr, "Echec du chargement de l'image %s\n", image_path);
        exit(EXIT_FAILURE);
    }
    GLuint texture_id_img;
    glGenTextures(1, &texture_id_img);
    glBindTexture(GL_TEXTURE_2D, texture_id_img);
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


	 for(int i=0;i<textures_path.size();i++){
		 if(i>4 || (i>=14 && i<=20)){
			/* Chargement de l'image */
			textures_surfaces[i] = IMG_Load(textures_path[i]);
			if(NULL == textures_surfaces[i]) {
				fprintf(stderr, "Echec du chargement de l'image %s\n", textures_path[i]);
				exit(EXIT_FAILURE);
			}

			/* Initialisation de la texture */
			glGenTextures(1, &textures_id[i]);
			glBindTexture(GL_TEXTURE_2D, textures_id[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			switch(textures_surfaces[i]->format->BytesPerPixel) {
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
					fprintf(stderr, "Format des pixels de l'image %s non supporte.\n", textures_path[i]);
					return EXIT_FAILURE;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textures_surfaces[i]->w, textures_surfaces[i]->h, 0, format, GL_UNSIGNED_BYTE, textures_surfaces[i]->pixels);
			glBindTexture(GL_TEXTURE_2D, 0);
		}else{

			/* Chargement */
			textures_surfaces[i] = IMG_Load(textures_path[i]);
			if(NULL == textures_surfaces[i]) {
				fprintf(stderr, "Echec du chargement de l'image %s\n", textures_path[i]);
				exit(EXIT_FAILURE);
			}
			/* Initialisation de la texture */
			glGenTextures(1, &textures_id[i]);
			glBindTexture(GL_TEXTURE_2D, textures_id[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textures_surfaces[i]->w, textures_surfaces[i]->h, 0, format, GL_UNSIGNED_BYTE, textures_surfaces[i]->pixels);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

	}
	
	/*** End load textures ***/

	
	/*** Init some variables ***/

	// Enemies
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

	// Time
	int time = 0;

	// Loop
	int loop = 1;

	// Drawing
	int touche = 0;
	float xTour = 0;
	float yTour = 0;
	float xInstallation = 0;
	float yInstallation = 0;
	int xMouse = 0;
	int yMouse = 0;
	bool dessinTour;
	bool dessinInstallation;
	bool check = false;
	bool check2 = true;

	/*** Start print the game ***/

	while( loop != 0 ) {
		// Process incoming events.
		if(!process_events(&app)) break;

		// Draw the screen.
		draw_screen(&app);

		/*** Menu loop ***/
		if(loop==1){
			// RESET ALL
			touche = 0;
			bool check = false;
			bool check2 = true;
			for(int i=0;i<waves.size();i++){
				waves.erase(waves.begin()+i);
			}
			for(int i=0;i<tours.size();i++){
				tours.erase(tours.begin()+i);
			}
			for(int i=0;i<bullets.size();i++){
				bullets.erase(bullets.begin()+i);
			}
			for(int i=0;i<installations.size();i++){
				installations.erase(installations.begin()+i);
			}
			myPlayer.setPV(100);
			myPlayer.setMoney(100);


			// Draw bg
			draw_background(textures_id[14]);
			if(start.isHovered(xMouse,yMouse)){
				draw_background(textures_id[15]);
			}
			if(help.isHovered(xMouse,yMouse)){
				draw_background(textures_id[16]);
			}
			if(quit.isHovered(xMouse,yMouse)){
				draw_background(textures_id[17]);
			}

			// Waiting for events
			SDL_Event e;
			while (SDL_PollEvent(&e)){
				if (e.type == SDL_QUIT) {
					break;
				}
				switch (e.type){
					case SDL_MOUSEBUTTONUP:	
						xMouse = e.button.x;
						yMouse = e.button.y;
						printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
						if(quit.isHovered(xMouse,yMouse)){
							loop =0;
						}
						if(start.isHovered(xMouse,yMouse)){
							loop =2;
						}
						if(help.isHovered(xMouse,yMouse)){
							loop =3;
						}
						break;
					case SDL_MOUSEMOTION:
						xMouse = e.button.x;
						yMouse = e.button.y;
						break;
					case SDL_KEYDOWN:
						touche = e.key.keysym.sym;
						printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						touche = e.key.keysym.sym;
						printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
						break;
					default : break;
				}
			}

			// Close the game
			if (touche == SDLK_q){
				loop = 0;
			}

			// End the screen
			SDL_GL_SwapBuffers( );

			SDL_Delay(10);

		}
		/*** Game loop ***/
		else if (loop == 2){

			//Draw map
			if(myPlayer.getPV()>75){
				draw_background(textures_id[0]);
			}else if(myPlayer.getPV()>50){
				draw_background(textures_id[1]);
			}else if(myPlayer.getPV()>25){
				draw_background(textures_id[2]);
			}else{
				draw_background(textures_id[3]);
			}

		// Init drawing
		dessinTour = false;
		dessinInstallation = false;

		// Waiting for events
		//SDL_GL_SwapBuffers();
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
		        break;
		    }
			switch (e.type){
				case SDL_MOUSEBUTTONUP:
					if ((e.button.x<app.map->map_data->x-40) && (e.button.x>40) && (e.button.y<app.map->map_data->y-40) && (e.button.y>40)){ //Cannot draw tower outside the screen
						
					if ((touche == SDLK_r)||(touche == SDLK_v)||(touche == SDLK_j)||(touche == SDLK_b)){
						dessinTour = true;
						dessinInstallation = false;
					}
					if ((touche == SDLK_p)||(touche == SDLK_a)||(touche == SDLK_c)){
						dessinInstallation = true;
						dessinTour = false;
					}
						xTour = e.button.x;
						yTour = e.button.y;
						xInstallation = e.button.x;
						yInstallation = e.button.y;
					}
					printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
					break;
				case SDL_KEYDOWN:
					if(e.key.keysym.sym != SDLK_h){
						touche = e.key.keysym.sym;
					}
					printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
					break;
				case SDL_KEYUP:
					touche = e.key.keysym.sym;
					printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
					break;
				default : break;
			}
		}
		// Close the game
		if (touche == SDLK_q){
			loop = 0;
		}
		// Go to menu
		if (touche == SDLK_m){
			loop = 1;
		}
		// Go to help screen
		if (touche == SDLK_h){
			loop = 3;
		}

		// If player have no money, he can't draw elements anymore
		if(myPlayer.getMoney()<=0){
			dessinTour = false;
			dessinInstallation = false;
		}

		// Place a tower
		if (dessinTour == true){
			// Check position & collision
			check = positionCheck(app.map, xTour, yTour,0.1);
			int testCheck2=0;
			for (int i=0; i<tours.size();i++){
				check2 = colisionCheck(returnX(xTour, app),returnY(yTour, app),0.1,returnX(tours[i]->getPositionX(), app),returnY(tours[i]->getPositionY(), app), 0.1);
				if (check2 == false){
					testCheck2++;
				}
			}	
			if(testCheck2>0){
				check2 = false;
			}
			for (int i=0; i<installations.size();i++){
				check2 = colisionCheck(returnX(xTour, app),returnY(yTour, app),0.1,returnX(installations[i]->getPositionX(), app),returnY(installations[i]->getPositionY(), app), 0.07);
				if (check2 == false){
					break;
				}
			}	

			// Push new towers
			if (check && check2){

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
				tours[tours.size()-1]->setPosition(xTour,yTour);

			}
		}	

		// Place an installation
		if (dessinInstallation == true){

			//Check position && collision
			check = positionCheck(app.map, xInstallation, yInstallation,0.07);		
			for (int i=0; i<tours.size();i++){
				check2 = colisionCheck(returnX(xInstallation, app),returnY(yInstallation, app),0.07,returnX(tours[i]->getPositionX(), app),returnY(tours[i]->getPositionY(), app), 0.1);
				if (check2 == false){
					break;
				}
			}
			for (int i=0; i<installations.size();i++){
				check2 = colisionCheck(returnX(xInstallation, app),returnY(yInstallation, app),0.07,returnX(installations[i]->getPositionX(), app),returnY(installations[i]->getPositionY(), app), 0.07);
				if (check2 == false){
					break;
				}
			}	

			// Draw installations and affect towers	
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
				installations[installations.size()-1]->setPosition(xInstallation,yInstallation);

			}
		}

		// Draw towers
		for (int i=0; i<tours.size();i++){
			if (tours[i]->getType() == typeTowerR.getId()){
				draw_elem(textures_id[7],returnX(tours[i]->getPositionX(),app),returnY(tours[i]->getPositionY(),app),0.1f);

			}
			if (tours[i]->getType() == typeTowerV.getId()){
				draw_elem(textures_id[9],returnX(tours[i]->getPositionX(),app),returnY(tours[i]->getPositionY(),app),0.1f);
			}
			if (tours[i]->getType() == typeTowerJ.getId()){
				draw_elem(textures_id[8],returnX(tours[i]->getPositionX(),app),returnY(tours[i]->getPositionY(),app),0.1f);
			}
			if (tours[i]->getType() == typeTowerB.getId()){
				draw_elem(textures_id[10],returnX(tours[i]->getPositionX(),app),returnY(tours[i]->getPositionY(),app),0.1f);
			}
		}

		// Draw installations
		for (int i=0; i<installations.size();i++){
			if (installations[i]->getType() == radar.getId()){
				draw_elem(textures_id[13],returnX(installations[i]->getPositionX(),app),returnY(installations[i]->getPositionY(),app),0.07f);

			}else if (installations[i]->getType() == armement.getId()){
				draw_elem(textures_id[12],returnX(installations[i]->getPositionX(),app),returnY(installations[i]->getPositionY(),app),0.07f);
			}else if (installations[i]->getType() == munitions.getId()){
				draw_elem(textures_id[11],returnX(installations[i]->getPositionX(),app),returnY(installations[i]->getPositionY(),app),0.07f);
			}
		}
		
		// Create bullets if tower can
		for (int t=0; t<tours.size();t++){
			for(int i=0;i<waves.size();i++){
				for(int j=0;j<10;j++){
					if(waves[i][j].isAlive()){
						if(tours[t]->isInShootingRange(waves[i][j].getPositionX(), waves[i][j].getPositionY()) && (tours[t]->getCounter()>=(tours[t]->getCadence()*tours[t]->getCoeffCadence()) ) ){
							bullets.push_back(createBullet(tours[t], waves[i][j], app));
							tours[t]->setCounter(0);
							waves[i][j].setPV(waves[i][j].getPV()-(tours[t]->getPuissance()*tours[t]->getCoeffPuissance()));
							i=10;
							j=10;
						}
					}
					
				}
			}
		}

		// Draw bullets
		for(int i=0;i<bullets.size();i++){
			if(bullets[i]->moveToTarget()){
				draw_circle(bullets[i]->getPositionX(), bullets[i]->getPositionY(), 0.01);
			}else{
				bullets.erase(bullets.begin()+i);
			}
		}

		// Draw money
		draw_elem(textures_id[31],returnX(480, app),returnY(290, app),0.07f);
		int unite = myPlayer.getMoney() % 10;
		int dizaine = myPlayer.getMoney() / 10 % 10;
		int centaine = myPlayer.getMoney() / 100 % 10;
		draw_elem(textures_id[unite+21],returnX(460, app),returnY(290, app),0.07f);
		draw_elem(textures_id[dizaine+21],returnX(440, app),returnY(290, app),0.07f);
		draw_elem(textures_id[centaine+21],returnX(420, app),returnY(290, app),0.07f);	

		// Move the monster
		for(int i=0;i<waves.size();i++){
			for(int j=0;j<10;j++){
				if(waves[i][j].isAlive()){
					if(waves[i][j].getIdType() == 1){
						draw_enemy(textures_id[4],returnX(waves[i][j].getPositionX(), app),returnY(waves[i][j].getPositionY(), app),0.06f);
					}else if(waves[i][j].getIdType() == 2){
						draw_enemy(textures_id[5],returnX(waves[i][j].getPositionX(), app),returnY(waves[i][j].getPositionY(), app),0.06f);
					}else{
						draw_enemy(textures_id[6],returnX(waves[i][j].getPositionX(), app),returnY(waves[i][j].getPositionY(), app),0.06f);
					}
					if(waves[i][j].moveToNode(waves[i][j].getNextNodeX(), waves[i][j].getNextNodeY())){
						
						currentNode = app.map->nodes[waves[i][j].getIdNextNode()];
						waves[i][j].setCurrentNode(currentNode->id, currentNode->type, currentNode->x, currentNode->y);
						
						if(waves[i][j].getTypeCurrentNode() == 2){
							myPlayer.setPV(myPlayer.getPV()-waves[i][j].getDegats());
							if(!myPlayer.isAlive()){
								//printf("You lose !\n");
								loop=5;
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

		// Update time and variables
		time += 10;
		if(time>=2000){
			time=0;

			// Win if more than 10 waves
			if(waves.size()>=10){
				//printf("You win !\n");
				loop = 4;

			// Create a new wave if some (2) seconds have pass
			}else{
				int randTypeMonster = rand()%(3);
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

		// Update towers counter
		for(int i=0;i<tours.size();i++){
			tours[i]->setCounter(tours[i]->getCounter()+10);
		}

		SDL_Delay(10);
	}

	/*** Help screen ***/
	else if(loop == 3){
		draw_background(textures_id[18]);
			SDL_Event e;
			touche = 0;
			while (SDL_PollEvent(&e)){
				if (e.type == SDL_QUIT) {
					break;
				}
				switch (e.type){
					case SDL_MOUSEBUTTONUP:	
						xMouse = e.button.x;
						yMouse = e.button.y;
						printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
						break;
					case SDL_MOUSEMOTION:
						xMouse = e.button.x;
						yMouse = e.button.y;
						break;
					case SDL_KEYDOWN:
						printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						touche = e.key.keysym.sym;
						printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
						break;
					default : break;
				}
			}
			// Close the game
			if (touche == SDLK_q){
				loop = 0;
			}
			// Go to menu
			if (touche == SDLK_m){
				loop = 1;
			}
			// Close help screen and got to game
			if (touche == SDLK_h){
				// RESET ALL
				touche=0;
				bool check = false;
				bool check2 = true;
				loop = 2;
			}

			SDL_GL_SwapBuffers( );
			SDL_Delay(10);
	}
	/*** Win screen ***/
	else if(loop == 4){
		draw_background(textures_id[19]);
			SDL_Event e;
			touche = 0;
			while (SDL_PollEvent(&e)){
				if (e.type == SDL_QUIT) {
					break;
				}
				switch (e.type){
					case SDL_MOUSEBUTTONUP:	
						xMouse = e.button.x;
						yMouse = e.button.y;
						printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
						break;
					case SDL_MOUSEMOTION:
						xMouse = e.button.x;
						yMouse = e.button.y;
						break;
					case SDL_KEYDOWN:
						touche = e.key.keysym.sym;
						printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						touche = e.key.keysym.sym;
						printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
						break;
					default : break;
				}
			}
			// Close the game
			if (touche == SDLK_q){
				loop = 0;
			}
			// Go to menu
			if (touche == SDLK_m){
				loop = 1;
			}
			SDL_GL_SwapBuffers( );
			SDL_Delay(10);
	}
	/*** Lose screen ***/
	else if(loop == 5){
		draw_background(textures_id[20]);
			SDL_Event e;
			touche = 0;
			while (SDL_PollEvent(&e)){
				if (e.type == SDL_QUIT) {
					break;
				}
				switch (e.type){
					case SDL_MOUSEBUTTONUP:	
						xMouse = e.button.x;
						yMouse = e.button.y;
						printf("Clic en : (%d , %d)\n", e.button.x, e.button.y);
						break;
					case SDL_MOUSEMOTION:
						xMouse = e.button.x;
						yMouse = e.button.y;
						break;
					case SDL_KEYDOWN:
						touche = e.key.keysym.sym;
						printf("Touche pressee : (code : %d)\n", e.key.keysym.sym);
						break;
					case SDL_KEYUP:
						touche = e.key.keysym.sym;
						printf("Touche relachee : (code : %d)\n", e.key.keysym.sym);
						break;
					default : break;
				}
			}
			// Close the game
			if (touche == SDLK_q){
				loop = 0;
			}
			// Go to menu
			if (touche == SDLK_m){
				loop = 1;
			}
			SDL_GL_SwapBuffers( );
			SDL_Delay(10);
	}
	/** If loop is weird, close the game **/
	else {
		loop = 0;
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