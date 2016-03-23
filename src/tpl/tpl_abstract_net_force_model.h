/*!
 * \file tpl_abstract_net_force_model.h
 * \author Peng Fei
 * \brief Interface definition for Net Force Model.
 */

#ifndef TPL_ABSTRACT_NET_FORCE_MODEL_H
#define TPL_ABSTRACT_NET_FORCE_MODEL_H

#include <memory>

#include "utils.h"
#include "tpl_abstract_net_model.h"

namespace tpl {

    //! Interface definition for tpl net force model.
    class TplAbstractNetForceModel {
    public:
        //! Default constructor.
        TplAbstractNetForceModel() = default;

        //! Pure virtual destructor.
        virtual ~TplAbstractNetForceModel() {};

        //! Interface for computing all the free modules' net force matrix.
        /*!
         * \param NWx Net weight int x direction.
         * \param NWy Net weight int y direction.
         * \param Cx  SpMat The net force matrix in x direction to be computed.
         * \param Cy  SpMat The net force matrix in y direction to be computed.
         * \param dx  VectorXd storing the net force vector in x direction.
         * \param dy  VectorXd storing the net force vector in y direction.
         */
        virtual void compute_net_force_matrix(const NetWeight &NWx, const NetWeight &NWy,
                                              SpMat &Cx, SpMat &Cy, VectorXd &dx, VectorXd &dy) = 0;

        //! Interface for computing all the free modules' net force target position.
        /*!
         * \param NWx Net weight int x direction.
         * \param NWy Net weight int y direction.
         * \param x_target vector storing the target x positions
         * \param y_target vector storing the target y positions
         */
        virtual void compute_net_force_target(const NetWeight &NWx, const NetWeight &NWy,
                                              std::vector<double> &x_target, std::vector<double> &y_target) = 0;
    };

}//namespace tpl

#endif //TPL_ABSTRACT_NET_FORCE_MODEL_H
