//
// Created by pta on 15/08/2018.
//

#ifndef IRP_LINEARPIECE_H
#define IRP_LINEARPIECE_H

#include "memory"
#include <algorithm>
#include <string>
#include <vector>
#include "Params.h"

using namespace std;

constexpr double EPSILON = 1e-5;

inline bool le(const double &x, const double &y) { return x <= y + EPSILON; } // less than or equal to

inline bool lt(const double &x, const double &y) { return x + EPSILON < y; } //less than

inline bool eq(const double &x, const double &y) //equal
{
    return fabs(x - y) < EPSILON;
}

inline bool neq(const double &x, const double &y) { return !eq(x, y); } // not equal

inline bool gt(const double &x, const double &y) { return lt(y, x); } // greater than

inline bool ge(const double &x, const double &y) { return le(y, x); }

class Params;

struct Point
{
    double x;
    double y;
    bool isFeasible;

    Point() {}

    Point(double x, double y) : x(x), y(y) { isFeasible = true; }

    virtual ~Point()
    {
        //        cout << "Delete point" << endl;
    }
    friend bool operator==(const Point& lhs, const Point& rhs) {
        return eq(lhs.x, rhs.x) && eq(lhs.y, rhs.y); // 假设 x 和 y 是相关的成员
    }
    inline Point convolve(shared_ptr<Point> p)
    {
        double x = this->x + p->x;
        double y = this->y + p->y;

        Point pc = Point(x, y);

        //        if (this->x == 0 && p->x == 0)
        //            pc.isFeasible = false;

        return pc;
    }
};

class LinearPiece
{

public:
    double slope=0;
    shared_ptr<Point> p1;
    shared_ptr<Point> p2;

    shared_ptr<LinearPiece> next;
    //        shared_ptr<LinearPiece> pre;
    shared_ptr<LinearPiece> fromC_pre;
    shared_ptr<LinearPiece> fromC;
    shared_ptr<LinearPiece> fromF;
    shared_ptr<Insertion> fromInst;
    double replenishment_loss;
    double stockout_hm;

    LinearPiece();

    LinearPiece(double left_x, double left_y, double right_x, double right_y);
    void updateLinearPiece(double left_x, double left_y, double right_x, double right_y);
    void print();
    LinearPiece(LinearPiece *lp);
    std::shared_ptr<LinearPiece> getInpiece(double start, double end) const;
   

    double cost(double x);
    double invertCost(double y);

    std::shared_ptr<LinearPiece> getInBound(double lb, double ub);

    bool eqlp(const shared_ptr<LinearPiece> rhs);
    bool eqFromC(const shared_ptr<LinearPiece> rhs);
    bool eqFromCpre(const shared_ptr<LinearPiece> rhs);
    bool eqFromF(const shared_ptr<LinearPiece> rhs);

    bool eqDeep(const shared_ptr<LinearPiece> rhs);

    std::shared_ptr<LinearPiece> clone();
    std::shared_ptr<LinearPiece> cloneWithout();
    std::shared_ptr<LinearPiece> clone_addHolding(double InventoryCost, double daily);
    std::shared_ptr<LinearPiece> clone_addStockout(double stockout, double daily);
    void update(double left_x, double left_y, double right_x, double right_y);

    ~LinearPiece();
};

#endif // IRP_LINEARPIECE_H
