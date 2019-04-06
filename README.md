# TM1637 4 Digit LED Display
Full support of TM1637 4 digit LED display module, including functions for displaying numbers. One can inherit from base class to add keypad reading functions.
* Includes functions for displaying numbers (int, positive and negative)
* Includes a demo sketch to show how you can use this library
* Does not use delays in communication

Important things:
* You can add pull-up resistors if needed
* If yours module seems not working, make sure that pull-up resistors are placed correctly (if they are present on module), also make sure that filtering capacitors are correct - they should be 100pF. In my case two 10nF were installed, but module worked correctly.

### Only for non-commercial use.
