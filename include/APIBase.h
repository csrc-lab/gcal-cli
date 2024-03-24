#ifndef APIBASE_H
#define APIBASE_H

class ApiBase {
public:
    virtual void list() = 0;
    virtual void create() = 0;
};

#endif // APIBASE_H