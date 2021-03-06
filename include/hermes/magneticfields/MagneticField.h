#ifndef HERMES_MAGNETICFIELD_H
#define HERMES_MAGNETICFIELD_H

#include <memory>

#include "hermes/Units.h"
#include "hermes/Vector3.h"
#include "hermes/Vector3Quantity.h"

#ifdef HERMES_HAVE_MUPARSER
#include "muParser.h"
#endif

namespace hermes { namespace magneticfields {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class MagneticField
 @brief Abstract base class for magnetic fields.
 */
class MagneticField {
  public:
	virtual ~MagneticField() {}
	virtual Vector3QMField getField(const Vector3QLength &position) const {
		return Vector3QMField(0_muG);
	};
};

/**
 @class PeriodicMagneticField
 @brief Magnetic field decorator implementing periodic fields.
 */
class PeriodicMagneticField : public MagneticField {
	std::shared_ptr<MagneticField> field;
	Vector3QLength origin, extends;
	bool reflective;

  public:
	PeriodicMagneticField(std::shared_ptr<MagneticField> field,
	                      const Vector3QLength &extends);
	PeriodicMagneticField(std::shared_ptr<MagneticField> field,
	                      const Vector3QLength &extends,
	                      const Vector3QLength &origin, bool reflective);
	Vector3QLength &getOrigin();
	void setOrigin(const Vector3QLength &origin);
	Vector3QLength &getExtends();
	void setExtends(const Vector3QLength &origin);
	bool isReflective();
	void setReflective(bool reflective);
	Vector3QMField getField(const Vector3QLength &position) const override;
};

/**
 @class MagneticFieldList
 @brief Magnetic field decorator implementing a superposition of fields.
 */
class MagneticFieldList : public MagneticField {
	std::vector<std::shared_ptr<MagneticField>> fields;

  public:
	void addField(const std::shared_ptr<MagneticField> &field);
	Vector3QMField getField(const Vector3QLength &position) const override;
};

/**
 @class UniformMagneticField
 @brief Magnetic field with one B-field vector.
 */
class UniformMagneticField : public MagneticField {
	Vector3QMField value;

  public:
	explicit UniformMagneticField(const Vector3QMField &value) : value(value) {}
	Vector3QMField getField(const Vector3QLength &position) const override {
		return value;
	}
};

/** @} */
}}  // namespace hermes::magneticfields

#endif  // HERMES_MAGNETICFIELD_H
