/*
 * MesonCorrelator.h
 *
 *  Created on: Jul 23, 2012
 *      Author: spiem_01
 */

#ifndef NPRVERTEX_H_
#define NPRVERTEX_H_

#include "LatticeSweep.h"
#include "fermion_measurements/StochasticEstimator.h"
#include "dirac_operators/DiracOperator.h"
#include "inverters/BiConjugateGradient.h"

namespace Update {

class NPRVertex : public Update::LatticeSweep, public Update::StochasticEstimator {
public:
	NPRVertex();
	NPRVertex(const NPRVertex& toCopy);
	~NPRVertex();

	virtual void execute(environment_t& environment);

private:
	extended_dirac_vector_t randomNoise;
	extended_dirac_vector_t eta;
	extended_dirac_vector_t tmp[4*diracVectorLength];
	DiracOperator* diracOperator;
	DiracOperator* squareDiracOperator;
	BiConjugateGradient* biConjugateGradient;
};

} /* namespace Update */
#endif /* MESONCORRELATOR_H_ */
