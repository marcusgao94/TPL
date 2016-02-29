#include "tpl_gy_algorithm.h"

#include <cassert>
#include <fstream>
#include <iterator>

namespace tpl {
    using namespace std;

    TplGYNetForceModel::~TplGYNetForceModel()
    {
    }

    TplGYMoveForceModel::TplGYMoveForceModel(double r1, double r2, unsigned int grid_size)
            :TplStandardMoveForceModel(r1, r2, grid_size)
    {
    }

    TplGYMoveForceModel::~TplGYMoveForceModel()
    {
    }

    TplGYAlgorithm::~TplGYAlgorithm()
    {
    }

    void TplGYAlgorithm::initialize_models()
    {
        net_force_model  = new TplGYNetForceModel;
        move_force_model = new TplGYMoveForceModel(0.1, 0.2, 100000);
    }

}

