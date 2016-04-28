#include "tpl_standard_algorithm.h"

#include "tpl_db.h"
#include "tpl_standard_net_model.h"
#include "tpl_standard_net_force_model.h"
#include "tpl_standard_thermal_model.h"
#include "tpl_standard_thermal_force_model.h"

namespace tpl {
    using namespace std;

    void TplStandardAlgorithm::initialize_models()
    {
        _net_model           = shared_ptr<TplAbstractNetModel>(         new TplStandardNetModel);
        _net_force_model     = shared_ptr<TplAbstractNetForceModel>(    new TplStandardNetForceModel);
        _thermal_model       = shared_ptr<TplAbstractThermalModel>(     new TplStandardThermalModel);
        _thermal_force_model = shared_ptr<TplAbstractThermalForceModel>(new TplStandardThermalForceModel(_thermal_model));
    }

    void TplStandardAlgorithm::make_initial_placement()
    {
        vector<double> x_target, y_target;
        NetWeight NWx, NWy;

        _net_model->compute_net_weight(NWx, NWy);

        for(size_t i=0; i<5; ++i) {
            _net_force_model->compute_net_force_target(NWx, NWy, x_target, y_target);
            TplDB::db().modules.set_free_module_coordinates(x_target, y_target);

            x_target.clear();
            x_target.reserve(TplDB::db().modules.num_free());

            y_target.clear();
            y_target.reserve(TplDB::db().modules.num_free());
        }
    }

    void TplStandardAlgorithm::make_global_placement()
    {

    }

    void TplStandardAlgorithm::initialize_move_force_matrix()
    {
        vector<SpElem> coefficients;
        unsigned int num_free = TplDB::db().modules.num_free();
        vector<double> module_power;

        double avg_power = 0;
        double power;
        for(TplModules::iterator it=TplDB::db().modules.begin(); it!=TplDB::db().modules.end(); ++it) {
            power = it->width * it->height * it->power_density;
            avg_power +=  power;

            if(!it->fixed) module_power.push_back(power);
        }
        avg_power /= TplDB::db().modules.size();

        for(size_t i=0; i<module_power.size(); ++i) {
            coefficients.push_back(SpElem(i, i, module_power[i]/avg_power/num_free));
        }

        Cx0.setFromTriplets(coefficients.begin(), coefficients.end());
        Cy0.setFromTriplets(coefficients.begin(), coefficients.end());
    }

    void TplStandardAlgorithm::update_move_force_matrix()
    {

    }

}//end namespace tpl

