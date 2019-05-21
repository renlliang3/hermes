#ifdef HERMES_HAVE_CFITSIO

#include "hermes/neutralGasDensity/RingModelDensity.h"
#include "hermes/Common.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <array>
#include <algorithm>

namespace hermes {

RingData::RingData() {
	readDataFiles();
}

void RingData::readDataFiles() {
	ffile = std::make_unique<FITSFile>(
			FITSFile(getDataPath("RingModelDensity/NHrings_Ts300K.fits"))); 
	ffile->openFile(FITS::READ);

	n_lon = ffile->readKeyValueAsInt("NAXIS1");
	n_lat = ffile->readKeyValueAsInt("NAXIS2");
	n_rings = ffile->readKeyValueAsInt("NAXIS3");
	
	min_lon = ffile->readKeyValueAsDouble("CRVAL1");
	delta_lon = ffile->readKeyValueAsDouble("CDELT1");
	min_lat = ffile->readKeyValueAsDouble("CRVAL2");
	delta_lat = ffile->readKeyValueAsDouble("CDELT2");
	
	std::cerr << "Number of rings: " << n_rings << std::endl;

	int firstElement = 1;
	int nElements = n_lon*n_lat*n_rings;
	dataVector = ffile->readImageAsFloat(firstElement, nElements);
}

int RingData::getRingNumber() const {
	return n_rings;
}

QColumnDensity RingData::getColumnDensityInRing(int ring, const QDirection& dir) const {
	QAngle lat = dir[0];
    	QAngle lon = dir[1] + 180_deg; // becaue the galactic centre of
				       // the ring model is in the middle of the map

	int pxl_lat = static_cast<int>(
			std::round(
				static_cast<double>((lat / 180_deg) * n_lat))
			);
	int pxl_lon = static_cast<int>(
			std::round(
				static_cast<double>((lon / 360_deg) * n_lon))
			);

	// NAXIS1 x NAXIS2 x NAXIS3 => lon x lat x ring
	return dataVector[
		(ring * n_lat + pxl_lat) * n_lon + pxl_lon
	] / 1_cm2; // the data is given in cm^-2
}

Ring::Ring(std::size_t index_, std::shared_ptr<RingData> dataPtr_,
                        QLength innerR_, QLength outerR_) :
	index(index_), dataPtr(dataPtr_), innerR(innerR_), outerR(outerR_) {
}

std::size_t Ring::getIndex() const {
	return index;
}

std::pair<QLength, QLength> Ring::getBoundaries() const {
	return std::make_pair(innerR, outerR);
}

bool Ring::isInside(const Vector3QLength &pos) const {
	QLength rho = sqrt(pos.x*pos.x + pos.y*pos.y);
	return (rho > innerR && rho < outerR);
}

QColumnDensity Ring::getColumnDensity(const QDirection& dir_) const {
	return dataPtr->getColumnDensityInRing(index, dir_);
}

RingModelDensity::RingModelDensity() {
	dataPtr = std::make_shared<RingData>(RingData());
	fillRingContainer();	
}

void RingModelDensity::fillRingContainer() {
	for(std::size_t i = 0; i < dataPtr->getRingNumber(); ++i) {
		ringContainer.push_back(
			std::make_shared<Ring>(Ring(i, dataPtr, boundaries[i], boundaries[i+1])));
	}
}

int RingModelDensity::getRingNumber() const {
	return dataPtr->getRingNumber();
}

std::shared_ptr<Ring> RingModelDensity::operator[](const std::size_t i) const {
        return ringContainer[i];
}

RingModelDensity::iterator RingModelDensity::begin() {
        return ringContainer.begin();
}

RingModelDensity::const_iterator RingModelDensity::begin() const {
        return ringContainer.begin();
}

RingModelDensity::iterator RingModelDensity::end() {
        return ringContainer.end();
}

RingModelDensity::const_iterator RingModelDensity::end() const {
        return ringContainer.end();
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO
