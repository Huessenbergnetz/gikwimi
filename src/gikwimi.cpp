#include "gikwimi.h"

#include "root.h"

using namespace Cutelyst;

gikwimi::gikwimi(QObject *parent) : Application(parent)
{
}

gikwimi::~gikwimi()
{
}

bool gikwimi::init()
{
    new Root(this);

    return true;
}

