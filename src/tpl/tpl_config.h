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
#include <map>

#include <boost/variant.hpp>

namespace tpl {

    class TplConfig {
    public:
        //!< Singleton pattern for TplConfig.
        static TplConfig &instance();

        //!< Load configuration file into TplConfig singleton.
        bool load_configuration(const std::string &configfile);

        const boost::variant<int, double> &operator[](const std::string &key);

    private:
        std::map<std::string, boost::variant<int, double> > _data;

        TplConfig()  = default;
        ~TplConfig() = default;
    };
}

#endif //TPL_TPLCONFIG_H
