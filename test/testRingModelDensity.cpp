#include "gtest/gtest.h"

#include "hermes.h"
#include <array>

namespace hermes {

class RingModel : public ::testing::Test {
protected:
  //void SetUp() override {}
  // void TearDown() override {}
  RingModelDensity ringModel = RingModelDensity();
};

TEST_F(RingModel, RingBoundaries) {
	std::vector<std::pair<QLength, QLength> > result;

	for (auto ring : ringModel) {
		result.push_back(ring->getBoundaries());
        }

	std::array<QLength, 12> b = {0_kpc, 2_kpc, 3_kpc, 4_kpc, 5_kpc, 6_kpc,
                7_kpc, 9_kpc, 12_kpc, 15_kpc, 18_kpc, 35_kpc};

	EXPECT_EQ(static_cast<double>(result[0].second),
			static_cast<double>(b[1]));	
	EXPECT_EQ(static_cast<double>(result[1].second),
			static_cast<double>(b[2]));	
	EXPECT_EQ(static_cast<double>(result[11].first),
			static_cast<double>(b[11]));	
}

TEST_F(RingModel, isInside) {
	Vector3QLength pos(3.5_kpc, 0_kpc, 1_kpc);

	EXPECT_TRUE(ringModel[2]->isInside(pos));
	EXPECT_FALSE(ringModel[3]->isInside(pos));
}

TEST_F(RingModel, RingValues) {
	QDirection dir = {90_deg,5_deg};
	auto X0 = 1.8e20 / 1_cm2 / 1_K / 1_km * 1_s;
	QColumnDensity col_HI(0);
	QColumnDensity col_H2(0);

	for (auto ring : ringModel) {
		col_HI += ring->getHIColumnDensity(dir);
		col_H2 += X0*ring->getCOIntensity(dir);
        }

	EXPECT_NEAR(static_cast<double>(col_HI),
		2e26, 5e25);
	EXPECT_NEAR(static_cast<double>(col_H2),
		3e26, 5e25);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes
