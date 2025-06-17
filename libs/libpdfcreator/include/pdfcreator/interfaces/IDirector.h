#ifndef IDIRECTOR_H
#define IDIRECTOR_H

#include "IBuilder.h"

class IDirector {
public:
    virtual ~IDirector() = default;

    virtual void CreateDocument() = 0;
    virtual void SetBuilder(IBuilder& builder) = 0;
};

#endif //IDIRECTOR_H
