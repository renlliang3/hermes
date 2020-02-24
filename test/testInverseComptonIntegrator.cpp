#include "gtest/gtest.h"
#include <memory>
#include <chrono>

#include "hermes.h"

namespace hermes {

/* Analytical integral over CMB with constant cross-section */
TEST(InverseComptonIntegrator, integrateOverPhotonEnergyCMB) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto dummyCS = std::make_shared<DummyCrossSection>(DummyCrossSection(1));
	auto photonField = std::make_shared<CMB>(CMB());
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, dummyCS));

        Vector3QLength pos(0);
	QEnergy Egamma = 10_GeV;
        QEnergy Eelectron = 1_TeV;

	auto res = intIC->integrateOverPhotonEnergy(pos, Egamma, Eelectron);
	
	QTemperature T_CMB = 2.725_K;
	QPDensity analytical_res = 16_pi * pow<3>(k_boltzmann * T_CMB/(c_light * h_planck)) 
			 * 1.20206; // Zeta_f(3) = 1.20206
	// 410 photons/cm3
	EXPECT_NEAR(static_cast<double>(res * 1_cm3),
			static_cast<double>(analytical_res * 1_cm3), 3);
}

/* Integral over photon field energy with Klein-Nishina */
TEST(InverseComptonIntegrator, integrateOverPhotonEnergy) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));

        Vector3QLength pos(0);
	QEnergy Egamma = 1_GeV;
        QEnergy Eelectron = 1e14_eV;
	
	QTemperature T_CMB = 2.725_K;
	QDifferentialCrossSection sigma(1e-34);
	auto analytical_res = sigma * 16_pi * pow<3>(k_boltzmann * T_CMB/(c_light * h_planck)) 
			 * 1.20206; // Zeta_f(3) = 1.20206

	auto res = intIC->integrateOverPhotonEnergy(pos, Egamma, Eelectron);
	
	EXPECT_NEAR(static_cast<double>(res),
			static_cast<double>(analytical_res), 1e-34);
}

TEST(InverseComptonIntegrator, integrateOverEnergy) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	Vector3QLength pos;
	pos.setX(30_pc);
	pos.setY(100_pc);
	pos.setZ(20_pc);
	QEnergy Egamma = 10_GeV;

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	auto res = intIC->integrateOverEnergy(pos, Egamma);
	std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
	
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cerr << "res: " << res << std::endl;
	std::cerr << "Electron: " << milliseconds.count() << " ms" << std::endl;
	std::cerr << pos << std::endl;
	
	//EXPECT_NEAR(emissivity.getValue(), 3.915573e-55, 2e-56); // J/m^3
}

TEST(InverseComptonIntegrator, compareLOSIntegrations) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QDirection dir = {pi/2*1_rad, 0_rad};
	QLength maxDist = intIC->getMaxDistance(dir);
	QEnergy Egamma = 10_GeV;

	auto integrand = [intIC, dir, Egamma](const QLength &dist) {
		return intIC->integrateOverEnergy(
				getGalacticPosition(intIC->getSunPosition(), dist, dir),
				Egamma
			); };
	
	auto result_QAG = gslQAGIntegration<QDifferentialFlux, QICOuterIntegral>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300);
	
	auto result_SI  = simpsonIntegration<QDifferentialFlux, QICOuterIntegral>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300);


	EXPECT_NEAR(static_cast<double>(result_QAG),
			static_cast<double>(result_SI),
			1e-5);
}
/*
TEST(InverseComptonIntegrator, integrateOverLOS) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QDirection dir = {pi/2*1_rad, 0_rad};
	QLength maxDist = intIC->getMaxDistance(dir);
	QEnergy Egamma = 10_GeV;

	auto integrand = [intIC, dir, Egamma](const QLength &dist) {
		return intIC->integrateOverEnergy(
				getGalacticPosition(intIC->getSunPosition(), dist, dir),
				Egamma
			); };
	
	std::shared_ptr<gsl_integration_workspace> workspace_ptr;
	workspace_ptr = static_cast<std::shared_ptr<gsl_integration_workspace>>(
			gsl_integration_workspace_alloc(1000));

	auto result_QAG = gslQAGIntegration<QDifferentialFlux, QICOuterIntegral>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300, workspace_ptr);
	
	auto result_SI  = simpsonIntegration<QDifferentialFlux, QICOuterIntegral>(
                        [maxDist, dir, integrand](QLength dist) {return integrand(dist);}, 0, maxDist, 300);


	EXPECT_NEAR(static_cast<double>(result_QAG),
			static_cast<double>(result_SI),
			1e-5);
}*/


TEST(InverseComptonIntegrator, initCacheTable) {
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto intIC = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(simpleModel, photonField, kleinnishina));
	
	QEnergy Egamma = 10_GeV;
	Vector3QLength pos1(0);
	Vector3QLength pos2(1_kpc, 1_kpc, 0.5_kpc);
	
	auto res1_withoutcache = intIC->integrateOverEnergy(pos1, Egamma);
	auto res2_withoutcache = intIC->integrateOverEnergy(pos2, Egamma);

	EXPECT_FALSE(intIC->isCacheTableEnabled());
	intIC->setupCacheTable(20, 20, 5);
	EXPECT_TRUE(intIC->isCacheTableEnabled());
	
	auto res1_withcache = intIC->integrateOverEnergy(pos1, Egamma);
	auto res2_withcache = intIC->integrateOverEnergy(pos2, Egamma);
	
	EXPECT_NEAR(static_cast<double>(res1_withoutcache),
			static_cast<double>(res1_withcache), 10);
	EXPECT_NEAR(static_cast<double>(res2_withoutcache),
			static_cast<double>(res2_withcache), 10);
}

TEST(InverseComptonIntegrator, PerformanceTest) {
	std::vector<PID> particletypes = {Electron, Positron};
	auto dragonModel = std::make_shared<Dragon2DCRDensity>(Dragon2DCRDensity(particletypes)); 
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto kleinnishina = std::make_shared<KleinNishina>(KleinNishina());
	auto photonField = std::make_shared<CMB>(CMB()); 
	auto in = std::make_shared<InverseComptonIntegrator>(
		InverseComptonIntegrator(dragonModel, photonField, kleinnishina));
	auto Egamma = 1_GeV;
	auto skymap = std::make_shared<GammaSkymap>(GammaSkymap(4, Egamma));
	skymap->setIntegrator(in);
	
	in->setupCacheTable(20, 20, 5);

        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	skymap->compute();
        std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	float pxl_speed = milliseconds.count()/skymap->getNpix()*getThreadsNumber();
	
	std::cerr << "pxl spd: " << pxl_speed << " ms" << std::endl;
	
	EXPECT_LE(pxl_speed, 350); // ms
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes

