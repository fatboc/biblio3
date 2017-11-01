#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <sstream>
#include "templates.hpp"

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
    time_t pozyczona;

    void print();
    void print(WINDOW *);
    void modify(vector<string>& data);
    size_t find_text (string text);
    bool check();
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
    bool check();
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
    bool check();
    void item_export(ostream &out);
    string new_choice();
};

//user.cpp

int create_menu(WINDOW *, vector<string> &, string, string, bool);
int dialog(vector<string> &, string, string);
int find_longest(vector<string> &);
int init();
int menu_main(WINDOW* window, vector<Kategoria*> &kategorie, vector<Ksiazka*>& ksiazki, vector<Klient*> &klienci);
void zapisz(vector<Kategoria*> &kategorie, vector<Ksiazka*> &ksiazki, vector<Klient*> &klienci);

//data.cpp

int data_import(vector <Kategoria*> &kategorie, vector <Klient*> &klienci, vector <Ksiazka*> &ksiazki);
void add_book(vector<Ksiazka*> &ksiazki, vector<Kategoria*> &kategorie, vector<string> &data);
void add_client(vector<Klient*> &klienci, vector<string> &data);
void add_category(vector<Kategoria*> &kategorie, vector<string> &data);
int menu_kategorie(WINDOW * window, vector <Kategoria*>& kategorie);
int menu_klienci(WINDOW * window, vector <Klient*> &klienci);
int menu_ksiazki(WINDOW * window, vector <Ksiazka*> &ksiazki, vector <Kategoria*> &kategorie, vector <Klient*> klienci);
int data_export(vector<Kategoria*> &kategorie, vector<Ksiazka*> &ksiazki, vector<Klient*> &klienci);
void cleanup(WINDOW *window, vector<Kategoria*> &kategorie, vector<Klient*> &klienci, vector<Ksiazka*> &ksiazki);
