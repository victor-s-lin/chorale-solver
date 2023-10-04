/*
 * File: choraledisplay.h
 * Name: Victor Lin
 * ----------------------
 * This file defines the display for the chorale solver. It creates a keyboard of white and black keys.
 */

#ifndef CHORALEDISPLAY_H
#define CHORALEDISPLAY_H
#include "gwindow.h"
#include "gobjects.h"

enum KeyColor { BLACK, WHITE };

struct Key {
    GRect *rect;
    GLabel *label;
    KeyColor color;
    int number;
    int keyCount; // Which white key or black key it is, to make it easier to set the coordinates
};

class ChoraleDisplay {
public:
    ChoraleDisplay();
    ~ChoraleDisplay();

    /**
     * Method: repaint
     * This method repaints the window. Call it whenever you update something in the graphical display.
     */

    void repaint();

    /**
     * Method: setTitle
     * This method sets the title of the graphical display. It should only be called once during initialization.
     */

    void setTitle(const std::string& title);

    /**
     * Method: getKeyColor
     * This method returns the key's color given the key number.
     */

    KeyColor getKeyColor(const int keyNumber) const;

    /**
     * Method: initializeKeys
     * This method draws the keys on the graphical display and adds each Key into the vector of Keys stored as a private data member.
     */

    void initializeKeys();

    /**
     * Method: makeLabel
     * This is a helper function that positions the labels on the keys.
     */

    void makeLabel(Key &key);

    /**
     * Method: highlightKey
     * This method will highlight or unhighlight a key in the specified color, depending on whether the flag is true (highlight) or false (unhighlight).
     */
    void highlightKey(const int keyNumber, std::string color, bool flag);

    int getWidth();
    int getHeight();

private:
    GWindow window;
    std::string windowTitle;

    /* Note: each key's index in the vector is also stored as the key's number. */
    std::vector<Key> keys;

    static const int N_WHITE_KEYS = 26;
    static const int N_KEYS = 44;
    static const int LABEL_PADDING = 4;
    static const int WINDOW_HEIGHT = 240;
    static const int WINDOW_WIDTH = 900;
    static constexpr const double KEY_WIDTH = WINDOW_WIDTH / (N_WHITE_KEYS * 1.0);
};

#endif // CHORALEDISPLAY_H
