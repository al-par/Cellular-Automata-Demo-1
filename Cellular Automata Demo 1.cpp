//standard includes
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

//Allegro 5 includes
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>


//display properties
#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
#define FRAMERATE 60 //This value is in frames per second

//cell properties
#define CELL_SIZE 10 //must be a common multiple of both DISPLAY_WIDTH and DISPLAY_HEIGHT
#define CELL_SPAWN_CHANCE_INITIAL 10 //higher values decrease the probability of a cell spawning initially

using namespace std;


int main()
{
	srand(time(NULL));
	int returnCode = 0;

	//___________________________________________________________________________________________________________________________________________________________________
	//initialize allegro 5

	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_TIMER* timer;

	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_primitives_addon();

	display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FRAMERATE);

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_timer_event_source(timer));

	al_start_timer(timer);


	//___________________________________________________________________________________________________________________________________________________________________
	//update loop variable declarations

	//update loop boolean
	bool running = true;

	//keyboard
	bool pressed_keys[ALLEGRO_KEY_MAX];
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++) pressed_keys[i] = false; //set all key presses to false by default

	//cells
	bool cellAlive[DISPLAY_WIDTH / CELL_SIZE][DISPLAY_HEIGHT / CELL_SIZE];
	for (int i = 0; i < DISPLAY_WIDTH / CELL_SIZE; i++) //spawn initial cells
	{
		for (int j = 0; j < DISPLAY_HEIGHT / CELL_SIZE; j++)
		{
			if (rand() % CELL_SPAWN_CHANCE_INITIAL == 0)
			{
				cellAlive[i][j] = true;
			}
			else cellAlive[i][j] = false;
		}
	}


	//___________________________________________________________________________________________________________________________________________________________________
	//game update loop
	while (running)
	{
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------
		//Event handling
		ALLEGRO_EVENT event;
		bool waiting = true;
		while (waiting) //Make update loop wait for an event
		{
			al_wait_for_event(queue, &event);
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) /*Terminate Program*/
			{
				running = false;
				waiting = false;
				returnCode -= 1; // changing the return code like this instead of setting it to a new value allows the program to return with multiple return codes.
				//yes, I know logging would be 'better', but this isnt a very complex program, and there aren't many return codes.
			}

			//---------------------------------------------------------------------------------------------------------------------------------------------------------------
			//Update Keyboard
			else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
				{
					if (event.keyboard.keycode == i)
					{
						pressed_keys[i] = true;
						cout << i << endl;
					}
				}
			}
			else if (event.type == ALLEGRO_EVENT_KEY_UP)
			{
				for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
				{
					if (event.keyboard.keycode == i)
					{
						pressed_keys[i] = false;
					}
				}
			}


			//---------------------------------------------------------------------------------------------------------------------------------------------------------------
			//update and display page
			else if (event.type == ALLEGRO_EVENT_TIMER)
			{
				al_clear_to_color(al_map_rgba(255, 255, 255, 0));
				
				//display living cells
				for (int i = 0; i < DISPLAY_WIDTH / CELL_SIZE; i++)
				{
					for (int j = 0; j < DISPLAY_HEIGHT / CELL_SIZE; j++)
					{
						if (cellAlive[i][j] == true)
						{
							al_draw_filled_rectangle(
								i * CELL_SIZE, j * CELL_SIZE,
								i * CELL_SIZE + CELL_SIZE, j * CELL_SIZE + CELL_SIZE,
								al_map_rgb(0, 0, 0));
						}
					}
				}

				//update cell state
				for (int i = 0; i < DISPLAY_WIDTH / CELL_SIZE; i++)
				{
					for (int j = 0; j < DISPLAY_HEIGHT / CELL_SIZE; j++)
					{ 
						//count neighbors (moore neighbors)
						int neighborsAlive = 0;
						int x = DISPLAY_WIDTH / CELL_SIZE;
						int y = DISPLAY_HEIGHT / CELL_SIZE;

						if (j - 1 > 0)
						{
							if (i - 1 > 0)	if (cellAlive[i - 1][j - 1] == true) neighborsAlive++; //top left
											if (cellAlive[i    ][j - 1] == true) neighborsAlive++; //top middle
							if (i + 1 < x)	if (cellAlive[i + 1][j - 1] == true) neighborsAlive++; //top right
						}
						if (j + 1 < y)
						{
							if (i - 1 > 0) if (cellAlive[i - 1][j + 1] == true) neighborsAlive++; //bottom left
										   if (cellAlive[i    ][j + 1] == true) neighborsAlive++; //bottom middle
							if (i + 1 < x) if (cellAlive[i + 1][j + 1] == true) neighborsAlive++; //bottom right
						}
						if (i - 1 > 0) if (cellAlive[i - 1][j] == true) neighborsAlive++; //middle left
						if (i + 1 < x) if (cellAlive[i + 1][j] == true) neighborsAlive++; //middle right
					}
				}

				al_flip_display();
				waiting = false;
			}
		}
	}


	//___________________________________________________________________________________________________________________________________________________________________
	//destructors and cleanup

	al_destroy_display(display);
	al_uninstall_keyboard();
	al_uninstall_mouse();
	al_destroy_timer(timer);

	//end of program
	return returnCode;
}