#include "hermes/example.h"

#include <iostream>
#include <memory>

namespace hermes {

void playground() {

	// magnetic field models
	auto B = Vector3QMField(1_muG, 0_muG, 0_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	auto PT11 = std::make_shared<PT11Field>(PT11Field());

	// cosmic ray density models
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity(1_GeV, 1e4_GeV, 10));
	//auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_3D.fits", Electron)); 

	// gas models
	auto gas = std::make_shared<HII_Cordes91>(HII_Cordes91());

	// integrators
	auto synchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(JF12, simpleModel));
	auto RM = std::make_shared<RMIntegrator>(RMIntegrator(JF12, gas));

	int nside = 4;	

	//auto skymaps = std::make_shared<SynchroSkymapRange>(SynchroSkymapRange(nside, 1_MHz, 500_MHz, 10));
	//auto skymap = std::make_shared<SynchroSkymap>(SynchroSkymap(nside, 408_MHz));
	auto skymap = std::make_shared<RMSkymap>(RMSkymap(nside));
	//auto ptr_skymap_range = std::make_shared<SynchroSkymapRange>(SynchroSkymap(nside, MinF, MaxF, nsteps));
	//skymap->setIntegrator(synchro);
	skymap->setIntegrator(RM);

	skymap->compute();

	skymap->printPixels();
	
#ifdef HERMES_HAVE_CFITSIO
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	skymap->save(output);
#endif // HERMES_HAVE_CFITSIO

}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}

