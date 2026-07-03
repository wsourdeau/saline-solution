/****************************************************************
 * Programme de calcul de pH, des [],... d'une solution saline. *
 * Ecriture : 11/11/1994 -> 13/11/1994                          *
 ****************************************************************/

#define _GNU_SOURCE 1
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


double Kw;

const char *msgs[] = {"à réaction acide/d'acide fort et de base faible",
    "à réaction basique/d'acide faible et de base forte",
    "d'acide faible et de base faible",
    "neutre/d'acide fort et de base forte",
    NULL};

typedef struct {
    const char * name;
    double pKa;
    int vAc;
} acid;

typedef struct {
    const char * name;
    double pKb;
    int vBa;
} base;

typedef struct {
    ssize_t acid_nbr;
    ssize_t base_nbr;
} salt_coordinates;

const acid acids[] = {
    {"K",15.6,1},
    {"Na",14.8,1},
    {"NH4",9.2,1},
    {"Ca",12.7,2},
    {"Pb",8.8,2},
    {"Cd",8.5,2},
    {"Cu",8,2},
    {"Zn",7.9,2},
    {"Sn",1.7,2},
    {"Al",4,3},
    {"Cr",3.9,3}
};
const ssize_t n_acids = sizeof(acids) / sizeof(acid);

const base bases[] = {
    {"Cl",21,1},
    {"Br",23,1},
    {"F",10.8,1},
    {"I",25,1},
    {"NO2",10.6,1},
    {"CN",4.7,1},
    {"CH3COO",9.25,1},
    {"S",1,2},
    {"PO4",2.1,2},
    {"CO3",3.6,2},
    {"AsO4",2.5,3}
};
const ssize_t n_bases = sizeof(bases) / sizeof(base);

int x = 0;

const ssize_t n_salts = n_acids * n_bases;
size_t longest_salt;
char ** salts;

static inline int is_upper(char c)
{
    return (c >= 65) && (c <= 90);
}

static ssize_t count_uppers(const char * str)
{
    ssize_t count = 0;
    int i = 0;

    while (str[i])
    {
        if (is_upper(str[i]))
            count++;
        i++;
    }

    return count;
}

static bool has_hydrogen(const char * str)
{
    int i = 0;
    bool result = false;

    while (str[i] && !result)
        if (str[i] == 'H')
            result = true;
        else
            i++;

    return result;
}

static void build_salts()
{
    int acid_c, base_c;
    int grac, grba;
    char *salt;
    size_t salt_len;
    const char * open_paren_acid, * close_paren_acid;
    const char * open_paren_base, * close_paren_base;
    char vAcStr[8], vBaStr[8];

    salts = malloc(n_salts * sizeof(char *));

    longest_salt = 0;
    for (base_c = 0; base_c < n_bases; base_c++)
    {
        const base * current_base = &bases[base_c];
        grba = count_uppers(current_base->name) > 1;
        if (current_base->vBa == 1)
            *vBaStr = 0;
        else
            sprintf(vBaStr, "%d", current_base->vBa);
        for (acid_c = 0; acid_c < n_acids; acid_c++)
        {
            const acid * current_acid = &acids[acid_c];
            grac = count_uppers(current_acid->name) > 1;
            if (current_acid->vAc == 1)
                *vAcStr = 0;
            else
                sprintf(vAcStr, "%d", current_acid->vAc);
            if (current_base->vBa != 1 && grac)
            {
                open_paren_acid = "(";
                close_paren_acid = ")";
            }
            else
            {
                open_paren_acid = "";
                close_paren_acid = "";
            }

            if (current_acid->vAc != 1 && grba)
            {
                open_paren_base = "(";
                close_paren_base = ")";
            }
            else
            {
                open_paren_base = "";
                close_paren_base = "";
            }

            asprintf(&salt, "%s%s%s%s%s%s%s%s",
                     open_paren_acid, current_acid->name, close_paren_acid, vBaStr,
                     open_paren_base, current_base->name, close_paren_base, vAcStr);
            salt_len = strlen(salt);
            if (salt_len > longest_salt)
                longest_salt = salt_len;
            salts[acid_c * n_bases + base_c] = salt;
        }
    }
}

static void beep()
{
    printf("%c", 7);
}

static ssize_t find_salt_number(const char *salt)
{
    ssize_t result = -1;
    size_t salt_c;

    for (salt_c = 0; salt_c < n_salts; salt_c++)
        if (strcmp(salt, salts[salt_c]) == 0)
        {
            result = salt_c;
            break;
        }

    return result;
}

static salt_coordinates find_salt_coordinates(const char *salt)
{
    salt_coordinates result;
    ssize_t salt_number;

    salt_number = find_salt_number(salt);
    if (salt_number == -1)
    {
        result.acid_nbr = -1;
        result.base_nbr = -1;
    }
    else
    {
        result.acid_nbr = salt_number / n_bases;
        result.base_nbr = salt_number % n_bases;
    }

    return result;
}

static void calculate(const char *salt, const salt_coordinates * coordinates)
{
    const base * salt_base;
    const acid * salt_acid;
    char *acid2;
    char *base2;
    int concentration = 0;
    bool has_hyd;

    while (concentration == 0)
    {
        printf("Quelle concentration : ");
        scanf("%d", &concentration);
    }

    salt_base = &bases[coordinates->base_nbr];
    salt_acid = &acids[coordinates->acid_nbr];

    printf("Solution saline : %s (+H2O) = \n", salt);

    asprintf(&acid2, "H%s%.*s", salt_base->name, salt_base->vBa-1, "--------");
    has_hyd = has_hydrogen(salt_acid->name);

    if (has_hyd)
    {
        asprintf(&base2, "%s", salt_acid->name);
        size_t len = strlen(base2);
        base2[len-1]--;
    }
    else
        asprintf(&base2, "%sOH", salt_acid->name);

    double Ka = pow(10, -salt_acid->pKa);
    printf("1) Réaction  %s%.*s  +  H2O :   ->  %s  +  H3O+\npKa = %g Ka = %g\n\n",
           salt_acid->name, salt_acid->vAc, "+++++++", base2,
           salt_acid->pKa, Ka);

    double Kb = pow(10, -salt_base->pKb);
    printf("2) Réaction  %s%.*s  +  H2O :   ->  %s  +  OH-\npKb = %g Kb = %g\n\n",
           salt_base->name, salt_base->vBa, "------", acid2,
           salt_base->pKb, Kb);

    double pKa2 = 14 - salt_base->pKb;
    double Ka2 = pow(10, -pKa2);
    double Kc = Ka / Ka2;
    printf("3) Réaction  %s%.*s  +  %s%.*s  (+H2O) :   ->  %s  +  %s\nKc = %g\n\n",
           salt_acid->name, salt_acid->vAc, "+++++++",
           salt_base->name, salt_base->vBa, "-------",
           acid2, base2, Kc);

    printf("4) Réaction  H2O  +  H2O :   ->  H3O+  +  OH-\nKw = %g\n\n", Kw);

    double K = Ka;
    unsigned int dr = 0;
    if (K < Kb)
    { K = Kb; dr = 1; };
    if (K < Kc)
    { K = Kc; dr = 2; };
    if (K < Kw)
    { K = Kw; dr = 3; };
    printf("La réaction déterminante est la %dº\nC'est un sel %s.\n", (dr+1), msgs[dr]);

    double H3O, OH, pH, pOH;
    switch (dr)
    {
    case 0:
        H3O = sqrt(Ka * concentration);
        OH = pow(10, -14) / H3O;
        pH = -log(H3O) / log(10);
        pOH = -log(OH) / log(10);
        Kc = Ka;
        break;
    case 1:
        OH = sqrt(Kb * concentration);
        H3O = pow(10, -14) / OH;
        pH = -log(H3O) / log(10);
        pOH = -log(OH) / log(10);
        Kc = pow(10, -15.7) / pow(10, salt_base->pKb - 14);
        break;
    case 2:
        pH = (salt_acid->pKa + pKa2) / 2;
        H3O = pow(10, -pH);
        pOH = 14 - pH;
        OH = pow(10, -pOH);
        break;
    case 3:
        pH = 7;
        H3O = pow(10, -7);
        pOH = pH;
        OH = H3O;
        break;
    }

    printf("--------------------------\n");
    printf("[H3O+] = %g  [OH-] = %g\npH = %g  pOH = %g\n", H3O, OH, pH, pOH);

    if (dr != 3)
    {
        printf("Kc = %g\n", Kc);
        double x = 55.5 * Kc * concentration / (55.5 * Kc + Kc * concentration);
        printf("x = %g\n", x);
        if (dr == 0)
            printf("[%s%.*s] = %g    [%s] = %g\n",
                   salt_acid->name, salt_acid->vAc, "+++++++",
                   (concentration - x),
                   base2, x);
        else if (dr == 1)
            printf("[%s%.*s] = %g    [%s] = %g\n",
                   salt_base->name, salt_base->vBa, "-------",
                   (concentration- x),
                   acid2, x);
    }
    printf("\n");
    free(acid2);
    free(base2);
}

static ssize_t get_screen_width()
{
    ssize_t screen_width;
    char * env_columns;

    env_columns = getenv("COLUMNS");
    if (env_columns == NULL)
        screen_width = 80;
    else
    {
        screen_width = atoi(env_columns);
        if (screen_width == 0)
            screen_width = 80;
    }

    return screen_width;
}

static void list_salts()
{
    int i, j;
    char * salt;
    ssize_t screen_width, column_width, nbr_columns;

    screen_width = get_screen_width();
    nbr_columns = screen_width / (longest_salt + 1);
    column_width = screen_width / nbr_columns;
    salt = malloc(column_width + 1);

    for (i = 0; i < n_salts; i++)
    {
        ssize_t len = strlen(salts[i]);
        if (len > column_width)
            len = column_width;
        strncpy(salt, salts[i], len);
        for (j = len; j < column_width; j++)
            salt[j] = ' ';
        salt[column_width] = 0;
        printf("%s", salt);
        if ((i % nbr_columns) == (nbr_columns - 1))
            printf("\n");
    }
    printf("\n");
}

int main()
{
    bool fin = false;
    char command[20];

    Kw = pow(10, -14);
    build_salts();

    printf("Programme de calcul des valeurs dans une solution saline.\n"
           "Ecrit en Novembre 1994 par Wolfgang Sourdeau\n"
           "\n"
           "Commandes possibles :\n"
           "L -> liste des sels\n"
           "F -> fin\n"
           "\n");

    while (!fin)
    {
        printf("Sel (ou commande) : ");
        scanf("%20s", command);
        if (command[1] == 0)
        {
            switch (command[0])
            {
            case 'L':
            case 'l':
                list_salts();
                break;
            case 'F':
            case 'f':
                fin = true;
                break;
            default:
                beep();
            }
        }
        else
        {
            salt_coordinates coordinates;

            coordinates = find_salt_coordinates(command);
            if (coordinates.acid_nbr > -1 && coordinates.acid_nbr > -1)
                calculate(command, &coordinates);
            else
                beep();
        }
    }

    return 0;
}
