#ifndef PROBLEM2_H
#define PROBLEM2_H

#include "IProblem.h"
#include "IVector.h"

class Problem2 : public IProblem
{
public:
    Problem2();
    ~Problem2() override;

    int getId() const override;

    int goalFunction(IVector const* args, IVector const* params, double& res) const override;
    int goalFunctionByArgs(IVector const*  args, double& res) const override;
    int goalFunctionByParams(IVector const*  params, double& res) const override;
    int getArgsDim(size_t& dim) const override;
    int getParamsDim(size_t& dim) const override;

    int setParams(IVector const* params) override;
    int setArgs(IVector const* args) override;

    int derivativeGoalFunction(size_t order, size_t idx, DerivedType dr, double& value, IVector const* args, IVector const* params) const override;
    int derivativeGoalFunctionByArgs(size_t order, size_t idx, DerivedType dr, double& value, IVector const* args) const override;
    int derivativeGoalFunctionByParams(size_t order, size_t idx, DerivedType dr, double& value, IVector const* params) const override;

private:
    /*non default copyable*/
    Problem2(const Problem2& other) = delete;
    void operator=(const Problem2& other) = delete;

    int derivativeByArg(size_t order, size_t idx, double& value, IVector const* args, IVector const* params) const;
    int derivativeByParam(size_t order, size_t idx, double& value, IVector const* args, IVector const* params) const;

    const IVector *mArgs, *mParams;
    static const unsigned int mArgsDim = 2, mParamsDim = 3;
};

#endif // PROBLEM2_H
