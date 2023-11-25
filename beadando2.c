#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

// Függvények
void init(char *arg);
void uj_szallitmany();
void listazas(char *arg);
void lisazas_szolotermo_videk_szerint(char *arg);
void adatmodositas(char *arg);
void adattorles(char *arg);
void feldolgozo_folyamat(char *arg);
// Segédfüggvények
char ***listazas_soronkent(char *arg);
int id_lokalizalas(char *arg, int id);
int menu(int max_menupont);
// globális változó
int max_rec_id = 0;
int rec_count = 0;
int main_menupont = 10;
int idealis_szolomennyiseg = 90;

void handler(int signumber)
{
    printf("~~~~~~~~~~Feldolgozó üzenete fogadva\n");
}

// main
// Paraméterek: fájl név
// Funkció: fő funkciók meghívása
int main(int argc, char **argv)
{
    char *filename = "szolo.dat";
    init(filename);
    while (main_menupont != 0)
    {
        printf("\n\n-.-.-.-.-.-.-.-.-.-.-Főmenü-.-.-.-.-.-.-.-.-.-\n");
        printf("Kerem valasszon a menupontok kozul:\n0.Kilepes\n1.uj szallitmany rogzitese\n2.listazas\n3.listazas-videk szerint\n4.adatmodositas\n5.adat torles\n6.feldolgozo folyamat\n");
        main_menupont = menu(6);
        switch (main_menupont)
        {
        case 1:
            uj_szallitmany(filename);
            break;
        case 2:
            listazas(filename);
            break;
        case 3:
            lisazas_szolotermo_videk_szerint(filename);
            break;
        case 4:
            adatmodositas(filename);
            break;
        case 5:
            adattorles(filename);
            break;
        case 6:
            feldolgozo_folyamat(filename);
            break;
        }
    }
}

void feldolgozo_folyamat(char *arg)
{ // move at the end of processes
    char ***eredmeny = listazas_soronkent(arg);
    struct Szolo
    {
        char szolofajta[20];
        int kg;
    };
    struct Szolo harslevelu;
    strcpy(harslevelu.szolofajta, "Harslevelu");
    harslevelu.kg = 0;
    struct Szolo furmint;
    strcpy(furmint.szolofajta, "Furmint");
    furmint.kg = 0;
    struct Szolo muskotaly;
    strcpy(muskotaly.szolofajta, "Muskotaly");
    muskotaly.kg = 0;

    struct Szolo szolo_kuldesre[3];
    int size_of_szolo_kuldesre = 0;

    int i;
    for (i = 0; i < rec_count; i++)
    {
        if (strcmp(eredmeny[i][3], "Harslevelu") == 0) // strip ide mehetne
        {
            harslevelu.kg += atoi(eredmeny[i][2]);
        }
        if (strcmp(eredmeny[i][3], "Furmint") == 0)
        {
            furmint.kg += atoi(eredmeny[i][2]);
        }
        if (strcmp(eredmeny[i][3], "Muskotaly") == 0)
        {
            muskotaly.kg += atoi(eredmeny[i][2]);
        }
    }

    if (harslevelu.kg >= idealis_szolomennyiseg)
    {
        szolo_kuldesre[size_of_szolo_kuldesre] = harslevelu;
        size_of_szolo_kuldesre++;
    }
    if (furmint.kg >= idealis_szolomennyiseg)
    {
        szolo_kuldesre[size_of_szolo_kuldesre] = furmint;
        size_of_szolo_kuldesre++;
    }
    if (muskotaly.kg >= idealis_szolomennyiseg)
    {
        szolo_kuldesre[size_of_szolo_kuldesre] = muskotaly;
        size_of_szolo_kuldesre++;
    }

    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Feldolgozo folyamat-.-.-.-.-.-.-.-.-.-\n");
    printf("\nHa egy szőlőfajtából összegyült %i kg, feldolgozásra elküldjük", idealis_szolomennyiseg);
    printf("\nHarslevelu mennyisege: %i, Furmint mennyisege: %i, Muskotaly mennyisege: %i\n", harslevelu.kg, furmint.kg, muskotaly.kg);
    if (size_of_szolo_kuldesre > 0)
    {
        signal(SIGTERM, handler);
        int pipe_szolo[2];
        int pipe_bor[2];
        pid_t feldolgozo;
        pipe(pipe_szolo);
        pipe(pipe_bor);
        feldolgozo = fork();

        if (feldolgozo > 0)
        {
            sleep(1);
            printf("----------|Feldolgozo folyamat NSZT: Várakozás a feldolgozóüzemre|\n");
            pause();
            close(pipe_szolo[0]); // parent will write
            close(pipe_bor[1]);   // parent will read

            for (int i = 0; i < size_of_szolo_kuldesre; i++)
            {
                write(pipe_szolo[1], &szolo_kuldesre[i], sizeof(struct Szolo));
                printf("----------|Feldolgozo folyamat NSZT: %s szolobol elkuldve %i kg|\n", szolo_kuldesre[i].szolofajta, szolo_kuldesre[i].kg);
            }
            close(pipe_szolo[1]);
            for (int i = 0; i < size_of_szolo_kuldesre; i++)
            {
                pause();
                float liter;
                read(pipe_bor[0], &liter, sizeof(float));
                printf("----------|Feldolgozo folyamat NSZT: %f liter bor készül a küldött szőlőből|\n", liter);
            }
            close(pipe_bor[0]);
            int status;
            wait(&status);
        }
        else
        {
            // Feldolgozó
            sleep(10);
            printf("..........|Feldolgozo folyamat feldolgozo: Feldogozás készen áll, jelzés elküldése.|\n");
            sleep(3);
            kill(getppid(), SIGTERM);
            close(pipe_szolo[1]);
            close(pipe_bor[0]);
            for (int i = 0; i < size_of_szolo_kuldesre; i++)
            {
                struct Szolo fogadott_szolo;
                read(pipe_szolo[0], &fogadott_szolo, sizeof(struct Szolo));
                printf("..........|Feldolgozo folyamat feldolgozo: %s szolobol fogadva %i kg|\n", fogadott_szolo.szolofajta, fogadott_szolo.kg);
                sleep(3);
                kill(getppid(), SIGTERM);
                int randomSleep = rand() % 6 + 5;
                float randomLiter = 0.6 + ((double)rand() / RAND_MAX) * (0.6 - 0.8);
                float liter = fogadott_szolo.kg * randomLiter;
                sleep(randomSleep);

                printf("..........|Feldolgozo folyamat feldolgozo: a borból %f liter / kg, így összesen %f -liter készíthető|\n", randomLiter, liter);

                write(pipe_bor[1], &liter, sizeof(float));
            }
            close(pipe_szolo[0]);
            close(pipe_bor[1]);
            // kill(getppid(), SIGTERM);
            exit(0);
        }
    }
}

void init(char *arg)
{
    char dummy1[50] = "Tokaji,Bela,20,Furmint,1\n";
    max_rec_id++;
    rec_count++;
    char dummy2[50] = "Egri,Janos,100,Harslevelu,2\n";
    max_rec_id++;
    rec_count++;
    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    write(g, dummy1, strlen(dummy1));
    write(g, dummy2, strlen(dummy2));
    close(g);
}

void uj_szallitmany(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Új szállítmány-.-.-.-.-.-.-.-.-.-\n");
    max_rec_id++;
    rec_count++;
    printf("Szolotermelo videk neve:\n1.Tokaji\n2.Egri\n3.Balatoni\n");
    int szolotermelo_videk = menu(3);
    char szolotermo_videk_str[9];
    switch (szolotermelo_videk)
    {
    case 1:
        strcpy(szolotermo_videk_str, "Tokaji");
        break;
    case 2:
        strcpy(szolotermo_videk_str, "Egri");
        break;
    case 3:
        strcpy(szolotermo_videk_str, "Balatoni");
        break;
    }
    printf("Termelo neve: ");
    char nev[30];
    scanf("%s", nev);
    while (fgetc(stdin) != '\n')
        ;
    printf("Atvett mennyiseg (kg): ");
    int mennyiseg; // Q: mi volt a baj a double tipussal?
    scanf("%d", &mennyiseg);
    while (fgetc(stdin) != '\n')
        ;
    printf("Szolofajta neve: :\n1.Furmint\n2.Harslevelu\n3.Muskotaly");
    int szolofajta = menu(3);
    char szolofajta_str[11];
    switch (szolofajta)
    {
    case 1:
        strcpy(szolofajta_str, "Furmint");
        break;
    case 2:
        strcpy(szolofajta_str, "Harslevelu");
        break;
    case 3:
        strcpy(szolofajta_str, "Muskotaly");
        break;
    }
    printf("-----------------\nMegadott adatok:\n-Szolotermelo videk: %s\n-Termelo neve: %s\n-Atvett mennyiseg: %d\n-Szolofajta: %s\n-----------------\n", szolotermo_videk_str, nev, mennyiseg, szolofajta_str);
    printf("File-ba iras\n");
    char mennyiseg_str[4];
    char max_rec_id_str[6];
    sprintf(mennyiseg_str, "%d", mennyiseg);
    sprintf(max_rec_id_str, "%d", max_rec_id);

    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    write(g, szolotermo_videk_str, strlen(szolotermo_videk_str));
    write(g, ",", strlen(","));
    write(g, nev, strlen(nev));
    write(g, ",", strlen(","));
    write(g, mennyiseg_str, strlen(mennyiseg_str));
    write(g, ",", strlen(","));
    write(g, szolofajta_str, strlen(szolofajta_str));
    write(g, ",", strlen(","));
    write(g, max_rec_id_str, strlen(max_rec_id_str));
    write(g, "\n", strlen("\n"));
    close(g);
}

void listazas(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Listázás-.-.-.-.-.-.-.-.-.-\n");
    int g;
    g = open(arg, O_RDONLY);
    char c;
    while (read(g, &c, sizeof(c)))
    {
        printf("%c", c);
    }
}

void lisazas_szolotermo_videk_szerint(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Listázás területenként-.-.-.-.-.-.-.-.-.-\n");
    char ***eredmeny = listazas_soronkent(arg);
    char szolotermo_videk[10];
    int i;
    printf("Kerem adja meg a szolotermo videk nevet: ");
    scanf("%s", szolotermo_videk);
    while (fgetc(stdin) != '\n')
        ;
    printf("\nSzállítmányok a(z) %s borvidékről:\n", szolotermo_videk);
    for (i = 0; i < rec_count; i++)
    {
        if (strcmp(eredmeny[i][0], szolotermo_videk) == 0)
        {
            printf("%s, %s, %s, %s, %s\n", eredmeny[i][0], eredmeny[i][1], eredmeny[i][2], eredmeny[i][3], eredmeny[i][4]);
        }
    }
}

void adatmodositas(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Adatmódosítás-.-.-.-.-.-.-.-.-.-\n");
    char ***eredmeny = listazas_soronkent(arg);
    int id;
    int id_loc;
    char uj_ertek[100];
    printf("Kérem adja meg az id-t: ");
    scanf("%d", &id);
    while (fgetc(stdin) != '\n')
        ;
    id_loc = id_lokalizalas(arg, id);
    printf("\nA modosítani kívánt rekord: %s, %s, %s, %s, %s\n", eredmeny[id_loc][0], eredmeny[id_loc][1], eredmeny[id_loc][2], eredmeny[id_loc][3], eredmeny[id_loc][4]);
    printf("Kérem adja meg a módosítani kívánt adatot:\n1.Szolotermelo videk\n2.Termelo neve:\n3.Atvett mennyiseg\n4.Szolofajta\n");
    int menupont = menu(4);
    while (fgetc(stdin) != '\n')
        ;
    switch (menupont)
    {
    case 1:
        printf("Szolotermelo videk neve:\n1.Tokaji\n2.Egri\n3.Balatoni\n");
        int szolotermelo_videk = menu(3);
        char szolotermo_videk_str[9];
        switch (szolotermelo_videk)
        {
        case 1:
            strcpy(szolotermo_videk_str, "Tokaji");
            break;
        case 2:
            strcpy(szolotermo_videk_str, "Egri");
            break;
        case 3:
            strcpy(szolotermo_videk_str, "Balatoni");
            break;
        }
        eredmeny[id_loc][0] = szolotermo_videk_str;
        break;
    case 2:
        printf("Termelo neve: ");
        scanf("%s", uj_ertek);
        eredmeny[id_loc][1] = uj_ertek;
        break;
    case 3:
        printf("Atvett mennyiseg (kg): ");
        scanf("%s", uj_ertek);
        eredmeny[id_loc][2] = uj_ertek;
        break;
    case 4:
        printf("Szolofajta neve: :\n1.Furmint\n2.Harslevelu\n3.Muskotaly");
        int szolofajta = menu(3);
        char szolofajta_str[11];
        switch (szolofajta)
        {
        case 1:
            strcpy(szolofajta_str, "Furmint");
            break;
        case 2:
            strcpy(szolofajta_str, "Harslevelu");
            break;
        case 3:
            strcpy(szolofajta_str, "Muskotaly");
            break;
        }
        eredmeny[id_loc][3] = szolofajta_str;
        break;
    }
    printf("\nA módosított rekord: %s, %s, %s, %s, %s\n", eredmeny[id_loc][0], eredmeny[id_loc][1], eredmeny[id_loc][2], eredmeny[id_loc][3], eredmeny[id_loc][4]);

    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int i;
    for (i = 0; i < rec_count; i++)
    {
        write(g, eredmeny[i][0], strlen(eredmeny[i][0]));
        write(g, ",", strlen(","));
        write(g, eredmeny[i][1], strlen(eredmeny[i][1]));
        write(g, ",", strlen(","));
        write(g, eredmeny[i][2], strlen(eredmeny[i][2]));
        write(g, ",", strlen(","));
        write(g, eredmeny[i][3], strlen(eredmeny[i][3]));
        write(g, ",", strlen(","));
        write(g, eredmeny[i][4], strlen(eredmeny[i][4]));
        write(g, "\n", strlen("\n"));
    }
    close(g);
}

void adattorles(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Törlés-.-.-.-.-.-.-.-.-.-\n");
    char ***eredmeny = listazas_soronkent(arg);
    int id;
    int id_loc;
    printf("Kérem adja meg az id-t: ");
    scanf("%d", &id);
    while (fgetc(stdin) != '\n')
        ;
    id_loc = id_lokalizalas(arg, id);
    printf("\nA törölni kívánt rekord: %s, %s, %s, %s, %s\n", eredmeny[id_loc][0], eredmeny[id_loc][1], eredmeny[id_loc][2], eredmeny[id_loc][3], eredmeny[id_loc][4]);
    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int i;
    for (i = 0; i < rec_count; i++)
    {
        if (atoi(eredmeny[i][4]) != id)
        {
            write(g, eredmeny[i][0], strlen(eredmeny[i][0]));
            write(g, ",", strlen(","));
            write(g, eredmeny[i][1], strlen(eredmeny[i][1]));
            write(g, ",", strlen(","));
            write(g, eredmeny[i][2], strlen(eredmeny[i][2]));
            write(g, ",", strlen(","));
            write(g, eredmeny[i][3], strlen(eredmeny[i][3]));
            write(g, ",", strlen(","));
            write(g, eredmeny[i][4], strlen(eredmeny[i][4]));
            write(g, "\n", strlen("\n"));
        }
    }
    printf("Törles sikeres");
    rec_count--;
    close(g);
}

char ***listazas_soronkent(char *arg)
{
    int g;
    g = open(arg, O_RDONLY);
    char c;
    int sorok = 0;
    char ***arr = malloc(sorok * sizeof(char **));
    char line[100];
    char *token;
    strcpy(line, "");
    while (read(g, &c, sizeof(c)))
    {
        if (c != '\n')
        {
            strncat(line, &c, sizeof(c));
        }
        else
        {
            const char delim[2] = ",";
            token = strtok(line, delim);
            int szavak = 0;
            arr = realloc(arr, (sorok + 1) * sizeof(char **));
            arr[sorok] = malloc(5 * sizeof(char *));
            while (token != NULL)
            {
                arr[sorok][szavak] = strdup(token);
                token = strtok(NULL, ",");
                szavak++;
            }
            sorok++;
            strcpy(line, "");
        }
    }
    close(g);
    return arr;
}

int id_lokalizalas(char *arg, int id)
{
    char ***eredmeny = listazas_soronkent(arg);
    int i;
    int loc = -1;
    for (i = 0; i < rec_count; i++)
    {
        if (atoi(eredmeny[i][4]) == id)
        {
            loc = i;
        }
    }
    return loc;
}

int menu(int max_menupont)
{
    printf("\nVálasztása: ");
    int sucess = 0;
    int menupont;
    while (sucess == 0 | menupont<0 | menupont> max_menupont)
    {
        sucess = scanf("%d", &menupont);
        while (fgetc(stdin) != '\n')
            ;
        if (sucess == 0 | menupont<0 | menupont> max_menupont)
        {
            printf("Nem letezo menupont, probalja ujra\nVálasztása: ");
        }
    }
    return menupont;
}