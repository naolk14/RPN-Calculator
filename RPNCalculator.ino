// PROJECT   :RPN Calculator
// PURPOSE   :Develop an RPN calculator through the use of stack push and pop
// COURSE    :ICS3U-E
// AUTHOR    :N. Ketema
// DATE      :2025 04 19
// MCU       :328P (Standalone)
// REFERENCE :Working

#include "Stack.h"          // Stack class
#include <LiquidCrystal.h>  // Liquid Crystal Library

#define DURATION 300
#define DEBUG 1

float currentNumber = 0.0;  // Stores the number being entered
int stackSize = 0;          // Counter to track the number of elements in the stack
Stack stack;                // Stack for storing # for post-fix operations

const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;  // constant LCD pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                 // Creates an instance

uint16_t thresholds[] = { 55, 58, 62, 66, 74, 81, 88, 97, 116, 131, 151, 178, 254, 340, 510, 1023 };  // max analog readings for each button on keypad
char keys[] = { '+', 'E', '.', '0', '-', '3', '2', '1', '*', '6', '5', '4', '/', '9', '8', '7' };     // corresponding button on the keypad

char inputBuffer[16];  // Buffer to store user input (digits and decimal point)
int bufferIndex = 0;   // Tracks the current position in the buffer

void setup() {                  // setup
  lcd.begin(16, 2);             // Initialize LCD
  lcd.setCursor(4, 0);          // shift over one column to put the text in the middle
  lcd.print("Welcome!");        // Print "Welcome!"
  lcd.setCursor(1, 1);          // set cursor on the second row on 16x2 LCD and shift one space  to the right
  lcd.print("RPN Calculator");  // print "RPN Calculator"
  Serial.begin(9600);           // baud rate at 9600
  while (!Serial)
    ;
}

char getKey() {        // returns corresponding character from button key press after reading it
  uint16_t value = 0;  // stores the 16-bit analog reading from key presses
  do {
    value = analogRead(A5);  // read it from analog pin 5 of the Nano
  } while (value == 0);      // reads until value is zero
  delay(DURATION);           // delay 300ms
  if (DEBUG) {               // shows analog value during debugging
    Serial.println(value);   // prints the analog value in the serial monitor (during debugging)
    Serial.print('\t');      // tab space
  }
  uint8_t index = 0;                          // Initialize the index to 0
  while (value > thresholds[index]) index++;  // Increment index until value is within threshold range
  return keys[index];                         // returns the key corresponding to index
}

void multipleDigits(char ch) {                    // allows multi-digit #s and not just single digit
  if ((ch >= '0' && ch <= '9') || ch == '.') {    // Allow digits and decimal point
    if (bufferIndex < sizeof(inputBuffer) - 1) {  // Ensure buffer doesn't overflow
      inputBuffer[bufferIndex++] = ch;            // Add character to the buffer
      inputBuffer[bufferIndex] = '\0';            // Null-terminate the string
      lcd.clear();                                // clear display
      lcd.print("Input: ");                       // display "Input:"
      lcd.print(inputBuffer);                     // Display the current input
    } else {
      lcd.clear();            // clear LCD if there is an overflow
      lcd.print("Error:");    //  print "Error: Overflow" on LCD
      lcd.setCursor(0, 1);    // set cursor at second row
      lcd.print("Overflow");  // triggered by
      lcd.setCursor(7, 0);
      lcd.print("|Reset|");
      lcd.setCursor(0, 1);
      lcd.print("Press = (Equal)");
      if (ch == 'E') {
        loop();
      }
    }
  }
}

void displayFormattedResult(float result, int decimalPlaces) {  //result with decimal places
  char formattedResult[16];                                     // Buffer to store the formatted result
  dtostrf(result, 4, 4, formattedResult);                       // Format the result ()
  lcd.clear();                                                  // clear display
  lcd.print("Result: ");                                        // print "Result: " on the display
  lcd.print(formattedResult);                                   // Display the formatted result
}

void pushStack() {
  if (bufferIndex > 0) {                // Ensure there's input in the buffer
    currentNumber = atof(inputBuffer);  // Convert the buffer to a float
    stack.push(currentNumber);          // Push the number onto the stack
    stackSize++;                        // stack increments by one
    // Reset the buffer for the next input
    bufferIndex = 0;           // sets position to the very beginning
    inputBuffer[0] = '\0';     // clears any previous input to allow buffer to be ready to accept new data
    lcd.print("Pushed: ");     // print "Pushed: " on the LCD
    lcd.print(currentNumber);  //print the number that is entered
    lcd.clear();               // clear display
    delay(400);                // clear for 400 miliseconds
    lcd.print("Input:");       // print "Input:"
  } else {                     // if not
    lcd.clear();               // clear display
    lcd.print("Error:");       // print "Error: No Input" on the LCD
    lcd.setCursor(0, 1);       // set cursor at second row
    lcd.print("No Input");
  }
}

void performOperation(char op) {
  // if the stack has less than two inputs, print "Error: Stack Empty"
  if (stackSize < 2) {    // if stack has less than 2
    lcd.clear();          // clear LCD display
    lcd.print("Error:");  // print "Error: Stack Empty" on LCD
    lcd.setCursor(0, 1);  // set cursor at second row
    lcd.print("Stack Empty");
    return;  // exits function
  }
  float b = stack.pop();               // removes the second input from stack and returns it
  stackSize--;                         // decrements by one
  float a = stack.pop();               // removes the first input
  stackSize--;                         // decrements by one
  float result = 0.0;                  // no inuts left on the stack
  if (op == '+') result = a + b;       // if the operator is a plus symbol, add inputs a and b
  else if (op == '-') result = a - b;  // if the operator is a minus symbol, subtract b from a
  else if (op == '*') result = a * b;  // if the operator is a muliplication symbol, multiply inputs a and b
  else if (op == '/') {                // if the operator is a slash symbol, make b the divisor and a the dividend and finish the operation
    if (b == 0) {                      // if the denominator is 0
      lcd.clear();                     // clear the LCD display
      lcd.print("Error: ");            // print "Error: Divide by 0" on the LCD
      lcd.setCursor(0, 1);             // set cursor at second row
      lcd.print("Divide by 0");
      return;  // exit function
    }
    result = a / b;  // the result is the output of a divided by b
  }
  stack.push(result);     // adds the value to the top of the stack
  stackSize++;            // stack size increment
  lcd.clear();            // clear display
  lcd.print("Result: ");  // print "Result: " on LCD
  lcd.print(result, 4);   // print the result to four decimal places
}

void loop() {
  char ch = getKey();  // Get the key pressed
  Serial.println(ch);  // serial print the character

  if ((ch >= '0' && ch <= '9') || ch == '.') {  // checks to see if any of the characters pressed are numbers or a decimal point
    multipleDigits(ch);                         // Handle digit or decimal point input
  } else if (ch == 'E') {                       // if the character is an E (push the current number to the stack)
    pushStack();                                // Push the current number to the stack
  } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
    if (bufferIndex > 0) pushStack();  // Push the current number before performing the operation
    performOperation(ch);              // Perform the operation
  }
}
