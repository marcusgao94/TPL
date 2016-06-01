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

        void pushup(int low, int high, int idx);
        void update(int l, int h, int flag, int idx);
        int binarySearch(int low, int high, double target);
        void build(int low, int high, int idx);
        bool shouldStop();


    private:
        std::vector<Segment> segments;
        std::vector<Node> nodes;
        std::vector<double> pos;
    };


    //! Standard implementation for tpl move force model.
    class TplStandardThermalForceModel : public TplAbstractThermalForceModel {
    public:
        //! Constructor.
        explicit  TplStandardThermalForceModel(std::shared_ptr<TplAbstractThermalModel> tmodel) : TplAbstractThermalForceModel(tmodel) {}

        //! Virtual destructor.
        virtual ~TplStandardThermalForceModel() {}

        //! Standard implementation for compute_head_flux_vector.
        virtual void compute_heat_flux_vector(const std::vector<std::vector<double>> &tss, VectorXd &HFx, VectorXd &HFy) const;



        //! Stop condition for global placement
        virtual bool shouldStop();

    private:
        static const int TIMES;

        // member used by shouldStop()
        Terminate terminate;
    };





}

#endif //TPL_STANDARD_THERMAL_FORCE_MODEL_H
