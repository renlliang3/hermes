#ifndef HERMES_H
#define HERMES_H

#include "hermes/Units.h"
#include "hermes/Vector3.h"
#include "hermes/Vector3Quantity.h"
#include "hermes/Grid.h"
#include "hermes/GridTools.h"
#include "hermes/Random.h"
#include "hermes/HEALPixBits.h"
#include "hermes/FITSWrapper.h"
#include "hermes/Common.h"

#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapTemplate.h"
#include "hermes/skymaps/RMSkymap.h"
#include "hermes/skymaps/SynchroSkymap.h"
#include "hermes/skymaps/SynchroSkymapRange.h"

#include "hermes/integrators/Integrator.h"
#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"

#include "hermes/magneticField/MagneticField.h"
#include "hermes/magneticField/MagneticFieldGrid.h"
#include "hermes/magneticField/JF12Field.h"
#include "hermes/magneticField/PT11Field.h"

#include "hermes/gasDensity/GasDensity.h"
#include "hermes/gasDensity/HII_Cordes91.h"

#include "hermes/cosmicRayDensity/CosmicRayDensity.h"
#include "hermes/cosmicRayDensity/SimpleCRDensity.h"
#include "hermes/cosmicRayDensity/DragonCRDensity.h"

#include "hermes/outputs/Output.h"
#include "hermes/outputs/FITSOutput.h"

#endif // HERMES_H
