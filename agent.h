#include<stdio.h>
#include <vector>

#pragma once

struct sensoryNeuron{

	int posX,posY;
	int (*sensorFunction)(char direction);
	char direction;

};

struct motorNeuron{

	int posX,posY;
	//Biggest impulse this motor neuron got in this think cycle.
	int biggestImpulse;
	void (*motorFunction)(char direction);
	//Parameter to pass to to the function
	char direction;

};

class agent{

	public:
		agent(int size,motorNeuron motorNeuronArray[],int motorNeuronCount,sensoryNeuron sensoryNeuronArray[],int sensoryNeuronCount);
		~agent();
		unsigned short* getNeuralMatrix();
		void thinkAndAct();
		unsigned short int fitness;

		
		
		
	private:
		void fireImpulse(int intensity,int* neuronLoc,std::vector<int> &neuronChain,int* currentRecursion);

		int neuralSize;
		unsigned short* neuralMatrix;
		motorNeuron* motorNeuronArray;
		int motorNeuronCount;
		sensoryNeuron* sensoryNeuronArray;
		int sensoryNeuronCount;

};
