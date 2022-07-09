#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <algorithm> 
#include <iostream>

#include "agent.h"
#include "breeder.h"

//A list of of 'numOfParents' number of agents that have the best fitness score
int* fitnessList;

//Percentage of neurons that will be mutated per agent
//Must be below 100
const int mutationRatePercent = 2;

//How effective mutations are at max
const int mutationEff = 70;

breeder::breeder(int numOfParents,int neuralSize){

    this->brainSize = neuralSize;
    this->numOfParents = numOfParents;
    this->numOfTotalAgents = numOfTotalAgents;

    //A linked list may be better for this?
    fitnessList = new int[numOfParents];


    
}

breeder::~breeder(){
    
    delete [] fitnessList;

}

void breeder::updateFitnessList(std::vector<agent> &agents){

    //fitnessList contains the index of the agents with the best fitnesses
    //NOT THEIR FITNESS VALUES!

    for(int i=0;i < numOfParents;i++){
        fitnessList[i] = i;
    }

    //this sort algorithm can probably be optimized

    for(int y=0;y < numOfParents;y++){

        for(int x=y;x < numOfParents;x++){

            if(agents[fitnessList[x]].fitness < agents[fitnessList[y]].fitness){

                int temp = fitnessList[x];
                fitnessList[x] = fitnessList[y];
                fitnessList[y] = temp; 

            }

        }

    }

    //-------end of sort-------

    for(int i=numOfParents;i < agents.size();i++){
        //We start with the agent that has the 'numOfParents' index as all the previous ones are already in the list
        int a = numOfParents;
        
        while(agents[i].fitness > agents[fitnessList[a-1]].fitness){
            //Go down one by one until we find it's place
            a--;

            if(a == 0){
                break;
            }
            
        }
        if(a == numOfParents){
            continue;
        }else{

            //We slide down the list
            for(int x=numOfParents-2;x >= a;x--){
                fitnessList[x+1] = fitnessList[x];
            }
            fitnessList[a] = i;

        }
    
    }
        
}


void breeder::mutate(unsigned short* parentMatrix,unsigned short* targetMatrix){


    std::copy(parentMatrix,parentMatrix+(brainSize*brainSize),targetMatrix);

    for(int i=0;i < brainSize*100/mutationRatePercent;i++){

        //We might mutate a neuron twice, but checking for the neurons we edited before would be too expensive with very small benefit
        int randIndex = rand() % (brainSize*brainSize);

        if(parentMatrix[randIndex] >= 10000){
            //Don't do anything if it is a special neuron
            continue;
        }

        //Range of mutation will be [-mutationEff,+mutationEff]
        int randEffect = (rand() % ((mutationEff*2)+1)) - mutationEff;

        targetMatrix[randIndex] += randEffect;

        if(targetMatrix[randIndex] > 2000){
            targetMatrix[randIndex] = 2000;
        }else if(targetMatrix[randIndex] < 1){
            targetMatrix[randIndex] = 1;
        }
        
        //targetMatrix[randIndex] = parentMatrix[randIndex] + randEffect;
        //^^We may do it like this if we don't want mutation effects to be added on top

    }

}


int breeder::breed(std::vector<agent> &agents){

    updateFitnessList(agents);

    //We are using all of the parents one by one on repeat
    //This var stores which parent we will use in the next iteration
    //Wraps back to 0 when it is equal to 'numOfParents'
    int parentQueue = 0;

    for(int i=0;i < agents.size();i++){

        //Don't modify the agent if it's amongst the parents
        bool isParent = false;
        for(int a=0;a < numOfParents;a++){
            if(i == fitnessList[a]){
                isParent = true;
                break;
            }
        }
        if (isParent) continue;


        mutate(agents[fitnessList[parentQueue]].getNeuralMatrix(),agents[i].getNeuralMatrix());


        parentQueue++;
        if(parentQueue >= numOfParents){
            parentQueue = 0;
        }

    }

    return fitnessList[99];

}