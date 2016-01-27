/*
 * PureGaugeHMCUpdater.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: spiem_01
 */

#include "PureGaugeHMCUpdater.h"
#include "hmc_integrators/Integrate.h"
#include "actions/GaugeAction.h"
#include "hmc_forces/TestForce.h"
#include "io/GlobalOutput.h"
#include <string>
//#define REVERSIBILITY_CHECK

namespace Update {

PureGaugeHMCUpdater::PureGaugeHMCUpdater() : LatticeSweep(), HMCUpdater() { }

PureGaugeHMCUpdater::~PureGaugeHMCUpdater() { }

void PureGaugeHMCUpdater::execute(environment_t& environment) {
	//Initialize the momenta
	this->randomMomenta(momenta);

	//Copy the lattice and the environment
	environmentNew = environment;

	real_t beta = environment.configurations.get<double>("beta");

	//Get the gauge action
	GaugeAction* gaugeAction = GaugeAction::getInstance(environment.configurations.get<std::string>("name_action"),beta);

	//Get the initial energy of momenta
	long_real_t oldMomentaEnergy = this->momentaEnergy(momenta);
	//Get the initial energy of the lattice
	long_real_t oldLatticeEnergy = gaugeAction->energy(environment);

	//The t-length of a single integration step
	real_t t_length = environment.configurations.get<double>("hmc_t_length");
	//The numbers of integration steps
	std::vector<unsigned int> numbers_steps = environment.configurations.get< std::vector<unsigned int> >("number_hmc_steps");
	if (numbers_steps.size() != 1) {
		if (isOutputProcess()) std::cout << "PureGaugeHMCUpdater::Warning, pure gauge does not support multiple time integration!" << std::endl;
		numbers_steps.resize(1);
	}

	Integrate* integrate = Integrate::getInstance(environment.configurations.get<std::string>("name_integrator"));

	//The vector of the force for the integrator
	std::vector<Force*> force;
	force.push_back(gaugeAction);

	//Integrate numerically the equation of motion
	//integrate->integrate(environmentNew, momenta, force, numbers_steps, t_length);

	long_real_t energy_diff = 0., energy_diff_sq = 0.;
	int NN = 30;

	for (int i = 0; i < NN; ++i) {
		environmentNew = environment;
		this->randomMomenta(momenta);
		gaugeAction->setBeta(beta+0.1);
		integrate->integrate(environmentNew, momenta, force, numbers_steps, t_length);

		gaugeAction->setBeta(beta-0.25);
		integrate->integrate(environmentNew, momenta, force, numbers_steps, 0.1);

		gaugeAction->setBeta(beta+0.1);
		integrate->integrate(environmentNew, momenta, force, numbers_steps, t_length);

		//Get the final energy of momenta
		long_real_t newMomentaEnergy = this->momentaEnergy(momenta);
		//Get the final energy of the lattice
		long_real_t newLatticeEnergy = gaugeAction->energy(environmentNew);
		
		energy_diff += - (oldMomentaEnergy + oldLatticeEnergy) + (newMomentaEnergy + newLatticeEnergy);
		energy_diff_sq += (- (oldMomentaEnergy + oldLatticeEnergy) + (newMomentaEnergy + newLatticeEnergy))*(- (oldMomentaEnergy + oldLatticeEnergy) + (newMomentaEnergy + newLatticeEnergy));

		if (isOutputProcess()) std::cout << "Energy difference: " <<  - (oldMomentaEnergy + oldLatticeEnergy) + (newMomentaEnergy + newLatticeEnergy) << std::endl;
		
	}
	if (isOutputProcess()) std::cout << "Mean energy difference: " << energy_diff/NN << " +/ " << energy_diff_sq/(NN) - energy_diff*energy_diff/(NN*NN) << std::endl;

	gaugeAction->setBeta(beta);
#ifdef REVERSIBILITY_CHECK
	integrate->integrate(environmentNew, momenta, force, numbers_steps, -t_length);
#endif

	//Get the final energy of momenta
	long_real_t newMomentaEnergy = this->momentaEnergy(momenta);
	//Get the final energy of the lattice
	long_real_t newLatticeEnergy = gaugeAction->energy(environmentNew);

	//Global Metropolis Step
	bool metropolis = this->metropolis(oldMomentaEnergy + oldLatticeEnergy, newMomentaEnergy + newLatticeEnergy);
	if (metropolis) {
		environment = environmentNew;
		if (environment.measurement && isOutputProcess()) {
                        GlobalOutput* output = GlobalOutput::getInstance();

                        output->push("hmc_history");

                        output->write("hmc_history", - (oldMomentaEnergy + oldLatticeEnergy) + (newMomentaEnergy + newLatticeEnergy));
                        output->write("hmc_history", 1);

                        output->pop("hmc_history");
                }
	}
	else {
		if (environment.measurement && isOutputProcess()) {
                        GlobalOutput* output = GlobalOutput::getInstance();

                        output->push("hmc_history");

                        output->write("hmc_history", - (oldMomentaEnergy + oldLatticeEnergy) + (newMomentaEnergy + newLatticeEnergy));
                        output->write("hmc_history", 0);

                        output->pop("hmc_history");
                }
	}

#ifdef DEBUGFORCE
	//Test of the force
	TestForce testForce;
	testForce.testForce(environment.gaugeLinkConfiguration, gaugeAction, gaugeAction->force(environment, 5, 2), 5, 2);
#endif

	delete integrate;
        delete gaugeAction;
}

} /* namespace Update */
