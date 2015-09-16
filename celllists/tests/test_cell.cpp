// CellList is a 3D domain decomposition library.
// Copyright (C) 2011-2015 The CellList Development Team
//
// This file is part of CellList.
//
// CellList is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// CellList is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>
//
//--


#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <celllists/cell.h>
#include <celllists/vec3.h>

#include "common.h"


namespace cl = celllists;
namespace vec3 = celllists::vec3;


// Fixtures
// ========

class CellTest : public ::testing::Test {
 public:
  virtual void SetUp() = 0;

  void set_up_data() {
    // Example tests
    std::fill(myvecs, myvecs + 9, 0.0);
    myvecs[0] = 2;
    myvecs[4] = 1;
    myvecs[8] = 4;
    if (nvec == 2) {
      myvecs[4] = 0.0;
      myvecs[5] = 4.0;
    }
    mycell.reset(new cl::Cell(myvecs, nvec));
    // Singular cell vectors
    std::fill(singvecs, singvecs + 9, 0.0);
    if (nvec > 1) {
      singvecs[0] = 1.0;
      singvecs[3] = 0.5;
    }
    if (nvec == 3) {
      singvecs[3] = 0.0;
      singvecs[4] = 2.0;
      singvecs[6] = 0.5;
      singvecs[7] = 0.8;
    }
  }

  std::unique_ptr<cl::Cell> create_random_cell(const unsigned int seed,
      const double scale = 1.0, const bool cuboid = false) {
    return create_random_cell_nvec(seed, nvec, scale, cuboid);
  }

  int nvec;
  std::unique_ptr<cl::Cell> mycell;
  double myvecs[9];
  double singvecs[9];
};


class CellTestP : public CellTest, public ::testing::WithParamInterface<int> {
 public:
  virtual void SetUp() {
    nvec = GetParam();
    set_up_data();
  }
};


class CellTest0 : public CellTest {
 public:
  virtual void SetUp() {
    nvec = 0;
    set_up_data();
  }
};


class CellTest1 : public CellTest {
 public:
  virtual void SetUp() {
    nvec = 1;
    set_up_data();
  }
};


class CellTest2 : public CellTest {
 public:
  virtual void SetUp() {
    nvec = 2;
    set_up_data();
  }
};


class CellTest3 : public CellTest {
 public:
  virtual void SetUp() {
    nvec = 3;
    set_up_data();
  }
};


// Tests grouped by main method being tested
// =========================================


// Constructor
// ~~~~~~~~~~~


TEST_P(CellTestP, constructor_singular) {
  EXPECT_THROW(cl::Cell cell(singvecs, GetParam()), cl::singular_cell_vectors);
}


TEST_F(CellTest3, constructor_nvec_negative) {
  double vecs[9] = {1, 0, 1, 0, 1, 0, 0.5, 0.5, 0};
  EXPECT_THROW(cl::Cell cell(vecs, -1), std::domain_error);
}


TEST_F(CellTest3, constructor_nvec_too_large) {
  double vecs[9] = {1, 0, 1, 0, 1, 0, 0.5, 0.5, 0};
  EXPECT_THROW(cl::Cell cell(vecs, 4), std::domain_error);
}


TEST_F(CellTest0, constructor) {
  EXPECT_EQ(0, mycell->nvec());
  EXPECT_TRUE(std::isnan(mycell->volume()));
  EXPECT_TRUE(std::isnan(mycell->gvolume()));
  std::unique_ptr<cl::Cell> gcell(mycell->create_reciprocal());
  EXPECT_EQ(0, gcell->nvec());
  EXPECT_TRUE(std::isnan(gcell->volume()));
  EXPECT_TRUE(std::isnan(gcell->gvolume()));
}


TEST_P(CellTestP, constructor_simple) {
  double vecs[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  cl::Cell cell(vecs, nvec);
  EXPECT_EQ(nvec, cell.nvec());
  EXPECT_EQ(1.0, cell.vec(0)[0]);
  EXPECT_EQ(0.0, cell.vec(0)[1]);
  EXPECT_EQ(0.0, cell.vec(0)[1]);
  EXPECT_EQ(0.0, cell.vec(1)[0]);
  EXPECT_EQ(1.0, cell.vec(1)[1]);
  EXPECT_EQ(0.0, cell.vec(1)[2]);
  EXPECT_EQ(0.0, cell.vec(2)[0]);
  EXPECT_EQ(0.0, cell.vec(2)[1]);
  EXPECT_EQ(1.0, cell.vec(2)[2]);
  EXPECT_EQ(1.0, cell.gvec(0)[0]);
  EXPECT_EQ(0.0, cell.gvec(0)[1]);
  EXPECT_EQ(0.0, cell.gvec(0)[2]);
  EXPECT_EQ(0.0, cell.gvec(1)[0]);
  EXPECT_EQ(1.0, cell.gvec(1)[1]);
  EXPECT_EQ(0.0, cell.gvec(1)[2]);
  EXPECT_EQ(0.0, cell.gvec(2)[0]);
  EXPECT_EQ(0.0, cell.gvec(2)[1]);
  EXPECT_EQ(1.0, cell.gvec(2)[2]);
  EXPECT_EQ(1.0, cell.volume());
  EXPECT_EQ(1.0, cell.gvolume());
  EXPECT_EQ(1.0, cell.spacings()[0]);
  EXPECT_EQ(1.0, cell.spacings()[1]);
  EXPECT_EQ(1.0, cell.spacings()[2]);
  EXPECT_EQ(1.0, cell.gspacings()[0]);
  EXPECT_EQ(1.0, cell.gspacings()[1]);
  EXPECT_EQ(1.0, cell.gspacings()[2]);
  EXPECT_EQ(true, cell.cubic());
  EXPECT_EQ(true, cell.cuboid());
}


// create_reciprocal
// ~~~~~~~~~~~~~~~~~

TEST_P(CellTestP, create_reciprocal) {
  std::unique_ptr<cl::Cell> gcell(mycell->create_reciprocal());
  // Make sure pointers are not copied.
  EXPECT_NE(gcell->vecs(), mycell->gvecs());
  EXPECT_NE(gcell->gvecs(), mycell->vecs());
  EXPECT_NE(gcell->lengths(), mycell->glengths());
  EXPECT_NE(gcell->glengths(), mycell->lengths());
  EXPECT_NE(gcell->spacings(), mycell->gspacings());
  EXPECT_NE(gcell->gspacings(), mycell->spacings());
  // Make sure the contents of the data members are the same
  EXPECT_TRUE(std::equal(gcell->vecs(), gcell->vecs() + 3*nvec, mycell->gvecs()));
  EXPECT_TRUE(std::equal(gcell->gvecs(), gcell->gvecs() + 3*nvec, mycell->vecs()));
  EXPECT_TRUE(std::equal(gcell->lengths(), gcell->lengths() + nvec, mycell->glengths()));
  EXPECT_TRUE(std::equal(gcell->glengths(), gcell->glengths() + nvec, mycell->lengths()));
  EXPECT_TRUE(std::equal(gcell->spacings(), gcell->spacings() + nvec, mycell->gspacings()));
  EXPECT_TRUE(std::equal(gcell->gspacings(), gcell->gspacings() + nvec, mycell->spacings()));
}


// create_subcell
// ~~~~~~~~~~~~~~

TEST_P(CellTestP, create_subcell_simple) {
  double vecs[9]{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  cl::Cell cell(vecs, nvec);
  int shape[3]{10, 10, 10};
  double spacings[3]{0.1, 0.1, 0.1};
  bool pbc[3] = {false, false, true};
  std::unique_ptr<cl::Cell> subcell(cell.create_subcell(shape, spacings, pbc));
  EXPECT_EQ(3, subcell->nvec());
  EXPECT_EQ(0.1, subcell->vecs()[0]);
  EXPECT_EQ(0.0, subcell->vecs()[1]);
  EXPECT_EQ(0.0, subcell->vecs()[2]);
  EXPECT_EQ(0.0, subcell->vecs()[3]);
  EXPECT_EQ(0.1, subcell->vecs()[4]);
  EXPECT_EQ(0.0, subcell->vecs()[5]);
  EXPECT_EQ(0.0, subcell->vecs()[6]);
  EXPECT_EQ(0.0, subcell->vecs()[7]);
  EXPECT_EQ(0.1, subcell->vecs()[8]);
  EXPECT_EQ(nvec>0, pbc[0]);
  EXPECT_EQ(nvec>1, pbc[1]);
  EXPECT_EQ(nvec>2, pbc[2]);
}


TEST_P(CellTestP, create_subcell_random) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    int shape[3];
    double spacings[3];
    fill_random_int(irep + NREP, shape, 3, 1, 20);
    fill_random_double(irep, spacings, 3, 0.1, 2.0);
    bool pbc[3] = {irep%2==0, (irep>>1)%2==0, (irep>>1)%2==0};
    std::unique_ptr<cl::Cell> subcell(cell->create_subcell(shape, spacings, pbc));
    double vol = cell->volume();
    for (int ivec = 0; ivec < nvec; ++ivec) {
      EXPECT_NEAR(cell->lengths()[ivec]/shape[ivec], subcell->lengths()[ivec], 1e-10);
      vol /= shape[ivec];
      EXPECT_EQ(true, pbc[ivec]);
    }
    for (int ivec = nvec; ivec < 3; ++ivec) {
      EXPECT_NEAR(cell->lengths()[ivec]*spacings[ivec-nvec], subcell->lengths()[ivec], 1e-10);
      vol *= spacings[ivec-nvec];
      EXPECT_EQ(false, pbc[ivec]);
    }
    EXPECT_NEAR(vol, subcell->volume(), 1e-10);
  }
}


TEST_F(CellTest1, subcell_example) {
  int shape[1]{4};
  double spacings[2]{0.1, 0.2};
  bool pbc[3] = {false, true, true};
  std::unique_ptr<cl::Cell> subcell(mycell->create_subcell(shape, spacings, pbc));
  EXPECT_EQ(3, subcell->nvec());
  EXPECT_NEAR(0.5, subcell->vecs()[0], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[1]);
  EXPECT_EQ(0.0, subcell->vecs()[2]);
  EXPECT_EQ(0.0, subcell->vecs()[3]);
  EXPECT_NEAR(0.1, subcell->vecs()[4], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[5]);
  EXPECT_EQ(0.0, subcell->vecs()[6]);
  EXPECT_EQ(0.0, subcell->vecs()[7]);
  EXPECT_NEAR(0.2, subcell->vecs()[8], 1e-10);
  EXPECT_EQ(true, pbc[0]);
  EXPECT_EQ(false, pbc[1]);
  EXPECT_EQ(false, pbc[2]);
}


TEST_F(CellTest2, subcell_example) {
  int shape[2]{10, 16};
  double spacings[1]{0.55};
  bool pbc[3] = {false, false, true};
  std::unique_ptr<cl::Cell> subcell(mycell->create_subcell(shape, spacings, pbc));
  EXPECT_EQ(3, subcell->nvec());
  EXPECT_NEAR(0.2, subcell->vecs()[0], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[1]);
  EXPECT_EQ(0.0, subcell->vecs()[2]);
  EXPECT_EQ(0.0, subcell->vecs()[3]);
  EXPECT_EQ(0.0, subcell->vecs()[4]);
  EXPECT_NEAR(0.25, subcell->vecs()[5], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[6]);
  EXPECT_NEAR(-0.55, subcell->vecs()[7], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[8]);
  EXPECT_EQ(true, pbc[0]);
  EXPECT_EQ(true, pbc[1]);
  EXPECT_EQ(false, pbc[2]);
}


TEST_F(CellTest3, subcell_example) {
  int shape[3]{20, 5, 8};
  bool pbc[3] = {false, false, false};
  std::unique_ptr<cl::Cell> subcell(mycell->create_subcell(shape, nullptr, pbc));
  EXPECT_EQ(3, subcell->nvec());
  EXPECT_NEAR(0.1, subcell->vecs()[0], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[1]);
  EXPECT_EQ(0.0, subcell->vecs()[2]);
  EXPECT_EQ(0.0, subcell->vecs()[3]);
  EXPECT_NEAR(0.2, subcell->vecs()[4], 1e-10);
  EXPECT_EQ(0.0, subcell->vecs()[5]);
  EXPECT_EQ(0.0, subcell->vecs()[6]);
  EXPECT_EQ(0.0, subcell->vecs()[7]);
  EXPECT_NEAR(0.5, subcell->vecs()[8], 1e-10);
  EXPECT_EQ(true, pbc[0]);
  EXPECT_EQ(true, pbc[1]);
  EXPECT_EQ(true, pbc[2]);
}


// iwrap
// ~~~~~

TEST_F(CellTest1, iwrap_example) {
  double delta[3] = {2.5, 4.3, 3.0};
  mycell->iwrap(delta);
  EXPECT_EQ(0.5, delta[0]);
  EXPECT_EQ(4.3, delta[1]);
  EXPECT_EQ(3.0, delta[2]);
}


TEST_F(CellTest2, iwrap_example) {
  double delta[3] = {2.0, 5.3, 3.0};
  mycell->iwrap(delta);
  EXPECT_EQ(0.0, delta[0]);
  EXPECT_EQ(5.3, delta[1]);
  EXPECT_EQ(-1.0, delta[2]);
}


TEST_F(CellTest3, iwrap_example) {
  double delta[3] = {2.0, 0.3, 3.0};
  mycell->iwrap(delta);
  EXPECT_EQ(0.0, delta[0]);
  EXPECT_EQ(0.3, delta[1]);
  EXPECT_EQ(-1.0, delta[2]);
}


TEST_F(CellTest1, iwrap_edges) {
  double delta[3] = {-1.0, -0.5, -2.0};
  mycell->iwrap(delta);
  EXPECT_EQ(1.0, delta[0]);
  EXPECT_EQ(-0.5, delta[1]);
  EXPECT_EQ(-2.0, delta[2]);
}


TEST_F(CellTest2, iwrap_edges) {
  double delta[3] = {-1.0, -0.5, -2.0};
  mycell->iwrap(delta);
  EXPECT_EQ(1.0, delta[0]);
  EXPECT_EQ(-0.5, delta[1]);
  EXPECT_EQ(2.0, delta[2]);
}


TEST_F(CellTest3, iwrap_edges) {
  double delta[3] = {-1.0, -0.5, -2.0};
  mycell->iwrap(delta);
  EXPECT_EQ(1.0, delta[0]);
  EXPECT_EQ(0.5, delta[1]);
  EXPECT_EQ(2.0, delta[2]);
}


TEST_P(CellTestP, iwrap_random) {
  int num_wrapped = 0;
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    double delta[3];
    fill_random_double(irep + NREP, delta, 3, -6.0, 6.0);
    double frac[3];

    // For test sufficiency check
    cell->to_frac(delta, frac);
    for (int ivec=0; ivec < nvec; ++ivec) {
      if (fabs(frac[ivec] > 0.5)) ++num_wrapped;
    }

    // Actual test
    cell->iwrap(delta);
    cell->to_frac(delta, frac);
    for (int ivec=0; ivec < nvec; ++ivec) {
      EXPECT_LT(frac[ivec], 0.5);
      EXPECT_GE(frac[ivec], -0.5);
    }
  }
  // Check whether the test is sufficient.
  EXPECT_LT((NREP*nvec)/3, num_wrapped);
}


TEST_P(CellTestP, iwrap_consistency) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    int coeffs[3];
    fill_random_int(irep, coeffs, nvec, -5, 5);
    double frac[3];
    double cart1[3];
    double cart2[3];
    fill_random_double(irep, frac, 3);
    cell->to_cart(frac, cart1);
    cell->to_cart(frac, cart2);
    cell->iadd_vec(cart2, coeffs);
    cell->iwrap(cart2);
    EXPECT_NEAR(cart2[0], cart1[0], 1e-10);
    EXPECT_NEAR(cart2[1], cart1[1], 1e-10);
    EXPECT_NEAR(cart2[2], cart1[2], 1e-10);
  }
}


// to_frac and to_cart
// ~~~~~~~~~~~~~~~~~~~~~

TEST_F(CellTest1, to_frac_example) {
  double rcart[3] = {2.5, 4.3, 3.0};
  double rfrac[3];
  mycell->to_frac(rcart, rfrac);
  EXPECT_NEAR(1.25, rfrac[0], 1e-10);
  EXPECT_NEAR(4.3, rfrac[1], 1e-10);
  EXPECT_NEAR(3.0, rfrac[2], 1e-10);
}


TEST_F(CellTest2, to_frac_example) {
  double rcart[3] = {2.5, 4.3, 3.0};
  double rfrac[3];
  mycell->to_frac(rcart, rfrac);
  EXPECT_NEAR(1.25, rfrac[0], 1e-10);
  EXPECT_NEAR(0.75, rfrac[1], 1e-10);
  EXPECT_NEAR(-4.3, rfrac[2], 1e-10);
}


TEST_F(CellTest3, to_frac_example) {
  double rcart[3] = {2.5, 4.3, 3.0};
  double rfrac[3];
  mycell->to_frac(rcart, rfrac);
  EXPECT_NEAR(1.25, rfrac[0], 1e-10);
  EXPECT_NEAR(4.3, rfrac[1], 1e-10);
  EXPECT_NEAR(0.75, rfrac[2], 1e-10);
}


TEST_F(CellTest1, to_cart_example) {
  double rfrac[3] = {0.5, 0.2, -1.5};
  double rcart[3];
  mycell->to_cart(rfrac, rcart);
  EXPECT_NEAR(1.0, rcart[0], 1e-10);
  EXPECT_NEAR(0.2, rcart[1], 1e-10);
  EXPECT_NEAR(-1.5, rcart[2], 1e-10);
}


TEST_F(CellTest2, to_cart_example) {
  double rfrac[3] = {0.5, 0.2, -1.5};
  double rcart[3];
  mycell->to_cart(rfrac, rcart);
  EXPECT_NEAR(1.0, rcart[0], 1e-10);
  EXPECT_NEAR(1.5, rcart[1], 1e-10);
  EXPECT_NEAR(0.8, rcart[2], 1e-10);
}


TEST_F(CellTest3, to_cart_example) {
  double rfrac[3] = {0.5, 0.2, -1.5};
  double rcart[3];
  mycell->to_cart(rfrac, rcart);
  EXPECT_NEAR(1.0, rcart[0], 1e-10);
  EXPECT_NEAR(0.2, rcart[1], 1e-10);
  EXPECT_NEAR(-6.0, rcart[2], 1e-10);
}


TEST_P(CellTestP, to_cart_to_frac_consistency) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    double rfrac[3];
    double rcart1[3];
    double rcart2[3];
    fill_random_double(irep, rcart1, 3, -5.0, 5.0);
    cell->to_frac(rcart1, rfrac);
    cell->to_cart(rfrac, rcart2);
    EXPECT_NEAR(rcart2[0], rcart1[0], 1e-10);
    EXPECT_NEAR(rcart2[1], rcart1[1], 1e-10);
    EXPECT_NEAR(rcart2[2], rcart1[2], 1e-10);
  }
}


// iadd_vec
// ~~~~~~~~

TEST_P(CellTestP, iadd_vec_consistency) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    int coeffs[3];
    fill_random_int(irep, coeffs, 3, -5, 5);
    double cart1[3];
    double cart2[3];
    double frac1[3];
    double frac2[3];
    fill_random_double(irep, cart1, 3, -10.0, 10.0);
    cart2[0] = cart1[0];
    cart2[1] = cart1[1];
    cart2[2] = cart1[2];
    cell->iadd_vec(cart2, coeffs);
    cell->to_frac(cart1, frac1);
    cell->to_frac(cart2, frac2);
    for (int ivec=0; ivec < nvec; ++ivec) {
      EXPECT_NEAR(coeffs[ivec], frac2[ivec] - frac1[ivec], 1e-10);
    }
    for (int ivec=nvec; ivec < 3; ++ivec) {
      EXPECT_NEAR(0.0, frac2[ivec] - frac1[ivec], 1e-10);
    }
  }
}


// The accessors
// ~~~~~~~~~~~~~

// nvec() is already tested above

TEST_P(CellTestP, vec_vecs_gvecs) {
  double vecs[9];
  std::unique_ptr<cl::Cell> cell;
  while (true) {
    try {
      fill_random_double(1487, vecs, 9, -2.0, 2.0);
      cell.reset(new cl::Cell(vecs, nvec));
      break;
    } catch (cl::singular_cell_vectors) {}
  }
  for (int ivec=0; ivec < nvec; ++ivec) {
    EXPECT_EQ(vecs[3*ivec + 0], cell->vec(ivec)[0]);
    EXPECT_EQ(vecs[3*ivec + 1], cell->vec(ivec)[1]);
    EXPECT_EQ(vecs[3*ivec + 2], cell->vec(ivec)[2]);
    EXPECT_EQ(vecs[3*ivec + 0], cell->vecs()[3*ivec + 0]);
    EXPECT_EQ(vecs[3*ivec + 1], cell->vecs()[3*ivec + 1]);
    EXPECT_EQ(vecs[3*ivec + 2], cell->vecs()[3*ivec + 2]);
    EXPECT_EQ(cell->gvec(ivec)[0], cell->gvecs()[3*ivec + 0]);
    EXPECT_EQ(cell->gvec(ivec)[1], cell->gvecs()[3*ivec + 1]);
    EXPECT_EQ(cell->gvec(ivec)[2], cell->gvecs()[3*ivec + 2]);
  }
}


TEST_P(CellTestP, domain) {
  std::unique_ptr<cl::Cell> cell(create_random_cell(1));
  EXPECT_THROW(cell->vec(-1), std::domain_error);
  EXPECT_THROW(cell->vec(3), std::domain_error);
  EXPECT_THROW(cell->gvec(-1), std::domain_error);
  EXPECT_THROW(cell->gvec(3), std::domain_error);
}


TEST_F(CellTest1, example) {
  EXPECT_NEAR(0.5, mycell->gvec(0)[0], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(0)[1], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(0)[2], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(1)[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->gvec(1)[1], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(1)[2], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(2)[0], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(2)[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->gvec(2)[2], 1e-10);
  EXPECT_NEAR(2.0, mycell->volume(), 1e-10);
  EXPECT_NEAR(2.0, mycell->lengths()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->lengths()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->lengths()[2], 1e-10);
  EXPECT_NEAR(2.0, mycell->spacings()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->spacings()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->spacings()[2], 1e-10);
  EXPECT_NEAR(0.5, mycell->glengths()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->glengths()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->glengths()[2], 1e-10);
  EXPECT_NEAR(0.5, mycell->gspacings()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->gspacings()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->gspacings()[2], 1e-10);
}


TEST_F(CellTest2, example) {
  EXPECT_NEAR(0.5, mycell->gvec(0)[0], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(0)[1], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(0)[2], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(1)[0], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(1)[1], 1e-10);
  EXPECT_NEAR(0.25, mycell->gvec(1)[2], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(2)[0], 1e-10);
  EXPECT_NEAR(-1.0, mycell->gvec(2)[1], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(2)[2], 1e-10);
  EXPECT_NEAR(8.0, mycell->volume(), 1e-10);
  EXPECT_NEAR(2.0, mycell->lengths()[0], 1e-10);
  EXPECT_NEAR(4.0, mycell->lengths()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->lengths()[2], 1e-10);
  EXPECT_NEAR(2.0, mycell->spacings()[0], 1e-10);
  EXPECT_NEAR(4.0, mycell->spacings()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->spacings()[2], 1e-10);
  EXPECT_NEAR(0.5, mycell->glengths()[0], 1e-10);
  EXPECT_NEAR(0.25, mycell->glengths()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->glengths()[2], 1e-10);
  EXPECT_NEAR(0.5, mycell->gspacings()[0], 1e-10);
  EXPECT_NEAR(0.25, mycell->gspacings()[1], 1e-10);
  EXPECT_NEAR(1.0, mycell->gspacings()[2], 1e-10);
}


TEST_F(CellTest3, example) {
  EXPECT_NEAR(0.5, mycell->gvec(0)[0], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(0)[1], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(0)[2], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(1)[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->gvec(1)[1], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(1)[2], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(2)[0], 1e-10);
  EXPECT_NEAR(0.0, mycell->gvec(2)[1], 1e-10);
  EXPECT_NEAR(0.25, mycell->gvec(2)[2], 1e-10);
  EXPECT_NEAR(8.0, mycell->volume(), 1e-10);
  EXPECT_NEAR(2.0, mycell->lengths()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->lengths()[1], 1e-10);
  EXPECT_NEAR(4.0, mycell->lengths()[2], 1e-10);
  EXPECT_NEAR(2.0, mycell->spacings()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->spacings()[1], 1e-10);
  EXPECT_NEAR(4.0, mycell->spacings()[2], 1e-10);
  EXPECT_NEAR(0.5, mycell->glengths()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->glengths()[1], 1e-10);
  EXPECT_NEAR(0.25, mycell->glengths()[2], 1e-10);
  EXPECT_NEAR(0.5, mycell->gspacings()[0], 1e-10);
  EXPECT_NEAR(1.0, mycell->gspacings()[1], 1e-10);
  EXPECT_NEAR(0.25, mycell->gspacings()[2], 1e-10);
}


TEST_P(CellTestP, signed_volume) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    double sv = vec3::triple(cell->vec(0), cell->vec(1), cell->vec(2));
    EXPECT_NEAR(cell->volume(), fabs(sv), 1e-10);
    if (nvec < 3) EXPECT_LT(0.0, sv);
  }
}

// cubic and cuboid
// ~~~~~~~~~~~~~~~~

TEST_P(CellTestP, cubic_cuboid_random1) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    EXPECT_FALSE(cell->cubic());
    EXPECT_FALSE(cell->cuboid());
  }
}


TEST_P(CellTestP, cubic_cuboid_random2) {
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep, 1.0, true));
    if (nvec == 1) {
      EXPECT_TRUE(cell->cubic());
    } else {
      EXPECT_FALSE(cell->cubic());
    }
    EXPECT_TRUE(cell->cuboid());
  }
}


TEST_F(CellTest1, cubic_cuboid_example) {
  EXPECT_TRUE(mycell->cubic());
  EXPECT_TRUE(mycell->cuboid());
}


TEST_F(CellTest2, cubic_cuboid_example) {
  EXPECT_FALSE(mycell->cubic());
  EXPECT_FALSE(mycell->cuboid());
}


TEST_F(CellTest3, cubic_cuboid_example) {
  EXPECT_FALSE(mycell->cubic());
  EXPECT_TRUE(mycell->cuboid());
}


// ranges_cutoff
// ~~~~~~~~~~~~~~~~~~~~

TEST_F(CellTest1, ranges_cutoff_example) {
  double center[3] = {6.3, 0.2, -0.8};
  int ranges_begin[1];
  int ranges_end[1];
  int ncell = 0;
  ncell = mycell->ranges_cutoff(center, 1.0, ranges_begin, ranges_end);
  EXPECT_EQ(2, ncell);
  EXPECT_EQ(2, ranges_begin[0]);
  EXPECT_EQ(4, ranges_end[0]);
  ncell = mycell->ranges_cutoff(center, 2.0, ranges_begin, ranges_end);
  EXPECT_EQ(3, ncell);
  EXPECT_EQ(2, ranges_begin[0]);
  EXPECT_EQ(5, ranges_end[0]);
  ncell = mycell->ranges_cutoff(center, 3.0, ranges_begin, ranges_end);
  EXPECT_EQ(4, ncell);
  EXPECT_EQ(1, ranges_begin[0]);
  EXPECT_EQ(5, ranges_end[0]);
}


TEST_F(CellTest1, ranges_cutoff_edge) {
  double center[3] = {2.0, 0.2, -0.8};
  int ranges_begin[1];
  int ranges_end[1];
  int ncell = 0;
  ncell = mycell->ranges_cutoff(center, 1.0, ranges_begin, ranges_end);
  EXPECT_EQ(2, ncell);
  EXPECT_EQ(0, ranges_begin[0]);
  EXPECT_EQ(2, ranges_end[0]);
  ncell = mycell->ranges_cutoff(center, 2.0, ranges_begin, ranges_end);
  EXPECT_EQ(2, ncell);
  EXPECT_EQ(0, ranges_begin[0]);
  EXPECT_EQ(2, ranges_end[0]);
  ncell = mycell->ranges_cutoff(center, 3.0, ranges_begin, ranges_end);
  EXPECT_EQ(4, ncell);
  EXPECT_EQ(-1, ranges_begin[0]);
  EXPECT_EQ(3, ranges_end[0]);
}


TEST_F(CellTest2, ranges_cutoff_example) {
  double center[3] = {6.3, 0.2, -5.0};
  int ranges_begin[2];
  int ranges_end[2];
  int ncell = 0;
  ncell = mycell->ranges_cutoff(center, 1.1, ranges_begin, ranges_end);
  EXPECT_EQ(2*2, ncell);
  EXPECT_EQ(2, ranges_begin[0]);
  EXPECT_EQ(-2, ranges_begin[1]);
  EXPECT_EQ(4, ranges_end[0]);
  EXPECT_EQ(0, ranges_end[1]);
}


TEST_F(CellTest2, ranges_cutoff_edge) {
  double center[3] = {4.0, 0.2, -2.0};
  int ranges_begin[2];
  int ranges_end[2];
  int ncell = 0;
  ncell = mycell->ranges_cutoff(center, 2.0, ranges_begin, ranges_end);
  EXPECT_EQ(2, ncell);
  EXPECT_EQ(1, ranges_begin[0]);
  EXPECT_EQ(-1, ranges_begin[1]);
  EXPECT_EQ(3, ranges_end[0]);
  EXPECT_EQ(0, ranges_end[1]);
}


TEST_F(CellTest3, ranges_cutoff_example) {
  double center[3] = {6.3, 2.2, -5.8};
  int ranges_begin[3];
  int ranges_end[3];
  int ncell = 0;
  ncell = mycell->ranges_cutoff(center, 1.0, ranges_begin, ranges_end);
  EXPECT_EQ(2*3*1, ncell);
  EXPECT_EQ(2, ranges_begin[0]);
  EXPECT_EQ(1, ranges_begin[1]);
  EXPECT_EQ(-2, ranges_begin[2]);
  EXPECT_EQ(4, ranges_end[0]);
  EXPECT_EQ(4, ranges_end[1]);
  EXPECT_EQ(-1, ranges_end[2]);
}


TEST_F(CellTest3, ranges_cutoff_edge) {
  double center[3] = {10.0, -2.0, -6.0};
  int ranges_begin[3];
  int ranges_end[3];
  int ncell = 0;
  ncell = mycell->ranges_cutoff(center, 2.0, ranges_begin, ranges_end);
  EXPECT_EQ(2*4*1, ncell);
  EXPECT_EQ(4, ranges_begin[0]);
  EXPECT_EQ(-4, ranges_begin[1]);
  EXPECT_EQ(-2, ranges_begin[2]);
  EXPECT_EQ(6, ranges_end[0]);
  EXPECT_EQ(0, ranges_end[1]);
  EXPECT_EQ(-1, ranges_end[2]);
}


TEST_P(CellTestP, ranges_cutoff_domain) {
  double center[3] = {6.3, 2.2, -5.8};
  int ranges_begin[3];
  int ranges_end[3];
  EXPECT_THROW(mycell->ranges_cutoff(center, -1.0, ranges_begin, ranges_end), std::domain_error);
  EXPECT_THROW(mycell->ranges_cutoff(center, 0.0, ranges_begin, ranges_end), std::domain_error);
}


TEST_P(CellTestP, ranges_cutoff_random) {
  int npoint_total = 0;
  for (int irep = 0; irep < NREP; ++irep) {
    std::unique_ptr<cl::Cell> cell(create_random_cell(irep));
    double center[3];
    int ranges_begin[3];
    int ranges_end[3];
    double cutoff = 0.3*(irep + 1);
    fill_random_double(irep + 2, center, 3, -5.0, 5.0);
    cell->ranges_cutoff(center, cutoff, ranges_begin, ranges_end);
    for (int ipoint=0; ipoint < NPOINT; ++ipoint) {
      double point[3];
      double norm;
      random_point(ipoint + irep*NPOINT, center, cutoff, point, &norm);
      if (norm <= cutoff) {
        double frac[3];
        cell->to_frac(point, frac);
        for (int ivec=0; ivec < nvec; ++ivec) {
          EXPECT_LE(ranges_begin[ivec], frac[ivec]);
          EXPECT_GE(ranges_end[ivec], frac[ivec]);
        }
        ++npoint_total;
      }
    }
  }
  // Check sufficiency
  EXPECT_LT((NREP*NPOINT)/3, npoint_total);
}


// bars_cutoff
// ~~~~~~~~~~~

TEST_P(CellTestP, bars_cutoff_domain) {
  double center[3] = {2.5, 3.4, -0.6};
  int shape[3] = {10, 10, 10};
  bool pbc[3] = {true, true, true};
  std::vector<int> bars;
  EXPECT_THROW(mycell->bars_cutoff(center, 0.0, shape, pbc, &bars), std::domain_error);
  EXPECT_THROW(mycell->bars_cutoff(center, -1.0, shape, pbc, &bars), std::domain_error);
  cl::Cell zero_cell(nullptr, 0);
  EXPECT_THROW(zero_cell.bars_cutoff(center, 1.0, shape, pbc, &bars), std::domain_error);
}


TEST_F(CellTest1, bars_cutoff_example) {
  // All the parameters
  double cutoff = 5.0;
  double center[3] = {2.5, 3.4, -0.6};
  int shape[1] = {10};
  bool pbc[1] = {true};

  // Call
  std::vector<int> bars;
  size_t nbar = mycell->bars_cutoff(center, cutoff, shape, pbc, &bars);
  EXPECT_EQ(1, nbar);
  EXPECT_EQ(2, bars.size());

  // Check results
  // lower end: -2.5 (-2 #> 8)
  // upper end:  7.5 (8 #> 4) non-inclusive
  EXPECT_EQ(-2, bars[0]);
  EXPECT_EQ(4, bars[1]);
}


TEST_F(CellTest2, bars_cutoff_example) {
  // All the parameters
  double cutoff = 5.0;
  double center[3] = {2.5, 3.4, -0.6};
  int shape[2] = {10, 5};
  bool pbc[2] = {true, false};

  // Call
  std::vector<int> bars;
  size_t nbar = mycell->bars_cutoff(center, cutoff, shape, pbc, &bars);
  EXPECT_EQ(6, nbar);
  EXPECT_EQ(6*3, bars.size());

  // Test
  for (size_t ibar = 0; ibar < nbar; ++ibar) {
    EXPECT_EQ(ibar - 2, bars[3*ibar]);
    EXPECT_EQ(0, bars[3*ibar + 1]);
    if ((ibar == 0) || (ibar == 1) || (ibar == 5)) {
      EXPECT_EQ(1, bars[3*ibar + 2]);
    } else {
      EXPECT_EQ(2, bars[3*ibar + 2]);
    }
  }
}


TEST_F(CellTest3, bars_cutoff_example) {
  // All the parameters
  double cutoff = 1.9;
  double center[3] = {2.0, 2.0, 2.0};
  int shape[3] = {10, 5, 7};
  bool pbc[3] = {true, true, true};

  // Call
  std::vector<int> bars;
  size_t nbar = mycell->bars_cutoff(center, cutoff, shape, pbc, &bars);
  EXPECT_EQ(8, nbar);
  EXPECT_EQ(8*4, bars.size());

  // Test
  for (size_t ibar = 0; ibar < nbar; ++ibar) {
    std::vector<int> bar(&bars[ibar*4], &bars[(ibar + 1)*4]);
    EXPECT_EQ(bar[0], ibar/4);
    EXPECT_EQ(bar[1], ibar%4);
    EXPECT_EQ(bar[2], 0);
    EXPECT_EQ(bar[3], 1);
  }
}


TEST_P(CellTestP, bars_cutoff_random) {
  size_t nbar_total = 0;
  for (int irep = 0; irep < NREP; ++irep) {
    // Test parameters:
    // - Random cell
    std::unique_ptr<cl::Cell> cell(create_random_cell(2*irep));
    // - Increasing cutoff
    double cutoff = (irep + 1)*0.1;
    // - Random center
    double center[3];
    fill_random_double(47332 + irep, center, 3, -1.0, 1.0);
    // - Alternating values for shape and pbc
    int shape[3];
    bool pbc[3];
    for (int ivec=0; ivec < 3; ++ivec) {
      shape[ivec] = ((irep*(ivec + 1)) % 5) + 1;
      pbc[ivec] = (irep >> ivec) % 2;
    }

    // Compute the bars.
    std::vector<int> bars;
    size_t nbar = cell->bars_cutoff(center, cutoff, shape, pbc, &bars);
    EXPECT_EQ(nbar*(nvec + 1), bars.size());
    nbar_total += nbar;

    // Construct a random vector in a cubic box around the cutoff sphere.
    double cart[3];
    fill_random_double(123 + irep, cart, 3, -cutoff*1.1, cutoff*1.1);
    double norm = vec3::norm(cart);
    // Center of the box must coincide with center of the sphere.
    cart[0] += center[0];
    cart[1] += center[1];
    cart[2] += center[2];
    // For the rest of the test, we need this random vector in fractional coordinates.
    double frac[3];
    cell->to_frac(cart, frac);

    // Does the fractional coordinate fit in one of the bars?
    int index[3] = {
      static_cast<int>(floor(frac[0])),
      static_cast<int>(floor(frac[1])),
      static_cast<int>(floor(frac[2]))
    };
    bool in_bar = false;
    for (size_t ibar=0; ibar < nbar; ++ibar) {
      std::vector<int> bar(&bars[ibar*(nvec + 1)], &bars[(ibar + 1)*(nvec + 1)]);

      if (nvec > 1) {
        if (bar[0] != index[0])
          continue;
      }
      if (nvec > 2) {
        if (bar[1] != index[1])
          continue;
      }
      if (index[nvec - 1] < bar[nvec - 1])
        continue;
      if (index[nvec - 1] >= bar[nvec])
        continue;
      in_bar = true;
      break;
    }

    // Does the relative vector sit in the cutoff sphere, taking into account
    // non-periodic boundaries that truncate the cutoff sphere.
    bool in_sphere = (norm < cutoff);
    if (in_sphere) {
      for (int ivec=0; ivec < nvec; ++ivec) {
        if (!pbc[ivec]) {
          if (frac[ivec] < 0) {
            in_sphere = false;
            break;
          }
          if (frac[ivec] >= shape[ivec]) {
            in_sphere = false;
            break;
          }
        }
      }
    }

    if (in_sphere) {
      // First test: if the vector is in the cutoff and non-periodic boundaries, the
      //             point must be in a bar.
      EXPECT_TRUE(in_bar);
    } else if (!in_bar) {
      // Second test: if not in a bar, the norm must be larger than the cutoff,
      //              or the point is outside a non-periodic boundary.
      EXPECT_FALSE(in_sphere);
    }
    // Clean up
  }
  // Sufficiency check
  EXPECT_LE(NREP*((nvec - 1)*3 + 1), nbar_total);
}


TEST_P(CellTestP, bars_cutoff_corners) {
  size_t nbar_total = 0;
  for (int irep = 0; irep < NREP; ++irep) {
    // Test parameters:
    // - Random cell
    std::unique_ptr<cl::Cell> cell(create_random_cell(2*irep));
    // - Increasing cutoff
    double cutoff = (irep + 1)*0.1;
    // - Random center
    double center[3];
    fill_random_double(47332 + irep, center, 3, -2.0, 2.0);
    // - Alternating values for shape and pbc
    int shape[3];
    bool pbc[3];
    for (int ivec=0; ivec < 3; ++ivec) {
      shape[ivec] = ((irep*(ivec + 1)) % 5) + 1;
      pbc[ivec] = true;
    }

    // Compute the bars.
    std::vector<int> bars;
    size_t nbar = cell->bars_cutoff(center, cutoff, shape, pbc, &bars);
    EXPECT_EQ(nbar*(nvec + 1), bars.size());
    nbar_total += nbar;

    // Test if the corners of each bar fall outside of the sphere
    for (size_t ibar=0; ibar < nbar; ++ibar) {
      std::vector<int> bar(&bars[ibar*(nvec + 1)], &bars[(ibar + 1)*(nvec + 1)]);

      double frac_corner[3] = {0, 0, 0};
      double cart_corner[3] = {0, 0, 0};
      double dist;

      // loop of begin and end of the bar (last two integers in the bar).
      cell->to_frac(center, frac_corner);
      for (int ilast = 0; ilast < 2; ++ilast) {
        frac_corner[nvec-1] = bar[nvec-ilast];
        if (nvec == 1) {
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
        } else if (nvec == 2) {
          //
          frac_corner[0] = bar[0];
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
          //
          frac_corner[0] = bar[0] + 1;
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
        } else if (nvec == 3) {
          //
          frac_corner[0] = bar[0];
          frac_corner[1] = bar[1];
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
          //
          frac_corner[0] = bar[0] + 1;
          frac_corner[1] = bar[1];
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
          //
          frac_corner[0] = bar[0];
          frac_corner[1] = bar[1] + 1;
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
          //
          frac_corner[0] = bar[0] + 1;
          frac_corner[1] = bar[1] + 1;
          cell->to_cart(frac_corner, cart_corner);
          dist = vec3::distance(cart_corner, center);
          EXPECT_GT(dist, cutoff);
        }
      }
    }
  }
  // Sufficiency check
  EXPECT_LE(NREP*((nvec - 1)*3 + 1), nbar_total);
}


// Instantiation of parameterized tests
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

INSTANTIATE_TEST_CASE_P(CellTest123, CellTestP, ::testing::Range(1, 4));


// smart_wrap
// ~~~~~~~~~~

TEST(SmartWrap, examples) {
  EXPECT_EQ(0, cl::smart_wrap(-15, 5, true));
  EXPECT_EQ(0, cl::smart_wrap(-5, 5, true));
  EXPECT_EQ(2, cl::smart_wrap(-3, 5, true));
  EXPECT_EQ(4, cl::smart_wrap(-1, 5, true));
  EXPECT_EQ(0, cl::smart_wrap(0, 5, true));
  EXPECT_EQ(3, cl::smart_wrap(3, 5, true));
  EXPECT_EQ(0, cl::smart_wrap(5, 5, true));
  EXPECT_EQ(1, cl::smart_wrap(6, 5, true));
  EXPECT_EQ(0, cl::smart_wrap(10, 5, true));
  EXPECT_EQ(2, cl::smart_wrap(12, 5, true));
  EXPECT_EQ(-1, cl::smart_wrap(-15, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(-5, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(-3, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(-1, 5, false));
  EXPECT_EQ(0, cl::smart_wrap(0, 5, false));
  EXPECT_EQ(3, cl::smart_wrap(3, 5, false));
  EXPECT_EQ(4, cl::smart_wrap(4, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(5, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(6, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(10, 5, false));
  EXPECT_EQ(-1, cl::smart_wrap(12, 5, false));
}

// vim: textwidth=90 et ts=2 sw=2
