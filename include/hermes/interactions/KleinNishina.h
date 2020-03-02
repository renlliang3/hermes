#ifndef HERMES_KLEINNISHINA_H
#define HERMES_KLEINNISHINA_H

#include "hermes/interactions/DifferentialCrossSection.h"

#include <memory>

namespace hermes {

class KleinNishina: public DifferentialCrossSection {
public:
	KleinNishina();
	
	// not used
	QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const;

	QDifferentialCrossSection getDiffCrossSection(
			const QEnergy &E_electron,
			const QEnergy &E_photon,
			const QEnergy &E_gamma) const;
};

} // namespace hermes

#endif // HERMES_KLEINNISHINA_H
