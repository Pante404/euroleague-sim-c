#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define LENGTH 100

typedef struct {
    char on[LENGTH];  /* team name */
    char cy[LENGTH];  /* country */
    char es[LENGTH];  /* home strength (string) */
    char ek[LENGTH];  /* away strength (string) */
} Team;

/*Helper: simulate one match*/
static void play_match(int A, int B, Team *teamsArray,
                       int *home_strength, int *away_strength,
                       int *psy, int *vathmoi,
                       const char *final4_country, int knockout)
{
    int d, d2;

    /* Same country → both use home strength */
    if (strcmp(teamsArray[A].cy, teamsArray[B].cy) == 0) {
        d  = home_strength[A];
        d2 = home_strength[B];
    } else {
        d  = home_strength[A];
        d2 = away_strength[B];
    }

    /* Host-country advantage (Final Four only) */
    if (final4_country != NULL) {
        if (strcmp(teamsArray[A].cy, final4_country) == 0) d = home_strength[A];
        if (strcmp(teamsArray[B].cy, final4_country) == 0) d2 = home_strength[B];
    }

    int pointA = 50 + rand() % (1 + d / 2) + 2 * psy[A];
    int pointB = 50 + rand() % (1 + d2 / 2) + 2 * psy[B];

    /* Update psychology */
    if (pointA > pointB) {
        if (psy[A] < 10) psy[A]++;
        if (psy[B] > 0)  psy[B]--;
    } else if (pointB > pointA) {
        if (psy[B] < 10) psy[B]++;
        if (psy[A] > 0)  psy[A]--;
    }

    /* Update points */
    if (pointA > pointB) {
        vathmoi[A] += 2;
        vathmoi[B] += 1;
    } else if (pointB > pointA) {
        vathmoi[B] += 2;
        vathmoi[A] += 1;
    } else {
        int coin = rand() % 2 + 1;
        vathmoi[A] += coin;
        if (coin > 1) vathmoi[B]++;
        else vathmoi[B] += 2;
    }
}

/*Sort teams by points*/
static void sort_by_points_desc(int *points, Team *arr, int n,
                                int *home_strength, int *away_strength, int *psy)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            if (points[j] < points[j + 1]) {
                int tp = points[j]; points[j] = points[j + 1]; points[j + 1] = tp;

                int th = home_strength[j]; home_strength[j] = home_strength[j+1]; home_strength[j+1] = th;
                int ta = away_strength[j]; away_strength[j] = away_strength[j+1]; away_strength[j+1] = ta;
                int ps = psy[j]; psy[j] = psy[j+1]; psy[j+1] = ps;

                Team tt; memcpy(&tt, &arr[j], sizeof(Team));
                memcpy(&arr[j], &arr[j + 1], sizeof(Team));
                memcpy(&arr[j + 1], &tt, sizeof(Team));
            }
        }
    }
}

int main(void)
{
    srand((unsigned)time(NULL));

    FILE *fp, *fp3;
    char out_name[LENGTH], final4_country[LENGTH];
    int teams = 0;

    fp = fopen("teams.txt", "r");
    if (!fp) { printf("Error opening teams1.txt\n"); return 1; }

    printf("Enter output file name: ");
    scanf("%99s", out_name);

    fp3 = fopen(out_name, "w+");
    if (!fp3) { printf("Error creating output file\n"); fclose(fp); return 1; }

    /* Count teams */
    char tmp[LENGTH * 4];
    while (fgets(tmp, sizeof(tmp), fp)) teams++;
    rewind(fp);

    if (teams < 4) {
        fprintf(stderr, "Need at least 4 teams\n");
        fclose(fp); fclose(fp3);
        return 1;
    }

    Team teamsArray[teams];
    int psy[teams], vathmoi[teams];
    int home_strength[teams], away_strength[teams];

    /* Read teams */
    for (int idx = 0; idx < teams; idx++) {
        if (fscanf(fp, "%99s %99s %99s %99s",
                   teamsArray[idx].on,
                   teamsArray[idx].cy,
                   teamsArray[idx].es,
                   teamsArray[idx].ek) != 4) {
            fprintf(stderr, "Input error at line %d\n", idx+1);
            fclose(fp); fclose(fp3);
            return 1;
        }
        home_strength[idx] = atoi(teamsArray[idx].es);
        away_strength[idx] = atoi(teamsArray[idx].ek);

        psy[idx] = home_strength[idx] / 10;
        if (psy[idx] < 0) psy[idx] = 0;
        if (psy[idx] > 10) psy[idx] = 10;
        vathmoi[idx] = 0;
    }
    fclose(fp);

    /* Ask for Final4 country */
    int valid = 0;
    while (!valid) {
        printf("Enter country for Final 4: ");
        scanf("%99s", final4_country);

        if (!isupper(final4_country[0])) {
            printf("Country must start with uppercase.\n");
            continue;
        }

        for (int k = 0; k < teams; k++) {
            if (strcmp(final4_country, teamsArray[k].cy) == 0) {
                valid = 1; break;
            }
        }
        if (!valid) printf("Country not found in teams list. Try again.\n");
    }

    /*Round1*/
    for (int i = 0; i < teams; i++) {
        for (int j = 0; j < teams; j++) {
            if (i == j) continue;
            play_match(i, j, teamsArray, home_strength, away_strength, psy, vathmoi, NULL, 0);
        }
    }

    sort_by_points_desc(vathmoi, teamsArray, teams, home_strength, away_strength, psy);
    fprintf(fp3, "Result:\n");
    for (int i = teams-1; i >= 0; i--) {  // print reversed
        if (i >= teams-8)
            printf("Round1: %d %s %s\n", vathmoi[i], teamsArray[i].on, teamsArray[i].cy);
        else
            printf("R1 TOP 8: %d %s %s\n", vathmoi[i], teamsArray[i].on, teamsArray[i].cy);
    }
    printf("\n");

    /*Round 2*/
    if (teams >= 8) {
        int left[4]  = {0, 1, 2, 3};
        int right[4] = {7, 6, 5, 4};
        for (int k = 0; k < 4; k++) {
            play_match(left[k], right[k], teamsArray, home_strength, away_strength, psy, vathmoi, NULL, 0);
        }
        sort_by_points_desc(vathmoi, teamsArray, 4, home_strength, away_strength, psy);
        for (int i = 3; i >= 0; i--) { // reversed
            printf("Round2 %d %s %s\n", vathmoi[i], teamsArray[i].on, teamsArray[i].cy);
        }
        printf("\n");
    }

    /*Final 4*/
    if (teams >= 4) {
        for (int i = 0; i < 4; i += 2) {
            play_match(i, i+1, teamsArray, home_strength, away_strength, psy, vathmoi, final4_country, 1);
        }
        sort_by_points_desc(vathmoi, teamsArray, 4, home_strength, away_strength, psy);
        for (int i = 3; i >= 0; i--) { // reversed
            printf("Final4 %d %s %s\n", vathmoi[i], teamsArray[i].on, teamsArray[i].cy);
        }
    }

    fclose(fp3);
    return 0;
}
