#ifndef MORSEUTILS_H
#define MORSEUTILS_H

// Include the QMap class for key-value pairs
#include <QMap>
// Include the QString class for string manipulation
#include <QString>

// Utility class for Morse code operations
class MorseUtils {
public:
    // Static method to retrieve the map of Characters to Morse Code strings
    static QMap<QChar, QString> getMorseMap() {
        // Static local variable to ensure the map is initialized only once
        static QMap<QChar, QString> map;
        
        // Check if the map is empty (first time call)
        if (map.isEmpty()) {
            // Insert alphabet characters and their Morse representations
            map.insert('A', ".-"); map.insert('B', "-..."); map.insert('C', "-.-.");
            map.insert('D', "-.."); map.insert('E', "."); map.insert('F', "..-.");
            map.insert('G', "--."); map.insert('H', "...."); map.insert('I', "..");
            map.insert('J', ".---"); map.insert('K', "-.-"); map.insert('L', ".-..");
            map.insert('M', "--"); map.insert('N', "-."); map.insert('O', "---");
            map.insert('P', ".--."); map.insert('Q', "--.-"); map.insert('R', ".-.");
            map.insert('S', "..."); map.insert('T', "-"); map.insert('U', "..-");
            map.insert('V', "...-"); map.insert('W', ".--"); map.insert('X', "-..-");
            map.insert('Y', "-.--"); map.insert('Z', "--..");
            
            // Insert numeric digits
            map.insert('1', ".----"); map.insert('2', "..---"); map.insert('3', "...--");
            map.insert('4', "....-"); map.insert('5', "....."); map.insert('6', "-....");
            map.insert('7', "--..."); map.insert('8', "---.."); map.insert('9', "----.");
            map.insert('0', "-----");
            
            // Insert punctuation mappings
            map.insert(',', "--..--"); map.insert('.', ".-.-.-"); map.insert('?', "..--..");
            map.insert('/', "-..-."); map.insert('-', "-....-");
            map.insert('(', "-.--."); map.insert(')', "-.--.-");
        }
        // Return the populated map
        return map;
    }

    // Static method to get a list of training words
    static QStringList getTrainingWords() {
        // Return a statically defined list of words for practice
        return {
            "ARDUINO", "LINUX", "KEYER", "RADIO", "SIGNAL", "CQ", "SOS", "TEST",
            "PARIS", "HELLO", "WORLD", "PYTHON", "CODE", "HAM", "CW", "73"
        };
    }
};

#endif // MORSEUTILS_H
