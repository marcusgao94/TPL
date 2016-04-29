#include "tpl_config.h"

#include <boost/property_tree/json_parser.hpp>

namespace tpl {
    using std::string;

    TplConfig &TplConfig::instance()
    {
        static TplConfig _instance;
        return _instance;
    }

    bool TplConfig::load_configuration(const std::string &configfile)
    {
        try {
            namespace pt = boost::property_tree;
            pt::ptree tree;
            pt::read_json(configfile, tree);

            init_grid_size = tree.get<int>("init_grid_size");
            r1             = tree.get<double>("r1");
            r2             = tree.get<double>("r2");
            mu             = tree.get<int>("mu");



            return true;

        } catch(...) {
            return false;
        }
    }
}
