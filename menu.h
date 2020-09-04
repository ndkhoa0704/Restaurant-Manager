#ifndef _MENU_H_
#define _MENU_H_
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "dish.h"
#include "essentials.hpp"

using namespace std;

class Menu {
	vector<Dish*> menu;
public:
	//void input();
	void output();
	void save();
	void load();
	void add(Dish* a);
	const vector<Dish*> getMenuc();
	vector<Dish*> getMenu();
	void newDish();
	void removeDish();
	void changDish();

};
#endif