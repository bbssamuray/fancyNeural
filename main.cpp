#include <stdio.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>
#include <string>

#include "raylib.h"

#include "agent.h"
#include "breeder.h"

//Total neuron amount will be brainSize*brainSize
const int brainSize = 20;

//Amount of agents in every generation
const int batchSize = 10000;

//Amount of parents that will be taken from the last gen
const int parentAmount = 100;

bool shouldClose = false;


short int playerX = 0;
short int playerY = 0;
short int foodX = 0;
short int foodY = 0;

void movePlayer(char direction){

	//	  5
	//	0 + 2
	//	  3
	// 0 is left, 2 is right
	// 5 is up, 3 is down
	//Now that i look back to this, doing it this way was a really stupid idea...
	//Looks pretty here at least :p

	if(direction < 3){
		playerX += direction -1;
	}else{
		playerY += direction -4;
	}

}

int senseFood(char direction){
	//Uses the same direction scheme as the "movePlayer" function
	
	int impulse = 0;

	if(direction == 0){
		impulse = playerX - foodX;
	}else if(direction == 2){
		impulse = foodX - playerX;
	}else if(direction == 5){
		impulse = foodY - playerY;
	}else if(direction == 3){
		impulse = playerY - foodY;
	}

	if(impulse <= 0){
		return 0;
	}
	
	return 10000/impulse;

}

int distanceToFood(){
	//Player's distance to the food. Used in the fitness function

	const int x = playerX - foodX;
	const int y = playerY - foodY;

	return x*x + y*y;

}

void* window(void* fittest){

	unsigned short* fittestBrain = (unsigned short*)fittest;

	SetTargetFPS(20);

	Vector2 screenSize = {1300,650};
	
	InitWindow(screenSize.x, screenSize.y, "Neural evolution");

	Rectangle neuronRecs[brainSize*brainSize] = { 0 };
	Rectangle arenaRecs[brainSize*brainSize] = { 0 };

    const float neuronSquareSize = (screenSize.y-50)/brainSize;

    for (int i = 0; i < brainSize*brainSize; i++){
        neuronRecs[i].x = 25.0f + neuronSquareSize *(i%brainSize);
        neuronRecs[i].y = 25.0f + neuronSquareSize *(i/brainSize);
        neuronRecs[i].width = neuronSquareSize - (neuronSquareSize/6);
        neuronRecs[i].height = neuronSquareSize - (neuronSquareSize/6);
    }


    Vector2 mousePoint = { 0.0f, 0.0f };

    while (!WindowShouldClose())
    {
        bool isMouseOverNeuron = false;

		//Value of the neuron mouse is over
		unsigned short neuronValue;

        mousePoint = GetMousePosition();

		//Drawing the brain

        for (int i = 0; i < brainSize*brainSize; i++){
			unsigned short neuronWeight = fittestBrain[i];
			 
			if(neuronWeight < 10000){
				//Clamp the weight values so the values between the 500 and 1500 look more distinct
				short neuronColor = neuronWeight-500;
				if(neuronColor > 1000){
					neuronColor = 1000;
				}else if(neuronColor < 0){
					neuronColor = 0;
				}
				neuronColor = (neuronColor/1000.0)*255;
				
            	DrawRectangle(neuronRecs[i].x, neuronRecs[i].y,neuronRecs[i].width,neuronRecs[i].height , Color{0,(unsigned char)neuronColor,0,255});
			}else if(neuronWeight >= 20000){
				//Weight is more than 200000 which means this is a sensory neuron
				DrawRectangle(neuronRecs[i].x, neuronRecs[i].y,neuronRecs[i].width,neuronRecs[i].height , Color{255,0,0,255});
			}else{
				//Weight is between 10000 and 20000 which means this is a motor neuron
				DrawRectangle(neuronRecs[i].x, neuronRecs[i].y,neuronRecs[i].width,neuronRecs[i].height , Color{0,0,255,255});
			}

            if (CheckCollisionPointRec(mousePoint, neuronRecs[i])){
				neuronValue = neuronWeight;
                isMouseOverNeuron = true;
            }

        }

		//Show neuron weight when mouse is over it
        if(isMouseOverNeuron){

            DrawRectangle(mousePoint.x-50, mousePoint.y-45, 100,40 , GRAY);
            DrawText(std::to_string(neuronValue).c_str(), mousePoint.x-30, mousePoint.y-40, 30, WHITE);

        }


		//Draw the arena

		const int arenaSize = 11;

		const float arenaSquareSize = (screenSize.y-50)/arenaSize;

		for(int i=0;i < arenaSize*arenaSize;i++){
			DrawRectangleLinesEx(Rectangle{675+i%arenaSize*arenaSquareSize,25+i/arenaSize*arenaSquareSize,arenaSquareSize,arenaSquareSize},2,BLACK);
		}


        BeginDrawing();
        ClearBackground(RAYWHITE);
            
        EndDrawing();
    }

    CloseWindow();
	shouldClose = true;

}



int main(){

	srand(time(NULL));

	

	motorNeuron motorArray[4];
	printf("asdasdasda: %d",sizeof(motorArray)/sizeof(motorNeuron));
	/*motorArray[0] = motorNeuron{6,6,0,&movePlayer,0};
	motorArray[1] = motorNeuron{13,6,0,&movePlayer,2};
	motorArray[2] = motorNeuron{6,13,0,&movePlayer,5};
	motorArray[3] = motorNeuron{13,13,0,&movePlayer,3};*/

	motorArray[0] = motorNeuron{rand()%brainSize,rand()%brainSize,0,&movePlayer,0};
	motorArray[1] = motorNeuron{rand()%brainSize,rand()%brainSize,0,&movePlayer,2};
	motorArray[2] = motorNeuron{rand()%brainSize,rand()%brainSize,0,&movePlayer,5};
	motorArray[3] = motorNeuron{rand()%brainSize,rand()%brainSize,0,&movePlayer,3};


	sensoryNeuron sensoryArray[4];
	/*sensoryArray[0] = sensoryNeuron{2,2,&senseFood,0};
	sensoryArray[1] = sensoryNeuron{17,2,&senseFood,2};
	sensoryArray[2] = sensoryNeuron{2,17,&senseFood,5};
	sensoryArray[3] = sensoryNeuron{17,17,&senseFood,3};*/

	sensoryArray[0] = sensoryNeuron{rand()%brainSize,rand()%brainSize,&senseFood,0};
	sensoryArray[1] = sensoryNeuron{rand()%brainSize,rand()%brainSize,&senseFood,2};
	sensoryArray[2] = sensoryNeuron{rand()%brainSize,rand()%brainSize,&senseFood,5};
	sensoryArray[3] = sensoryNeuron{rand()%brainSize,rand()%brainSize,&senseFood,3};


	std::vector<agent> agentBatch;
	agentBatch.reserve(batchSize);

	agent* temp;
	
	for(int i=0;i < batchSize;i++){

		temp = new agent(brainSize,
			motorArray  ,sizeof(motorArray)/sizeof(motorNeuron),
			sensoryArray,sizeof(sensoryArray)/sizeof(sensoryNeuron));

		agentBatch.emplace_back(*temp);
		
	}

	unsigned short* fittestBrain = agentBatch[0].getNeuralMatrix();

	pthread_t   thread;
    int result = pthread_create(&thread, NULL, window, fittestBrain);
    if (result == 0){

		breeder breed = breeder(parentAmount,brainSize);

		int fittest = 0;

		for(int x=0; x < 35;x++){

			for(int i=0;i<batchSize;i++){

				//We will put the food in the up-right diagonal first
				playerX = 0;
				playerY = 0;
				foodX = 3;
				foodY = 3;

				for(int a=0;a < 10;a++){
					//We will give every agent 10 moves
					agentBatch[i].thinkAndAct();
				}
				if(i == fittest){
					printf("pos0: %d %d\n",playerX,playerY);
				}

				int distance = distanceToFood(); //Temporary place to keep first fitness

				//And then we will put it in the left-down diagonal so it learns all 4 directions properly
				playerX = 0;
				playerY = 0;
				foodX = -3;
				foodY = -3;

				for(int a=0;a < 10;a++){
					//We will give every agent 10 moves
					agentBatch[i].thinkAndAct();
				}
				if(i == fittest){
					printf("pos1: %d %d\n",playerX,playerY);
				}
				
				distance += distanceToFood();
				agentBatch[i].fitness = 10000-(distance);


			}

			playerX = 0;
			playerY = 0;
			foodX = 3;
			foodY = -3;

			for(int a=0;a < 10;a++){
			
				agentBatch[fittest].thinkAndAct();

			}

			printf("test result: %d %d\n",playerX,playerY);

			fittest = breed.breed(agentBatch);
			printf("\ngen %d fittest: %d fitness: %d distance: %d\n",x,fittest,agentBatch[fittest].fitness,distanceToFood());

		}

		while(!shouldClose){
			WaitTime(500);
		}
	}
}