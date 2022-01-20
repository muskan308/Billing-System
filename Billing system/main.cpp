/**
 * @file main.cpp
 * @author Muskan Thapa (thapamuskan771@gmail.com)
 * @brief  Billing system to help employees calculate the bill and apply relevant discounts to the customers. Application integerating MySQL and Cpp.
 * @version 0.1
 * @date 2022-01-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include <windows.h>
#include <mysql.h>
#include <vector>
#include <time.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <ctime>

using namespace std;

void Color(int color){ //function to change the text color
 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

MYSQL* connectiondb(){//connection to mysql
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "M1s2k3n4@", "cppdb", 3306,  NULL, 0);
    if(!conn){
        cout << "NOT CONNECTED";
    }
}

bool login(MYSQL* conn){ //validation of employee
    string email, pass;
    cout << "Enter your UserID: ";
    cin >> email;
    cout << "Enter your password: ";
    cin >> pass;

    MYSQL_RES* res;
    MYSQL_ROW row;

    int qstate = mysql_query(conn, "SELECT * FROM LOGIN");
    if(!qstate){ //qstate = 0 means connected
        res = mysql_store_result(conn);
        while(row = mysql_fetch_row(res)){
            if(row[1] == email && row[2] == pass){
                    system("CLS");
                cout << "-----------Welcome "+ email + "-----------\n-----------We help you to generate bills.------------\n";
                return true;
            }
        }

        return false;
    }
    cout << "Cannot validate\n";
    return false;
}

void purchaseDetails(MYSQL* conn){ //to fetch the details of all purchases
    MYSQL_ROW row;
    MYSQL_RES* res;
    if(conn){
        int qstate = mysql_query(conn, "SELECT * FROM bill_detail");
        if(qstate == 0){
            res = mysql_store_result(conn);
            int count = mysql_num_fields(res);
            cout << "BILL NO.\tNAME\t\tPHONE\t\t\tDATE\t\t\tPRICE\n";
            while(row = mysql_fetch_row(res)){
                for(int i = 0; i < count ; i++){
                    cout << row[i] << "\t\t" ;
                }
                cout << endl;
            }

        }
    }
    else{
        cout << "Failed to fetch...\n";
    }
}

void customerDetails(MYSQL* conn){ //to fetch the details of all the customers
    MYSQL_ROW row;
    MYSQL_RES* res;
    if(conn){
        int qstate = mysql_query(conn, "SELECT * FROM registered_customers");
        if(qstate == 0){
            res = mysql_store_result(conn);
            int count = mysql_num_fields(res);
            cout << "PHONE\t\tNAME\n";
            while(row = mysql_fetch_row(res)){
                for(int i = 0; i < count ; i++){
                    cout << row[i] << "\t" ;
                }
                cout << endl;
            }

        }
    }
    else{
        cout << "Failed to fetch...\n";
    }
}

float selectProduct(MYSQL* conn){ //selecting product and calculating final price
    cout << "************ENTER THE NUMBER CORRESPONDING TO THE ITEMS TO BUY THEN ENTER THE AMOUNT**************\n ";
     MYSQL_RES* res;
    MYSQL_ROW row;

    vector<float> prices;
    int qstate = mysql_query(conn, "SELECT * FROM PRODUCTS");
    if(!qstate){ //qstate = 0 means connected
        res = mysql_store_result(conn);
        int count = mysql_num_fields(res);
        cout << "Sno.\t\tProducts\tPrice(per kg)\n";
        while(row = mysql_fetch_row(res)){
            for(int i = 0; i < count ; i++){
                cout << row[i] <<"\t\t";
            }
            prices.push_back(stof(row[2]));
            cout <<"\n";
        }
    }

    int select;
    int num, amount;
    cout << "\nDo you want to select anything ?(Type 1 for Yes and 0 for No)\n";
    cin >> select;
    float price = 0;
    while(select){
        cout << "Enter the product number : ";
        cin >> num;
        cout << "Enter the amount (in kgs) : ";
        cin >> amount;
        price += prices[num-1]*amount;
        cout << "Select more items ?(0 or 1) ";
        cin >> select;
    }
    cout << "\n\nTotal price :" << price<<"\n" ;
    return price;
}

void Bill(MYSQL* conn, string phone, string name, int discount){ //displaying the bill
    cout <<"\n";
    float price = selectProduct(conn);
    price = price - ((price*discount)/100);
    stringstream ss;
    ss << "INSERT INTO bill_detail (name,phone,date,purchase_amount) values('"+name+"',"+phone+",curdate(),"+to_string(price)+")"; //inserting details in customer table
    string query = ss.str();
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);
    if(!qstate){
        name[0] = toupper(name[0]);
        time_t timetoday;
        time (&timetoday);
        cout << "\n\nDATE : "<< asctime(localtime(&timetoday));
        cout << "NAME : " << name <<"\n";
        cout << "PHONE NUMBER : " <<phone <<"\n";
        cout << "TOTAL (WITH DISCOUNT APPLICABLE) : "<< price;
        Color(2);
        cout << "\n\n---------------------THANK YOU FOR SHOPPING WITH US-------------------\n";
        Color(7);
    }
}

void customerInfo(MYSQL* conn){ //storing customer info
    Color(7);
    srand(time(0));
    string name;
    string phone;
    cout << "Enter customer details--\n\n";
    cout << "Enter the name of the customer: ";
    cin >> name;
    cout << "Enter the phone number: ";
    cin >> phone;
    bool invalid = false; //bool to check validity of mobile number
    if(phone.length() != 10){
            Color(4);
        cout << "INVALID PHONE NUMBER ENTERED\n";
        invalid = true;
        customerInfo(conn);
    }

    transform(name.begin(),name.end(),name.begin(),::tolower);//converting input name to lowercase
    MYSQL_RES* res;
    MYSQL_ROW row;
    //bool flag = false;
    int check = mysql_query(conn, "Select * from registered_customers");//to check if the customer is already registered
    if(!check && !invalid){ //check = 0 means connected
        res = mysql_store_result(conn);
        while(row = mysql_fetch_row(res)){
            string n = row[1];
            transform(n.begin(),n.end(),n.begin(),::tolower); //converting name to lowercase
            if(row[0] == phone){
                name[0] = toupper(name[0]);
                cout << "----------------------------------------------------------------------------------------\n\n\n";
                Color(2);
                cout << "Registered Customer\n";
                Color(7);
                cout << "Discount of 10% is applicable.\n";
                row[1][0] = toupper(row[1][0]);
                cout << "REGISTERED NAME : " << row[1] << "\n";
                cout << "PHONE : " << row[0] << "\n";
                Bill(conn, phone, name, 10);
                return;
            }
        }

        stringstream pp;
        pp << "INSERT INTO REGISTERED_CUSTOMERS values('"+phone+"','"+name+"')"; //inserting details in registered customer table
        string query2 = pp.str();
        const char* q2 = query2.c_str();
        int qstate2 = mysql_query(conn, q2);
        if(!qstate2){
            Color(2);
            cout << "Customer Successfully Registered.\n";
            name[0] = toupper(name[0]);
            cout << "NAME : "<<name << "\n";
            cout << "PHONE : "<<phone <<"\n";
            Color(7);
            Bill(conn, phone, name, 0);
            return;
        }
        else{
            cout << "Error\n";
        }
    }
}

void first(MYSQL * conn){//main interface
    cout << "*********WELCOME TO THE SHOP*********\n";
    bool lg = login(conn); //returns true if validated
    int i = 0;
    while(!lg && i<4){ //loop till i==4 giving 4 more chances after one wrong
        cout << "---------Wrong Password---------\n";
        if(login(conn))
            break;
        i++;
    }
    if(i == 4){ //allows only 5 tries
        cout << "\n.\n.\n.\n.\nSo many tries. Try again later...\n";
        return;
    }

    cout << "SELECT 1 TO GET CUSTOMER DETAILS.\n";
    cout << "SELECT 2 TO GET THE DETAILS OF ALL BILLS\n";
    cout << "SELECT 3 TO GENERATE NEW BILL\n";
    int s;
    cin >> s;
    switch(s){
        case 1: customerDetails(conn);
                break;
        case 2: purchaseDetails(conn);
                break;
        case 3: customerInfo(conn); //collecting user info
                break;
    }

}

int main() //main function
{
    MYSQL* conn = connectiondb();//db connnection
    first(conn);//first interface
    //customerInfo(conn);
    return 0;
}
