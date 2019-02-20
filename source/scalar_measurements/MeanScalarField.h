/*
 * MeanScalarField.h
 *
 *  Created on: Feb 29, 2012
 *      Author: spiem_01
 */

#ifndef MEANSCALARFIELD_H_
#define MEANSCALARFIELD_H_

#include "LatticeSweep.h"

namespace Update {

class MeanScalarField : public Update::LatticeSweep {
public:
	MeanScalarField();
	~MeanScalarField();

	/**
	 * This function measure the scalar field on the lattice and it prints out its value.
	 * @param enviroment
	 * @param sweep
	 * @param n
	 */
	virtual void execute(environment_t& environment);
	
	long_real_t meanValueSquared(const extended_adjoint_color_vector_t& field) const;
};

} /* namespace Update */
#endif /* PLAQUETTE_H_ */