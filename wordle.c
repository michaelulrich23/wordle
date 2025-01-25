#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

// !!
// !!
// Pustat len na Linuxe alebo WSL
// !!
// !!
// output bude s ANSI codes vyzerat nasledovne: https://prnt.sc/lBCzj72VweaU

// Makra pre farbenie pozadia pismen, dlzku slova a abecedy
#define MAX_WORD_LENGTH 5
#define ALPHABET_LENGTH 26
#define REDB(string) "\e[30;41m" string "\e[0m"
#define GRNB(string) "\e[30;42m" string "\e[0m"
#define YELB(string) "\e[30;43m" string "\e[0m"
#define WHTB(string) "\e[30;47m" string "\e[0m"

// Databaza slov, ktore program vybera na tipovanie (uplne dole v programe)
extern char* answer_words[];

// Hlavna struktura pre abecedu a "guess", ktory tipujeme
typedef struct alphabet{
    char letter;
    // typy viac opisane vo funkcii printFunction()
    int type;
} ALPHABET;

// Najde prvu poziciu charakteru v stringu a vrati index
int charPosition(const char string[], char guess_char){
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == guess_char) {
            return i;
        }
    }
    return -1;
}

// Prekonvertuje nase slovo na velke pismena
void guessToUpper(ALPHABET guess[]){
    int i = 0;
    while (guess[i].letter) {
        if (guess[i].letter >= 'a' && guess[i].letter <= 'z') {
            guess[i].letter -= 32;
        }
        i++;
    }
}

// Ziskanie slova od uzivatela
void getGuess(ALPHABET guess[]){
    char line[4096];
    while(1) {
        printf("Please enter your %d letter guess:\n", MAX_WORD_LENGTH);

        if (fgets(line, sizeof(line), stdin) == NULL){
            printf("Invalid format\n");
            continue;
        }
        // Ak je aj dlhsie slovo, zobere iba prvych 5 pismen
        for (int i = 0; i < MAX_WORD_LENGTH; i++) {
            // Clion s WSL ma nejaky bug, ktory sam od seba vypise naskenovane slovo do konzole (v debug wsl to ide normalne)
            // V inych kompilatoroch alebo prostrediach, napriklad na www.onlinegdb.com sa to nedeje,
            // a vlastne by sa to nemalo diat nikde, kedze ziadny vypis naprogramovany nie je

            sscanf(&line[i], " %c", &guess[i].letter);
        }

        guess[MAX_WORD_LENGTH].letter = '\0';
        for (int i = 0; i < MAX_WORD_LENGTH; i++) {
            // Ak su v slove ne-alpha znaky, tak znovu sa ziska vstup od uzivatela
            if (isalpha(guess[i].letter) == 0){
                //setbuf(stdout, NULL);
                printf("Invalid format\n");
                break;
            }
            // Ked sme na konci slova, tak prekonvertovat na velke pismena
            if(i==(MAX_WORD_LENGTH-1)){
                guessToUpper(guess);
                return;
            }
        }
    }
}

void printFunction(ALPHABET word){
    switch (word.type) {
        case 0:
            // Typ cislo 0 ma biele pozadie, znamena doposial nevyuzite pismeno
            printf(WHTB(" %c ") " ", word.letter);
            break;
        case 1:
            // Typ cislo 1 ma cervene pozadie, znamena, ze sa pismeno v slove nenachadza
            printf(REDB(" %c ") " ", word.letter);
            break;
        case 2:
            // Typ cislo 2 ma zle pozadie, znamena, ze pismeno sme trafili ale nachadza sa na zlom mieste
            printf(YELB(" %c ") " ", word.letter);
            break;
        case 3:
            // Typ cislo 3 ma zelene pozadie, znamena, ze pismeno sme trafili na spravne miesto
            printf(GRNB(" %c ") " ", word.letter);
            break;
    }
}

// Vypis abecedy
void printAlphabet(ALPHABET alphabet[]){
    printf("Letters of the alphabet: ");
    for (int i = 0; i < ALPHABET_LENGTH; i++) {
        printFunction(alphabet[i]);
    }
    printf("\n");
}

// Vypis nasho guess-u
void printGuess(ALPHABET guess[]){
    printf("Your guess: ");
    for (int i = 0; i < MAX_WORD_LENGTH; i++) {
        printFunction(guess[i]);
    }
    printf("\n");
}

// Inicializacia abecedy
void loadAlphabet(ALPHABET alphabet[]){
    for (int i = 0; i < ALPHABET_LENGTH; i++) {
        alphabet[i].letter = (char)(65+i);
        alphabet[i].type = 0;
    }
}

// Kontrola guess-u ziskaneho od uzivatela
bool guessCheck(ALPHABET guess[], const char answer[], ALPHABET alphabet[]){
    char temp1[MAX_WORD_LENGTH + 1];
    int temp2, correct_letters = 0;
    /*
    temp1 je v tejto funkcii ako fix, aby ked je napriklad slovo ktore tipujeme "feika", a tipneme
    slovo "fajka", tak aby sa iba jedno z tych dvoch 'a' rozsvietilo na nejaky typ farby,
    kedze sa nachadza iba jedno 'a' vo finalnom slove.
    Keby to tu nebolo osetrene, tak by sa prve 'a' ukazovalo zlto, ze je akoze na zlom mieste, a druhe na zeleno,
    iked sa vo finalnom slove nachadza iba jedno 'a'.
     */
    strcpy(temp1, answer);

    for (int i = 0; i < strlen(answer); i++) {
        // Najprv kontrolujeme, ci su nejake pismena na spravnom mieste
        if(guess[i].letter == answer[i]){
            alphabet[guess[i].letter-65].type = 3;
            guess[i].type = 3;
            temp1[i] = '*';
            correct_letters++;
        }
        else{
            alphabet[guess[i].letter-65].type = 1;
            guess[i].type = 1;
        }
    }
    // Ak su vsetky pismena spravne, tak vrati hodnotu 1
    if(correct_letters == MAX_WORD_LENGTH){
        return 1;
    }
    // Kontrola pre pismena na zlom mieste v slove
    for (int i = 0; i < strlen(answer); i++) {
        temp2 = charPosition(temp1,guess[i].letter);
        if(temp2 >= 0 && alphabet[guess[i].letter-65].type != 3){
            alphabet[guess[i].letter-65].type = 2;
            guess[i].type = 2;
            temp1[temp2] = '*';
        }
    }
    return 0;
}

int main(){
    setbuf(stdout, NULL);
    bool guessed_correctly = 0;
    int number_of_guesses = 0;
    srand (time(NULL));
    char answer[MAX_WORD_LENGTH];
    strcpy(answer, answer_words[rand()%12]);
    ALPHABET alphabet[ALPHABET_LENGTH];
    ALPHABET guess[MAX_WORD_LENGTH + 1];
    loadAlphabet(alphabet);

    while (number_of_guesses < MAX_WORD_LENGTH+1 && guessed_correctly == 0){
        number_of_guesses++;
        printf("Guess number %d\n", number_of_guesses);
        getGuess(guess);
        if(guessCheck(guess, answer, alphabet) == 1){
            guessed_correctly = 1;
            printGuess(guess);
            printAlphabet(alphabet);
            printf("\n");
            continue;
        }
        printGuess(guess);
        printAlphabet(alphabet);
        printf("\n");
    }

    if (guessed_correctly == 0) {
        printf("You lost!\nThe word was %s\n", answer);
    }
    else {
        printf("You won!\nEasy win :smirk_cat: :thumbsup:\n");
    }
    return 0;
}

// Databaza answer slov
char * answer_words[] = {
        "PARER",
        "FOYER",
        "CATCH",
        "WATCH",
        "MUMMY",
        "CATER",
        "COYLY",
        "TRITE",
        "FOUND",
        "TACIT",
        "CHEST",
        "CREPE"
};