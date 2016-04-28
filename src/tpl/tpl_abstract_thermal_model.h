/*!
 * \file tpl_abstract_thermal_model.h
 * \author Peng Fei
 * \brief Interface definition for Thermal Model. 
 */

#ifndef TPL_ABSTRACT_THERMAL_MODEL_H
#define TPL_ABSTRACT_THERMAL_MODEL_H

#include <memory>
//#include <stxxl/bits/mng/config.h>
#include <stxxl/bits/containers/matrix.h>

namespace tpl {
    /*!
     * \typedef stxxl::matrix<double, 64> TMat;
     * \brief SpMat type
     */
    typedef stxxl::matrix<double, 64> TMat;

    class TplAbstractThermalModel {
    public:
        //! Default constructor.
        TplAbstractThermalModel() {
//            stxxl::config * cfg = stxxl::config::get_instance();
//            stxxl::disk_config disk("/tmp/stxxl.tmp", 100LL * 1024 * 1024 * 1024, "syscall unlink");
//            disk.direct = stxxl::disk_config::DIRECT_ON; // force O_DIRECT
//            cfg->add_disk(disk);
        }

        //!< Pure virtual destructor.
        virtual ~TplAbstractThermalModel() {}

        virtual void compute_thermal_distribution( std::shared_ptr<TMat> distribution) = 0;
    };
}

#endif //TPL_ABSTRACT_THERMAL_MODEL_H
