/*!
 * \file tpl_config.h
 * \author Peng Fei
 * \brief Definition for configuration file parsing class.
 *
 * class TplConfig is implemented in singleton design pattern using c++11 static local variable initialization scheme,
 * which is thread safe automatically.
 */

#ifndef TPL_CONFIG_H
#define TPL_CONFIG_H

#include <string>

namespace tpl {

    class TplConfig {
    public:
        //!< Singleton pattern for TplConfig.
        static TplConfig &instance();

        //!< Load configuration file into TplConfig singleton.
        bool load_configuration(const std::string &configfile);

        int init_grid_size;
        double r1;
        double r2;
        int mu;

    private:

        TplConfig()  = default;
        ~TplConfig() = default;
    };
}

#endif //TPL_TPLCONFIG_H
