#include "Network.h"
#include <vector>
#include <iostream>
#include "TransfertFunction.h"
#include <cassert>

using namespace std;


Network::Network(const vector<unsigned> &architecture,const TransfertFunction &transfertFunction){
	unsigned numLayers=architecture.size();
	for(unsigned layerNum=0;layerNum < numLayers;++layerNum){
		m_layers.push_back(Layer());
		unsigned numOutputs= layerNum==architecture.size()-1? 0: architecture[layerNum+1];
		for(unsigned neuronNum=0; neuronNum<=architecture[layerNum];++neuronNum){
			m_layers.back().push_back(Neuron(numOutputs,neuronNum,transfertFunction));
			cout << "Made a Neuron!"<<endl;
		}
	}
}


//This function sends the output value obtained by the neuron network with the last inputs entered
void Network::getResultsNetwork(vector<double> &resultVals){
	resultVals.clear();
	for(unsigned n = 0; n < m_layers.back().size() - 1; ++n){
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
}


//This function updates the output values for each neuron in each layer according 
//to a set of input values
void Network::feedForwardNetwork(const vector<double> &inputVals){
	assert(inputVals.size()== m_layers[0].size() - 1);

	//Assign the input values into the input neurons
	for (unsigned i=0;i<inputVals.size();++i){
		m_layers[0][i].setOutputVal(inputVals[i]);
	}

	//Forward propragate
	for(unsigned layerNum=1;layerNum < m_layers.size();++layerNum){
		Layer &prevLayer=m_layers[layerNum - 1];
		for(unsigned n = 0; n< m_layers[layerNum].size() - 1;++n){
			m_layers[layerNum][n].feedForwardNeuron(prevLayer);
		}
	}
}


//This function does the backPropagation of the neural network
void Network::backPropNetwork(const vector<double> &targetVals){
	// Calculate overall net error 'RMS of the output neuron errors)
	Layer &outputLayer = m_layers.back();
	m_error = 0.0;

	for(unsigned n = 0; n < outputLayer.size() - 1; ++n){
		double delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta*delta;
	}
	m_error /= outputLayer.size() - 1;
	m_error = sqrt(m_error);

	// Implement a recent average measurement
	
	m_recentAverageError = 
		(m_recentAverageError * m_recentAverageSmoothingFactor + m_error) 
		/ ( m_recentAverageSmoothingFactor + 1 );


	//Calculate output layer gradients

	for(unsigned n = 0; n < outputLayer.size() - 1; ++n){
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}

	// Calculate gradients on hidden layers
	for( unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum){
		Layer &hiddenLayer = m_layers[layerNum];
		Layer &nextLayer = m_layers[layerNum + 1];

		for( unsigned n =0; n < hiddenLayer.size(); ++n){
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}
	// For all layers from outputs to first hidden layer.
	// update connection weights
	for( unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum){
		Layer &layer=m_layers[layerNum];
		Layer &prevLayer=m_layers[layerNum - 1];
		for(unsigned n = 0; n < layer.size(); ++ n){
			layer[n].updateInputWeights(prevLayer);
	}

}


