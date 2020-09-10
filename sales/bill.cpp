#include "bill.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <dirent.h>
#include "PCH.hpp"
#include <cstdlib>
#include "menu.h"
#include "order.hpp"

using namespace std;

/**********************
 * BILL MANAGER
**********************/
bill *bill_manager::FindBill(const string &bill_no)
{
    for (auto &i : bills)
    {
        if (bill_no == i->getBillNo())
            return i;
    }
    ERROR_LOG *log = log->instantiate();
    dirent *ent;
    stringstream path;
    for (int i = 1; i <= 12; ++i)
    {
        path << "../restaurant/bill/" << i;
        DIR *dir = opendir(path.str().c_str());
        if (dir == NULL)
        {
            closedir(dir);
            throw "Missing related folders";
        }
        readdir(dir);
        readdir(dir);
        while ((ent = readdir(dir)) != NULL)
        {
            string voucher_name = ent->d_name;
            date tmp;
            bill *bill_t = nullptr;
            try
            {
                //Remove expired voucher list
                if (tmp >= ConvertFromString(voucher_name))
                {
                    string path = "../restaurant/voucher/";
                    path += voucher_name;
                    remove(path.c_str());
                }
                voucher_t = new voucher(voucher_name);
                vouchers.emplace_back(voucher_t);
            }
            catch (const char *msg)
            {
                log->LOG(msg);
                delete voucher_t;
            };
        }
        closedir(dir);
    }
}

bill *bill_manager::NewBill()
{
    // order *order_system = order::instantiate();
    bill *new_bill = new bill;
    if (bills.size() >= 100)
        this->~bill_manager();
    int dish;
    do
    {
        system("cls");
        Menu *rest_menu = rest_menu->instantiate();
        rest_menu->output();
        cout << "0. Finalize order\n";
        cout << "-1. Cancel order\n";
        cout << "Option: ";
        int opt;
        while (!(cin >> dish) || dish < 1 || dish > rest_menu->getMenu().size() + 1)
        {
            cout << "Invalid!\n";
            cout << "Try again: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        do
        {
            system("cls");
            cout << rest_menu->getMenu()[dish - 1]->getID();
            cout << " " << rest_menu->getMenu()[dish - 1]->getName() << endl;
            cout << "1. Add\n";
            cout << "2. Remove\n";
            cout << "0. Exit\n";
            cout << "Option: ";
            while (!(cin >> opt) || opt < 0 || opt > 2)
            {
                cout << "Invalid!\n";
                cout << "Try again: ";
                cin.clear();
                cin.ignore(1000, '\n');
            }
            if (opt == 1)
            {
                new_bill->AddDish(rest_menu->getMenu()[dish - 1]->getID(),
                                  rest_menu->getMenu()[dish - 1]->getName(), rest_menu->getMenu()[dish - 1]->getPrice());
                UpdateDishQuant(dish - 1, opt);
            }
            else if (opt == 2)
            {
                if (new_bill->RemoveDish(rest_menu->getMenu()[dish - 1]->getID(), rest_menu->getMenu()[dish - 1]->getPrice()))
                    UpdateDishQuant(dish - 1, opt);
                else
                {
                    system("cls");
                    cout << "This dish has been removed!\n";
                    system("pause");
                }
            }
        } while (opt);
    } while (dish && dish != -1);
    if (dish == -1)
        delete new_bill;
    else
    {
        bills.emplace_back(new_bill);
        return new_bill;
    }
    return nullptr;
}

bill_manager::bill_manager()
{
    ifstream file("../restaurant/DishOrdered");
    if (!file.is_open())
        throw "File DishOrdered is missing!";
    int tmp;
    while (!file.eof())
    {
        file >> tmp;
        quantity.emplace_back(tmp);
    }
    quantity.shrink_to_fit();
    file.close();
}

void bill_manager::updateQuantNewDish(const int &pos, const int &mode)
{
    if (mode)
        quantity.emplace(quantity.begin() + pos, 0);
    else
        quantity.erase(quantity.begin() + pos);
}

bill_manager::~bill_manager()
{
    ofstream file("../restaurant/DishOrdered");
    for (int i = 0; i < quantity.size() - 1; ++i)
        file << i << endl;
    file << quantity[quantity.size() - 1];
    file.close();
    for (auto &i : bills)
        delete i;
}

void bill_manager::UpdateDishQuant(const int &index, const int &mode)
{
    if (mode == 1)
        ++quantity[index];
    else
        --quantity[index];
}

/**********************
 * BILL
**********************/
u_int bill::count = 0;

bill::bill()
{
    Total = 0;
    date tmp;
    bill_no += tmp.d;
    bill_no += tmp.m;
    u_int count_t = count;
    int count_l = 0;
    while (count_t)
    {
        count_t /= 10;
        ++count_l;
    }
    count_l -= 6 - count_l;
    for (int i = 0; i < count_l; ++i)
        bill_no += "0";
    bill_no += count;
}

bill::~bill()
{
    stringstream path;
    path << "../restaurant/bill/" << Date.m;
    path << Date << "-" << bill_no;
    ofstream file(path.str());
    file << "BILL NO: " << bill_no << endl;
    file << "DATE: " << Date << endl;
    file << "TIME: " << Date.GetTime() << endl;
    file << "DISHES:\n";
    file << "NO" << setw(5) << "ID" << setw(15) << "NAME" << setw(35) << "QUANTITY" << setw(10) << "PRICE" << endl;
    for (int i = 1; i <= dish_IDs.size(); ++i)
    {
        cout << i << "." << setw(3) << dish_IDs[i];
        cout << setw(12) << dish_names[i];
        cout << setw(30) << quantity[i];
        cout << setw(10) << total_per_dish[i] << endl;
    }
    file << endl
         << "TOTAL: " << Total;
    file.close();
}
//     // time_t present = time(0);
//     // tm current;
//     // localtime_s(&current, &present);
//     double Total = 0, total;

//     string month = to_string(1 + current.tm_mon);
//     fstream fout(path + "/" + month + "/income.txt", ios::out);
//     string month = to_string(1 + current.tm_mon);
//     string day = to_string(current.tm_mday);
//     day.append(".txt");
//     fstream fout1(path + "/" + month + "/" + day, ios::app);
//     for (int i = 0; i < dish_names.size(); i++)
//     {
//         fout1 << endl
//               << dish_names[i] << ";" << quantity[i] << ";" << total_per_dish[i] << ";";
//     }
//     fout1 << Total << endl;
//     fout1.close();

//     for (int i = 0; i < 31; i++)
//     {

//         string day = to_string(i + 1);
//         day.append(".txt");
//         ifstream fin(path + "/" + month + "/" + day);
//         if (fin.is_open())
//         {
//             while (!fin.eof())
//             {
//                 string line;
//                 getline(fin, line);
//                 getline(fin, line);
//                 fin >> total;
//                 Total += total;
//             }
//         }
//         fin.close();
//     }
//     fout << "Income of the month: " << Total;

void bill::AddDish(const string &ID, const string &name, const double &price)
{
    int id_size = this->dish_IDs.size();
    for (int i = 0; i < id_size; ++i)
    {
        if (dish_IDs[i] == ID)
        {
            total_per_dish[i] += price;
            Total += price;
            ++quantity[i];
            return;
        }
    }
    dish_names.emplace_back(name);
    dish_IDs.emplace_back(ID);
    quantity.emplace_back(1);
}

bool bill::RemoveDish(const string &ID, const double &price)
{
    int id_size = this->dish_IDs.size();
    for (int i = 0; i < id_size; ++i)
    {
        if (dish_IDs[i] == ID)
        {
            if (quantity[i] == 1)
            {
                dish_IDs.erase(dish_IDs.begin() + i);
                dish_names.erase(dish_names.begin() + i);
                total_per_dish.erase(total_per_dish.begin() + i);
            }
            else
            {
                --quantity[i];
                total_per_dish[i] -= price;
            }
            Total -= price;
            return true;
        }
    }
    return false;
}

// void bill::GenerateBill()
// {
//     string path = "../restaurant/bill/";
//     double Total = 0;
//     time_t present = time(0);
//     tm current;
//     localtime_s(&current, &present);
//     cout << current.tm_hour << ":" << current.tm_min << " " << current.tm_mday << "/" << 1 + current.tm_mon << "/" << 1900 + current.tm_year << endl;

//     for (int i = 0; i < dish_names.size(); i++)
//     {
//         cout << dish_names[i] << "\t\t" << quantity[i] << "\t\t" << total_per_dish[i] << endl;

//         Total += total_per_dish[i];
//     }
//     cout << "Total: " << Total;
// }

void bill::DisplayBill()
{
    cout << "BILL NO: " << bill_no << endl;
    cout << "DATE: " << Date << endl;
    cout << "DISHES:\n";
    cout << "NO" << setw(5) << "ID" << setw(15) << "NAME" << setw(35) << "QUANTITY" << setw(10) << "PRICE" << endl;
    for (int i = 1; i <= dish_IDs.size(); ++i)
    {
        cout << i << "." << setw(5) << dish_IDs[i];
        cout << setw(15) << dish_names[i];
        cout << setw(30) << quantity[i];
        cout << setw(8) << total_per_dish[i] << endl;
    }
    cout << endl
         << "TOTAL: " << Total;
}

const string &bill::getBillNo() const
{
    return bill_no;
}
