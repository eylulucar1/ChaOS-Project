#ifndef DRIVERS_DRIVER_H
#define DRIVERS_DRIVER_H

#include <common/types.h>

class Driver {
public:
    Driver();
    ~Driver();

    virtual void Activate();
    virtual int Reset();
    virtual void Deactivate();
};

class DriverManager {
private:
    Driver* drivers[256];
    int numDrivers;

public:
    DriverManager();
    ~DriverManager();

    void AddDriver(Driver* drv);
    void ActivateAll();
};

#endif