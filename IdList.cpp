#include "IdList.h"
#include <vector>
#include <string>
#include <cstring>
using namespace std;
extern int yylineno;
extern int count_lines;
int var_used_line; // linia unde se afla variabila deja declarata
int fct_used;
int count = 0;     // pt table[]
int count_fct = 0; // pt t_fct[]
// int errors = 0;
int fct_used_line;   // linia unde se afla functia declarata prima data
int class_used_line; // linia unde se afla clasa declarata prima data

void IdList::yyerror(const char *s)
{
    printf("%s -> eroare la linia: %d\n", s, yylineno);
    // errors = 1;
}

// nu mai verificam daca o constanta a fost declarata deja in tabel, putem avea constante la fel in program
void IdList::addConstant(const char *name, const char *data_type, const char *value, bool const_var)
{

    int line_number = yylineno;
    varInfo var = {string(name), string(data_type), string(value), "global", const_var, line_number};
    var_table.push_back(var);
}

void IdList::addVar(const char *name, const char *data_type, const char *value, char *definedFrom, bool const_var)
{
    // tratam cazul in care avem definit un obiect de tipul unei clase (acesta poate fi declarat global sau in main)
    string clsName = string(data_type);
    int data_type_class = 0;
    string defFrom = string(definedFrom);

    for (classInfo &c : class_table)
    {
        if (clsName == c.name)
        {
            data_type_class = 1;
        }
    }
    if (data_type_class == 1)
    {
        if (defFrom == "global" || defFrom == "main")
        {
            if (!existsVar(name, definedFrom))
            {

                int line_number = yylineno;
                if (const_var)
                { // valoarea nu e constanta

                    string t = string(data_type);
                    t = "const " + t;
                    varInfo var = {string(name), t, string(value), string(definedFrom), const_var, line_number};
                    var_table.push_back(var);
                }
                else
                {
                    varInfo var = {string(name), string(data_type), string(value), string(definedFrom), const_var, line_number};
                    var_table.push_back(var);
                }
            }
            else
            {
                char s[200];
                sprintf(s, "Variabila <%s> este deja declarata la linia %d", name, var_used_line);
                yyerror(s);
                exit(0);
            }
        }
        else
        {
            char s[200];
            sprintf(s, "Obiectul <%s> nu poate fi declarat decat in global sau in functia main\n", name);
            yyerror(s);
            exit(0);
        }
    }
    else
    { // cazul pentru o variabila declarata normal
        if (!existsVar(name, definedFrom))
        {

            int line_number = yylineno;
            if (const_var)
            { // valoarea nu e constanta

                string t = string(data_type);
                t = "const " + t;
                varInfo var = {string(name), t, string(value), string(definedFrom), const_var, line_number};
                var_table.push_back(var);
            }
            else
            {
                varInfo var = {string(name), string(data_type), string(value), string(definedFrom), const_var, line_number};
                var_table.push_back(var);
            }
        }
        else
        {
            char s[200];
            sprintf(s, "Variabila <%s> este deja declarata la linia %d", name, var_used_line);
            yyerror(s);
            exit(0);
        }
    }
}

void IdList::addFct(const char *name, const char *ret_type, const char *param_type, const char *definedFrom, bool const_fct)
{
    if (!existsFct(name))
    {
        int line_number = yylineno;
        if (const_fct == true) // functia  este constanta
        {
            string t = string(ret_type);
            t = "const " + t;
            fctInfo fct = {string(name), t, string(param_type), string(definedFrom), const_fct, line_number};
            fct_table.push_back(fct);
        }
        else
        {
            fctInfo fct = {string(name), string(ret_type), string(param_type), string(definedFrom), const_fct, line_number};
            fct_table.push_back(fct);
        }
    }
    else
    {
        char s[200];
        sprintf(s, "Functia <%s> este deja declarata la linia %d", name, fct_used_line);
        yyerror(s);
        exit(0);
    }
}

bool IdList::existsFct(const char *fct)
{
    string strfct = string(fct);
    for (fctInfo &f : fct_table)
    {
        if (strfct == f.name)
        {
            fct_used_line = f.line_number;
            return true;
        }
    }
    return false;
}

void IdList::printFctsTable()
{
    FILE *fp;

    fp = fopen("symbol_table_functions.txt", "w+");
    fprintf(fp, "\n\n");
    fprintf(fp, "SYMBOL TABLE FUNCTIONS\n");
    fprintf(fp, "----------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(fp, "NAME  -->  RETURNED_TYPE   -->    PARAMETER_TYPE    -->    DEFINED_FROM    -->    LINE_NUMBER           \n");
    fprintf(fp, "-----------------------------------------------------------------------------------------------------------------------------------------\n\n\n");

    for (fctInfo &f : fct_table)
    {
        fprintf(fp, " %s   -->      %s    -->      %s     -->     %s      -->     %d              \n", f.name.c_str(), f.ret_type.c_str(), f.param_type.c_str(), f.definedFrom.c_str(), f.line_number);
        fprintf(fp, "----------------------------------------------------------------------------------------------------------------\n");
    }
    fclose(fp);
}

bool IdList::existsVar(const char *var, const char *defFrom) // cand dam de un vector in ambele parti ii verificam doar caracterele pana la [
{

    string varName = string(var);
    string definedFromVar = string(defFrom);
    int ok = 0;
    char nname1[10];
    char nname2[10];
    string new_name1;
    string new_name2;

    for (varInfo &v : var_table)
    {

        int poz1 = varName.find('[');

        if (poz1 != std::string::npos) // cazul in care avem o declarare de vector de ex int a[10]
        {
            new_name1 = varName.substr(0, poz1); // ii luam numele pana la [ adica 'a'
            // strcpy(nname1, new_name1.c_str()); //nname1=a
            // printf("Numele nou pentru variabila nou declarata este: %s\n", nname1);
        }
        else
        {
            new_name1 = varName; // daca nu ramane acelasi nume
        }

        // facem acelasi lucru si pentru variabila din tabel daca avem de ex a[100]
        int poz2 = v.name.find('[');

        if (poz2 != std::string::npos)
        {
            new_name2 = v.name.substr(0, poz2); // ii luam numele pana la [ adica 'a'
            // strcpy(nname2, new_name2.c_str()); //nname2=a
            // printf("Numele nou pentru variabila din tabel este: %s\n", nname2);
        }
        else
        {
            new_name2 = v.name; // daca nu ramane acelasi nume
        }

        if (new_name1 == new_name2 && v.definedFrom == definedFromVar)
        {
            var_used_line = v.line_number;
            return true;
        }
    }
    return false;
}

void IdList::printVarsTable()
{
    FILE *fp;

    fp = fopen("symbol_table.txt", "w+");
    fprintf(fp, "\n\n");
    fprintf(fp, "SYMBOL TABLE  VARIABLES                                                   \n");
    fprintf(fp, "-------------------------------------------------------------------------------------\n");
    fprintf(fp, " NAME  -->  DATATYPE  -->  VALUE   -->    DEFINED_FROM   -->    LINE_NUM             \n");
    fprintf(fp, "-------------------------------------------------------------------------------------\n");
    fprintf(fp, "\n\n");
    int i, max = 0;

    for (varInfo &v : var_table)
    {

        fprintf(fp, " %s  -->   %s  -->   %s  -->   %s   -->  %d                \n", v.name.c_str(), v.data_type.c_str(), v.value.c_str(), v.definedFrom.c_str(), v.line_number);
        fprintf(fp, "----------------------------------------------------------------------------------\n");
    }
    fclose(fp);
}

void IdList::isNotDefinedFct(const char *s)
{

    if (!existsFct(s))
    {

        char str[100];
        sprintf(str, "Functia cu numele <%s> nu a fost declarata!", s);
        yyerror(str);
        exit(0);
    }
}

void IdList::isNotDefinedVar(const char *s, char *def_func_var)
{

    string varName = string(s);
    string defFrom = string(def_func_var);
    int ok = 0;
    char nname[10];
    string new_name;

    for (varInfo &v : var_table)
    {

        int poz = v.name.find('[');
        // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[
        if (poz != std::string::npos)
        {
            // modificam si numele din tabel sa fie a[
            new_name = v.name.substr(0, poz + 1);

            // variabila are acelasi nume cu cea din tabel si este definita global sau face parte din functia curenta sau face parte dintr o clasa si functia curenta este main
            if (varName == new_name && (v.definedFrom == "global" || v.definedFrom == defFrom || (v.definedFrom.find("Class") != std::string::npos && defFrom == "main")))
                ok = 1;

            if (varName == new_name && ok == 0) // cazul in care am o variabila definita intr o clasa si poate fi apelata de metodele acelei clase (defFrom==getViteza si v.definedFrom="Clasa masina")
            {                                   // verificam daca metoda respectiva este definita in aceeasi clasa ca si functia din defFrom

                for (fctInfo &f : fct_table)
                {
                    if (defFrom == f.name)
                    {
                        if (f.definedFrom == v.definedFrom)
                        {
                            ok = 1;
                        }
                    }
                }
            }
        }
        else // cazul in care avem o variabila pur si simplu
        {
            if (varName == v.name && (v.definedFrom == "global" || v.definedFrom == defFrom || (v.definedFrom.find("Class") != std::string::npos && defFrom == "main")))
            {
                ok = 1;
                // printf("varName: %s, v.name: %s, v.definedFrom:%s, defFrom: %s\n", varName.c_str(), v.name.c_str(), v.definedFrom.c_str(), defFrom.c_str());
            }

            if (varName == v.name && ok == 0) // cazul in care am o variabila definita intr o clasa si poate fi apelata de metodele acelei clase (defFrom==getViteza si v.definedFrom="Clasa masin")
            {                                 // verificam daca metoda respectiva este definita in aceeasi clasa ca si functia din defFrom
                                              // de ex int getViteza(){ return viteza;} viteza.definedFrom="Class Masina" si getViteza.definedFrom="Clas Masina"
                for (fctInfo &f : fct_table)
                {
                    if (defFrom == f.name)
                    {
                        if (f.definedFrom == v.definedFrom)
                        { // printf("Cazul: varName: %s, v.name: %s, v.definedFrom:%s, defFrom: %s\n", varName.c_str(), v.name.c_str(), v.definedFrom.c_str(), defFrom.c_str());
                            ok = 1;
                        }
                    }
                }
            }
        }
    }

    if (!ok)
    {
        char str[100];
        sprintf(str, "Variabila cu numele <%s> nu a fost declarata!", s);
        yyerror(str);
        exit(0);
    }
}

void IdList::parFct(const char *type, const char *name_var, const char *name_fct)
{
    string strtype = string(type);
    string varname = string(name_var);
    string fctname = string(name_fct);
    for (fctInfo &f : fct_table)
    {
        if (fctname == f.name)
        {
            string t;
            t = '(' + strtype + " " + varname + ')';
            f.param_type = f.param_type + " " + t;
        }
    }
}
IdList::~IdList()
{
    var_table.clear();
}

bool IdList::existsClass(const char *className)
{

    string clsName = string(className);
    int line_number = yylineno;

    for (classInfo &c : class_table)
    {
        if (clsName == c.name)
        {
            class_used_line = c.line_number;
            return true;
        }
    }
    return false;
}

void IdList::addClass(const char *name)
{

    if (!existsClass(name))
    {
        int line_number = yylineno;

        classInfo cls = {string(name), line_number};
        class_table.push_back(cls);
    }
    else
    {
        char s[200];
        sprintf(s, "Clasa <%s> este deja declarata la linia %d", name, class_used_line);
        yyerror(s);
        exit(0);
    }
}

void IdList::verifClassObj(const char *name)
{

    if (!existsClass(name))
    {

        char s[200];
        sprintf(s, "Clasa <%s> nu a fost declarata!", name);
        yyerror(s);
        exit(0);
    }
}

string IdList::typeOfVar(char *var, const char *def_func_var)
{

    string defFrom = string(def_func_var);
    int ok = 0, found = 0;
    string typeExpr, rez1, rez2;
    //  printf("Am intrat in typeOfVar cu expresia %s\n", var);
    string str = string(var);
    if (str.find_first_of("+-*/%||&&<>=!=") == std::string::npos) // avem caz de baza (o variabila sau o constanta)
    {

        string varName = string(var);
        varName.erase(0, varName.find_first_not_of(' '));
        varName.erase(varName.find_last_not_of(' ') + 1);

        int n = varName.length();
        //  printf("Lungimea variabilei este %d\n", n);

        for (varInfo &v : var_table)
        {                             // caut in tabel tipul variabilei
            string new_name = v.name; // salvam numele din tabel intr o variabil

            // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[

            int poz = v.name.find('[');

            if (poz != std::string::npos)

                new_name = v.name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[

            if (new_name == varName && v.definedFrom == defFrom)
            {
                found = 1;
                // printf("Tipul variabilei este %s \n", v.data_type.c_str());
                return v.data_type;
            }
        }

        if (found == 0)
        { // cautam sa fie definita global

            for (varInfo &v : var_table)
            {
                string new_name = v.name; // salvam numele din tabel intr o variabil

                // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[

                int poz = v.name.find('[');

                if (poz != std::string::npos)

                    new_name = v.name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[

                if (new_name == varName && v.definedFrom == "global") // verificam daca apare in global
                {
                    found = 1;
                    // printf("Tipul variabilei este %s \n", v.data_type.c_str());
                    return v.data_type;
                }
            }
        }

        if (found == 0)
        { // cautam sa fie definita intr o clasa
            for (varInfo &v : var_table)
            {
                string new_name = v.name; // salvam numele din tabel intr o variabil

                // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[

                int poz = v.name.find('[');

                if (poz != std::string::npos)

                    new_name = v.name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[

                if (v.name == varName && v.definedFrom.find("Class") != std::string::npos) // verificam daca apare intr-o clasa
                {
                    found = 1;
                    //  printf("Tipul variabilei este %s \n", v.data_type.c_str());
                    return v.data_type;
                }
            }
        }

        if (found == 0)
        {
            for (fctInfo &f : fct_table)
            {
                if (varName == f.name)
                {

                    return f.ret_type;
                }
            }
        }
    }
    return "undefined";
}

string IdList::typeOf(char *expr, const char *def_func_var)
{
   // printf("Expresia este %s\n", expr);

    int g = 0;
    // if (strstr(expr, "true")!=0 && (strstr(expr, "+")!=0 || strstr(expr, "-")!=0 || strstr(expr, "*")!=0 || strstr(expr, "/")!=0 || strstr(expr, "%")!=0 || strstr(expr, "<=")!=0 || strstr(expr, "=>")!=0 || strstr(expr, ">")!=0 || strstr(expr, "<")!=0)){
    //    // printf("Am intrat in if\n");
    //     g = 1;
    // }

    //  if (strstr(expr, "true")!=0 && (strstr(expr, "+")!=0 || strstr(expr, "-")!=0 || strstr(expr, "*")!=0 || strstr(expr, "/")!=0 || strstr(expr, "%")!=0 || strstr(expr, "<=")!=0 || strstr(expr, "=>")!=0 || strstr(expr, ">")!=0 || strstr(expr, "<")!=0)){
    //    //  printf("Am intrat in if\n");
    //     g = 1;
    //  }

   if (strstr(expr, "+") != 0 || strstr(expr, "-") != 0 || strstr(expr, "*") != 0 || strstr(expr, "/") != 0 || strstr(expr, "%") != 0 || strstr(expr, "<=") != 0 || strstr(expr, "=>") != 0 || strstr(expr, ">") != 0 || strstr(expr, "<") != 0)
    {
        char exprCopy[100];
        strcpy(exprCopy, expr);
        string type;

        char *ex = strtok(exprCopy, "+-*/%<=>==");
        //  printf("Ex este %s\n\n", ex);

        while (ex != NULL)
        {

            type = typeOfVar(ex, def_func_var); // punem pur si simplu variabila fara sa o modificam
            if (type == "bool")
            {
                g = 1;
                break;
            }
            ex = strtok(NULL, "+-*/%||&&<=>===!=");
        }
    }
   if (g == 0)
    {
        char varTypes[100][100];
        int n = 0;
        string typeExpr, rez1, rez2;

        char exprCopy[100];
        strcpy(exprCopy, expr);
        string type;

        char *ex = strtok(exprCopy, "+-*/%||&&<=>===!=");
        //  printf("Ex este %s\n\n", ex);

        while (ex != NULL)
        {
            string new_name = string(ex); // cazul in care avem in expresie un element din vector gen p[j] il cauta in tabel cu p[
            int poz = new_name.find('[');

            if (poz != std::string::npos)
            {

                new_name = new_name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[
                char ex_vec[20];
                strcpy(ex_vec, new_name.c_str());

                type = typeOfVar(ex_vec, def_func_var);
            }
            else
            {

                type = typeOfVar(ex, def_func_var); // punem pur si simplu variabila fara sa o modificam
            }
            char typ[30];
            strcpy(typ, type.c_str());
            strcpy(varTypes[n++], typ);

            ex = strtok(NULL, "+-*/%||&&<=>===!=");
        }
        int ok = 1;

        if (n > 1)
        {
            for (int i = 0; i <= n - 2; i++)
            {
                for (int j = i + 1; j <= n - 1; j++)
                {
                    // printf("varTypes[%d]=%s\n  varTypes[%d]=%s\n \n", i, varTypes[i], j, varTypes[j]);
                    if (strcmp(varTypes[i], varTypes[j]) != 0) // am gasit doua tipuri care nu sunt egale

                        return "Variabilele din expresie nu sunt de acelasi tip!";
                }
            }
        }

        typeExpr = string(varTypes[0]);
        return typeExpr;
    }
    else
    {
        return "Expresia nu ar trebui sa contina o valoare de tip bool cu un operator aritmetic!";
    }
}
// TIP ID ASSIGN expr
// verifica pt expresia a:= 1+1 daca tipul lui a este int
void IdList::verifTypeVar(const char *id, const char *def_func_var, string typeExpr)
{

    //  printf("Am intrat in TypeOfVar cu variabila %s\n\n", id);
    string varType;
    string varName = string(id);
    string defName = string(def_func_var);
    int found = 0;
    char typExpr[30];
    strcpy(typExpr, typeExpr.c_str());

    for (varInfo &v : var_table)
    {
        string new_name = v.name; // salvam numele din tabel intr o variabil

        // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[

        int poz = v.name.find('[');

        if (poz != std::string::npos)

            new_name = v.name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[

        if (new_name == varName && v.definedFrom == defName) // verificam mai intai daca apare in functia respectiva
        {

            varType = v.data_type;
            found = 1;
        }
    }

    if (found == 0)
    { // cautam sa fie definita global
        for (varInfo &v : var_table)
        {
            string new_name = v.name; // salvam numele din tabel intr o variabil

            // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[

            int poz = v.name.find('[');

            if (poz != std::string::npos)

                new_name = v.name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[

            if (new_name == varName && v.definedFrom == "global") // verificam daca apare in global
            {
                varType = v.data_type;
                found = 1;
            }
        }
    }

    if (found == 0)
    { // cautam sa fie definita intr o clasa
        for (varInfo &v : var_table)
        {
            string new_name = v.name; // salvam numele din tabel intr o variabil

            // cazul in care dam de o variabila vector vom avea de ex a[100] iar numele de cautat pentru a[j]/a[2] va fi a[

            int poz = v.name.find('[');

            if (poz != std::string::npos)

                new_name = v.name.substr(0, poz + 1); // modificam si numele din tabel sa fie a[

            if (new_name == varName && v.definedFrom.find("Class") != std::string::npos) // verificam daca apare intr-o clasa
            {
                varType = v.data_type;
                found = 1;
            }
        }
    }

    if (typeExpr != varType)

    {
        char s[200];
        sprintf(s, "Variabila <%s> nu corespunde expresiei de tip %s! Variabila are tipul %s\n", id, typeExpr.c_str(), varType.c_str());
        yyerror(s);
        exit(0);
    }
}
void IdList::verifExpr(string rez)
{
    // printf("\n\nAm intrat in verifExpr\n\n");

    if (rez.find("Variabilele") != std::string::npos)
    { // adica variabilele din expresie nu au acelasi tip

        char s[300];
        strcpy(s, "Variabilele din expresie nu sunt de acelasi tip, limbajul nu suporta casting!\n");
        yyerror(s);
        exit(0);
    }
}

void IdList::typeOfError(string rez)
{

    if (rez.find("Variabilele") != std::string::npos)
    { // adica variabilele din expresie nu au acelasi tip

        char s[300];
        strcpy(s, "Variabilele din expresie petnru TypeOf nu sunt de acelasi tip!\n");
        yyerror(s);
        exit(0);
    }
    else if (rez.find("Expresia") != std::string::npos)
    {
        char s[300];
        strcpy(s, "Expresia nu ar trebui sa contina o valoare de tip bool cu un operator aritmetic!\n");
        yyerror(s);
        exit(0);
    }
}
bool IdList::exprOk(string rez)
{

    if (rez.find("Variabilele") != std::string::npos) 
    { // adica variabilele din expresie nu au acelasi tip

        return false;
    }

    if(rez.find("Expresia") != std::string::npos){

        return false;
    }


    return true;
}
