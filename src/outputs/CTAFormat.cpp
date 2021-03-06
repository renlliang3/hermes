#ifdef HERMES_HAVE_CFITSIO

#include "hermes/outputs/CTAFormat.h"

#include "hermes/HEALPixBits.h"
#include "hermes/Version.h"

namespace hermes { namespace outputs {

CTAFormat::CTAFormat(const std::string &filename)
    : Output(), filename(filename) {
	ffile = std::make_unique<FITSFile>(FITSFile(filename));
	ffile->createFile();
}

void CTAFormat::createTable(int nrows, const std::string &unit) {

	long naxis = 3;
	int nEnergies = 45;
	int nLat = 1000;
	int nLon = 1000;
	long int naxes[3] = {nEnergies, nLat, nLon};
	int firstElem = 1;
	long int nullnaxes[1] = {0};
	float nullArray[1] = {0};

	ffile->createImage(FITS::IMGDOUBLE, 0, nullnaxes);
	ffile->writeDate();

	const int tfields = 1;
	char *ttype[] = {const_cast<char *>("TFLOAT")};
	char *tform[] = {const_cast<char *>("1E")};
	char *tunit[] = {const_cast<char *>(unit.c_str())};
	const char extname[] = "xtension";

	ffile->createTable(FITS::BINARY, nrows, tfields, ttype, tform, tunit,
	                   extname);
	
	auto createdby = FITSKeyValue("SOFTWARE", "HERMES");
	ffile->writeKeyValue(createdby,
	                     "Created by Hermes, cosmicrays.github.io/hermes");
	auto version = FITSKeyValue("VERSION", g_GIT_DESC);
	ffile->writeKeyValue(version, "Hermes ver. [GIT_DESC]");
}

void CTAFormat::writeMetadata(int nside, double res, bool hasMask,
                                  const std::string &description) {
	auto str_type = FITSKeyValue("PIXTYPE", "HEALPIX");
	ffile->writeKeyValue(str_type, "HEALPIX Pixelisation");
	auto indx_str = FITSKeyValue("INDXSCHM", "IMPLICIT");
	ffile->writeKeyValue(indx_str, "Indexing: IMPLICIT or EXPLICIT");
	auto firstpix = FITSKeyValue("FIRSTPIX", 0);
	ffile->writeKeyValue(firstpix, "Lowest pixel index present");
	auto lastpix =
	    FITSKeyValue("LASTPIX", static_cast<long int>(nside2npix(nside) - 1));
	ffile->writeKeyValue(lastpix, "Highest pixel index present");
	auto ext_str = FITSKeyValue("EXTNAME", "xtension");
	ffile->writeKeyValue(ext_str, "Name of this binary table extension");
	auto bad_data = FITSKeyValue("BAD_DATA", UNSEEN);
	ffile->writeKeyValue(bad_data,
	                     "Sentinel value given to missing or bad pixels");
	auto ordering = FITSKeyValue("ORDERING", "RING    ");
	ffile->writeKeyValue(ordering,
	                     "Pixel ordering scheme, either RING or NESTED");
	auto coordsys = FITSKeyValue("COORDSYS", "G       ");
	ffile->writeKeyValue(coordsys,
	                     "Pixelisation coordinate system, G = Galactic");
	auto f_nside = FITSKeyValue("NSIDE", nside);
	ffile->writeKeyValue(f_nside, "Resolution parameter for HEALPIX");
	auto f_res = FITSKeyValue("RES", res);
	ffile->writeKeyValue(f_res, "Resolution of map");
	char skycoverage[8];
	if (hasMask)
		strcpy(skycoverage, "PARTIAL\0");
	else
		strcpy(skycoverage, "FULLSKY\0");
	auto object = FITSKeyValue("OBJECT", skycoverage);
	ffile->writeKeyValue(object, "Sky coverage, either FULLSKY or PARTIAL");
	auto process = FITSKeyValue("PROCESS", description.c_str());
	ffile->writeKeyValue(process, "Physical process / Integrator used");
}

void CTAFormat::writeKeyValueAsDouble(const std::string &key, double value,
                                          const std::string &description) {
	auto keyvalue = FITSKeyValue(key.c_str(), value);
	ffile->writeKeyValue(keyvalue, description.c_str());
}

void CTAFormat::writeKeyValueAsString(const std::string &key,
                                          const std::string &value,
                                          const std::string &description) {
	auto keyvalue = FITSKeyValue(key.c_str(), value.c_str());
	ffile->writeKeyValue(keyvalue, description.c_str());
}

void CTAFormat::writeColumn(int nElements, void *array) {
	ffile->writeColumn(FITS::FLOAT, 1, 1, 1, nElements, array);
}

}}  // namespace hermes::outputs

#endif  // HERMES_HAVE_CFITSIO
