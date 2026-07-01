#include "application.h"

int main() {
    Application app;
    app.init();
    app.run(); // Main loop
    app.stop();
    return 0;
}