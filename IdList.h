
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct varInfo{

     string name;
     string data_type;
     string value;
     string definedFrom;
     bool const_var;
    int line_number;

};

 struct fctInfo{
    string name;
    string ret_type;
    string param_type;
    string definedFrom;
    bool const_fct;
    int line_number;
 };

struct classInfo{
    string name;
    int line_number;
};



class IdList {
    
    //tabel pentru variabile
    vector<varInfo> var_table;
    //tabel pentru functii
    vector<fctInfo> fct_table;
    //table pentru clase
    vector<classInfo> class_table;

  
    

    public:
    bool existsVar( const char* s, const char* defFrom);
    bool existsFct( const char* fct_name);
    void addVar( const char* name, const char* data_type, const char* value, char* definedFrom, bool const_var); //adauga o variabila in tabel
    //adaugam si constante in tabel deoarece avem nevoie de tipul lor atunci cand evaluam tipul unei expresii
    void addConstant( const char* name, const char* data_type, const char* value, bool const_var); //adauga o valoare constanta in tabel ("true", "cuvant", 4, 5.2)
    void addFct(const char* name, const char* ret_type, const char* param_type, const char* definedFrom, bool const_fct); //adauga o functie in tabel
    void printVarsTable(); //printeaza tabelul cu variabile
    void printFctsTable(); //printeaza tabelul cu functii
    void yyerror(const char* s);
    void isNotDefinedFct(const char* s); //verifica daca o functie apelata a fost deja declarata
    void isNotDefinedVar(const char* s, char* def_func_var); //verifica daca o variabila folosita a fost deja declarata
    void parFct(const char* type,const char* name_var,const char* name_fct); //verifica paramentrii unei functii si ii adauga in addFct
    bool existsClass(const char* className); //verifica daca o clasa a fost definita
    void addClass(const char* name);
    void verifClassObj(const char* clsName); //verifica daca o clasa a fost declarata sau nu la declararea unui obiect: ID ID (Masina m)
    void verifTypeVar(const char*id, const char* def_func_var, string typeExpr); //verifica daca tipul variabilei din stanga e acelasi cu expresia din dreapta (cauta la fel ca la typeOfVar)
    string typeOfVar(char *var, const char* def_func_var); //returneaza tipul unei variablie sau unei constante (mai intai cauta in fct daca e definita, apoi daca e global)
    string typeOf(char *expr, const char* def_func_var); //returneaza tipul unei expresii
    bool exprOk(string rez); //verifica daca typeOf returneaza un tip sau ca expresia nu e corecta
    void verifExpr(string rez); //verifica daca toate variabilele din expresie sunt de acelasi tip sau nu
    void typeOfError(string rez);//arunca eroare pentru typeof
    ~IdList();
};
