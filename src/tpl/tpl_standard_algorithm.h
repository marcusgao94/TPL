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

    class TplConfig {
    public:
        static TplConfig *instance();
        static void destroy();

        const int &grid_size() const
        {
            return _gsize;
        }

        void set_grid_size(int gsize)
        {
            _gsize = gsize;
        }

        const double &r1() const
        {
            return _r1;
        }

        const double &r2() const
        {
            return _r2;
        }

        const double &mu() const
        {
            return _mu;
        }

        const int &times() const
        {
            return _times;
        }

    private:
        TplConfig();

        static TplConfig *_instance;

        int _gsize;
        double _r1;
        double _r2;
        double _mu;
        int _times;
    };

    extern TplConfig &tplconfig;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TplNetModelInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplNetModelInterface() = 0 {}

        //! Interface for computing a net's net weight.
        /*!
         * \param NWx The net weight in x directions to be computed.
         * \param NWy The net weight in y directions to be computed.
         */
        virtual void compute_net_weight(NetWeight &NWx, NetWeight &NWy) = 0;
    };

    class TplStandardNetModel : public TplNetModelInterface {
    public:
        virtual ~TplStandardNetModel() {}

        //! Standard implementation for interface compute_net_weight.
        /*!
         * \param NWx The net weight in x directions to be computed.
         * \param NWy The net weight in y directions to be computed.
         */
        virtual void compute_net_weight(NetWeight &NWx, NetWeight &NWy);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! Interface definition for tpl net force model.
    class TplNetForceModelInterface {
    public:
        //! Pure virtual destructor.
        virtual ~TplNetForceModelInterface() = 0 {}

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

    //! Standard implementation for tpl net force model.
    class TplStandardNetForceModel : public TplNetForceModelInterface {
    public:
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
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TplThermalModelInterface {
    public:
        //!< Pure virtual destructor.
        virtual ~TplThermalModelInterface() = 0 {}

        //! Interface for green function.
        /*!
         * \param i  x index for green function's first point.
         * \param j  y index for green function's first point.
         * \param i0  x index for green function's second point.
         * \param j0  y index for green function's second point.
         */
        virtual double green_function(int i, int j, int i0, int j0) const = 0;

        //! Interface for power density.
        /*!
         * \param i x index for power density.
         * \param j y index for power density.
         */
        virtual double power_density(int i, int j) const = 0;

        //! Interface for updating the power density.
        virtual void update_power_density() = 0;
    };

    class TplStandardThermalModel : public TplThermalModelInterface {
    public:
        TplStandardThermalModel();
        //!< virtual destructor.
        virtual ~TplStandardThermalModel() {}

        //! Standard implementation for interface green_function.
        /*!
         * \param i  x index for green function's first point.
         * \param j  y index for green function's first point.
         * \param i0  x index for green function's second point.
         * \param j0  y index for green function's second point.
         */
        virtual double green_function(int i, int j, int i0, int j0) const;

        //! Standard implementation for interface power_density.
        /*!
         * \param i x index for power density.
         * \param j y index for power density.
         */
        virtual double power_density(int i, int j) const;

        //! Standard implementation for interface update_power_density.
        virtual void update_power_density();

        const int &grid_size() const
        {
            return _gsize;
        }

        void set_grid_size(int gsize)
        {
            _gsize = gsize;
        }

        const double &grid_width() const
        {
            return _gwidth;
        }

        void set_grid_width(double gwidth)
        {
            _gwidth = gwidth;
        }

        const double &grid_height() const
        {
            return _gheight;
        }

        void set_grid_heihgt(double gheight)
        {
            _gheight = gheight;
        }

    protected:
        int    _gsize;   //!< Chip grid size.
        double _gwidth;  //!< A grid bin's width.
        double _gheight; //!< A grid bin's height.
        std::vector<std::vector<double> > _power_density; //!< Power density container.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! Interface definition for tpl move force model.
    class TplThermalForceModelInterface {
    public:
        //! Constructor.
        TplThermalForceModelInterface(TplThermalModelInterface *thermal_model);

        //! Pure virtual destructor.
        virtual ~TplThermalForceModelInterface() = 0;

        virtual void compute_heat_flux_vector(VectorXd &HFx, VectorXd &HFy) = 0;

    protected:
        virtual void initialize_thermal_model(TplThermalModelInterface *thermal_model) = 0;

        TplThermalModelInterface * _tmodel;
    };

    //! Standard implementation for tpl move force model.
    class TplStandardThermalForceModel : public TplThermalForceModelInterface {
    public:
        //! Constructor.
        TplStandardThermalForceModel(TplThermalModelInterface *thermal_model);

        //! Virtual destructor.
        virtual ~TplStandardThermalForceModel() {}

        virtual void compute_heat_flux_vector(VectorXd &x_heat_flux, VectorXd &y_heat_flux);

    protected:
        virtual void initialize_thermal_model(TplThermalModelInterface *thermal_model);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        TplNetModelInterface          *net_model;        //!< Pointer to a TplNetModel.
        TplNetForceModelInterface     *net_force_model;  //!< Pointer to a TplNetForceModel.
        TplThermalModelInterface      *thermal_model;    //!< Pointer to a TplThermalModel.
        TplThermalForceModelInterface *thermal_force_model; //!< Pointer to a TplMoveForceModel.
    };

    //! Standard implementation for tpl algorithm.
    class TplStandardAlgorithm : public TplAlgorithmInterface {
    public:
        //! Constructor.
        //TplStandardAlgorithm();

        //! Virtual destructor.
        virtual ~TplStandardAlgorithm() {}

        //! Standard implementation for interface initialize_models.
        virtual void initialize_models();

        //! Standard implementation for interface make_initial_placement.
        virtual void make_initial_placement();

        //! standard implementation for interface make_global_placement.
        virtual void make_global_placement();

    protected:
        virtual void initialize_move_force_matrix();
        virtual void update_move_force_matrix();

        NetWeight NWx, NWy;
        SpMat Cx,  Cy;
        VectorXd HFx, HFy;
        SpMat Cx0, Cy0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}//end namespace tpl

#endif//TPL_STANDARD_ALGORITHM_H
