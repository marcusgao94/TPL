/*!
 * \file tpl_standard_net_force_model.h
 * \author Peng Fei
 * \brief Standard implementation for Net Force Model.
 */

#ifndef TPL_STANDARD_NET_FORCE_MODEL_H
#define TPL_STANDARD_NET_FORCE_MODEL_H

#include "tpl_abstract_net_force_model.h"

namespace tpl {

    //! Standard implementation for tpl net force model.
    class TplStandardNetForceModel : public TplAbstractNetForceModel {
    public:

        //! Default constructor.
        TplStandardNetForceModel();

        //! Virtual destructor.
        virtual ~TplStandardNetForceModel() {}

        //! Standard implementation for interface compute_net_force_matrix.
        /*!
         * \param NWx Net weight int x direction.
         * \param NWy Net weight int y direction.
         * \param Cx  SpMat The net force matrix in x direction to be computed.
         * \param Cy  SpMat The net force matrix in y direction to be computed.
         * \param dx  VectorXd storing the net force vector in x direction.
         * \param dy  VectorXd storing the net force vector in y direction.
         */
        virtual void compute_net_force_matrix(const NetWeight &NWx, const NetWeight &NWy,
                                              SpMat &Cx, SpMat &Cy, VectorXd &dx, VectorXd &dy);

        //! Standard implementation for interface compute_net_force_target.
        /*!
         * \param NWx Net weight int x direction.
         * \param NWy Net weight int y direction.
         * \param x_target vector storing the target x positions
         * \param y_target vector storing the target y positions
         */
        virtual void compute_net_force_target(const NetWeight &NWx, const NetWeight &NWy,
                                              std::vector<double> &x_target, std::vector<double> &y_target);

        double lastNetLength;
		VectorXd x_target, y_target;
		SpMat Cx, Cy;
		VectorXd dx, dy;
    };

}//namespace tpl

#endif //TPL_STANDARD_NET_FORCE_MODEL_H
