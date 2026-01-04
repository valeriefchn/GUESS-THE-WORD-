/* AOL Algopro Question 2
GUESS THE WORD!
1. Arby Tahta Mahesa Al-Irfan - 2902617283
2. Freya Aurora Santosa - 2902723206
3. Valerie Felicia Chen - 2902579972
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>

#define MAX_WORD_LENGTH 100
#define MAX_USERS 1000
#define MAX_LINE 256

//utilities
void strip_newline(char *s) {
    size_t L = strlen(s);
    if (L > 0 && s[L-1] == '\n') s[L-1] = '\0';
}

int password_valid(const char *pass) {
    if ((int)strlen(pass) < 6) return 0;
    int has_digit = 0;
    for (int i = 0; pass[i]; i++){
	    if (isdigit((unsigned char)pass[i])){
	        has_digit = 1;	
		}
	}
    return has_digit;
}

//randomize (scramble) + hide original word (setiap huruf jadi * di compiler, pake conio.h)
void randomizeWord(char *str){
    int len = (int)strlen(str);
    if (len <= 1) return;

    char original[MAX_WORD_LENGTH];
    strcpy(original, str);

    int same;
    do {
        for (int i = 0; i < len; i++) {
            int r = rand() % len;
            char tmp = str[i];
            str[i] = str[r];
            str[r] = tmp;
        }
        same = (strcmp(str, original) == 0);
        if (same) strcpy(str, original); //save dulu yg ori, trs scramble lagi
    } while (same);
}

void inputTersembunyi(char *word, int max_len) {
    int i = 0;
    char ch;
    
    while (1) {
        ch = getch(); //ambil karakter tanpa ditampilin

        //jika menekan enter (ASCII 13)
        if (ch == 13 || ch == 10) {
            word[i] = '\0';
            break;
        } 
        //jika menekan backspace (ASCII 8)
        else if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b"); //hapus tanda '*' terakhir di layar
            }
        } 
        //jika karakter biasa dan belum melebihi batas
        else if (i < max_len - 1) {
            word[i++] = ch;
            printf("*"); //tampilkan bintang sebagai pengganti
        }
    }
    printf("\n");
}

//username + pass (file"nya)
int username_exists(const char *username) {
    FILE *f = fopen("users.txt", "r");
    if (!f) return 0;
    char u[MAX_LINE], p[MAX_LINE];
    while (fscanf(f, "%s %s", u, p) == 2) {
        if (strcmp(u, username) == 0) { fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

int verify_user(const char *username, const char *password) {
    FILE *f = fopen("users.txt", "r");
    if (!f) return 0;
    char u[MAX_LINE], p[MAX_LINE];
    while (fscanf(f, "%s %s", u, p) == 2) {
        if (strcmp(u, username) == 0 && strcmp(p, password) == 0) { fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

void register_user() {
    char username[100], password[100];
    printf("=== REGISTER ===\n");
    printf("Enter new username: ");
    scanf("%s", username);

    if (username_exists(username)) {
        printf("Username already exists. Aborting register.\n");
        return;
    }

    do {
        printf("Enter password (min 6 chars, must contain a digit): ");
        scanf("%s", password);
        if (!password_valid(password))
            printf("Password does not meet rules. Try again.\n");
    } while (!password_valid(password));

    FILE *f = fopen("users.txt", "a");
    if (!f) { printf("Failed to open users file for writing.\n"); return; }
    fprintf(f, "%s %s\n", username, password);
    fclose(f);
    printf("Registration successful for %s\n\n", username);
}

//scores (filenya)
int load_total_score(const char *username) {
    FILE *f = fopen("scores.txt", "r");
    if (!f) return 0;
    char u[MAX_LINE];
    int s;
    while (fscanf(f, "%s %d", u, &s) == 2) {
        if (strcmp(u, username) == 0) { fclose(f); return s; }
    }
    fclose(f);
    return 0;
}

void update_total_score(const char *username, int add) {
    //baca filenya
    char users[MAX_USERS][100];
    int scores[MAX_USERS];
    int count = 0;
    FILE *f = fopen("scores.txt", "r");
    if (f) {
        while (fscanf(f, "%s %d", users[count], &scores[count]) == 2) count++;
        fclose(f);
    }

    //update atau tambahke filenya
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i], username) == 0) { scores[i] += add; found = 1; break; }
    }
    if (!found) {
        strcpy(users[count], username);
        scores[count] = add;
        count++;
    }

    //menulis ulang file
    f = fopen("scores.txt", "w");
    if (!f) { printf("Failed to open scores file for writing.\n"); return; }
    for (int i = 0; i < count; i++) fprintf(f, "%s %d\n", users[i], scores[i]);
    fclose(f);
}

//match logging and pair leaderboard
void append_match_record(const char *p1, const char *p2, int s1, int s2) {
    FILE *f = fopen("matches.txt", "a");
    if (!f) return;
    fprintf(f, "%s %s %d %d\n", p1, p2, s1, s2);
    fclose(f);
}

//fungsi biar pairnya alphabetical order di leaderboard
void make_pair_name(const char *a, const char *b, char *out) {
    if (strcmp(a, b) <= 0) sprintf(out, "%s-%s", a, b);
    else sprintf(out, "%s-%s", b, a);
}

void update_pair_total(const char *p1, const char *p2, int pair_add) {
    char pairname[201];
    make_pair_name(p1, p2, pairname);

    //membaca pair
    char pairs[MAX_USERS][201];
    int totals[MAX_USERS];
    int count = 0;
    FILE *f = fopen("pairs.txt", "r");
    if (f) {
        while (fscanf(f, "%s %d", pairs[count], &totals[count]) == 2) count++;
        fclose(f);
    }

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(pairs[i], pairname) == 0) { totals[i] += pair_add; found = 1; break; }
    }
    if (!found) {
        strcpy(pairs[count], pairname);
        totals[count] = pair_add;
        count++;
    }

    //tulis ulang
    f = fopen("pairs.txt", "w");
    if (!f) { printf("Failed to write pairs file.\n"); return; }
    for (int i = 0; i < count; i++) fprintf(f, "%s %d\n", pairs[i], totals[i]);
    fclose(f);
}

//sorting + display leaderboard
void bubbleSort(char names[][201], int scores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // cek jika skor saat ini lebih kecil dari skor berikutnya
            if (scores[j] < scores[j + 1]) {
                //tukar skor
                int tempScore = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tempScore;

                //tukar nama (harus ikut ditukar agar data tidak tertukar)
                char tempName[201];
                strcpy(tempName, names[j]);
                strcpy(names[j], names[j + 1]);
                strcpy(names[j + 1], tempName);
            }
        }
    }
}

void show_player_leaderboard() {
    char users[MAX_USERS][201];
    int scores[MAX_USERS];
    int count = 0;

    FILE *f = fopen("scores.txt", "r");
    if (!f) { printf("No scores yet.\n"); return; }

    while (fscanf(f, "%s %d", users[count], &scores[count]) == 2) {
        count++;
    }
    fclose(f);

    //panggil fungsi sorting
    bubbleSort(users, scores, count);

    printf("\n--- Player Leaderboard (Sorted) ---\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s : %d\n", i + 1, users[i], scores[i]);
    }
    printf("-----------------------------------\n");
}

void show_pair_leaderboard() {
    char pairs[MAX_USERS][201];
    int totals[MAX_USERS];
    int count = 0;

    FILE *f = fopen("pairs.txt", "r");
    if (!f) { printf("No pair data yet.\n"); return; }

    while (fscanf(f, "%s %d", pairs[count], &totals[count]) == 2) {
        count++;
    }
    fclose(f);

    bubbleSort(pairs, totals, count);

    printf("\n--- Pair Leaderboard (Sorted) ---\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s : %d\n", i + 1, pairs[i], totals[i]);
    }
    printf("---------------------------------\n");
}

//input helpers (biar inputnya ga ngaco)
int read_int_in_range(const char *prompt, int minv, int maxv) {
    int v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &v) == 1) {
            if (v >= minv && v <= maxv) {
                //clear newline
                int c; while ((c = getchar()) != '\n' && c != EOF) {}
                return v;
            }
        }
        //clear bad input
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Invalid input. Try again.\n");
    }
}

//GAMEPLAY
int play_rounds_and_get_scores(const char *player1, const char *player2) {
    //return pair total (p1+p2) + save skor individual ke file
    int p1score = 0, p2score = 0;
    char w1[MAX_WORD_LENGTH], w2[MAX_WORD_LENGTH];
    char s1[MAX_WORD_LENGTH], s2[MAX_WORD_LENGTH];
    char guess[MAX_WORD_LENGTH];

    int rounds = read_int_in_range("How many rounds do you want to play? (1-10): ", 1, 10);

    for (int r = 1; r <= rounds; r++) {
        printf("\n=== ROUND %d ===\n", r);

        //Player1 memberikan kata utk Player2 tebak
        printf("%s: Enter a word (no spaces): ", player1);
        inputTersembunyi(w1, MAX_WORD_LENGTH);

        //Player2 memberikan kata utk Player1 tebak
        printf("%s: Enter a word (no spaces): ", player2);
        inputTersembunyi(w2, MAX_WORD_LENGTH);

        //scramble copies
        strcpy(s1, w1); randomizeWord(s1);
        strcpy(s2, w2); randomizeWord(s2);

        //Player2 menebak w1
        printf("\n%s's turn to guess. Scrambled: %s\n", player2, s1);
        for (int attempt = 1; attempt <= 3; attempt++) {
            printf("Attempt %d: ", attempt);
            scanf("%s", guess);
            if (strcmp(guess, w1) == 0) {
                int pts = 4 - attempt; // 3,2,1
                printf("Correct! %s gets %d points.\n", player2, pts);
                p2score += pts;
                break;
            } else {
                printf("Incorrect.\n");
            }
        }

        //Player1 menebak w2
        printf("\n%s's turn to guess. Scrambled: %s\n", player1, s2);
        for (int attempt = 1; attempt <= 3; attempt++) {
            printf("Attempt %d: ", attempt);
            scanf("%s", guess);
            if (strcmp(guess, w2) == 0) {
                int pts = 4 - attempt;
                printf("Correct! %s gets %d points.\n", player1, pts);
                p1score += pts;
                break;
            } else {
                printf("Incorrect.\n");
            }
        }
    }

    //menunjukkan hasil round
    printf("\n=== Match Result ===\n");
    printf("%s : %d points\n", player1, p1score);
    printf("%s : %d points\n", player2, p2score);

    //update files: total scores, matches, pair totals
    update_total_score(player1, p1score);
    update_total_score(player2, p2score);
    append_match_record(player1, player2, p1score, p2score);
    int pair_total = p1score + p2score;
    update_pair_total(player1, player2, pair_total);
    
    if (p1score > p2score) printf("\nWinner: %s\n", player1);
    else if (p2score > p1score) printf("\nWinner: %s\n", player2);
    else printf("\nIt's a tie!\n");

    return 0;
}

//FUNGSI MAIN
int main() {
    srand((unsigned int)time(NULL));

    printf("=== Welcome to Tebak Kata ===\n");

    while (1) {
        printf("\nMAIN MENU\n1. Register\n2. Login (both players)\n3. View Leaderboards\n4. Exit\nChoose: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        if (choice == 1) {
            register_user();
        } else if (choice == 2) {
            //login utk kedua player
            char user1[100], pass1[100];
            char user2[100], pass2[100];

            printf("\n=== Player 1 Login ===\n");
            printf("Username: "); scanf("%s", user1);
            printf("Password: "); scanf("%s", pass1);
            if (!verify_user(user1, pass1)) {
                printf("Player 1 login failed.\n");
                continue;
            }

            printf("\n=== Player 2 Login ===\n");
            printf("Username: "); scanf("%s", user2);
            printf("Password: "); scanf("%s", pass2);
            if (!verify_user(user2, pass2)) {
                printf("Player 2 login failed.\n");
                continue;
            }

            if (strcmp(user1, user2) == 0) {
                printf("Both players cannot use the same account. Login again.\n");
                continue;
            }

            printf("\nLogin successful for both players!\n");

            //menu di dalam game untuk player" yg sudah login 
            while (1) {
                printf("\nGAME MENU\n1. Play Game (P1 vs P2)\n2. View My Total Scores\n3. Logout to Main Menu\nChoose: ");
                int gchoice;
                if (scanf("%d", &gchoice) != 1) { int c; while ((c = getchar()) != '\n' && c != EOF) {} continue; }
                if (gchoice == 1) {
                    play_rounds_and_get_scores(user1, user2);
                } else if (gchoice == 2) {
                    int s1 = load_total_score(user1);
                    int s2 = load_total_score(user2);
                    printf("%s total score: %d\n", user1, s1);
                    printf("%s total score: %d\n", user2, s2);
                } else if (gchoice == 3) {
                    printf("Logging out players...\n");
                    break;
                } else {
                    printf("Invalid option.\n");
                }
            }
        } else if (choice == 3) {
            //menu leaderboard
            while (1) {
                printf("\nLEADERBOARDS\n1. Player Totals\n2. Pair Totals\n3. Back\nChoose: ");
                int lchoice;
                if (scanf("%d", &lchoice) != 1) { int c; while ((c = getchar()) != '\n' && c != EOF) {} continue; }
                if (lchoice == 1) show_player_leaderboard();
                else if (lchoice == 2) show_pair_leaderboard();
                else if (lchoice == 3) break;
                else printf("Invalid.\n");
            }
        } else if (choice == 4) {
            printf("Goodbye!\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
