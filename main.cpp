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


short int playerX = 0;
short int playerY = 0;
short int foodX = 0;
short int foodY = 0;

bool isTraining = true;
bool shouldClose = false;

//The agent that will move around the interactive arena
agent* arenaAgent;

void movePlayer(char direction){

	//	  0
	//	1 + 2
	//	  3

	switch (direction){
		case 0:
			playerY += 1; 
			break;
		
		case 1:
			playerX -= 1;
			break;

		case 2:
			playerX += 1;
			break;

		case 3:
			playerY -= 1;
			break;
	}

}

int senseFood(char direction){
	//Uses the same direction scheme as the "movePlayer" function
	
	int impulse = 0;

	switch (direction){
		case 0:
			impulse = foodY - playerY;
			break;
		
		case 1:
			impulse = playerX - foodX;
			break;

		case 2:
			impulse = foodX - playerX;
			break;

		case 3:
			impulse = playerY - foodY;
			break;
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

	const int arenaSize = 11;

	const float arenaSquareSize = (screenSize.y-50)/arenaSize;
    const float neuronSquareSize = (screenSize.y-50)/brainSize;

	//initialize neuron rectangles
    for (int i = 0; i < brainSize*brainSize; i++){
        neuronRecs[i].x = 25.0f + neuronSquareSize *(i%brainSize);
        neuronRecs[i].y = 25.0f + neuronSquareSize *(i/brainSize);
        neuronRecs[i].width = neuronSquareSize - (neuronSquareSize/6);
        neuronRecs[i].height = neuronSquareSize - (neuronSquareSize/6);
    }

	//initialize arena rectangles
	for (int i = 0; i < arenaSize*arenaSize; i++){
        arenaRecs[i].x =  (screenSize.x - 100)/2 + 25.0f + arenaSquareSize *(i%arenaSize);
        arenaRecs[i].y = 25.0f + arenaSquareSize *(i/arenaSize);
        arenaRecs[i].width = arenaSquareSize;
        arenaRecs[i].height = arenaSquareSize;
    }

    Vector2 mousePoint = { 0.0f, 0.0f };

	int frame = 0;

    while (!WindowShouldClose())
    {
		frame++;
        bool isMouseOverNeuron = false;

		ClearBackground(RAYWHITE);

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

            DrawRectangle(mousePoint.x-50, mousePoint.y-45, 115,40 , GRAY);
            DrawText(std::to_string(neuronValue).c_str(), mousePoint.x-30, mousePoint.y-40, 30, WHITE);

        }


		

		Rectangle arenaRectangle = {(screenSize.x - 100)/2 + 25,25,arenaSquareSize*arenaSize,arenaSquareSize*arenaSize};


		if(isTraining){

			DrawRectangleRec(arenaRectangle,GRAY);
			DrawText("Currently Training\nPlease Wait.",(screenSize.x - 100)/2+100,75,40,BLACK);

		}else{

			//Draw the arena if done training

			if(frame % 15 == 0){
				arenaAgent->thinkAndAct();
			}

			for(int i=0;i < arenaSize*arenaSize;i++){
				
				const int visuralCorrection = arenaSize/2;

				for(int i=0;i < arenaSize*arenaSize;i++){
					if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
						if (CheckCollisionPointRec(mousePoint, arenaRecs[i])){
							playerX = 0;
							playerY = 0;
							foodX = i % arenaSize -visuralCorrection;
							foodY = i / arenaSize -visuralCorrection;
						}
					}

					//0,0 will be top left instead of middle if we don't do this
					const int playerScreenX = playerX + visuralCorrection;
					const int playerScreenY = playerY + visuralCorrection;
					const int foodScreenX = foodX + visuralCorrection;
					const int foodScreenY = foodY + visuralCorrection;

					DrawRectangleLinesEx(arenaRecs[i],2,BLACK);
					DrawRectangleRec(arenaRecs[playerScreenY*arenaSize+playerScreenX],RED); //Draw player
					DrawRectangleRec(arenaRecs[foodScreenY*arenaSize+foodScreenX],GREEN); //Draw food

				}
			}

		}

		
        BeginDrawing();
        

        EndDrawing();
    }

    CloseWindow();
	shouldClose = true;

}



int main(){

	srand(time(NULL));

	motorNeuron motorArray[4];
	sensoryNeuron sensoryArray[4];

	//We need to make sure there are no overlaps between neuron locations
	//Ideally, they shouldn't be next to each other either, but that is a problem for another day
	
	const int totalSpecialNeuronCount = sizeof(motorArray)/sizeof(motorNeuron) + sizeof(sensoryArray)/sizeof(sensoryNeuron);

	struct basicCoords{
		int x;
		int y;
	};
	

	basicCoords SNeuronLocs[totalSpecialNeuronCount]; //Special Neuron Locations
	//Keep neuron locations in an array so we can check for overlaps

	for(int i=0; i < totalSpecialNeuronCount;i++){

		SNeuronLocs[i] = {rand()%brainSize,rand()%brainSize};

		for(int a=0;a < i;a++){
			if(SNeuronLocs[a].x == SNeuronLocs[i].x && SNeuronLocs[a].y == SNeuronLocs[i].y){
				//Loop won't finish until every neuron has a unique coordinate
				i--;
				break;
			}
		}
	}

	//Apply values to the neurons
	for(int i=0; i < sizeof(motorArray)/sizeof(motorNeuron);i++){
		motorArray[i] = motorNeuron{SNeuronLocs[i].x,SNeuronLocs[i].y,0,&movePlayer,char(i)};
	}
	for(int i=0; i < sizeof(sensoryArray)/sizeof(sensoryNeuron);i++){
		sensoryArray[i] = sensoryNeuron{SNeuronLocs[i+4].x,SNeuronLocs[i+4].y,&senseFood,char(i)};
	}




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

		//Number of generations to be trained
		for(int x=0; x < 50;x++){

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
				
				distance += distanceToFood();
				agentBatch[i].fitness = 10000-(distance);


			}
		
			fittest = breed.breed(agentBatch);
			printf("gen %d fittest: %d fitness: %d distance: %d\n",x,fittest,agentBatch[fittest].fitness,distanceToFood());

			if(agentBatch[fittest].fitness == 10000){ break; }

		}

		arenaAgent = &agentBatch[fittest];
		isTraining = false;


		while(!shouldClose){
			WaitTime(500);
		}
	}
}