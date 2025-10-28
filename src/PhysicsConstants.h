#ifndef PHYSICS_CONSTANTS_H
#define PHYSICS_CONSTANTS_H

namespace Physics {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    constexpr double DEG_TO_RAD = PI / 180.0;
    constexpr double RAD_TO_DEG = 180.0 / PI;
    
    constexpr double DEFAULT_SAMPLE_RATE = 1000.0; // Hz
    constexpr double DEFAULT_DURATION = 5.0;       // seconds
    
    constexpr double MIN_AMPLITUDE = 0.1;
    constexpr double MAX_AMPLITUDE = 10.0;
    constexpr double MIN_FREQUENCY = 0.1;
    constexpr double MAX_FREQUENCY = 10.0;
    constexpr double MIN_PHASE = 0.0;
    constexpr double MAX_PHASE = 360.0;
}

#endif // PHYSICS_CONSTANTS_H