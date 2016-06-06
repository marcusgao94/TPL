/*!
 * \file tpl_standard_algorithm.h
 * \author Peng Fei
 * \brief Standard definition for TPL algorithm interface
 */

#ifndef TPL_STANDARD_ALGORITHM_H
#define TPL_STANDARD_ALGORITHM_H

#include "tpl_abstract_algorithm.h"

#include <vector>
#include <memory>

#include "tpl_abstract_algorithm.h"
#include "tpl_standard_net_model.h"
#include "tpl_standard_net_force_model.h"
#include "tpl_standard_thermal_force_model.h"
#include "utils.h"

namespace tpl {
    using std::vector;

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

        //! standard implementation of detail placement using NTUPlacer3
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

    protected:
        void initialize_move_force_matrix();
        void update_move_force_matrix(const VectorXd &delta_x, const VectorXd &delta_y, double mu);
        bool should_stop_global_placement() const;
		bool should_stop_initial_placement() const;

        std::shared_ptr<TplStandardNetModel>          _net_model;           //!< Pointer to a TplNetModel.
        std::shared_ptr<TplStandardNetForceModel>     _net_force_model;     //!< Pointer to a TplNetForceModel.
        std::shared_ptr<TplStandardThermalForceModel> _thermal_force_model; //!< Pointer to a TplThermalForceModel.

        NetWeight NWx, NWy;
        SpMat Cx,  Cy;
        VectorXd HFx, HFy;
        SpMat Cx0, Cy0;
    };

    struct SegmentEvent {
        double x1 , x2, y;
        int f;

        SegmentEvent(double a, double b, double c, int d) : x1(a) , x2(b) , y(c) , f(d) {}

        bool operator < (const SegmentEvent &cmp) const {
            return y < cmp.y;
        }
    };

	class SegmentNode {
	public:
		int low, high, cover;
		double len;
	};

    class SegmentTree {
    public:
		int m;

		inline double get_len() { return nodes[0].len; }

		void build(vector<double> xpos);
		void build(int idx, int l, int h);

		int binsearch(int l, int h, double target);

        void update(int idx, int l, int h, int flag);

        void push_up(int idx);

        vector<double> pos;
		vector<SegmentNode> nodes;
    };

}//end namespace tpl

#endif //TPL_STANDARD_ALGORITHM_H
