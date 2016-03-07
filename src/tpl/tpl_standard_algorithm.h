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

    //! Interface definition for tpl net force model.
    class TplNetForceModelInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplNetForceModelInterface() = 0;

        //! Interface for computing all the free modules' net force target position.
        /*!
         * \param x_target vector storing the target x positions
         * \param y_target vector storing the target y positions
         */
        virtual void compute_net_force_target(std::vector<double> &x_target, std::vector<double> &y_target) = 0;

        //! Interface for computing all the free modules' net force matrix.
        /*!
         * \param Cx SpMat storing the net force matrix in x direction.
         * \param dx VectorXd storing the net force vector in x direction.
         * \param Cy SpMat storing the net force matrix in y direction.
         * \param dy VectorXd storing the net force vector in y direction.
         */
        virtual void compute_net_force_matrix(SpMat &Cx, VectorXd &dx, SpMat &Cy, VectorXd &dy) = 0;

        //! Interface for computing a net's net weight.
        /*!
         * \param x_net_weight storing the net weight in x directions.
         * \param y_net_weight storing the net weight in y directions.
         */
        virtual void compute_net_weight(NetWeight &x_net_weight, NetWeight &y_net_weight) = 0;
    };

    //! Interface definition for tpl move force model.
    class TplMoveForceModelInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplMoveForceModelInterface() = 0;

        //virtual void initialize_move_force_matrix(SpMat &Cx0, SpMat &Cy0) = 0;

        virtual void compute_move_force_matrix(SpMat &Cx0, SpMat &Cy0) = 0;

        virtual void compute_heat_flux(VectorXd &x_heat_flux, VectorXd &y_heat_flux) = 0;

        ////////////////////////// Member Access ///////////////////////////////////////
        //! Interface for accessing green function value in index pair idx and idx0.
        /*!
         * \param idx  ranges from 0 to grid_size in both x and y direction.
         * \param idx0 ranges from -grid_size to 2*grid_size in both x and y direction.
         * \return green function value.
         */
        //virtual double green_function(const std::pair<int, int> &idx, const std::pair<int, int> &idx0) const = 0;

        //! Interface for accessing power density value in (i,j) position.
        /*!
         * \param i x grid bin index.
         * \param j y grid bin index.
         * \return power density value.
         */
        //virtual double power_density(int i, int j) const = 0;
        ////////////////////////// Member Access ///////////////////////////////////////

        ///////////////////////// Modifiers   //////////////////////////////////////////
        //! Interface for updating the grid size.
        /*!
         * \param grid_size unsigned int value indicating the new grid size.
         */
        //virtual void update_green_function(unsigned int grid_size) = 0;

        //! Interface for updating the power density.
        /*!
         */
        //virtual void update_power_density() = 0;
        ///////////////////////// Modifiers   //////////////////////////////////////////
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

    protected:
        TplNetForceModelInterface  *net_force_model;  //!< Pointer to a TplNetForceModelInterface.
        TplMoveForceModelInterface *move_force_model; //!< Pointer to a TplMoveForceModelInterface.
    };

    //! Standard implementation for tpl net force model.
    class TplStandardNetForceModel : public TplNetForceModelInterface {
    public:
        ///////////////////////// Constructors /////////////////////////////////////////
        //! Virtual destructor.
        virtual ~TplStandardNetForceModel();
        ///////////////////////// Constructors /////////////////////////////////////////

        //! Standard implementation for interface compute_net_force_target.
        /*!
         * \param x_target vector storing the target x positions
         * \param y_target vector storing the target y positions
         */
        virtual void compute_net_force_target(std::vector<double> &x_target, std::vector<double> &y_target);

        //! Standard implementation for interface compute_net_force_matrix.
        /*!
         * \param Cx SpMat storing the net force matrix in x direction.
         * \param dx VectorXd storing the net force vector in x direction.
         * \param Cy SpMat storing the net force matrix in y direction.
         * \param dy VectorXd storing the net force vector in y direction.
         */
        virtual void compute_net_force_matrix(SpMat &Cx, VectorXd &dx, SpMat &Cy, VectorXd &dy);

        //! Standard implementation for interface compute_net_weight.
        /*!
         * \param x_net_weight storing the net weight in x directions.
         * \param y_net_weight storing the net weight in y directions.
         */
        virtual void compute_net_weight(NetWeight &x_net_weight, NetWeight &y_net_weight);

    protected:

        //! Private routine for computing all the free module's net force target position.
        /*!
         * \param net_weight a map of Id pair to double representing the net weight.
         * \param target vector for storing the target  positions.
         * \param dim Dimension flag enum.
         */
        virtual void compute_net_force_target(const SpMat &C, const VectorXd &d, std::vector<double> &target);

        //! Private routine for computing all the free modules' net force matrix.
        /*!
         * \param net_weight a map of TplPin address pair to double representing the net weight.
         * \param dim Dimension flag enum.
         * \param C SpMat storing the net force matrix in x direction.
         * \param d VectorXd storing the net force vector in x direction.
         */
        virtual void compute_net_force_matrix(const NetWeight &net_weight, const Dimension dim, SpMat &C, VectorXd &d);

        //! Private routine for computing a net's net weight.
        /*!
         * \param pins vector storing the pins' memory addresses.
         * \param coordinates vector storing the pins' coordinates.
         * \param min storing the minimum pin coordinate.
         * \param max storing the maximum pin coordinate.
         * \param min_idx storing the minimum-coordinate pin's index.
         * \param max_idx storing the maximum-coordinate pin's index.
         * \param net_weight storing the computed net weight.
         */
        virtual void compute_net_weight(const std::vector<TplPin*> &pins, const std::vector<double> &coordinates,
                                        const double &min, const double &max, const size_t &min_idx, const size_t &max_idx, NetWeight &net_weight);
    };

    //! Standard implementation for tpl move force model.
    class TplStandardMoveForceModel : public TplMoveForceModelInterface {
    public:
        ///////////////////////// Constructors /////////////////////////////////////////
        //! Constructor using grid_size, r1 and r2 radius.
        TplStandardMoveForceModel(double r1, double r2, unsigned int grid_size);
        //! Virtual destructor.
        virtual ~TplStandardMoveForceModel();
        ///////////////////////// Constructors /////////////////////////////////////////

        virtual void initialize_move_force_matrix(SpMat &Cx0, SpMat &Cy0);

        virtual void compute_move_force_matrix(SpMat &Cx0, SpMat &Cy0);

        virtual void compute_heat_flux(VectorXd &x_heat_flux, VectorXd &y_heat_flux);

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
        virtual void update_green_function(unsigned int grid_size);

        //! Standard implementation for interface update_power_density.
        virtual void update_power_density();

    protected:
        double _r1; //!< Green function R1 radius.
        double _r2; //!< Green function R2 radius.
        unsigned int _grid_size; //!< Storing grid size during tpl algorithm computation.
        double _bin_width;       //!< A grid bin's width.
        double _bin_height;      //!< A grid bin's height.
        std::map<std::pair<std::pair<int, int>, std::pair<int, int> >, double> _green_function; //!< Green function container.
        std::vector<std::vector<double> > _power_density;                                       //!< Power density container.
    };

    //! Standard implementation for tpl algorithm.
    class TplStandardAlgorithm : public TplAlgorithmInterface {
    public:
        //! Virtual destructor.
        virtual ~TplStandardAlgorithm();

        //! Standard implementation for interface initialize_models.
        virtual void initialize_models();

        //! Standard implementation for interface make_initial_placement.
        virtual void make_initial_placement();
    };
}//end namespace tpl

#endif//TPL_STANDARD_ALGORITHM_H
