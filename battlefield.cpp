// main.cpp
#include "Battlefield.h.cpp"

int main() {
    // Create a 10x5 battlefield
    Battlefield battlefield(10, 5);

    // Place some robots
    battlefield.placeRobot("Alpha", 2, 2);
    battlefield.placeRobot("Beta", 5, 1);
    battlefield.placeRobot("Gamma", 7, 4);

    // Display the battlefield
    battlefield.display();

    return 0;
}
