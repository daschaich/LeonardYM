/*
 * NFlavorQCDUpdater.h
 *
 *  Created on: May 2, 2012
 *      Author: spiem_01
 */

#ifndef MULTISTEPNFLAVORQCDUPDATER_H_
#define MULTISTEPNFLAVORQCDUPDATER_H_

#include "LatticeSweep.h"
#include "FermionHMCUpdater.h"
#include "Polynomial.h"
#include "RationalApproximation.h"
#include "NFlavorQCDAction.h"
#include "GaugeAction.h"

#include <vector>

namespace Update {

class MultiStepNFlavorQCDUpdater: public Update::LatticeSweep, public Update::FermionHMCUpdater {
public:
	MultiStepNFlavorQCDUpdater();
	MultiStepNFlavorQCDUpdater(const MultiStepNFlavorQCDUpdater& toCopy);
	~MultiStepNFlavorQCDUpdater();

	virtual void execute(environment_t& environment);

protected:
	void initializeApproximations(environment_t& environment);

	void checkTheory(const environment_t& environment) const;

protected:
	//The new environment, provided by HMC
	environment_t environmentNew;
	//The conjugate momenta
	extended_gauge_lattice_t momenta;
	//The pseudofermion field
	std::vector<extended_dirac_vector_t> pseudofermions;
	std::vector<RationalApproximation> rationalApproximationsHeatBath;
	std::vector<RationalApproximation> rationalApproximationsMetropolis;
	std::vector< std::vector<RationalApproximation> > rationalApproximationsForce;
	//The vector of polynomia
	//The tmp pseudofermion field
	extended_dirac_vector_t tmp_pseudofermion;
	//The action of the theory
	NFlavorAction* nFlavorQCDAction;
	//The gauge action of the theory
	GaugeAction* gaugeAction;
	//The fermion action of the theory
	NFlavorFermionAction** fermionAction;
	//The dirac operators for the theory
	DiracOperator* squareDiracOperator;
	DiracOperator* diracOperator;
};

} /* namespace Update */
#endif /* NFLAVORQCDUPDATER_H_ */