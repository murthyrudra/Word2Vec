/*
 * ActivationFunction.h
 *
 *  Created on: Sep 27, 2014
 *      Author: rudramurthy
 */

#ifndef ACTIVATIONFUNCTION_H_
#define ACTIVATIONFUNCTION_H_
#include <math.h>
#include <vector>
#include <algorithm>

namespace neuralNetwork
{
	class ActivationFunction
	{
		public:
			ActivationFunction();
			ActivationFunction(const ActivationFunction& temp)
			{

			}
			virtual ~ActivationFunction();
			virtual double calculateActivation(double) = 0;
			virtual double activationDifferentiation(double val) = 0;
			void calculateActivation(std::vector<double>& values)
			{

			}
			void calculateActivation(double* values, int n)
			{

			}
	};

	class SigmoidActivationFunction: public ActivationFunction
	{
		public:

			double calculateActivation(double val)
			{
				if (val < -45)
					return 0.0;
				else if (val > 45)
					return 1.0;
				else
					return (1.0 / (1.0 + exp(-val)));
			}

			double activationDifferentiation(double val)
			{
				return val * ( 1.0 - val);
			}
	};

	class LinearActivationFunction: public ActivationFunction
	{
		public:
			double calculateActivation(double val)
			{
				return val;
			}

			double activationDifferentiation(double val)
			{
				return 1.0;
			}
	};

	class SoftMaxActivationFunction: public ActivationFunction
	{
		public:
			double calculateActivation(double val)
			{
				return val;
			}

			double activationDifferentiation(double val)
			{
				return 1.0;
			}

			void calculateActivation(std::vector<double>& values)
			{
				double maximum = *std::max_element(values.begin(), values.end());

				for(unsigned int i = 0;  i < values.size(); i++)
				{
					if (values[i] != -std::numeric_limits<double>::infinity())
					{
						values[i] = exp(values[i] - maximum);
					}
				}
			}

			void calculateActivation(double* values, int n)
			{
				double maximum = -100.0;
				for(int i = 0;  i < n; i++)
				{
					if(maximum < values[i])
					{
						maximum = values[i];
					}
				}

				for(int i = 0;  i < n; i++)
				{
					if (values[i] != -std::numeric_limits<double>::infinity())
					{
						values[i] = exp(values[i] - maximum);
					}
				}
			}
	};

	class TanhActivationFunction: public ActivationFunction
	{
		public:
			double calculateActivation(double val)
			{
				double pez = exp(val);
				double nez = exp(-val);

				return (pez - nez) / (pez + nez);
			}

			double activationDifferentiation(double val)
			{
				return ( 1.0 - val * val);
			}
	};

	class HardTanhActivationFunction: public ActivationFunction
	{
		public:
			double calculateActivation(double val)
			{
				if(val > 1.0)
					return 1.0;
				else if(val < -1.0)
					return -1.0;
				else
					return val;
			}

			double activationDifferentiation(double val)
			{
				if(val > 1.0 || val < -1.0)
					return 0.0;
				else
					return 1.0;
			}
	};

	class RectifiedLinearUnitsActivationFunction: public ActivationFunction
	{
		public:
			double calculateActivation(double val)
			{
				if(val > 0.0)
					return val;
				else
					return 0.0;
			}

			double activationDifferentiation(double val)
			{
				if(val > 0.0)
					return 1.0;
				else
					return 0.0;
			}
	};

	class ClippedRectifiedLinearUnitsActivationFunction: public ActivationFunction
	{
		public:
			double calculateActivation(double val)
			{
				if(val > 1.0)
					return 1.0;
				else if(val > 0.0)
					return val;
				else
					return 0.0;
			}

			double activationDifferentiation(double val)
			{
				if(val > 1.0)
					return 0.0;
				else if(val > 0.0)
					return 1.0;
				else
					return 0.0;
			}
	};

} /* namespace neuralNetwork */

#endif /* ACTIVATIONFUNCTION_H_ */
