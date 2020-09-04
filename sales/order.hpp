#ifndef ORDER_HPP
#define ORDER_HPP

#include <vector>
#include <list>
#include "bill.hpp"

using namespace std;

class order
{
private:
    order();
    static order *instance;
    vector<bill *> bills;
    list<bill *> orders;
    vector<int> quantity;

public:
    ~order();
    order(const order &other) = delete;
    order &operator=(const order &other) = delete;
    static order *instantiate();
    void NewOrder();
    void UpdateDishQuant(const int &index, const int &mode);
    void update(const int &pos, const int &mode);
    const vector<int> &getOrderedDishQuantity();
    bool CompleteOrderInQueue();
    void displayNewestOrder();
    void displayOldestOrder();
    void ListCurrentOrders();
};

#endif