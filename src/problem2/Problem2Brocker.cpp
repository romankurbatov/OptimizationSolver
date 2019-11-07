#include "IBrocker.h"
#include "Problem2.h"
#include "SHARED_EXPORT.h"
#include "ILog.h"
#include "error.h"

class Problem2Brocker : public IBrocker
{
public:
    Problem2Brocker(Problem2 *problem);

    int getId() const override;

    bool canCastTo(Type type) const override;
    void* getInterfaceImpl(Type type) const override;

    int release() override;

protected:
    ~Problem2Brocker() override;

private:
    /*non default copyable*/
    Problem2Brocker(const Problem2Brocker& other) = delete;
    void operator=(const Problem2Brocker& other) = delete;

    Problem2 *mProblem;
};

Problem2Brocker::Problem2Brocker(Problem2 *problem) :
    mProblem(problem)
{
}

Problem2Brocker::~Problem2Brocker() {
    delete mProblem;
}

int Problem2Brocker::getId() const {
    return IBrocker::INTERFACE_0;
}

bool Problem2Brocker::canCastTo(Type type) const {
    return (type == IBrocker::PROBLEM);
}

void *Problem2Brocker::getInterfaceImpl(Type type) const {
    if (type != IBrocker::PROBLEM) {
        ILog::report("Problem2Brocker::getInterfaceImpl: Incorrect type");
        return nullptr;
    }

    return mProblem;
}

int Problem2Brocker::release() {
    delete this;
    return ERR_OK;
}

extern "C" SHARED_EXPORT void *getBrocker() {
    Problem2 *problem = new Problem2();
    if (!problem) {
        ILog::report("getBrocker: Unable to create problem");
        return nullptr;
    }

    Problem2Brocker *brocker = new Problem2Brocker(problem);
    if (!brocker) {
        ILog::report("getBrocker: Unable to create brocker");
        delete problem;
        return nullptr;
    }

    return brocker;
}
