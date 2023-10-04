/*
 * File: chorale-solver.cpp
 * Name: Victor Lin
 * ---------------
 * This file contains the main part of the chorale solver program. It contains all the user interface and the algorithms necessary to calculate which notes are in the next chord.
 */

#include <iostream>
#include "console.h"
#include "simpio.h" // getLine
#include "gobjects.h"
#include "choraledisplay.h"
#include "chorale-constants.h"
#include "vector.h"

/**
 * Function: welcome
 * -----------------
 * Prints out a fun welcome message to the user.
 */

void welcome() {
    setConsoleFont("SansSerif-16");
    std::cout << "Welcome to the 4-Part Chorale Solver!" << std::endl;
    std::cout << std::endl;
    std::cout << "A four-part chorale refers to music written for four voices ";
    std::cout << "where the various musical parts can give a different note ";
    std::cout << "for each chord of the music." << std::endl;
    std::cout << std::endl;
    std::cout << "In the 17th century, there were many rules about how you ";
    std::cout << "could and couldn't write 4-part chorales. That means you ";
    std::cout << "can use those rules to generate a 4-part chorale on a ";
    std::cout << "computer! Input the bass line, and we will generate one ";
    std::cout << "possible chorale for you." << std::endl;
    std::cout << std::endl;
    getLine("Press ENTER to continue...   ");
}

/**
 * Function: menu
 * --------------
 * Displays the menu and gets the user's choice. Adapted from the Huffman assignment.
 */

static std::string menu() {
    std::cout << std::endl;
    std::cout << "1) input bass line" << std::endl;
    std::cout << "2) look up key number" << std::endl;
    std::cout << "3) more information" << std::endl;
    std::cout << "Q) quit" << std::endl;

    std::cout << std::endl;
    std::string choice = toUpperCase(trim(getLine("Your choice? ")));
    return choice;
}

/**
 * Function: setUpKeyMap
 * ---------------------
 * Creates a map containing all the key numbers on the interface and the names of the keys they would map to on a real-life piano.
 */

static Map<int, std::string> setUpKeyMap() {
    for (int i = BASS_MIN; i <= SOPRANO_MAX; ++i) {
        if (i % 12 == 0) keyMap[i] = "C";
        else if (i % 12 == 1) keyMap[i] = "C#";
        else if (i % 12 == 2) keyMap[i] = "D";
        else if (i % 12 == 3) keyMap[i] = "Eb";
        else if (i % 12 == 4) keyMap[i] = "E";
        else if (i % 12 == 5) keyMap[i] = "F";
        else if (i % 12 == 6) keyMap[i] = "F#";
        else if (i % 12 == 7) keyMap[i] = "G";
        else if (i % 12 == 8) keyMap[i] = "Ab";
        else if (i % 12 == 9) keyMap[i] = "A";
        else if (i % 12 == 10) keyMap[i] = "Bb";
        else if (i % 12 == 11) keyMap[i] = "B";
    }
    return keyMap;
}

/**
 * Function: displayRules
 * ----------------------
 * Displays the rules that the algorithm follows to generate the chorale.
 */

static void displayRules() {
    std::cout << std::endl;
    std::cout << "Here are the rules I am using to generate harmonies:" << std::endl;
    std::cout << "\tThere must be at least 3 chords in the sequence" << std::endl;
    std::cout << "\tEach note in the bass line must be in the scale of the starting note" << std::endl;
    std::cout << "\tThe sequence must begin and end on the same chord" << std::endl;
    std::cout << "\tThe chord preceding the last one must be a V chord" << std::endl;
    std::cout << "\tIf possible, use root-position chords, and 1st inversion only when necessary" << std::endl;
    std::cout << "\tThere are limits to how low and high each voice can go, since the parts are meant to be sung by humans" << std::endl;
    std::cout << "\tThe top and second-to-top voice should never be more than an octave apart, and the two middle voices should never be more than an octave apart" << std::endl;
    std::cout << "\tParallel octaves and 5ths are not allowed" << std::endl;
    std::cout << "\tBetween the four voices, two should play the root of the chord, one should play the third tone, and one should play the fifth tone" << std::endl;
    std::cout << std::endl;
}

/**
 * Function: setUpChordRels
 * ----------------------
 * This function creates a vector of vectors, where the index of the big vector corresponds to a chord, and contains all acceptable chords that may follow it.
 */

static std::vector<std::vector<int>> setUpChordRels() {
    std::vector<std::vector<int>> chordRelations;
    chordRelations = { {}, {1, 2, 3, 4, 5, 6, 7}, {5, 7}, {4, 6}, {1, 2, 5}, {1, 6}, {2, 4}, {1, 5} };
    if (!majorKey) {
        chordRelations.push_back({3});
        chordRelations[1].push_back(8);
    }
    return chordRelations;
}

/**
 * Function: notInScale
 * --------------------
 * This function returns true if the next note is NOT in the scale of the starting note.
 * Note: The parameters are the numbers displayed on the keys.
 */

static bool notInScale(int nextNote, int startNote, bool majorKey) {
    // In this context, the variable distance refers to the distance between the key numbers of the two notes.
    int distance = (nextNote - startNote) % 12;
    // Ensure the distance is a positive number
    while (distance < 0)
        distance += 12;
    // If the key is major, the distances 1, 3, 6, 8, and 10 are not allowed
    if (majorKey && (distance == 1 || distance == 3 || distance == 6 || distance == 8 || distance == 10)) return true;
    // If the key is minor, the distances 1, 4, 6, 8, and 9 are not allowed
    else if (!majorKey && (distance == 1 || distance == 4 || distance == 6 || distance == 9)) return true;
    // Otherwise, the distance is allowed
    else return false;
}

/**
 * Function: getNotes
 * ------------------
 * This function gets the user's inputted bass line. It also handles whether the input is in major or minor, and makes sure the input is well-formed.
 */

static std::vector<int> getNotes(ChoraleDisplay& display) {
    std::vector<int> bassLine;
    // Re-initialize majorKey if it was set to false in an earlier run
    majorKey = true;
    std::string majorMinor = getLine("Press ENTER for a major key, or type \"minor\" for a minor key: ");
    if (toLowerCase(majorMinor) == "minor") {
        std::cout << "We are in a minor key!" << std::endl;
        majorKey = false;
    }
    else {
        std::cout << "We are in a major key!" << std::endl;
    }
    // Initialize length to 0 - ensure that the inputted line is long enough to analyze (3 or more notes)
    int length = 0;
    std::cout << "Type in your bass line one note at a time! Enter \"done\" to see the harmonization!" << std::endl;
    int startNote = getInteger("Enter your starting note - this note defines the key: ");
    while (startNote < BASS_MIN || startNote > BASS_MAX) {
        std::cout << "Oops! Your starting note must be between " << BASS_MIN << " and " << BASS_MAX << ". ";
        startNote = getInteger("Please try again: ");
    }
    // Add the start note to the bass line vector
    bassLine.push_back(startNote);
    // Increase the length of the melody
    ++length;
    // Highlight the note on the keyboard
    display.highlightKey(startNote, "purple", true);
    pause(1000);
    display.highlightKey(startNote, "purple", false);

    // Get notes from the user, ensuring the input is well-formed
    std::string nextNoteStr = getLine("Next note: ");
    while (true) {
        // If the user is done inputting notes, make sure the input was well-formed
        if (toLowerCase(nextNoteStr) == "done") {
            // Bass line must be 3 or more notes
            if (bassLine.size() < 3) {
                std::cout << "Oops! Your bass line is not long enough. ";
                nextNoteStr = getLine("Please try again: ");
            }
            // Bass line must end with I
            else if ((bassLine.back() - startNote) % 12 != 0) {
                std::cout << "Your melody must end with I. ";
                nextNoteStr = getLine("Please try again: ");
            }
            // Otherwise, input is well formed; stop the loop and return the vector
            else {
                break;
            }
        }

        // Check to make sure it's a number

        // Convert string to an int
        int nextNote = stringToInteger(nextNoteStr);

        // Note must be in valid bass range (0-24)
        if (nextNote < BASS_MIN || nextNote > BASS_MAX) {
            std::cout << "Oops! The note must be between " << BASS_MIN << " and " << BASS_MAX << ". ";
            nextNoteStr = getLine("Please try again: ");
        }
        // Note must be in the scale of the starting note
        else if (notInScale(nextNote, startNote, majorKey)) {
            std::cout << "That note isn't in the scale. ";
            nextNoteStr = getLine("Please try again: ");
        }
        // Otherwise, no errors - add this note to the vector, highlight it, and get the next note
        else {
            bassLine.push_back(nextNote);
            display.highlightKey(nextNote, "purple", true);
            pause(1000);
            display.highlightKey(nextNote, "purple", false);
            nextNoteStr = getLine("Next note: ");
        }
    }
    return bassLine;
}

/**
 * Function: distanceToChord
 * -------------------------
 * This function is a conversion that takes in the difference between the key numbers of some note and the starting note of the melody, and returns what interval that makes.
 */

static int distanceToChord(int distance) {
    // Ensure interval is positive
    while (distance < 0)
        distance += 12;
    if (majorKey) {
        switch (distance % 12) {
        case 0: return 1;
        case 2: return 2;
        case 4: return 3;
        case 5: return 4;
        case 7: return 5;
        case 9: return 6;
        case 11: return 7;
        default: return 0;
        }
    }
    else {
        switch (distance % 12) {
        case 0: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        case 7: return 5;
        case 8: return 6;
        case 11: return 7;
        case 10: return 8;
        default: return 0;
        }
    }
}

/**
 * Function: createChordProgression
 * --------------------------------
 * This function is a helper to the following function of the same name that contains more information. It creates a chord progression based on the user's inputted bass line. This function uses recursion, because as long as the next note is found in the current chord's list of acceptable progressions and can form an acceptable progression, it returns true.
 */

static bool createChordProgression(std::vector<int>& bass, Vector<int>& chords, const std::vector<std::vector<int>>& chordRelations, int index, int startingNote, int currentChord) {
    // Base case - chord before last must be V
    if (index == (int)(bass.size() - 2)) {
        if (currentChord != 5) {
            return false;
        }
        else {
            chords.push_back(1);
            return true;
        }
    }
    else {
        // Try making the next note the root of the next chord - use the distanceToChord conversion to see what chord that interval is.
        int distance = (bass[index + 1] - startingNote) % 12;
        int nextChord = distanceToChord(distance);
        // VII should never be in root position. If the chord is VII, change it to V in first inversion.
        if (nextChord == 7) nextChord = 5;
        for (int possibleChord: chordRelations[currentChord]) {
            // This block should only execute once. If the next chord is in the list of acceptable progressions, we will try adding it as the root of the chord.
            if (possibleChord == nextChord) {
                chords.push_back(nextChord);
                if (createChordProgression(bass, chords, chordRelations, index + 1, startingNote, nextChord)) {
                    return true;
                }
                chords.remove(index);
            }
        }

        // If that doesn't work, we will try to add it as the third degree of the chord (1st inversion).
        int firstInvChord = (nextChord - 2) % 12;
        // If the chord is II, subtracting 2 gives 0, when it should give 7 or 8 (depending on what chord comes after). In a major key it will always be 7.
        if (nextChord == 2) {
            if (majorKey)
                firstInvChord = 7;
            // Minor key
            else {
                // If the next chord's distanceToChord is a 3, make the current chord 8 (major VII), because major VII goes to III. We are guaranteed that there are at least two chords following the current one, because our base case executes when there are only two chords remaining.
                int nextDistance = (bass[index + 2] - startingNote) % 12;
                int nextNextChord = distanceToChord(nextDistance);
                if (nextNextChord == 3)
                    firstInvChord = 8;
                else
                    firstInvChord = 7;
            }
        }
        for (int possibleChord: chordRelations[currentChord]) {
            if (possibleChord == firstInvChord) {
                chords.push_back(firstInvChord);
                if (createChordProgression(bass, chords, chordRelations, index + 1, startingNote, firstInvChord)) {
                    return true;
                }
                chords.remove(index);
            }
        }
    }
    // If none of the above work, we have not found a suitable chord progression and will return false.
    return false;
}

/**
 * Function: createChordProgression
 * --------------------------------
 * This function is a wrapper around the previous recursive function. It starts off the chords vector with a I chord, since by our rules we want all chorales to start with I and end with V-I.
 */

static bool createChordProgression(std::vector<int>& bass, Vector<int>& chords, const std::vector<std::vector<int>>& chordRelations) {
    // Assume that bass is well formed - more than 3 notes, all notes in key, begins and ends with I.
    int startingNote = bass[0];
    chords.push_back(1);
    return createChordProgression(bass, chords, chordRelations, 0, startingNote, 1);
}

/**
 * Function: fillMajorChordVector
 * ------------------------------
 * This function takes in a subvector of notesInChords and an index, which is the note at the root of the chord to be added. It adds the root, the note 4 keys above the root, and the note 7 keys above the root (3 above the previous) to form a major chord, then repeats until the note goes out of range.
 */

static void fillMajorChordVector(std::vector<int>& v, int index) {
    while (index <= SOPRANO_MAX) {
        v.push_back(index);
        index += 4;
        if (index <= SOPRANO_MAX) {
            v.push_back(index);
            index += 3;
            if (index <= SOPRANO_MAX) {
                v.push_back(index);
                index += 5;
            }
        }
    }
}

/**
 * Function: fillMinorChordVector
 * ------------------------------
 * This function takes in a subvector of notesInChords and an index, which is the note at the root of the chord to be added. It adds the root, the note 3 keys above the root, and the note 7 keys above the root (4 above the previous) to form a minor chord, then repeats until the note goes out of range.
 */

static void fillMinorChordVector(std::vector<int> &v, int index) {
    while (index <= SOPRANO_MAX) {
        v.push_back(index);
        index += 3;
        if (index <= SOPRANO_MAX) {
            v.push_back(index);
            index += 4;
            if (index <= SOPRANO_MAX) {
                v.push_back(index);
                index += 5;
            }
        }
    }
}

/**
 * Function: fillDimChordVector
 * ------------------------------
 * This function takes in a subvector of notesInChords and an index, which is the note at the root of the chord to be added. It adds the root, the note 3 keys above the root, and the note 6 keys above the root (3 above the previous) to form a diminished chord, then repeats until the note goes out of range.
 */

static void fillDimChordVector(std::vector<int> &v, int index) {
    while (index <= SOPRANO_MAX) {
        v.push_back(index);
        index += 3;
        if (index <= SOPRANO_MAX) {
            v.push_back(index);
            index += 3;
            if (index <= SOPRANO_MAX) {
                v.push_back(index);
                index += 6;
            }
        }
    }
}

/**
 * Function: establishNotesInChords
 * --------------------------------
 * This function takes a starting note, and then creates a vector of vectors containing the chords of all the notes in the scale the starting note. For example, given C major, the notesInChords vector would end up with all notes in a C major triad in index 1, a d minor triad in index 2, etc.
 */

static std::vector<std::vector<int>> establishNotesInChords(int startNote, bool majorKey) {
    // Get the start note in the lowest octave
    while (startNote >= 12)
        startNote -= 12;

    std::vector<std::vector<int>> notesInChords;
    // Distance between the current note and the start note
    int distance = 0;
    // Fill the vector with 9 empty sub-vectors
    for (int i = 0; i < 9; ++i) {
        notesInChords.push_back({});
    }
    if (majorKey) {
        // Fill vectors index 1-7 with their respective chords
        for (int i = 1; i <= 7; ++i) {
            // If it's a major chord (i == 1, 4, 5) add a major chord to the ith vector in notesInChords
            if (i == 1 || i == 4 || i == 5) {
                fillMajorChordVector(notesInChords[i], startNote + distance);
            }
            // If it's a minor chord (i == 2, 3, 6) add a minor chord to the ith vector in notesInChords
            if (i == 2 || i == 3 || i == 6) {
                fillMinorChordVector(notesInChords[i], startNote + distance);
            }
            // If it's a diminished chord (i == 7) add a diminished chord to the ith vector in notesInChords
            if (i == 7) {
                fillDimChordVector(notesInChords[i], startNote + distance);
            }
            // Increment interval
            ++distance;
            if (i != 3) {
                ++distance;
            }
        }
    }
    // Minor key
    else {
        // Fill the vectors indexed 1-8 with all notes in their respective chords
        for (int i = 1; i <= 8; ++i) {
            // If it's a major chord (i == 3, 5, 6, 8) add a major chord to the ith vector in notesInChords
            if (i == 3 || i == 5 || i == 6 || i == 8) {
                fillMajorChordVector(notesInChords[i], startNote + distance);
            }
            // If it's a minor chord (i == 1, 4) add a minor chord to the ith vector in notesInChords
            if (i == 1 || i == 4) {
                fillMinorChordVector(notesInChords[i], startNote + distance);
            }
            // If it's a diminished chord (i == 2, 7) add a diminished chord to the ith vector in notesInChords
            if (i == 2 || i == 7) {
                fillDimChordVector(notesInChords[i], startNote + distance);
            }
            // Increment interval
            ++distance;
            if (i != 2 && i != 5) {
                ++distance;
            }
            if (i == 6) {
                ++distance;
            }
            if (i == 7) {
                distance -= 2;
            }
        }
    }
    return notesInChords;
}

/**
 * Function: nextLowerNote
 * -----------------------
 * This function takes in a chord (sorted vector) and a note in the previous chord in the sequence, and returns the highest note in the second chord that is lower than or equal to the note passed in.
 */

static int nextLowerNote(const std::vector<int>& chord, int note) {
    // Binary search
    int lhs = 0;
    int rhs = chord.size() - 1;
    while (lhs <= rhs) {
        int avg = (lhs + rhs) / 2;
        if (chord[avg] == note) return chord[avg];
        if (chord[avg] < note) {
            lhs = avg + 1;
        }
        else {
            rhs = avg - 1;
        }
    }
    // Because the average calculation uses integer division, if we are not at the target note, the right hand side will be lower than the left hand side.
    return chord[rhs];
}

/**
 * Function: nextHigherNote
 * -----------------------
 * This function takes in a chord (sorted vector) and a note in the previous chord in the sequence, and returns the lowest note in the second chord that is higher than or equal to the note passed in.
 */

static int nextHigherNote(const std::vector<int>& chord, int note) {
    // Binary search
    int lhs = 0;
    int rhs = chord.size() - 1;
    while (lhs <= rhs) {
        int avg = (lhs + rhs + 1) / 2;
        if (chord[avg] == note) return chord[avg];
        if (chord[avg] < note) {
            lhs = avg + 1;
        }
        else {
            rhs = avg - 1;
        }
    }
    // Since lhs is greater than rhs if the loop terminates, we want to return the greater value.
    return chord[lhs];
}

/**
 * Function: canCreateChoraleHelper
 * --------------------------------
 * This function is a helper function to canCreateChorale. It handles the recursive component. We take advantage of the fact that a good chorale can generally be found by finding the lowest note above the current note in the next chord if the bass is moving down (or up a fourth) and finding the highest note below the current note in the next chord if the bass is moving up.
 */

static bool canCreateChoraleHelper(Vector<int>& chords, const std::vector<std::vector<int>>& notesInChords, std::vector<int>& soprano, std::vector<int>& alto, std::vector<int>& tenor, const std::vector<int>& bass, int index, bool LTCorrected) {
    // Base case - if index == chords.size(), then we have finished
    if (index >= chords.size()) {
        return true;
    }

    // Make sure parts are not going out of range
    if (soprano.back() > SOPRANO_MAX || alto.back() > ALTO_MAX || tenor.back() > TENOR_MAX || soprano.back() < SOPRANO_MIN || alto.back() < ALTO_MIN || tenor.back() < TENOR_MIN) return false;

    // Check if bass is moving up or down (index compared to index - 1)
    // Move voices in opposite direction using nextLowerNote or nextHigherNote (pass in chords[index] as the vector). This method should ensure the right distribution of scale tones and prevent parallel 5ths/octaves.
    // The only exception to this is if the soprano has a leading tone in a V chord (distanceToChord == 7) and the bass moves up. In that case, the soprano should also move up. In that case, the soprano should be treated like it moved down (the nextLowerNote call should pass in the note the soprano should have had).
    // If any other checks fail (voice crossing, parts going out of range) return false

    // If the bass is moving down, or if the bass is moving up a distance of 5
    if (bass[index] < bass[index - 1] || (bass[index] - bass[index - 1] == 5)) {
        // LTCorrected is only true if the soprano moved differently than it should have due to a leading tone. This check ensures that the soprano is not impacted by that change by passing in the next lower note in that chord.
        if (LTCorrected) {
            soprano.push_back(nextHigherNote(notesInChords[chords[index]], soprano.back() - 3));
        }
        // Move other voices up
        else {
            soprano.push_back(nextHigherNote(notesInChords[chords[index]], soprano.back()));
        }
        alto.push_back(nextHigherNote(notesInChords[chords[index]], alto.back()));
        tenor.push_back(nextHigherNote(notesInChords[chords[index]], tenor.back()));
        // Make sure parts are not going out of range
        if (soprano.back() > SOPRANO_MAX || alto.back() > ALTO_MAX || tenor.back() > TENOR_MAX) return false;
        // Voice crossing - tenor lower than upcoming bass
        if (index < (int)(bass.size() - 1)) {
            if (tenor.back() < bass[index + 1]) return false;
        }
    }
    // If the bass is moving up
    else if (bass[index] > bass[index - 1]) {
        // Push leading tone up if necessary (previous chord is V, and soprano has a leading tone)
        if (chords[index - 1] == 5 && distanceToChord(soprano.back() - bass[0]) == 7) {
            int leadingTone = soprano.back();
            soprano.push_back(leadingTone + 1);
            LTCorrected = true;
        }
        // LTCorrected is only true if the soprano moved differently than it should have due to a leading tone. This check ensures that the soprano is not impacted by that change.
        else if (LTCorrected) {
            soprano.push_back(nextLowerNote(notesInChords[chords[index]], soprano.back() - 3));
            LTCorrected = false;
        }
        else {
            soprano.push_back(nextLowerNote(notesInChords[chords[index]], soprano.back()));
        }
        // Move other voices down
        alto.push_back(nextLowerNote(notesInChords[chords[index]], alto.back()));
        tenor.push_back(nextLowerNote(notesInChords[chords[index]], tenor.back()));
        // Voice crossing - tenor lower than upcoming bass
        if (index < (int)(bass.size() - 1)) {
            if (tenor.back() < bass[index + 1]) return false;
        }
    }
    // Recurse
    if (canCreateChoraleHelper(chords, notesInChords, soprano, alto, tenor, bass, index + 1, LTCorrected)) return true;
    return false;
}

/**
 * Function: canCreateChorale
 * --------------------------
 * This function is a wrapper around the recursive function canCreateChoraleHelper. It basically tries different combinations of soprano, alto, and tenor until it works with the bass line.
 */

static bool canCreateChorale(Vector<int>& chords, const std::vector<std::vector<int>>& notesInChords, std::vector<int>& soprano, std::vector<int>& alto, std::vector<int>& tenor, const std::vector<int>& bass) {
    // Note that the way notesInChords is designed makes all 1's of the chord at indices congruent to 0 % 3, all 3's congruent to 1 % 3, and all 5's congruent to 2 % 3
    // Each chord must be as close to stepwise motion as possible
    // Each part must be between the MIN and MAX values specified
    // Unless the bass has a 3, one part should have a 1, another a 3, and another a 5.
    // No parallel octaves or 5ths
    // S/A and A/T must never be more than an octave apart

    // Try soprano as the highest tonic, alto as the dominant below that, tenor as the mediant below that
    int highestTonicIndex = ((notesInChords[1].size() - 1) / 3) * 3;
    if ((notesInChords[1][highestTonicIndex - 1] > ALTO_MAX || notesInChords[1][highestTonicIndex - 2] > TENOR_MAX) && notesInChords[1][highestTonicIndex - 3] > SOPRANO_MIN) {
        highestTonicIndex -= 3;
    }
    soprano.push_back(notesInChords[1][highestTonicIndex]);
    alto.push_back(notesInChords[1][highestTonicIndex - 1]);
    tenor.push_back(notesInChords[1][highestTonicIndex - 2]);
    if (canCreateChoraleHelper(chords, notesInChords, soprano, alto, tenor, bass, 1, false)) return true;
    // Try lots of different possibilities that aren't really in any sort of pattern
    soprano.clear();
    alto.clear();
    tenor.clear();
    // Reset highest tonic
    if ((notesInChords[1][highestTonicIndex + 3]) <= SOPRANO_MAX)
        highestTonicIndex += 3;
    // Try giving mediant to alto and dominant to tenor
    if (notesInChords[1][highestTonicIndex - 4] > bass[0] && notesInChords[1][highestTonicIndex - 4] > TENOR_MIN) {
        soprano.push_back(notesInChords[1][highestTonicIndex]);
        alto.push_back(notesInChords[1][highestTonicIndex - 2]);
        tenor.push_back(notesInChords[1][highestTonicIndex - 4]);
        if (canCreateChoraleHelper(chords, notesInChords, soprano, alto, tenor, bass, 1, false)) return true;
    }
    soprano.clear();
    alto.clear();
    tenor.clear();
    if ((notesInChords[1][highestTonicIndex + 1]) <= SOPRANO_MAX) {
        // Try starting soprano on mediant
        soprano.push_back(notesInChords[1][highestTonicIndex + 1]);
        alto.push_back(notesInChords[1][highestTonicIndex]);
        tenor.push_back(notesInChords[1][highestTonicIndex - 1]);
        if (canCreateChoraleHelper(chords, notesInChords, soprano, alto, tenor, bass, 1, false)) return true;
    }
    else if (notesInChords[1][highestTonicIndex - 4] > bass[0]) {
        // Try starting soprano on mediant, one octave lower
        soprano.push_back(notesInChords[1][highestTonicIndex - 2]);
        alto.push_back(notesInChords[1][highestTonicIndex]);
        tenor.push_back(notesInChords[1][highestTonicIndex - 1]);
        if (canCreateChoraleHelper(chords, notesInChords, soprano, alto, tenor, bass, 1, false)) return true;
    }
    return false;
}

int main() {
    ChoraleDisplay display;
    welcome();
    while (true) {
        std::string choice = menu();
        if (choice == "1") {
            // Set up vector indicating which chords can lead to which
            std::vector<std::vector<int>> chordRelations = setUpChordRels();
            // Get user input for bass line
            std::vector<int> bass = getNotes(display);
            Vector<int> chords;
            // Recursively create a chord progression
            if (!createChordProgression(bass, chords, chordRelations)) {
                std::cout << "No suitable chord progression found." << std::endl;
            }
            else {
                std::cout << "A chord progression was found! ";
                // Print out chord progression
                for (int i: chords) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;
                // Establish which notes are in which chords
                std::vector<std::vector<int>> notesInChords = establishNotesInChords(bass[0], majorKey);
                std::vector<int> soprano;
                std::vector<int> alto;
                std::vector<int> tenor;
                // Create other parts recursively
                if (!canCreateChorale(chords, notesInChords, soprano, alto, tenor, bass)) {
                    std::cout << "No solutions were found for that chord progression." << std::endl;
                }
                else {
                    std::cout << "Success!" << std::endl;
                    for (int i = 0; i < bass.size(); ++i) {
                        display.highlightKey(soprano[i], "blue", true);
                        display.highlightKey(alto[i], "green", true);
                        display.highlightKey(tenor[i], "red", true);
                        display.highlightKey(bass[i], "purple", true);
                        pause(1500);
                        display.highlightKey(soprano[i], "blue", false);
                        display.highlightKey(alto[i], "green", false);
                        display.highlightKey(tenor[i], "red", false);
                        display.highlightKey(bass[i], "purple", false);
                    }
                }
                std::cout << std::endl;
            }
        }
        else if (choice == "2") {
            keyMap = setUpKeyMap();
            int keyNumber = getInteger("Input the number of the key you want to look up: ");
            if (keyNumber >= BASS_MIN && keyNumber <= SOPRANO_MAX) {
                std::cout << keyNumber << " maps to " << keyMap[keyNumber] << std::to_string(keyNumber / 12 + 2) << std::endl;
                display.highlightKey(keyNumber, "blue", true);
                pause(1000);
                display.highlightKey(keyNumber, "blue", false);
            }
            else {
                std::cout << "Invalid key number." << std::endl;
            }
        }
        else if (choice == "3") {
            displayRules();
        }
        else if (choice[0] == 'Q') {
            std::cout << std::endl;
            std::cout << "Thanks for using the 4-Part Chorale Solver! Have a nice day!" << std::endl;
            break;
        }
    }
    return 0;
}
