#euroleague-sim-c

A simple Euroleague-style basketball tournament simulator written in C.

Teams have:

    Attack power (home & away strength)

    Psychology factor (0–10, evolves with wins/losses)

    Home-court advantage (when playing in their own country)

The program simulates the entire tournament and saves results to a .txt file chosen by the user.
Features

    Reads teams from teams.txt

    Simulates all rounds of a Euroleague-style format

    Psychology changes dynamically after each game

    Results are printed on screen and also saved to an output file

Tournament Format

    Round 1: all teams play each other

    Round 2 (Top 8 Knockouts): seeded matchups (0–7, 1–6, 2–5, 3–4)

    Final 4: semifinals & final ranking

Input Format

    File: teams.txt

    Each line = one team, format:

    Team_Name Country HomeStrength AwayStrength
