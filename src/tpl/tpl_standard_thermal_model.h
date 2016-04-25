/*!
 * \file tpl_standard_thermal_model.h
 * \author Peng Fei
 * \brief Standard implementation for Thermal Model.
 */

#ifndef TPL_TPL_STANDARD_THERMAL_MODEL_H
#define TPL_TPL_STANDARD_THERMAL_MODEL_H

#include "tpl_abstract_thermal_model.h"

namespace tpl {

    class TplStandardThermalModel : public TplAbstractThermalModel {
    public:
        //! Default constructor.
        TplStandardThermalModel();

        //!< virtual destructor.
        virtual ~TplStandardThermalModel() {}

        virtual void compute_thermal_distribution(std::vector<std::vector<double>> &distribution) const;

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

        //! Update the power density.
        void update_power_density();

    protected:
        //! Compute green function for head conduction equation.
        /*!
         * \param i  x index for green function's first point.
         * \param j  y index for green function's first point.
         * \param i0  x index for green function's second point.
         * \param j0  y index for green function's second point.
         */
        double green_function(int i, int j, int i0, int j0) const;

        //! Compute power density for grid (i,j).
        /*!
         * \param i x index for power density.
         * \param j y index for power density.
         */
        double power_density(int i, int j) const;

        int    _gsize;   //!< Chip grid size.
        double _gwidth;  //!< A grid bin's width.
        double _gheight; //!< A grid bin's height.
        std::vector<std::vector<double> > _power_density; //!< Power density container.

    private:
        static const int TIMES;

    };

}

#endif //TPL_TPL_STANDARD_THERMAL_MODEL_H
