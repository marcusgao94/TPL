/*!
 * \file tpl_standard_thermal_force_model.h
 * \author Peng Fei
 * \brief Standard implementation for Thermal Force Model.
 */

#ifndef TPL_STANDARD_THERMAL_FORCE_MODEL_H
#define TPL_STANDARD_THERMAL_FORCE_MODEL_H

#include "tpl_abstract_thermal_force_model.h"
#include "tpl_db.h"
#include <vector>

#include <memory>

namespace tpl {

    class Terminate {
    public:
        class Segment {
        public:
            double x1, x2, y;
            int flag;

            Segment(double a, double b, double c, int f){
                x1 = a;
                x2 = b;
                y = c;
                flag = f;
            };

            bool operator < (const Segment &s) const {
                return this->y < s.y;
            }
        };

        class Node {
        public:
            int low, high;
            int cover;
            double len;

            Node(int a, int b, int c, double d) {
                low = a;
                high = b;
                cover = c;
                len = d;
            }
        };

        void pushup(int idx);
        void update(int l, int h, int flag, int idx);
        int binarySearch(int low, int high, double target);
        void build(int low, int high, int idx);
        bool shouldStop();


    private:
        std::vector<Segment> segments;
        std::vector<Node> nodes;
        std::vector<double> pos;
    };

    /*!
     * \typedef stxxl::matrix<double, 64> TMat;
     * \brief SpMat type
     */
    typedef stxxl::matrix<double, 64> TMat;

    typedef stxxl::block_scheduler<stxxl::matrix_swappable_block<double, 64> > block_schedular_type;

    //! Standard implementation for tpl move force model.
    class TplStandardThermalForceModel : public TplAbstractThermalForceModel {
    public:
        //! Constructor.
        TplStandardThermalForceModel();

        bool initialize_model();

        //! Standard implementation for compute_head_flux_vector.
        void compute_heat_flux_vector(VectorXd &x_heat_flux, VectorXd &y_heat_flux) const;

        //! Stop condition for global placement
        virtual bool shouldStop();

    private:
        static const int TIMES;

        // member used by shouldStop()
        Terminate terminate;

    protected:
        //! Generate the power density.
        void generate_power_density() const;

        //! Fetch power density for grid (i,j).
        /*!
         * \param i x index for power density.
         * \param j y index for power density.
         */
        double power_density(int i, int j) const;

        //! Compute green function for head conduction equation.
        /*!
         * \param i  x index for green function's first point.
         * \param j  y index for green function's first point.
         * \param i0  x index for green function's second point.
         * \param j0  y index for green function's second point.
         */
        double green_function(int i, int j, int i0, int j0) const;

        //! Generate the thermal profile of the chip.
        void generate_thermal_profile() const;

        //! Generate heat flux grid for both x and y direction.
        void generate_heat_flux_grid() const;

        int _gw_num;     //!< Number of bin in x direction.
        int _gh_num;     //!< Number of bin in y direction.

        block_schedular_type _bs;
        std::shared_ptr<TMat> _power_density; //!< Power density container.
        std::shared_ptr<TMat> _thermal_signature; //!< Thermal profile for the chip.
        std::shared_ptr<TMat> _xhf_grid; //!< Heat flux grid in x direction.
        std::shared_ptr<TMat> _yhf_grid; //!< Heat flux grid in y direction.

        static double BIN_WIDTH;  //!< A grid bin's width.
        static double BIN_HEIGHT; //!< A grid bin's height.
        static double R1;
        static double R2;
        static int MU;
    };

}

#endif //TPL_STANDARD_THERMAL_FORCE_MODEL_H
