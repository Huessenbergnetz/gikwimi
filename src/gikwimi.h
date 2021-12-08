#ifndef GIKWIMI_H
#define GIKWIMI_H

#include <Cutelyst/Application>

using namespace Cutelyst;

class gikwimi : public Application
{
Q_OBJECT
CUTELYST_APPLICATION(IID "gikwimi")
public:
    Q_INVOKABLE explicit gikwimi(QObject *parent = nullptr);
    ~gikwimi();

    bool init() override;
};

#endif // GIKWIMI_H
