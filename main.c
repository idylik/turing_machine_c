#include <stdlib.h>
#include <stdio.h>
#include "main.h"

typedef unsigned char byte;
typedef int error_code;

#define ERROR (-1)
#define HAS_ERROR(code) ((code) < 0)
#define HAS_NO_ERROR(code) ((code) >= 0)

/**
 * Cette fonction compare deux chaînes de caractères.       
 * @param p1 la première chaîne
 * @param p2 la deuxième chaîne
 * @return le résultat de la comparaison entre p1 et p2. Un nombre plus petit que
 * 0 dénote que la première chaîne est lexicographiquement inférieure à la deuxième.
 * Une valeur nulle indique que les deux chaînes sont égales tandis qu'une valeur positive
 * indique que la première chaîne est lexicographiquement supérieure à la deuxième.
 */

void freeMemoire(int nbLignes, char** descriptions, char *ruban, transition** tableauTransitions);

int strcmp(char *p1, char *p2) {
    char *s1 = (char *) p1;
    char *s2 = (char *) p2;
    char c1, c2;
    do {
        c1 = (char) *s1++;
        c2 = (char) *s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

/**
 * Ex. 1: Calcul la longueur de la chaîne passée en paramètre selon
 * la spécification de la fonction strlen standard
 * @param s un pointeur vers le premier caractère de la chaîne
 * @return le nombre de caractères dans le code d'erreur, ou une erreur
 * si l'entrée est incorrecte
 */
error_code strlen2(char *s) {
    unsigned int acc = 0;

    while(*s != '\0') {
        s++;
        acc++;
    }
    return acc;
}

/**
 * Ex.2 :Retourne le nombre de lignes d'un fichier sans changer la position
 * courante dans le fichier.
 * @param fp un pointeur vers le descripteur de fichier
 * @return le nombre de lignes, ou -1 si une erreur s'est produite
 */
error_code no_of_lines(FILE *fp) {

    long anciennePos = ftell(fp);

    rewind(fp);

    // Si le fichier n'est pas bien mis en input
    if (fp == NULL) {
        printf("Erreur dans l'ouverture du fichier\n");
        return -1;
    }

    int nbLignes = 1;
    char caractereActuel = fgetc(fp);

        if (feof(fp)) { //Si fichier vide
            printf("Fichier vide\n");
            rewind(fp);
            return 0;
        }
    rewind(fp);

    while (!feof(fp)) {
        caractereActuel = fgetc(fp);
        if (caractereActuel == '\n') {
                nbLignes++;
            }
    }

    fseek(fp, anciennePos, SEEK_SET);

    return nbLignes;
}

/**
 * Ex.3: Lit une ligne au complet d'un fichier
 * @param fp le pointeur vers la ligne de fichier
 * @param out le pointeur vers la sortie
 * @param max_len la longueur maximale de la ligne à lire
 * @return le nombre de caractère ou ERROR si une erreur est survenue
 */
error_code readline(FILE *fp, char **out, size_t max_len) {

    if (fp == NULL) {
        printf("Erreur dans l'ouverture du fichier\n");
        return -1;
    }

    //On doit réserver un caractère supplémentaire \0 qui indique la fin de la chaine
    char *ptrTableau = malloc(sizeof(char)*(max_len+1));
    if (ptrTableau == NULL) {return ERROR;}

    int count = 0;
    char cActuel = fgetc(fp);

    while (cActuel != '\0' && cActuel != '\n' && cActuel != EOF && count < max_len) {
        ptrTableau[count] = cActuel;
        count++;
        cActuel = fgetc(fp);
    }

    if (out != NULL) {
        *out = ptrTableau;
    }

    ptrTableau[count] = '\0';

    return count;
}

/**
 * Ex.4: Copie un bloc mémoire vers un autre
 * @param dest la destination de la copie
 * @param src  la source de la copie
 * @param len la longueur (en byte) de la source
 * @return nombre de bytes copiés ou une erreur s'il y a lieu
 */
error_code memcpy2(void *dest, void *src, size_t len) {

    if (dest == NULL || src == NULL) {
        return ERROR;
    }

    char* charDest = (char*) dest;
    char* charSrc = (char*) src;

    //Copier byte par byte
    for (int i=0; i < len; i++) {
        charDest[i] = charSrc[i];
    }
}

/**
 * Ex.5: Analyse une ligne de transition
 * @param line la ligne à lire
 * @param len la longueur de la ligne
 * @return la transition ou NULL en cas d'erreur
 */
transition *parse_line(char *line, size_t len) {

    //Il s'agit d'une transition
    if (len > 5) {
        transition *nouvelleTransition = malloc(sizeof(transition));

        if (nouvelleTransition == NULL) {
            return NULL;
        }

        nouvelleTransition->current_state = malloc(sizeof(char)*5);
        if (nouvelleTransition->current_state == NULL) {
            free(nouvelleTransition);
            return NULL;
        }

        nouvelleTransition->next_state = malloc(sizeof(char)*5);
        if (nouvelleTransition->next_state == NULL) {
            free(nouvelleTransition->current_state);
            free(nouvelleTransition);
            return NULL;
        }


        sscanf(line, "(%[^,],%c)->(%[^,],%c,%c)",
                nouvelleTransition->current_state,
                &(nouvelleTransition->read),
                nouvelleTransition->next_state,
                &(nouvelleTransition->write),
                &(nouvelleTransition->movement));
        
        // Gauche
        if (nouvelleTransition->movement == 'G') {
            nouvelleTransition->movement = (int) -1;
        }
        // Reste
        else if (nouvelleTransition->movement == 'R') {
            nouvelleTransition->movement = (int) 0;
        }
        // Droite
        else {
            nouvelleTransition->movement = (int) 1;
        }

        return nouvelleTransition;
    }

    return NULL;
}

/**
 * Ex.6: Execute la machine de turing dont la description est fournie
 * @param machine_file le fichier de la description
 * @param input la chaîne d'entrée de la machine de turing
 * @return le code d'erreur
 */
error_code execute(char *machine_file, char *input) {

    int inputSize = strlen2(input);
    if (inputSize == 0) {
        return 1;
    }

    FILE *descriptionMachine = NULL;
    descriptionMachine = fopen(machine_file, "r");
    if (descriptionMachine == NULL) {
        return ERROR;
    }


    int nbLignes = no_of_lines(descriptionMachine);
    if (nbLignes < 4) {
        fclose(descriptionMachine);
        return ERROR;
    }

    //Allocation tableau contenant descriptions.
    //Chaque position sera un pointeur (adresse) du début de chaque string (ligne)
    char** descriptions = malloc(sizeof(char*)*nbLignes); //Tableau de pointeurs
    if (descriptions == NULL) {
        fclose(descriptionMachine);
        return ERROR;
    }

    //On remplit le tableau descriptions en lisant le fichier
    for (int i=0; i < nbLignes; i++) {
        char *pointeurLigne;
        int nbCaract = readline(descriptionMachine, &pointeurLigne, 25);

        if (nbCaract > 0) {
            descriptions[i] = pointeurLigne;
        } else {
            free(pointeurLigne);
            descriptions[i] = NULL;
        }
    }

   fclose(descriptionMachine);

    //Création ruban de travail (il faut l'initialiser avec des ' ')
    int rubanSize = inputSize*2;
    char *ruban = malloc(sizeof(char)*rubanSize);
    if (ruban == NULL) {
        freeMemoire(nbLignes, descriptions, NULL, NULL);
        return ERROR;
    }
    for (int i = 0; i < inputSize*2; i++) {
        ruban[i] = ' ';
    }

    //Copie du mot sur le ruban
    memcpy2(ruban, input, inputSize); //Note: ne met pas de \0 à la fin du mot input

    char* etatInitial = descriptions[0];
    char* etatAcceptant = descriptions[1];
    char* etatRejetant = descriptions[2];
    char* etatActuel = etatInitial;

    //Position (index) de la tete de lecture sur le ruban
    int indexTete = 0;
    char carActuel = ruban[indexTete];

    //Allocation tableau transitions (tableau d'adresses pointant vers une transition)
    transition** tableauTransitions = malloc(sizeof(transition)*(nbLignes-3));
    if (tableauTransitions == NULL) {
        freeMemoire(nbLignes, descriptions, ruban, NULL);
        return ERROR;
    }

    //Remplir le tableau transitions
    for (int i=0; i < nbLignes-3; i++) {
        //Commencer à l'index 3 du tableau descriptions
        if (descriptions[i+3] != NULL) {
            transition* pointeurTransition = parse_line(descriptions[i+3], strlen2(descriptions[i+3]));
            tableauTransitions[i] = pointeurTransition;
        } else {
            tableauTransitions[i] = NULL;
        }
    }

    //MISE EN MARCHE DE LA MACHINE
    //Tant que l'état actuel n'est pas acceptant et n'est pas rejetant
    while (strcmp(etatActuel, etatAcceptant) != 0 && strcmp(etatActuel, etatRejetant) != 0) {

        transition* ptrTransitionPrise = NULL;

        //Chercher la bonne transition à utiliser:
        for (int i=0; i<nbLignes-3; i++) {
            if (tableauTransitions[i] != NULL) {
                char *trCurr = tableauTransitions[i]->current_state;
                char trRead = tableauTransitions[i]->read;
                if (strcmp(etatActuel, trCurr) == 0 && carActuel == trRead) { //C'est la bonne transition à utiliser
                    //printf("etatActuel: %s, trCurr: %s, carActuel: %c, trRead: %c\n", etatActuel, trCurr, carActuel, trRead);

                    ptrTransitionPrise = tableauTransitions[i];

                    //Prendre la transition vers le prochain état:
                    etatActuel = ptrTransitionPrise->next_state;

                    //Écrire le caractère:
                    ruban[indexTete] = ptrTransitionPrise->write;

                    //Se déplacer
                    indexTete += (int) ptrTransitionPrise->movement;

                    if (indexTete < 0) {
                        indexTete = 0;
                    }

                    //Changer taille du ruban au besoin
                    if (indexTete >= rubanSize) {
                        //printf("changement taille ruban\n");
                        //Doubler taille ruban
                        ruban = realloc(ruban, rubanSize*2);

                        if (ruban == NULL) {
                            //Si la réallocation n'a pas fonctionné
                            freeMemoire(nbLignes, descriptions, ruban, tableauTransitions);
                            return ERROR;
                        }

                        //Remplir de caractères vides:
                        for (int i = rubanSize; i < rubanSize*2; i++) {
                            ruban[i] = ' ';
                        }
                        rubanSize *= 2;
                    }

                    //Nouveau caractère lu
                    carActuel = ruban[indexTete];
                break; //On continue à la prochaine itération du while
                }
            }
        }

        //Si on n'a pas trouvé de transition
        if (ptrTransitionPrise == NULL) {
            freeMemoire(nbLignes, descriptions, ruban, tableauTransitions);
            return ERROR;
        }

    }

    if (strcmp(etatActuel, etatAcceptant) == 0){
        printf("Etat acceptant\n");
        freeMemoire(nbLignes, descriptions, ruban, tableauTransitions);
        return 1;
    }

    if (strcmp(etatActuel, etatRejetant) == 0) {
        printf("Etat rejetant\n");
        freeMemoire(nbLignes, descriptions, ruban, tableauTransitions);
        return 0;
    }

    freeMemoire(nbLignes, descriptions, ruban, tableauTransitions);
    return ERROR;
}


void freeMemoire(int nbLignes, char** descriptions, char *ruban, transition** tableauTransitions) {

    if (descriptions != NULL) {
        for (int i = 0; i < nbLignes; i++) {
            if (descriptions[i] != NULL) {
                free(descriptions[i]);
            }
        }
        free(descriptions);
    }

    if (ruban != NULL) {
        free(ruban);
    }

    if (tableauTransitions != NULL) {
        for (int i=0; i < nbLignes-3; i++) {
            if (tableauTransitions[i] != NULL) {
                free(tableauTransitions[i]->current_state);
                free(tableauTransitions[i]->next_state);
            }
            free(tableauTransitions[i]);
        }
        free(tableauTransitions);
    }
}

// ATTENTION! TOUT CE QUI EST ENTRE LES BALISES ༽つ۞﹏۞༼つ SERA ENLEVÉ! N'AJOUTEZ PAS D'AUTRES ༽つ۞﹏۞༼つ

// ༽つ۞﹏۞༼つ

int main() {
//Vous pouvez ajouter des tests pour les fonctions ici



    return 0;
}

// ༽つ۞﹏۞༼つ
