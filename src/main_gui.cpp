#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <iostream>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Wave Simulator");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Physics Education Tools");
    app.setOrganizationDomain("waveslab.edu");
    
    try {
        // Create and show main window
        MainWindow window;
        window.show();
        
        std::cout << "🌊 Wave Simulator GUI Started Successfully!" << std::endl;
        std::cout << "Enjoy exploring the fascinating world of waves!" << std::endl;
        
        return app.exec();
        
    } catch (const std::exception& e) {
        std::cerr << "Error starting Wave Simulator GUI: " << e.what() << std::endl;
        return 1;
    }
}