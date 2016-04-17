/*!
 * \file tpl_standard_net_model.h
 * \author Peng Fei
 * \brief Standard implementation for Net Model.
 */

#ifndef TPL_STANDARD_NET_MODEL_H
#define TPL_STANDARD_NET_MODEL_H

#include "tpl_abstract_net_model.h"

namespace tpl {

    class TplStandardNetModel : public TplAbstractNetModel {
    public:
        //! Default constructor.
        TplStandardNetModel() : TplAbstractNetModel() {}

        //! Virtual destructor.
        virtual ~TplStandardNetModel() {}

        //! Standard implementation for interface compute_net_weight.
        /*!
         * \param NWx The net weight in x directions to be computed.
         * \param NWy The net weight in y directions to be computed.
         */
        virtual void compute_net_weight(NetWeight &NWx, NetWeight &NWy);
    };

}//namespace tpl

#endif //TPL_STANDARD_NET_MODEL_H
