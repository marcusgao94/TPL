/*!
 * \file tpl_standard_algorithm.h
 * \author Peng Fei
 * \brief Standard definition for TPL algorithm interface
 */

#ifndef TPL_STANDARD_ALGORITHM_H
#define TPL_STANDARD_ALGORITHM_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>

#include "tpl_db.h"

namespace tpl {

    using Eigen::VectorXd;

    /*!
     * \typedef Eigen::SparseMatrix<double> SpMat;
     * \brief SpMat type
     */
    typedef Eigen::SparseMatrix<double> SpMat;
    /*!
     * \typedef Eigen::Triplet<double> SpElem;
     * \brief SpElem type
     */
    typedef Eigen::Triplet<double>      SpElem;
    /*!
     * \typedef Eigen::SimplicialLLT<SpMat> LLTSolver;
     * \brief LLTSolver type
     */
    typedef Eigen::SimplicialLLT<SpMat> LLTSolver;

    /*!
     * \typedef std::map<std::pair<TplPin*, TplPin*>, double> NetWeight;
     * \brief NetWeight type
     */
    typedef std::map<std::pair<TplPin*, TplPin*>, double> NetWeight;

    /*!
     * \enum class Dimension
     * \brief Dimension enum
     */
    enum class Dimension { X, Y };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TplNetModelInterface {
    public:
        virtual ~TplNetModelInterface() = 0;

        //! Interface for computing a net's net weight.
        /*!
         * \param x_net_weight storing the net weight in x directions.
         * \param y_net_weight storing the net weight in y directions.
         */
        virtual void compute_net_weight(NetWeight &x_net_weight, NetWeight &y_net_weight) = 0;
    };

    class TplStandardNetModel : public TplNetModelInterface {
    public:
        virtual ~TplStandardNetModel();

        //! Standard implementation for interface compute_net_weight.
        /*!
         * \param x_net_weight storing the net weight in x directions.
         * \param y_net_weight storing the net weight in y directions.
         */
        virtual void compute_net_weight(NetWeight &x_net_weight, NetWeight &y_net_weight);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! Interface definition for tpl net force model.
    class TplNetForceModelInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplNetForceModelInterface() = 0;

        //! Interface for computing all the free modules' net force matrix.
        /*!
         * \param NWx Net weight int x direction.
         * \param NWy Net weight int y direction.
         * \param Cx  SpMat storing the net force matrix in x direction.
         * \param Cy  SpMat storing the net force matrix in y direction.
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

    //! Standard implementation for tpl net force model.
    class TplStandardNetForceModel : public TplNetForceModelInterface {
    public:
        ///////////////////////// Constructors /////////////////////////////////////////
        //! Virtual destructor.
        virtual ~TplStandardNetForceModel();
        ///////////////////////// Constructors /////////////////////////////////////////

        //! Standard implementation for interface compute_net_force_matrix.
        /*!
         * \param NWx Net weight int x direction.
         * \param NWy Net weight int y direction.
         * \param Cx  SpMat storing the net force matrix in x direction.
         * \param Cy  SpMat storing the net force matrix in y direction.
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
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! Interface definition for tpl move force model.
    class TplMoveForceModelInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplMoveForceModelInterface() = 0;

        //virtual void compute_move_force_matrix(SpMat &Cx0, SpMat &Cy0) = 0;

        virtual void compute_heat_flux_vector(VectorXd &HFx, VectorXd &HFy) = 0;
    };

    //! Standard implementation for tpl move force model.
    class TplStandardMoveForceModel : public TplMoveForceModelInterface {
    public:
        ///////////////////////// Constructors /////////////////////////////////////////
        //! Constructor using grid_size, r1 and r2 radius.
        TplStandardMoveForceModel(SpMat &Cx0, SpMat &Cy0, double r1, double r2, unsigned int grid_size, double mu);
        //! Virtual destructor.
        virtual ~TplStandardMoveForceModel();
        ///////////////////////// Constructors /////////////////////////////////////////


        //virtual void compute_move_force_matrix(SpMat &Cx0, SpMat &Cy0);

        virtual void compute_heat_flux_vector(VectorXd &x_heat_flux, VectorXd &y_heat_flux);

    protected:

        //! Standard implementation for interface green_function.
        /*!
         * \param idx  x and y index for green function.
         * \param idx0 x0 and y0 index for green function.
         */
        virtual double green_function(const std::pair<int, int> &idx, const std::pair<int, int> &idx0) const;

        //! Standard implementation for interface power_density.
        /*!
         * \param i x index for power density.
         * \param j y index for power density.
         */
        virtual double power_density(int i, int j) const;

        //! Standard implementation for interface update_green_function.
        /*!
         * \param grid_size New grid size for the chip.
         */
        //virtual void update_green_function(unsigned int grid_size);

        //! Standard implementation for interface update_power_density.
        virtual void update_power_density();

        double _r1; //!< Green function R1 radius.
        double _r2; //!< Green function R2 radius.
        unsigned int _grid_size; //!< Storing grid size during tpl algorithm computation.
        double _bin_width;       //!< A grid bin's width.
        double _bin_height;      //!< A grid bin's height.
        std::vector<std::vector<double> > _power_density; //!< Power density container.

        double _mu; //!< User defined algorithm precision parameter.
        //double _avg_move_distances; //!< Average module movement distance in Move Force Model.
        //std::map<size_t, std::pair<unsigned, unsigned> > _x_m2idx;
        //std::map<size_t, std::pair<unsigned, unsigned> > _y_m2idx;

        SpMat &_Cx0;
        SpMat &_Cy0;
    };

    //! Interface definition for tpl algorithm.
    class TplAlgorithmInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplAlgorithmInterface() = 0;

        //! Interface for initializing the force models.
        virtual void initialize_models() = 0;

        //! Interface for initializing the module placement.
        virtual void make_initial_placement() = 0;

        //! Interface for global placement.
        virtual void make_global_placement() = 0;

    protected:
        TplNetForceModelInterface  *net_force_model;  //!< Pointer to a TplNetForceModelInterface.
        TplMoveForceModelInterface *move_force_model; //!< Pointer to a TplMoveForceModelInterface.
    };

    //! Standard implementation for tpl algorithm.
    class TplStandardAlgorithm : public TplAlgorithmInterface {
    public:
        //! Constructor.
        TplStandardAlgorithm();

        //! Virtual destructor.
        virtual ~TplStandardAlgorithm();

        //! Standard implementation for interface initialize_models.
        virtual void initialize_models();

        //! Standard implementation for interface make_initial_placement.
        virtual void make_initial_placement();

        //! standard implementation for interface make_global_placement.
        virtual void make_global_placement();

    protected:
        virtual void initialize_move_force_matrix();
        virtual void update_move_force_matrix();

        SpMat Cx,  Cy;
        SpMat Cx0, Cy0;
        VectorXd HFx, HFy;
    };

}//end namespace tpl

#endif//TPL_STANDARD_ALGORITHM_H
