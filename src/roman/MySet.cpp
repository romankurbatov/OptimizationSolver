#include "ISet.h"

#include <algorithm>
#include <limits>
#include <QVector>
#include <QList>

#include "ILog.h"
#include "IVector.h"

namespace {

class MySet : public ISet {
public:
    int getId() const override;

    int put(IVector const *const item) override;
    int get(unsigned int index, IVector *&pItem) const override;
    int remove(unsigned int index) override;
    int contains(IVector const *const pItem, bool &rc) const override;
    unsigned int getSize() const override;
    int clear() override;

    class MyIterator : public IIterator
    {
    public:
        int next() override;
        int prev() override;
        bool isEnd() const override;
        bool isBegin() const override;

        int pos() const;
        void setPos(int pos);

        MyIterator(MySet const *const set, int pos);
        ~MyIterator() override;

    private:
        /*non default copyable*/
        MyIterator(const IIterator &other) = delete;
        void operator=(const IIterator &other) = delete;
        MySet const *const mSet;
        int mPos;
    };

    MyIterator *end() override;
    MyIterator *begin() override;

    int deleteIterator(IIterator *pIter) override;
    int getByIterator(IIterator const *pIter, IVector *&pItem) const override;

    MySet(size_t dim);
    ~MySet() override;

    int firstIndex() const;
    int nextIndex(int index) const;
    int prevIndex(int index) const;

    static const int indexAfterLast = std::numeric_limits<int>::max();

private:
    void maintainIteratorsAfterRemoval(int removedIndex);

    QVector<IVector *> mData;
    const size_t mDim;
    QList<MyIterator *> mIterators;
};

}

ISet* ISet::createSet(unsigned int R_dim) {
    if (R_dim == 0) {
        ILog::report("ISet::createSet: Dimension cannot be 0");
        return nullptr;
    }

    MySet *const set = new MySet(R_dim);
    if (!set) {
        ILog::report("ISet::createSet: Unable to create a set");
        return nullptr;
    }

    return set;
}

MySet::MySet(size_t dim) : mDim(dim) {
}

MySet::~MySet() {
    clear();
}

int MySet::getId() const {
    return ISet::INTERFACE_0;
}

int MySet::put(IVector const *const item) {
    if (!item) {
        ILog::report("MySet::put: New item cannot be null");
        return -1;
    }

    if (item->getDim() != mDim) {
        ILog::report("MySet::put: Dimensions mismatch");
        return -1;
    }

    for (int i = 0; i < mData.size(); ++i) {
        if (!mData[i])
            continue;

        bool isEqual;
        int code = mData[i]->eq(item, IVector::NORM_INF, isEqual, 0);
        if (code != ERR_OK) {
            ILog::report("MySet::put: Unable to compare vectors");
            return -1;
        }
        if (isEqual)
            return i;
    }

    IVector *const cloned = item->clone();
    if (!cloned) {
        ILog::report("MySet::put: Unable to clone the vector");
        return -1;
    }

    mData.append(cloned);
    return mData.size() - 1;
}

int MySet::get(unsigned int index, IVector *&pItem) const {
    if (index >= static_cast<unsigned int>(mData.size())) {
        ILog::report("MySet::get: Index is out of range");
        return ERR_OUT_OF_RANGE;
    }

    if (!mData[static_cast<int>(index)]) {
        ILog::report("MySet::get: Item has been deleted");
        return ERR_ANY_OTHER;
    }

    pItem = mData[static_cast<int>(index)]->clone();
    if (!pItem) {
        ILog::report("MySet::get: Unable to clone the vector");
        return ERR_ANY_OTHER;
    }

    return ERR_OK;
}

int MySet::remove(unsigned int index) {
    if (index >= static_cast<unsigned int>(mData.size())) {
        ILog::report("MySet::remove: Index is out of range");
        return ERR_OUT_OF_RANGE;
    }

    if (!mData[static_cast<int>(index)]) {
        ILog::report("MySet::remove: Item is already deleted");
        return ERR_ANY_OTHER;
    }

    delete mData[static_cast<int>(index)];
    mData[static_cast<int>(index)] = nullptr;
    maintainIteratorsAfterRemoval(static_cast<int>(index));

    return ERR_OK;
}

void MySet::maintainIteratorsAfterRemoval(int removedIndex) {
    int nextExisting = removedIndex;
    while (nextExisting < mData.size() && !mData[nextExisting])
        ++nextExisting;
    if (nextExisting == mData.size())
        nextExisting = indexAfterLast;

    foreach (MyIterator * const iter, mIterators)
        if (iter->pos() == removedIndex)
            iter->setPos(nextExisting);
}

int MySet::contains(IVector const *const pItem, bool &rc) const {
    rc = false;

    foreach (IVector const *const v, mData) {
        if (!v)
            continue;

        bool isEqual;
        int code = v->eq(pItem, IVector::NORM_INF, isEqual, 0);
        if (code != ERR_OK) {
            ILog::report("MySet::contains: Unable to compare vectors");
            return code;
        }
        if (isEqual) {
            rc = true;
            break;
        }
    }

    return ERR_OK;
}

unsigned int MySet::getSize() const {
    return static_cast<unsigned int>(
        std::count_if(mData.constBegin(), mData.constEnd(),
            [](IVector const *const v) {
                return (v != nullptr);
            }));
}

int MySet::clear() {
    qDeleteAll(mData);
    mData.clear();
    foreach (MyIterator *iter, mIterators)
        iter->setPos(indexAfterLast);
    return ERR_OK;
}

MySet::MyIterator *MySet::end() {
    MyIterator *iter = new MyIterator(this, indexAfterLast);
    if (!iter) {
        ILog::report("MySet::end: Unable to create an iterator");
        return nullptr;
    }
    mIterators.append(iter);
    return iter;
}

MySet::MyIterator *MySet::begin() {
    int i = 0;
    while(i < mData.size() && !mData[i])
        ++i;
    if (i == mData.size())
        i = indexAfterLast;

    MyIterator *iter = new MyIterator(this, i);
    if (!iter) {
        ILog::report("MySet::begin: Unable to create an iterator");
        return nullptr;
    }

    mIterators.append(iter);
    return iter;
}

int MySet::deleteIterator(IIterator *pIter) {
    if (!pIter) {
        ILog::report("MySet::deleteIterator: Iterator cannot be null");
        return ERR_WRONG_ARG;
    }

    QList<MyIterator *>::ConstIterator iter_iter =
            std::find(mIterators.constBegin(), mIterators.constEnd(),
                         reinterpret_cast<const MyIterator *>(pIter));
    if (iter_iter == mIterators.constEnd()) {
        ILog::report("MySet::deleteIterator: Iterator not found");
        return ERR_WRONG_ARG;
    }

    delete *iter_iter;

    mIterators.removeAll(reinterpret_cast<MyIterator *>(pIter));

    return ERR_OK;
}

int MySet::getByIterator(const IIterator *pIter, IVector *&pItem) const {
    if (!pIter) {
        ILog::report("MySet::getByIterator: Iterator cannot be null");
        return ERR_WRONG_ARG;
    }

    QList<MyIterator *>::ConstIterator iter_iter =
            std::find(mIterators.constBegin(), mIterators.constEnd(),
                         reinterpret_cast<const MyIterator *>(pIter));
    if (iter_iter == mIterators.constEnd()) {
        ILog::report("MySet::getByIterator: Iterator not found");
        return ERR_WRONG_ARG;
    }

//    int iterIndex = mIterators.indexOf(reinterpret_cast<const MyIterator *>(pIter));
//    if (iterIndex < 0) {
//        ILog::report("MySet::getByIterator: Iterator not found");
//        return ERR_WRONG_ARG;
//    }

    const int pos = (*iter_iter)->pos();

    if (!mData[pos]) {
        ILog::report("MySet::getByIterator: MySet internal error: iterator points to a deleted item");
        return ERR_ANY_OTHER;
    }

    pItem = mData[pos]->clone();
    if (!pItem) {
        ILog::report("MySet::getByIterator: Unable to clone the vector");
        return ERR_ANY_OTHER;
    }

    return ERR_OK;
}

int MySet::firstIndex() const {
    int index = 0;

    while (index < mData.size() && !mData[index])
        ++index;

    if (index == mData.size())
        index = indexAfterLast;

    return index;
}

int MySet::nextIndex(int index) const {
    if (index >= mData.size())
        return indexAfterLast;
    if (index < 0)
        index = 0;
    ++index;
    while (index < mData.size() && !mData[index])
        ++index;
    if (index == mData.size())
        index = indexAfterLast;
    return index;
}

int MySet::prevIndex(int index) const {
    if (index >= mData.size()) {
        index = mData.size() - 1;
        while (index >= 0 && !mData[index])
            --index;
        if (index < 0)
            index = indexAfterLast;
        return index;
    }

    --index;
    while (index >= 0 && !mData[index])
        --index;
    if (index < 0)
        index = firstIndex();
    return index;
}

ISet::IIterator::IIterator(ISet const *const, int) {
}

MySet::MyIterator::MyIterator(MySet const *const set, int pos) :
    IIterator(set, pos),
    mSet(set),
    mPos(pos)
{
}

MySet::MyIterator::~MyIterator() {
}

int MySet::MyIterator::pos() const {
    return mPos;
}

void MySet::MyIterator::setPos(int pos) {
    mPos = pos;
}

int MySet::MyIterator::next() {
    mPos = mSet->nextIndex(mPos);
    return ERR_OK;
}

int MySet::MyIterator::prev() {
    mPos = mSet->prevIndex(mPos);
    return ERR_OK;
}

bool MySet::MyIterator::isBegin() const {
    return (mPos == mSet->firstIndex());
}

bool MySet::MyIterator::isEnd() const {
    return (mPos == MySet::indexAfterLast);
}
