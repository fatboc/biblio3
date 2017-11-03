#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <sstream>
#include <time.h>
#include "templates.hpp"

#define MIN 60
#define HR 3600
#define DAY 86400

using namespace std;

typedef struct Ksiazka Ksiazka;
typedef struct Klient Klient;
typedef struct Kategoria Kategoria;

struct Ksiazka
{
    string tytul;
    string autor;
    string rok_wydania;
    int id;
    bool dostepnosc;
    Klient * wypozyczajacy;
    Kategoria * kat;
    long int pozyczona;

    void print();
    void print(WINDOW *);
    void modify(vector<string>& data);
    size_t find_text (string text);
    bool check(int);
    void item_export(ostream &out);
    string new_choice();
};

struct Klient
{
    string imie;
    string nazwisko;
    string adres;
    string telefon;
    int id;
    vector <Ksiazka*> pozyczone;

    void print();
    void print(WINDOW *);
    void modify(vector<string>& data);
    size_t find_text(string text);
    bool check(int);
    void item_export(ostream &out);
    string new_choice();
};

struct Kategoria
{
    string symbol;
    string nazwa;
    vector <Ksiazka*> nalezace;
    int id;

    void print();
    void print(WINDOW *);
    void modify(vector<string>& data);
    size_t find_text(string text);
    void item_export(ostream &out);
    string new_choice();
};

//user.cpp

int create_menu(WINDOW *, vector<string> &, string, string, bool);
int dialog(vector<string> &, string, string);
int find_longest(vector<string> &);
int init();
int menu_main(WINDOW* window, vector<Kategoria*> &, vector<Ksiazka*>&, vector<Klient*> &);
int menu_kategorie(WINDOW *, vector <Kategoria*>&, vector<Ksiazka*> &, vector<Klient*> &);
int menu_klienci(WINDOW *, vector <Klient*> &, vector <Kategoria*>&);
int menu_ksiazki(WINDOW *, vector <Ksiazka*> &, vector <Kategoria*> &, vector <Klient*>&);
void zapisz(vector<Kategoria*> &, vector<Ksiazka*> &, vector<Klient*> &);
string search_form(string);
int item_form(WINDOW *, string, vector<string> &, vector<string> &, vector<string> &);
void elo();

//data.cpp

int data_import(vector <Kategoria*> &, vector <Klient*> &, vector <Ksiazka*> &);
int add_book(vector<Ksiazka*> &, vector<Kategoria*> &, vector<string> &);
int add_client(vector<Klient*> &, vector<string> &);
int add_category(vector<Kategoria*> &, vector<string> &);
int data_export(vector<Kategoria*> &, vector<Ksiazka*> &, vector<Klient*> &);
void cleanup(WINDOW *, vector<Kategoria*> &, vector<Klient*> &, vector<Ksiazka*> &);
char * trim(char *);

