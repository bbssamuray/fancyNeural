#include <stdio.h>
#include <vector>

#include "agent.h"

const int averageNeuronValue = 1000;

agent::agent(int neuralSize,motorNeuron motorNeuronArray[],int motorNeuronCount,sensoryNeuron sensoryNeuronArray[],int sensoryNeuronCount){

    this->neuralSize = neuralSize;
    this->motorNeuronArray = motorNeuronArray;
    this->motorNeuronCount = motorNeuronCount;
    this->sensoryNeuronArray = sensoryNeuronArray;
    this->sensoryNeuronCount = sensoryNeuronCount;

    neuralMatrix = new unsigned short[neuralSize*neuralSize];

    std::fill_n(neuralMatrix,neuralSize*neuralSize,averageNeuronValue);

    //We will give special neurons an absurdly high value so they will be easy to differenciate.
    //Motor neurons will have the values 10000,10001,10002...
    for(int i=0;i < motorNeuronCount;i++){
        neuralMatrix[motorNeuronArray[i].posY * neuralSize + motorNeuronArray[i].posX] = 10000 + i*1;
    }

    //Sensory neurons will have the values 20000,20001,20002
    for(int i=0;i < sensoryNeuronCount;i++){
        neuralMatrix[sensoryNeuronArray[i].posY * neuralSize + sensoryNeuronArray[i].posX] = 20000 + i*1;
    }

}

agent::~agent(){

    delete[] neuralMatrix;
    
}



void agent::thinkAndAct(){

    for(int i=0;i < motorNeuronCount;i++){

        motorNeuronArray[i].biggestImpulse = 0;

    }

    for(int i=0;i < sensoryNeuronCount;i++){
        std::vector<int> neuronChain;
        int currentNeuronLoc = sensoryNeuronArray[i].posY * neuralSize + sensoryNeuronArray[i].posX;
        int currentRecursion = 1;

        
        fireImpulse(sensoryNeuronArray[i].sensorFunction(sensoryNeuronArray[i].direction),&currentNeuronLoc,neuronChain,&currentRecursion);
        
    }


    int biggest = 0;
    for(int i=0;i < motorNeuronCount;i++){

        if(motorNeuronArray[i].biggestImpulse > motorNeuronArray[biggest].biggestImpulse){
            biggest = i;
        }
    
    }

    motorNeuronArray[biggest].motorFunction(motorNeuronArray[biggest].direction);

}

void agent::fireImpulse(int intensity,int* neuronLoc,std::vector<int> &neuronChain,int* currentRecursion){

    //Pretty sure we don't need this function to be recursive anymore
    //Will be a lot more efficent with a rewrite as the goal of this function has changed a lot since the beginning

    if(intensity == 0){
        return;
    }

    //This may be too aggressive
    if(*currentRecursion > neuralSize*neuralSize/4){
        return;
    }
    

    //We are finding the biggest impulse outcome in the neighbouring neurons
    //This will potentially create suboptimal paths
    //But we cannot recurse into every single neuron as that would be way too resource intensive
    int biggestMultiplier = 0;
    int biggestLocation;

    const int posX = *neuronLoc % neuralSize;
    const int posY = (*neuronLoc - posX) / neuralSize;

    //neuronChain.push_back();

    for(int y=-1;y <= 1;y++){
        for(int x=-1;x <= 1;x++){

            //We don't want to include the diagonal neurons
            if(x == y || x == -y){
                continue;
            }
            

            if(posX + x >= 0 && posX + x < neuralSize && posY + y >= 0 && posY + y < neuralSize){ //Simple boundary checks
                
                if (neuralMatrix[neuralSize*(posY + y) + (posX + x)] >= 10000){
                    
                    //If the value is above 10000, that means this is a special neuron
                    if(neuralMatrix[neuralSize*(posY + y) + (posX + x)] < 20000){
                        //It is a motor neuron if the value is between 10000 and 20000
                        //We can get it's index by subtracting 10000.
                        if (motorNeuronArray[neuralMatrix[neuralSize*(posY + y) + (posX + x)]-10000].biggestImpulse < intensity){
                            motorNeuronArray[neuralMatrix[neuralSize*(posY + y) + (posX + x)]-10000].biggestImpulse = intensity;
                            
                        }
                    }

                }else{ 
                    
                    //We prevent infinite loops by checking previous neurons we went through
                    bool duplicateNeuron = false;

                    for(int i=neuronChain.size()-1;i>=0;i--){
                        
                        //We will start looking for past neurons from the end

                        if(neuralSize*(posY + y) + (posX + x) == neuronChain[i]){
                            duplicateNeuron = true;
                            break;
                        }

                    }

                    if(duplicateNeuron){
                        continue;
                    }
                    
                    if(neuralMatrix[neuralSize*(posY + y) + (posX + x)] > biggestMultiplier){
                        biggestMultiplier = neuralMatrix[neuralSize*(posY + y) + (posX + x)];
                        biggestLocation = neuralSize*(posY + y) + (posX + x);  
                    }
                }
            }
        }    
    }

    //This function should be optimized further
    //It currently doesn't stop until all the paths it can take are blocked
    //That means it may go through every single neuron which will take time

    if (biggestMultiplier == 0){
        return;
    }else{

        neuronChain.push_back(*neuronLoc);
        *neuronLoc = biggestLocation;
        (*currentRecursion)++;

        fireImpulse(neuralMatrix[biggestLocation]*intensity/averageNeuronValue,neuronLoc,neuronChain,currentRecursion);

    }

}


unsigned short* agent::getNeuralMatrix(){

    return neuralMatrix;

}




