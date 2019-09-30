#include "ICompact.h"

#include <QtGlobal>
#include <QScopedPointer>
#include <QScopedArrayPointer>

#include "ILog.h"

namespace {

class SHARED_EXPORT MyCompact : public ICompact {
public:
    int getId() const override;

    int Intersection(const ICompact &c) override;
    int Union(const ICompact &c) override;
    int Difference(const ICompact &c) override;
    int SymDifference(const ICompact &c) override;
    int MakeConvex() override;

    int deleteIterator(IIterator *pIter) override;
    int getByIterator(IIterator const *pIter, IVector *&pItem) const override;

    class MyIterator : public ICompact::IIterator
    {
    public:
        int doStep() override;

        int setStep(IVector const *const step) override;

        MyIterator(MyCompact const *const compact, int pos, IVector const *const step);
        ~MyIterator() override;

    private:
        MyIterator(const MyIterator &other) = delete;
        void operator=(const MyIterator &other) = delete;
    };

    IIterator *end(IVector const *const step = nullptr) override;
    IIterator *begin(IVector const *const step = nullptr) override;

    int isContains(IVector const *const vec, bool &result) const override;
    int isSubSet(ICompact const *const other) const override;
    int isSimplyConn(bool &result) const override;
    int isIntersects(ICompact const *const other, bool &result) const override;
    int getNearestNeighbor(IVector const *vec, IVector *&nn) const override;

    MyCompact *clone() const override;

    ~MyCompact() override;

    MyCompact(IVector * const begin, IVector * const end, IVector * const step);
    static IVector *createDefaultStep(const IVector * const begin, const IVector * const end);
    static IVector *createCustomStep(const IVector *const begin, const IVector *const end, const IVector *const step);

protected:

private:
    MyCompact(const ICompact &other) = delete;
    void operator=(const ICompact &other) = delete;

    const IVector *mBegin, *mEnd, *mStep;

    static const size_t nDefaultSteps = 1000;
};

}

MyCompact::MyCompact(
        IVector *const begin,
        IVector *const end,
        IVector *const step) :
    mBegin(begin),
    mEnd(end),
    mStep(step)
{
}

int MyCompact::getId() const {
    return ICompact::INTERFACE_0;
}

ICompact* createCompact(
        IVector const *const begin,
        IVector const *const end,
        IVector const *const step)
{
    if (!begin) {
        ILog::report("createCompact: Vector begin cannot be null");
        return nullptr;
    }

    if (!end) {
        ILog::report("createCompact: Vector end cannot be null");
        return nullptr;
    }

    if (begin->getDim() != end->getDim() || (step && begin->getDim() != step->getDim())) {
        ILog::report("createCompact: Dimensions mismatch");
        return nullptr;
    }

    bool isLess = false;
    int code = begin->lt(end, IVector::NORM_1, isLess);
    if (code != ERR_OK) {
        ILog::report("createCompact: Unable to compare vectors");
        return nullptr;
    }
    if (!isLess) {
        ILog::report("createCompact: Begin must be less than end");
        return nullptr;
    }

    QScopedPointer<IVector> beginCloned(begin->clone());
    if (!beginCloned) {
        ILog::report("createCompact: Unable to clone vector begin");
        return nullptr;
    }

    QScopedPointer<IVector> endCloned(end->clone());
    if (!endCloned) {
        ILog::report("createCompact: Unable to clone vector end");
        return nullptr;
    }

    QScopedPointer<IVector> correctedStep(
        step ?
        MyCompact::createCustomStep(begin, end, step) :
        MyCompact::createDefaultStep(begin, end));

    if (!correctedStep) {
        return nullptr;
    }

    MyCompact *const compact = new MyCompact(beginCloned.data(), endCloned.data(), correctedStep.data());
    if (!compact) {
        ILog::report("createCompact: Unable to create a compact");
        return nullptr;
    }

    beginCloned.take();
    endCloned.take();
    correctedStep.take();

    return compact;
}

IVector *MyCompact::createDefaultStep(const IVector *const begin, const IVector *const end) {
    const QScopedPointer<const IVector> difference(IVector::subtract(end, begin));
    if (!difference) {
        ILog::report("createCompact: Unable to calculate vectors difference");
        return nullptr;
    }

    unsigned int dim;
    const double *differenceCoords;
    int code = difference->getCoordsPtr(dim, differenceCoords);
    if (code != ERR_OK) {
        ILog::report("createCompact: Unable to get vector coordinates");
        return nullptr;
    }

    QScopedArrayPointer<double> stepCoords(new double[dim]);
    if (!stepCoords) {
        ILog::report("createCompact: Unable to allocate memory");
        return nullptr;
    }
    for (unsigned int i = 0; i < dim; ++i)
        stepCoords[static_cast<int>(i)] = differenceCoords[i] / nDefaultSteps;

    IVector *const step = IVector::createVector(dim, stepCoords.data());
    if (!step) {
        ILog::report("createCompact: Unable to create vector");
        return nullptr;
    }

    return step;
}

IVector *MyCompact::createCustomStep(const IVector *const begin, const IVector *const end, const IVector *const step) {
    const QScopedPointer<const IVector> difference(IVector::subtract(end, begin));
    if (!difference) {
        ILog::report("createCompact: Unable to calculate vectors difference");
        return nullptr;
    }

    bool isLess = false;
    int code = step->lt(difference.data(), IVector::NORM_1, isLess);
    if (code != ERR_OK) {
        ILog::report("createCompact: Unable to compare vectors");
        return nullptr;
    }
    if (!isLess) {
        ILog::report("createCompact: Step must be less than difference of begin and end");
        return nullptr;
    }

    IVector *const stepCloned = step->clone();
    if (!stepCloned) {
        ILog::report("createCompact: Unable to clone a vector");
        return nullptr;
    }

    return stepCloned;
}

int MyCompact::Intersection(const ICompact &c) {
    const MyCompact *pAnother = dynamic_cast<const MyCompact *>(&c);
    if (!pAnother) {
        ILog::report("MyCompact::Intersection: Cannot intersect with another implementation");
        return ERR_ANY_OTHER;
    }
    const MyCompact &another = *pAnother;

    if (mBegin->getDim() != another.mBegin->getDim()) {
        ILog::report("MyCompact::Intersection: Dimensions mesmatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    unsigned int dim;
    const double *beginCoords, *endCoords,
            *anotherBeginCoords, *anotherEndCoords,
            *stepCoords, *anotherStepCoords;
    if (mBegin->getCoordsPtr(dim, beginCoords) != ERR_OK ||
            mEnd->getCoordsPtr(dim, endCoords) != ERR_OK ||
            another.mBegin->getCoordsPtr(dim, anotherBeginCoords) != ERR_OK ||
            another.mEnd->getCoordsPtr(dim, anotherEndCoords) != ERR_OK ||
            mStep->getCoordsPtr(dim, stepCoords) != ERR_OK ||
            another.mStep->getCoordsPtr(dim, anotherStepCoords) != ERR_OK) {
        ILog::report("MyCompact::Intersection: Unable to get vector coordinates");
        return ERR_ANY_OTHER;
    }

    QScopedArrayPointer<double>
            newBeginCoords(new double[dim]),
            newEndCoords(new double[dim]),
            newStepCoords(new double[dim]);
    if (!newBeginCoords || !newEndCoords || !newStepCoords) {
        ILog::report("MyCompact::Intersection: Memory allocation failed");
        return ERR_MEMORY_ALLOCATION;
    }

    bool generateNewStep = false;
    for (unsigned int i = 0; i < dim; ++i) {
        newBeginCoords[static_cast<int>(i)] = qMax(beginCoords[i], anotherBeginCoords[i]);
        newEndCoords[static_cast<int>(i)] = qMin(endCoords[i], anotherEndCoords[i]);
        newStepCoords[static_cast<int>(i)] = qMin(stepCoords[i], anotherStepCoords[i]);

        if (newBeginCoords[static_cast<int>(i)] >= newEndCoords[static_cast<int>(i)]) {
            ILog::report("MyCompact::Intersection: Compacts do not intersect");
            return ERR_ANY_OTHER;
        }

        if (newStepCoords[static_cast<int>(i)] >=
                newEndCoords[static_cast<int>(i)] - newBeginCoords[static_cast<int>(i)])
            generateNewStep = true;
    }

    QScopedPointer<const IVector>
            newBegin(IVector::createVector(dim, newBeginCoords.data())),
            newEnd(IVector::createVector(dim, newEndCoords.data()));
    if (!newBegin || !newEnd) {
        ILog::report("MyCompact::Intersection: Unable to create a vector");
        return ERR_ANY_OTHER;
    }

    QScopedPointer<const IVector>
            newStep(generateNewStep ?
                        createDefaultStep(newBegin.data(), newEnd.data()) :
                        IVector::createVector(dim, newStepCoords.data()));
    if (!newStep) {
        ILog::report("MyCompact::Intersection: Unable to create a vector");
        return ERR_ANY_OTHER;
    }

    delete mBegin;
    delete mEnd;
    delete mStep;

    mBegin = newBegin.take();
    mEnd = newEnd.take();
    mStep = newStep.take();

    return ERR_OK;
}

int MyCompact::Union(const ICompact &c) {
    const MyCompact *pAnother = dynamic_cast<const MyCompact *>(&c);
    if (!pAnother) {
        ILog::report("MyCompact::Union: Cannot unite with another implementation");
        return ERR_ANY_OTHER;
    }
    const MyCompact &another = *pAnother;

    if (mBegin->getDim() != another.mBegin->getDim()) {
        ILog::report("MyCompact::Union: Dimensions mesmatch");
        return ERR_DIMENSIONS_MISMATCH;
    }

    unsigned int dim;
    const double *beginCoords, *endCoords,
            *anotherBeginCoords, *anotherEndCoords,
            *stepCoords, *anotherStepCoords;
    if (mBegin->getCoordsPtr(dim, beginCoords) != ERR_OK ||
            mEnd->getCoordsPtr(dim, endCoords) != ERR_OK ||
            another.mBegin->getCoordsPtr(dim, anotherBeginCoords) != ERR_OK ||
            another.mEnd->getCoordsPtr(dim, anotherEndCoords) != ERR_OK ||
            mStep->getCoordsPtr(dim, stepCoords) != ERR_OK ||
            another.mStep->getCoordsPtr(dim, anotherStepCoords) != ERR_OK) {
        ILog::report("MyCompact::Union: Unable to get vector coordinates");
        return ERR_ANY_OTHER;
    }

    QScopedArrayPointer<double>
            newBeginCoords(new double[dim]),
            newEndCoords(new double[dim]),
            newStepCoords(new double[dim]);
    if (!newBeginCoords || !newEndCoords || !newStepCoords) {
        ILog::report("MyCompact::Union: Memory allocation failed");
        return ERR_MEMORY_ALLOCATION;
    }

    for (unsigned int i = 0; i < dim; ++i) {
        newBeginCoords[static_cast<int>(i)] = qMin(beginCoords[i], anotherBeginCoords[i]);
        newEndCoords[static_cast<int>(i)] = qMax(endCoords[i], anotherEndCoords[i]);
        newStepCoords[static_cast<int>(i)] = qMin(stepCoords[i], anotherStepCoords[i]);
    }

    QScopedPointer<const IVector>
            newBegin(IVector::createVector(dim, newBeginCoords.data())),
            newEnd(IVector::createVector(dim, newEndCoords.data())),
            newStep(IVector::createVector(dim, newStepCoords.data()));
    if (!newBegin || !newEnd || !newStep) {
        ILog::report("MyCompact::Union: Unable to create a vector");
        return ERR_ANY_OTHER;
    }

    delete mBegin;
    delete mEnd;
    delete mStep;

    mBegin = newBegin.take();
    mEnd = newEnd.take();
    mStep = newStep.take();

    return ERR_OK;
}

int MyCompact::SymDifference(const ICompact &c) {
    return Union(c);
}

int MyCompact::MakeConvex() {
    return ERR_OK;
}
