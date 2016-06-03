/*!
 * \file tpl_standard_algorithm.h
 * \author Peng Fei
 * \brief Standard definition for TPL algorithm interface
 */

#ifndef TPL_STANDARD_ALGORITHM_H
#define TPL_STANDARD_ALGORITHM_H

#include <ctime>
#include <memory>
#include <cstdlib>
#include <cstdio>

#include "tpl_abstract_algorithm.h"

#include "tpl_standard_net_model.h"
#include "tpl_standard_net_force_model.h"
#include "tpl_standard_thermal_force_model.h"

#include "../tools/def/defwWriter.hpp"
#include "../tools/rippledp/rippledp.h"

#include "utils.h"

namespace tpl {

    // standard implementation of global placement stop condition
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
                return y < s.y;
            }

            bool operator == (const Segment &s) const {
                return (x1 == s.x1 && x2 == s.x2 && y == s.y && flag == s.flag);
            }
        };

        class Node {
        public:
            int low, high;
            int cover;
            double len;

            Node() {
                low = high = cover = len = 0;
            }

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

    //! Standard implementation for tpl algorithm.
    class TplStandardAlgorithm : public TplAbstractAlgorithm {
    public:
        //! Constructor.
        TplStandardAlgorithm();

        //! Virtual destructor.
        virtual ~TplStandardAlgorithm() {}

        //! Standard implementation for interface initialize_models.
        virtual void initialize_models();

		//! Standard implementation for shredding macros into cells
        virtual void shred();

		//! Standard implementation for aggregating cells into macros
		virtual void aggregate();

        //! Standard implementation for interface make_initial_placement.
        virtual void make_initial_placement();

        //! standard implementation for interface make_global_placement.
        virtual void make_global_placement();

        //! standard implementation of saving global placement to def format
		/*!
		 * \param benchmark benchmark name
		 */
		virtual void saveDEF(std::string benchmark);

		//! standard implementation of detail placement using reppledp toolkit
		/*!
		 * \param filename name of the global placement result file
		 */
        virtual void make_detail_placement(std::string benchmark);


        //! main control function of a complete placement flow of a single benchmark
        /*!
         * \param path path of benchmark
         * \param mmp flag of whether enable macro placement
         */
        virtual void control(std::string path, bool mmp);

        Terminate terminate;

    protected:
        void initialize_move_force_matrix();
        void update_move_force_matrix(const VectorXd &delta_x, const VectorXd &delta_y, double mu);

        std::shared_ptr<TplStandardNetModel>          _net_model;           //!< Pointer to a TplNetModel.
        std::shared_ptr<TplStandardNetForceModel>     _net_force_model;     //!< Pointer to a TplNetForceModel.
        std::shared_ptr<TplStandardThermalForceModel> _thermal_force_model; //!< Pointer to a TplThermalForceModel.

        NetWeight NWx, NWy;
        SpMat Cx,  Cy;
        VectorXd HFx, HFy;
        SpMat Cx0, Cy0;
    };

}//end namespace tpl

#endif //TPL_STANDARD_ALGORITHM_H
