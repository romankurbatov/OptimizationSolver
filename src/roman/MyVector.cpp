#include "IVector.h"

#include <cstring>
#include <cmath>
#include <QString>

#include "ILog.h"

namespace  {

class MyVector : public IVector
{
public:
    int getId() const override;

    int add(IVector const *const right) override;
    int subtract(IVector const *const right) override;
    int multiplyByScalar(double scalar) override;
    int dotProduct(IVector const *const right, double &res) const override;

    int gt(IVector const *const right, NormType type, bool &result) const override;
    int lt(IVector const *const right, NormType type, bool &result) const override;
    int eq(IVector const *const right, NormType type, bool &result, double precision) const override;

    unsigned int getDim() const override;
    int norm(NormType type, double &res) const override;
    int setCoord(unsigned int index, double elem) override;
    int getCoord(unsigned int index, double &elem) const override;
    int setAllCoords(unsigned int dim, double *coords) override;
    int getCoordsPtr(unsigned int &dim, double const *&elem) const override;
    IVector *clone() const override;

    MyVector(unsigned int dim, double *const data);
    ~MyVector() override;

private:
    unsigned long mDim;
    double *mData;
};

}

MyVector::MyVector(unsigned int dim, double *const data) :
    mDim(dim),
    mData(data)
{
}

IVector *IVector::createVector(unsigned int size, const double *vals) {
    if (size == 0) {
        ILog::report("IVector::createVector: Size cannot be 0");
        return nullptr;
    }

    double *const data = new double[size];
    if (!data) {
        ILog::report("IVector::createVector: Unable to allocate memory");
        return nullptr;
    }

    if (vals)
        memcpy(data, vals, size * sizeof(double));
    else
        memset(data, 0, size * sizeof(double));

    return new MyVector(size, data);
}

int MyVector::getId() const {
    return IVector::INTERFACE_0;
}

int MyVector::add(const IVector *const right) {
    if (!right) {
        ILog::report("MyVector::add: Right argument is null");
        return ERR_WRONG_ARG;
    }

    unsigned int rightDim;
    const double *rightData;
    int code = right->getCoordsPtr(rightDim, rightData);
    if (code != ERR_OK) {
        ILog::report("MyVector::add: Unable to get coordinates of the right argument");
        return code;
    }

    if (rightDim != mDim) {
        ILog::report("MyVector::add: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    for (size_t i = 0; i < mDim; ++i) {
        mData[i] += rightData[i];
    }

    return ERR_OK;
}

int MyVector::subtract(IVector const *const right) {
    if (!right) {
        ILog::report("MyVector::subtract: Right argument is null");
        return ERR_WRONG_ARG;
    }

    unsigned int rightDim;
    const double *rightData;
    int code = right->getCoordsPtr(rightDim, rightData);
    if (code != ERR_OK) {
        ILog::report("MyVector::subtract: Unable to get coordinates of the right argument");
        return code;
    }

    if (rightDim != mDim) {
        ILog::report("MyVector::subtract: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    for (size_t i = 0; i < mDim; ++i) {
        mData[i] -= rightData[i];
    }

    return ERR_OK;
}

int MyVector::multiplyByScalar(double scalar) {
    for (size_t i = 0; i < mDim; ++i) {
        mData[i] *= scalar;
    }

    return ERR_OK;
}

int MyVector::dotProduct(const IVector *const right, double &res) const {
    if (!right) {
        ILog::report("MyVector::dotProduct: Right argument is null");
        return ERR_WRONG_ARG;
    }

    unsigned int rightDim;
    const double *rightData;
    int code = right->getCoordsPtr(rightDim, rightData);
    if (code != ERR_OK) {
        ILog::report("MyVector::dotProduct: Unable to get coordinates of the right argument");
        return code;
    }

    if (rightDim != mDim) {
        ILog::report("MyVector::dotProduct: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    res = 0;
    for (size_t i = 0; i < mDim; ++i) {
        res += mData[i] * rightData[i];
    }

    return ERR_OK;
}

int MyVector::gt(IVector const *const right, NormType, bool &result) const {
    if (!right) {
        ILog::report("MyVector::gt: Right argument is null");
        return ERR_WRONG_ARG;
    }

    unsigned int rightDim;
    const double *rightData;
    int code = right->getCoordsPtr(rightDim, rightData);
    if (code != ERR_OK) {
        ILog::report("MyVector::gt: Unable to get coordinates of the right argument");
        return code;
    }

    if (rightDim != mDim) {
        ILog::report("MyVector::gt: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    result = true;
    for (size_t i = 0; i < mDim; ++i) {
        if (mData[i] <= rightData[i]) {
            result = false;
            break;
        }
    }

    return ERR_OK;
}

int MyVector::lt(IVector const *const right, NormType, bool &result) const {
    if (!right) {
        ILog::report("MyVector::lt: Right argument is null");
        return ERR_WRONG_ARG;
    }

    unsigned int rightDim;
    const double *rightData;
    int code = right->getCoordsPtr(rightDim, rightData);
    if (code != ERR_OK) {
        ILog::report("MyVector::lt: Unable to get coordinates of the right argument");
        return code;
    }

    if (rightDim != mDim) {
        ILog::report("MyVector::lt: Dimensions mismatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    result = true;
    for (size_t i = 0; i < mDim; ++i) {
        if (mData[i] >= rightData[i]) {
            result = false;
            break;
        }
    }

    return ERR_OK;
}

int MyVector::eq(IVector const *const right, NormType type, bool &result, double precision) const {
    if (!right) {
        ILog::report("MyVector::eq: Right argument is null");
        return ERR_WRONG_ARG;
    }

    const IVector * const difference = IVector::subtract(this, right);
    if (!difference) {
        ILog::report("MyVector::eq: Unable to caluclate difference of the vectors");
        return ERR_ANY_OTHER;
    }

    double norm;
    int code = difference->norm(type, norm);
    if (code != ERR_OK) {
        ILog::report("MyVector::eq: Unable to caluclate difference norm");
        return code;
    }

    result = (norm <= precision);

    return ERR_OK;
}

unsigned int MyVector::getDim() const {
    return static_cast<unsigned int>(mDim);
}

int MyVector::norm(NormType type, double &res) const {
    switch (type) {
    case NORM_1:
        res = 0;
        for (size_t i = 0; i < mDim; ++i)
            res += fabs(mData[i]);
        break;
    case NORM_2:
        res = 0;
        for (size_t i = 0; i < mDim; ++i)
            res += mData[i]*mData[i];
        res = sqrt(res);
        break;
    case NORM_INF:
        res = fabs(mData[0]);
        for (size_t i = 1; i < mDim; ++i)
            if (fabs(mData[i]) > res)
                res = fabs(mData[i]);
        break;
    default:
        ILog::report("MyVector::norm: Unknown norm type");
        return ERR_NORM_NOT_DEFINED;
    }

    return ERR_OK;
}

int MyVector::setCoord(unsigned int index, double elem) {
    if (index >= mDim) {
        ILog::report("MyVector::setCoord: Index is out of range");
        return ERR_OUT_OF_RANGE;
    }

    mData[index] = elem;
    return ERR_OK;
}

int MyVector::getCoord(unsigned int index, double &elem) const {
    if (index >= mDim) {
        ILog::report("MyVector::getCoord: Index is out of range");
        return ERR_OUT_OF_RANGE;
    }

    elem = mData[index];
    return ERR_OK;
}

int MyVector::setAllCoords(unsigned int newDim, double *coords) {
    if (newDim == 0) {
        ILog::report("IVector::setAllCoords: Dimension cannot be 0");
        return ERR_WRONG_ARG;
    }

    if (newDim != mDim) {
        double *newData = new double[newDim];
        if (!newData) {
            ILog::report("MyVector::setAllCoords: Unable to allocate memory");
            return ERR_MEMORY_ALLOCATION;
        }

        delete mData;
        mData = newData;
        mDim = newDim;
    }

    memcpy(mData, coords, mDim * sizeof(double));
    return ERR_OK;
}

int MyVector::getCoordsPtr(unsigned int &dim, double const *&elem) const {
    dim = static_cast<unsigned int>(mDim);
    elem = mData;
    return ERR_OK;
}

IVector* MyVector::clone() const {
    return MyVector::createVector(static_cast<unsigned int>(mDim), mData);
}

MyVector::~MyVector() {
    delete mData;
}

IVector *IVector::add(IVector const *const left, IVector const *const right) {
    IVector *const result = left->clone();
    if (!result) {
        ILog::report("IVector::add: Unable to clone the left vector");
        return nullptr;
    }

    int code = result->add(right);
    if (code != ERR_OK) {
        ILog::report("IVector::add: Unable to perform addition");
        delete result;
        return nullptr;
    }

    return result;
}

IVector *IVector::subtract(IVector const *const left, IVector const *const right) {
    IVector *const result = left->clone();
    if (!result) {
        ILog::report("IVector::subtract: Unable to clone the left vector");
        return nullptr;
    }

    int code = result->subtract(right);
    if (code != ERR_OK) {
        ILog::report("IVector::subtract: Unable to perform subtraction");
        delete result;
        return nullptr;
    }

    return result;
}

IVector *IVector::multiplyByScalar(IVector const *const left, double scalar) {
    IVector *const result = left->clone();
    if (!result) {
        ILog::report("IVector::multiplyByScalar: Unable to clone the vector");
        return nullptr;
    }

    int code = result->multiplyByScalar(scalar);
    if (code != ERR_OK) {
        ILog::report("IVector::multiplyByScalar: Unable to perform multiplication");
        delete result;
        return nullptr;
    }

    return result;
}
