#ifdef HERMES_HAVE_CFITSIO

#include "hermes/outputs/HEALPixFormat.h"

#include <iostream>
#include <string>

namespace hermes { namespace outputs {

HEALPixFormat::HEALPixFormat(const std::string &filename)
    : Output(), filename(filename) {}

void HEALPixFormat::initOutput() {
	ffile = std::make_unique<FITSFile>(FITSFile(filename));

	ffile->createFile();

	int firstElem = 1;
	long int nullnaxes[1] = {1};
	float nullArray[1] = {0};

	ffile->createImage(FITS::IMGFLOAT, 1, nullnaxes);
	ffile->writeImage(FITS::FLOAT, 1, 1, nullArray);
}

void HEALPixFormat::createTable(int nrows) {
	const int tfields = 1;
	char *ttype[] = {const_cast<char *>("TFLOAT")};
	char *tform[] = {const_cast<char *>("1E")};
	char *tunit[] = {const_cast<char *>("nothing")};
	const char extname[] = "xtension";

	ffile->createTable(FITS::BINARY, nrows, tfields, ttype, tform, tunit,
	                   extname);
	ffile->writeDate();
}

void HEALPixFormat::writeMetadata(int nside, double res,
                                  const std::string &description) {
	auto str_type = FITSKeyValue("PIXTYPE", "HEALPIX");
	ffile->writeKeyValue(str_type, "HEALPIX Pixelisation");
	auto ext_str = FITSKeyValue("EXTNAME", "xtension");
	ffile->writeKeyValue(ext_str, "Name of this binary table extension");
	auto ordering = FITSKeyValue("ORDERING", "RING    ");
	ffile->writeKeyValue(ordering,
	                     "Pixel ordering scheme, either RING or NESTED");
	auto f_nside = FITSKeyValue("NSIDE", nside);
	ffile->writeKeyValue(f_nside, "Resolution parameter for HEALPIX");
	auto f_res = FITSKeyValue("RES", res);
	ffile->writeKeyValue(f_res, "Resolution of map");
	auto object = FITSKeyValue("OBJECT", "FULLSKY");
	ffile->writeKeyValue(object, "Sky coverage, either FULLSKY or PARTIAL");
	auto indx_str = FITSKeyValue("INDXSCHM", "IMPLICIT");
	ffile->writeKeyValue(indx_str, "Indexing: IMPLICIT or EXPLICIT");
	auto process = FITSKeyValue("PROCESS", description.c_str());
	ffile->writeKeyValue(process, NULL);
}

void HEALPixFormat::writeKeyValueAsDouble(const std::string &key, double value,
                                          const std::string &description) {
	auto keyvalue = FITSKeyValue(key.c_str(), value);
	ffile->writeKeyValue(keyvalue, description.c_str());
}

void HEALPixFormat::writeKeyValueAsString(const std::string &key,
                                          const std::string &value,
                                          const std::string &description) {
	auto keyvalue = FITSKeyValue(key.c_str(), value.c_str());
	ffile->writeKeyValue(keyvalue, description.c_str());
}

void HEALPixFormat::writeColumn(int nElements, void *array) {
	ffile->writeColumn(FITS::FLOAT, 1, 1, 1, nElements, array);
}

}}  // namespace hermes::outputs

#endif  // HERMES_HAVE_CFITSIO