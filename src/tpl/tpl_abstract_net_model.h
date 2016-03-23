/*!
 * \file tpl_abstract_net_model.h
 * \author Peng Fei
 * \brief Interface definition for Net Model.
 */

#ifndef TPL_ABSTRACT_NET_MODEL_H
#define TPL_ABSTRACT_NET_MODEL_H

#include "tpl_db.h"
#include <map>
#include <utility>

namespace tpl {


    /*!
     * \typedef std::map<std::pair<TplPin*, TplPin*>, double> NetWeight;
     * \brief NetWeight type
     */
    using NetWeight = std::map<std::pair<TplPin*, TplPin*>, double>;

    //! Interface definition for tpl net model.
    class TplAbstractNetModel {
    public:
        //! Default constructor.
        TplAbstractNetModel() = default;

        //! Pure virtual destructor.
        virtual ~TplAbstractNetModel() {}

        //! Interface for computing a net's net weight.
        /*!
         * \param NWx The net weight in x directions to be computed.
         * \param NWy The net weight in y directions to be computed.
         */
        virtual void compute_net_weight(NetWeight &NWx, NetWeight &NWy) = 0;
    };

    /*
    TplAbstractNetModel::~TplAbstractNetModel()
    {
    }
     */

}//namespace tpl

#endif //TPL_ABSTRACT_NET_MODEL_H
