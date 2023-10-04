/*
 * File: choraledisplay.cpp
 * Name: Victor Lin
 * ------------------------
 * This file contains the implementations of the functions defined in choraledisplay.h.
 */

#include "choraledisplay.h"
#include <iostream>

ChoraleDisplay::ChoraleDisplay() : window(WINDOW_WIDTH, WINDOW_HEIGHT) {
    window.setVisible(true);
    window.setWindowTitle("4-Part Chorale Solver");
    window.setRepaintImmediately(false);
    initializeKeys();
}

ChoraleDisplay::~ChoraleDisplay() {
    for (Key& key: keys) {
        delete key.rect;
        delete key.label;
    }
    window.close();
}

void ChoraleDisplay::repaint() {
    window.repaint();
}

void ChoraleDisplay::setTitle(const std::string& title) {
    window.setWindowTitle(title);
    windowTitle = title;
}

KeyColor ChoraleDisplay::getKeyColor(const int keyNumber) const {
    int index = keyNumber % 12;
    if (index == 0 || index == 2 || index == 4 || index == 5 || index == 7 || index == 9 || index == 11) return WHITE;
    else return BLACK;
}

void ChoraleDisplay::initializeKeys() {
    // Set coordinate counters
    int whiteKeyCounter = 0;
    int blackKeyCounter = 0;
    for (int i = 0; i < N_KEYS; ++i) {
        // White key
        if (getKeyColor(i) == WHITE) {
            // Create key
            Key whiteKey;

            // Create GRect with the same height as the window
            GRect *rect = new GRect(KEY_WIDTH, getHeight());

            // Draw rect
            rect->setX((KEY_WIDTH * whiteKeyCounter));
            rect->setY(0);
            rect->setFilled(false);
            rect->setColor("#000000");

            // Set struct data fields
            whiteKey.rect = rect;
            whiteKey.color = WHITE;
            whiteKey.number = i;
            whiteKey.keyCount = whiteKeyCounter;

            // Create label
            makeLabel(whiteKey);

            // Add to keys vector
            keys.push_back(whiteKey);

            // Draw key
            window.add(rect);
            window.add(whiteKey.label);

            ++whiteKeyCounter;
        }
        // Black key
        else {
            // Check to make sure key can be created
            if (blackKeyCounter % 7 == 2 || blackKeyCounter % 7 == 6)
                ++blackKeyCounter;

            // Create key
            Key blackKey;

            // Create GRect that is 2/3 window height
            GRect *rect = new GRect((KEY_WIDTH * 2.0) / 3, (getHeight() * 2.0) / 3);

            // Draw rect
            rect->setX((KEY_WIDTH * blackKeyCounter) + (KEY_WIDTH * 2.0 / 3));
            rect->setY(0);
            rect->setFilled(true);
            rect->setColor("#000000");

            // Set struct data fields
            blackKey.rect = rect;
            blackKey.color = BLACK;
            blackKey.number = i;
            blackKey.keyCount = blackKeyCounter;

            // Create label
            makeLabel(blackKey);

            // Add to keys vector
            keys.push_back(blackKey);

            // Draw key
            window.add(rect);
            window.add(blackKey.label);

            ++blackKeyCounter;
        }
    }
    repaint();
}

void ChoraleDisplay::makeLabel(Key& key) {
    if (key.color == WHITE) {
        // Label at the bottom of the screen
        GLabel *keyNumberLabel = new GLabel(std::to_string(key.number), (KEY_WIDTH * key.keyCount + LABEL_PADDING), getHeight() - LABEL_PADDING);
        keyNumberLabel->setColor("#000000");
        key.label = keyNumberLabel;
    }
    else {
        // Label at the bottom of the key
        GLabel *keyNumberLabel = new GLabel(std::to_string(key.number), ((KEY_WIDTH * key.keyCount) + (KEY_WIDTH * 2.0 / 3) + LABEL_PADDING), (getHeight() * 2.0) / 3 - LABEL_PADDING);
        keyNumberLabel->setColor("#ffffff");
        key.label = keyNumberLabel;
    }
}

void ChoraleDisplay::highlightKey(int keyNumber, std::string color, bool flag) {
    Key key = keys[keyNumber];
    if (flag) {
        // Set the color of the key based on the color (blue, green, red, or purple)
        if (color == "blue")
            key.rect->setFillColor("#0040c0");
        else if (color == "green")
            key.rect->setFillColor("#008000");
        else if (color == "red")
            key.rect->setFillColor("#ff0000");
        else if (color == "purple")
            key.rect->setFillColor("#800080");
        // For white keys, we have to redraw the neighboring keys
        if (key.color == WHITE) {
            key.rect->setFilled(true);
            window.add(key.rect);
            window.add(key.label);
            window.add(keys[keyNumber - 1].rect);
            window.add(keys[keyNumber - 1].label);
            window.add(keys[keyNumber + 1].rect);
            window.add(keys[keyNumber + 1].label);
        }
        // For black keys, just redraw the black key and its label
        else {
            window.add(key.rect);
            window.add(key.label);
        }
    }
    // Unhighlight by changing color back to black, and unfilling if white
    else {
        key.rect->setFillColor("#000000");
        // White key
        if (getKeyColor(keyNumber) == WHITE) {
            key.rect->setFilled(false);
            window.add(key.rect);
            window.add(key.label);
            window.add(keys[keyNumber - 1].rect);
            window.add(keys[keyNumber - 1].label);
            window.add(keys[keyNumber + 1].rect);
            window.add(keys[keyNumber + 1].label);
        }
        // Black key
        else {
            window.add(key.rect);
            window.add(key.label);
        }
    }
    repaint();
}

int ChoraleDisplay::getWidth() {
    return WINDOW_WIDTH;
}

int ChoraleDisplay::getHeight() {
    return WINDOW_HEIGHT;
}
