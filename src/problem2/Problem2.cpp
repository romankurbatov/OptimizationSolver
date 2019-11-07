#include "Problem2.h"

#include "IVector.h"
#include "ILog.h"
#include "error.h"

#include <QString>

Problem2::Problem2() :
    mArgs(nullptr),
    mParams(nullptr)
{
}

Problem2::~Problem2() {
    delete mArgs;
    delete mParams;
}

int Problem2::getId() const {
    return IProblem::INTERFACE_0;
}

int Problem2::goalFunction(IVector const* args, IVector const* params, double& res) const {
    if (!args) {
        ILog::report("Problem2::goalFunction: Args cannot be null");
        return ERR_WRONG_ARG;
    }

    if (!params) {
        ILog::report("Problem2::goalFunction: Params cannot be null");
        return ERR_WRONG_ARG;
    }

    if (args->getDim() != mArgsDim) {
        ILog::report("Problem2::goalFunction: Args dimension must be 2");
        ILog::report(QString("Problem2::setParams: Args dimension must be %1")
                     .arg(mArgsDim).toLocal8Bit().constData());
        return ERR_WRONG_ARG;
    }

    if (params->getDim() != mParamsDim) {
        ILog::report(QString("Problem2::setParams: Params dimension must be %1")
                     .arg(mParamsDim).toLocal8Bit().constData());
        return ERR_WRONG_ARG;
    }

    unsigned int dim;
    const double *a, *p;
    int retCode = args->getCoordsPtr(dim, a);
    if (retCode != ERR_OK) {
        ILog::report("Problem2::goalFunction: Unable to get args vector coords");
        return ERR_ANY_OTHER;
    }
    retCode = params->getCoordsPtr(dim, p);
    if (retCode != ERR_OK) {
        ILog::report("Problem2::goalFunction: Unable to get params vector coords");
        return ERR_ANY_OTHER;
    }

    res = p[0]*a[0]*a[0] + p[1]*a[1]*a[1] + p[2]*a[1];
    return ERR_OK;
}

int Problem2::goalFunctionByArgs(const IVector *args, double &res) const {
    if (!mParams) {
        ILog::report("Problem2::goalFunctionByArgs: Params not set");
        return ERR_ANY_OTHER;
    }

    return goalFunction(args, mParams, res);
}

int Problem2::goalFunctionByParams(const IVector *params, double &res) const {
    if (!mArgs) {
        ILog::report("Problem2::goalFunctionByParams: Args not set");
        return ERR_ANY_OTHER;
    }

    return goalFunction(mArgs, params, res);
}

int Problem2::getArgsDim(size_t& dim) const {
    dim = mArgsDim;
    return ERR_OK;
}

int Problem2::getParamsDim(size_t &dim) const {
    dim = mParamsDim;
    return ERR_OK;
}

int Problem2::setParams(const IVector *params) {
    if (!params) {
        ILog::report("Problem2::setParams: Params cannot be null");
        return ERR_WRONG_ARG;
    }

    if (params->getDim() != mParamsDim) {
        ILog::report(QString("Problem2::setParams: Params dimension must be %1")
                     .arg(mParamsDim).toLocal8Bit().constData());
        return ERR_WRONG_ARG;
    }

    mParams = params->clone();
    if (!mParams) {
        ILog::report("Problem2::setParams: Unable to clone params vector");
        return ERR_ANY_OTHER;
    }

    return ERR_OK;
}

int Problem2::setArgs(const IVector *args) {
    if (!args) {
        ILog::report("Problem2::setArgs: Args cannot be null");
        return ERR_WRONG_ARG;
    }

    if (args->getDim() != mArgsDim) {
        ILog::report(QString("Problem2::setArgs: Args dimension must be %1")
                     .arg(mArgsDim).toLocal8Bit().constData());
        return ERR_WRONG_ARG;
    }

    mArgs = args->clone();
    if (!mArgs) {
        ILog::report("Problem2::setArgs: Unable to clone args vector");
        return ERR_ANY_OTHER;
    }

    return ERR_OK;
}

int Problem2::derivativeGoalFunction(
        size_t order,
        size_t idx,
        DerivedType dr,
        double &value,
        const IVector *args,
        const IVector *params)
const {
    if (!args) {
        ILog::report("Problem2::derivativeGoalFunction: Args cannot be null");
        return ERR_WRONG_ARG;
    }

    if (!params) {
        ILog::report("Problem2::derivativeGoalFunction: Params cannot be null");
        return ERR_WRONG_ARG;
    }

    if (args->getDim() != mArgsDim) {
        ILog::report(QString("Problem2::derivativeGoalFunction: Args dimension must be %1")
                     .arg(mArgsDim).toLocal8Bit().constData());
        return ERR_WRONG_ARG;
    }

    if (params->getDim() != mParamsDim) {
        ILog::report(QString("Problem2::derivativeGoalFunction: Params dimension must be %1")
                     .arg(mParamsDim).toLocal8Bit().constData());
        return ERR_WRONG_ARG;
    }

    switch (dr) {
    case BY_ARGS:
        return derivativeByArg(order, idx, value, args, params);
    case BY_PARAMS:
        return derivativeByParam(order, idx, value, args, params);
    default:
        ILog::report("Problem2::derivativeGoalFunction: Unknown derivative type");
        return ERR_WRONG_ARG;
    }
}

int Problem2::derivativeByParam(
        size_t order,
        size_t idx,
        double &value,
        const IVector *args,
        const IVector *params
) const {
    if (idx >= mParamsDim) {
        ILog::report("Problem2::derivativeByParam: Index cannot be greater than number of parameters");
        return ERR_WRONG_ARG;
    }

    if (order == 0) {
        return goalFunction(args, params, value);
    }

    if (order >= 2) {
        value = 0;
        return ERR_OK;
    }

    switch (idx) {
    case 0: {
        double a0;
        int retVal = args->getCoord(0, a0);
        if (retVal != ERR_OK) {
            ILog::report("Problem2::derivativeByParam: Unable to get args vector coordinate");
            return retVal;
        }
        value = a0*a0;
        break;
    }
    case 1: {
        double a1;
        int retVal = args->getCoord(0, a1);
        if (retVal != ERR_OK) {
            ILog::report("Problem2::derivativeByParam: Unable to get args vector coordinate");
            return retVal;
        }
        value = a1*a1;
        break;
    }
    case 2: {
        double a2;
        int retVal = args->getCoord(0, a2);
        if (retVal != ERR_OK) {
            ILog::report("Problem2::derivativeByParam: Unable to get args vector coordinate");
            return retVal;
        }
        value = a2;
        break;
    }
    }

    return ERR_OK;
}

int Problem2::derivativeByArg(
        size_t order,
        size_t idx,
        double &value,
        const IVector *args,
        const IVector *params
) const {
    if (idx >= mArgsDim) {
        ILog::report("Problem2::derivativeByArg: Index cannot be greater than number of arguments");
        return ERR_WRONG_ARG;
    }

    unsigned int dim;
    const double *a, *p;
    int retCode = args->getCoordsPtr(dim, a);
    if (retCode != ERR_OK) {
        ILog::report("Problem2::derivativeByArg: Unable to get args vector coords");
        return ERR_ANY_OTHER;
    }
    retCode = params->getCoordsPtr(dim, p);
    if (retCode != ERR_OK) {
        ILog::report("Problem2::derivativeByArg: Unable to get params vector coords");
        return ERR_ANY_OTHER;
    }

    if (order == 0) {
        return goalFunction(args, params, value);
    }

    if (order >= 3) {
        value = 0;
        return ERR_OK;
    }

    switch (idx) {
    case 0:
        switch (order) {
        case 1:
            value = 2*p[0]*a[0];
            break;
        case 2:
            value = 2*p[0];
            break;
        }
        break;
    case 1:
        switch (order) {
        case 1:
            value = 2*p[1]*a[1] + p[2];
            break;
        case 2:
            value = 2*p[1];
            break;
        }
        break;
    }

    return ERR_OK;
}

int Problem2::derivativeGoalFunctionByArgs(
        size_t order,
        size_t idx,
        DerivedType dr,
        double &value,
        const IVector *args
) const {
    if (!mParams) {
        ILog::report("Problem2::derivativeGoalFunctionByArgs: Params not set");
        return ERR_ANY_OTHER;
    }

    return derivativeGoalFunction(order, idx, dr, value, args, mParams);
}

int Problem2::derivativeGoalFunctionByParams(
        size_t order,
        size_t idx,
        DerivedType dr,
        double &value,
        const IVector *params
) const {
    if (!mArgs) {
        ILog::report("Problem2::derivativeGoalFunctionByParams: Args not set");
        return ERR_ANY_OTHER;
    }

    return derivativeGoalFunction(order, idx, dr, value, mArgs, params);
}
