#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Structure to store patient data
typedef struct {
    float currentGlucose;    // Current glucose level (mg/dL)
    float insulinDosed;      // Amount of insulin dosed (units)
    int lastMealCarbs;       // Carbohydrates from last meal (grams)
    int activityLevel;       // Activity level (0-100)
    time_t lastCheck;        // Time of last check
} PatientData;

// Function to simulate glucose reading (random for demonstration)
float getGlucoseReading() {
    return 70 + (rand() % 200); // Random glucose between 70 and 270 mg/dL
}

// Function to calculate required insulin dose
float calculateInsulinDose(float glucose, int carbs, int activity) {
    float baseDose = (glucose - 100) / 50; // Base dose based on glucose deviation from 100
    float carbFactor = carbs * 0.1;        // 0.1 units per gram of carbs
    float activityAdjustment = activity * 0.01; // Reduce dose if active

    float totalDose = baseDose + carbFactor - activityAdjustment;
    return (totalDose > 0) ? totalDose : 0; // Ensure no negative dose
}

// Function to check if glucose is in danger zone
int isEmergency(float glucose) {
    return (glucose < 70 || glucose > 250); // Hypo (<70) or Hyper (>250)
}

// Function to simulate alert system
void sendAlert(float glucose, time_t time) {
    char timeStr[26];
    ctime_r(&time, timeStr);
    timeStr[24] = '\0'; // Remove newline

    if (isEmergency(glucose)) {
        printf("ALERT at %s: Glucose level %.2f mg/dL is critical! Contact healthcare provider.\n", timeStr, glucose);
    } else if (glucose > 180) {
        printf("WARNING at %s: High glucose (%.2f mg/dL). Monitor closely.\n", timeStr, glucose);
    } else if (glucose < 80) {
        printf("WARNING at %s: Low glucose (%.2f mg/dL). Consider a snack.\n", timeStr, glucose);
    }
}

// Main function to simulate the system
int main() {
    PatientData patient = {0};
    srand(time(NULL)); // Seed random number generator

    printf("InsulinSmart System Simulation Started\n");
    printf("------------------------------------\n");

    // Simulate 24 hours (24 checks, one per hour)
    for (int hour = 0; hour < 24; hour++) {
        patient.currentGlucose = getGlucoseReading();
        patient.lastMealCarbs = rand() % 100; // Random carbs (0-99g)
        patient.activityLevel = rand() % 101; // Random activity (0-100)
        patient.lastCheck = time(NULL);

        printf("\nHour %d:\n", hour + 1);
        printf("Current Glucose: %.2f mg/dL\n", patient.currentGlucose);
        printf("Carbs Consumed: %d grams\n", patient.lastMealCarbs);
        printf("Activity Level: %d%%\n", patient.activityLevel);

        // Calculate and suggest insulin dose
        patient.insulinDosed = calculateInsulinDose(patient.currentGlucose, patient.lastMealCarbs, patient.activityLevel);
        printf("Suggested Insulin Dose: %.2f units\n", patient.insulinDosed);

        // Check for alerts
        sendAlert(patient.currentGlucose, patient.lastCheck);

        // Simulate delay (1 second for demo)
        //sleep(1); // Note: sleep() might need <unistd.h> on Unix systems
    }

    printf("\nSimulation Complete. Review data with healthcare provider.\n");
    return 0;
}