#include "hermes/integrators/RMIntegrator.h"

#include <iostream>

namespace hermes {

RMIntegrator::RMIntegrator(const std::shared_ptr<MagneticField> mfield,
			   const std::shared_ptr<GasDensity> gdensity) : mfield(mfield), gdensity(gdensity) {	
}

RMIntegrator::~RMIntegrator() { }

RMSkymap::tPixel RMIntegrator::integral(tDirection dir) {

	constexpr auto const_a0 = pow<3>(e_plus)/(8*pi*pi*epsilon0*squared(m_electron)*pow<3>(c_light));
	Vector3Length pos(0_m);
	QLength delta_d = 10.0_pc;

	//double dtr = 1.0;
	dir[0] = pi/2.0*radian-dir[0];
	//auto bdeg = dir[0]/dtr;
	//auto ldeg = dir[1]/dtr;

	QRotationMeasure sum(0);

	for(QLength dist = 0.0_pc; dist <= 30.0_kpc; dist += delta_d) {


		QLength R = distanceFromGC(dir, dist);
		Vector3Length pos = sphericalToCartesian(dist, dir[0], dir[1]);

		//	R2 = (R_sun > 0) ? pow(R_sun, 2) + pow(d * cos(b), 2) - 2.0 * R_sun * d * cos(b) * cos(l) 
                //        : pow(R_sun, 2) + pow(d * cos(b), 2) + 2.0 * R_sun * d * cos(b) * cos(l); 

		//       if (R2 < pc * pc) 
		// 	   R2 = pc * pc;
		// 	R = sqrt(R2);
               
		// 	x = (R_sun > 0) ? R_sun - d * cos(b) * cos(l) : R_sun + d * cos(b) * cos(l);
		//	y = d * cos(b) * sin(l);
		//	z = d * sin(b);

		// integrate \int_0^\infty dd I(l,b,d) -> \Delta d \sum_i I_i(x, y, z <- l, b, d)

	
		//std::cout << sum.getValue() << std::endl;
		//sum += (delta_d*(const_a0 * mfield->getField(pos).getR() * gdensity->getDensity(pos)));
		sum += (const_a0 * mfield->getField(pos).getR() * gdensity->getDensity(pos) * delta_d);
	}

	//std::cout << (mfield->getField(pos).getR()).getValue() << std::endl;

	return sum;
}

} // namespace hermes 
