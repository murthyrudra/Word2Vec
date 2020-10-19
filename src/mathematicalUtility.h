/*
 * mathematicalUtility.h
 *
 *  Created on: Oct 8, 2014
 *      Author: rudramurthy
 */

#ifndef MATHEMATICALUTILITY_H_
#define MATHEMATICALUTILITY_H_
#include<vector>

namespace neuralNetwork
{

	class mathematicalUtility
	{
		public:
			mathematicalUtility();
			virtual ~mathematicalUtility();

		static void multiplyMatrix(std::vector<double> inputVector,std::vector<double> weights, std::vector<double>& output, int m, int n)
		{
			for(int i = 0; i < m; i++)
			{
				output[i] = 0.0;
				for(int j = 0; j < n; j++)
				{
					output[i] += weights[i * n + j] * inputVector[j];
				}
			}
		}
		static void addBias(std::vector<double> bias, std::vector<double>& output, int m)
		{
			for(int i = 0; i < m; i++)
			{
				output[i] += bias[i];
			}
		}
	};

} /* namespace neuralNetwork */

#endif /* MATHEMATICALUTILITY_H_ */
