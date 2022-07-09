#include<stdio.h>
#include <vector>

#pragma once




class breeder{

	public:
		breeder(int numOfParents,int neuralSize);
		~breeder();
		int breed(std::vector<agent> &agents);

		
	private:
		void updateFitnessList(std::vector<agent> &agents);
		void mutate(unsigned short* parentMatrix,unsigned short* targetMatrix);
		int numOfParents;
		int brainSize;
		int numOfTotalAgents;

};
