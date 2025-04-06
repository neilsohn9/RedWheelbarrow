 1. #include <iostream>
 2. #include <cstdlib>
 3. #include <ctime>
 4. #include <string>
 5. #include <thread>
 6. #include <chrono>
 7. 
 8. using namespace std; // Add this to avoid using std:: prefix everywhere
 9. 
10. // Patient data class to encapsulate patient information
11. class Patient {
12. private:
13.     float currentGlucose;    // Current glucose level (mg/dL)
14.     float insulinDosed;      // Amount of insulin dosed (units)
15.     int lastMealCarbs;       // Carbohydrates from last meal (grams)
16.     int activityLevel;       // Activity level (0-100)
17.     time_t lastCheck;        // Time of last check
18. 
19.     // Private helper to generate random float within range
20.     float randomFloat(float min, float max) {
21.         return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
22.     }
23. 
24. public:
25.     Patient() : currentGlucose(0), insulinDosed(0), lastMealCarbs(0), activityLevel(0), lastCheck(0) {
26.         cout << "Patient object created successfully at line 9.\n"; // Debug output
27.     }
28. 
29.     // Simulate glucose reading
30.     void updateGlucose() {
31.         currentGlucose = randomFloat(70.0f, 270.0f); // Random glucose between 70 and 270 mg/dL
32.         lastMealCarbs = rand() % 100;                // Random carbs (0-99g)
33.         activityLevel = rand() % 101;                // Random activity (0-100)
34.         lastCheck = time(nullptr);
35.     }
36. 
37.     // Calculate required insulin dose
38.     float calculateDose() const {
39.         float baseDose = (currentGlucose - 100.0f) / 50.0f; // Base dose based on deviation from 100
40.         float carbFactor = lastMealCarbs * 0.1f;            // 0.1 units per gram of carbs
41.         float activityAdjustment = activityLevel * 0.01f;    // Reduce dose if active
42. 
43.         float totalDose = baseDose + carbFactor - activityAdjustment;
44.         return (totalDose > 0) ? totalDose : 0.0f; // Ensure no negative dose
45.     }
46. 
47.     // Check if glucose is in danger zone
48.     bool isEmergency() const {
49.         return (currentGlucose < 70.0f || currentGlucose > 250.0f); // Hypo (<70) or Hyper (>250)
50.     }
51. 
52.     // Getters
53.     float getGlucose() const { return currentGlucose; }
54.     int getCarbs() const { return lastMealCarbs; }
55.     int getActivity() const { return activityLevel; }
56.     time_t getLastCheck() const { return lastCheck; }
57.     float getInsulinDosed() const { return insulinDosed; }
58. 
59.     // Setters
60.     void setInsulinDosed(float dose) {
61.         cout << "Debug: Setting insulin dose to: " << dose << " units\n";
62.         if (dose < 0 || dose > 50) { // Safety check
63.             cout << "WARNING: Invalid insulin dose " << dose << " units. Setting to 0.\n";
64.             insulinDosed = 0;
65.         } else {
66.             insulinDosed = dose;
67.         }
68.     }
69. 
70.     // Display current status
71.     void displayStatus() const {
72.         char timeStr[26];
73.         ctime_r(&lastCheck, timeStr);
74.         timeStr[24] = '\0'; // Remove newline
75. 
76.         cout << "Time: " << timeStr << endl;
77.         cout << "Glucose Level: " << currentGlucose << " mg/dL" << endl;
78.         cout << "Carbs Consumed: " << lastMealCarbs << " grams" << endl;
79.         cout << "Activity Level: " << activityLevel << "%" << endl;
80.         cout << "Current Insulin Dosed: " << insulinDosed << " units" << endl; // Add this for tracking
81.     }
82. };
83. 
84. // Alert system class
85. class AlertSystem {
86. public:
87.     static void sendAlert(const Patient& patient) {
88.         if (patient.isEmergency()) {
89.             cout << "CRITICAL ALERT: Glucose level " << patient.getGlucose() 
90.                      << " mg/dL is dangerous! Contact healthcare provider immediately!\n";
91.         } else if (patient.getGlucose() > 180.0f) {
92.             cout << "WARNING: High glucose (" << patient.getGlucose() 
93.                      << " mg/dL). Monitor closely.\n";
94.         } else if (patient.getGlucose() < 80.0f) {
95.             cout << "WARNING: Low glucose (" << patient.getGlucose() 
96.                      << " mg/dL). Consider a snack.\n";
97.         }
98.     }
99. };
100. 
101. // Main simulation class
102. class InsulinSmartSystem {
103. private:
104.     Patient patient;
105. 
106. public:
107.     void runSimulation(int hours) {
108.         cout << "InsulinSmart System Simulation Started\n";
109.         cout << "------------------------------------\n";
110. 
111.         for (int hour = 0; hour < hours; hour++) {
112.             patient.updateGlucose();
113.             patient.displayStatus();
114. 
115.             float dose = patient.calculateDose();
116.             patient.setInsulinDosed(dose);
117. 
118.             cout << "Suggested Insulin Dose: " << dose << " units\n";
119.             AlertSystem::sendAlert(patient);
120. 
121.             cout << "----------------------------------------\n";
122. 
123.             // Simulate delay (1 second for demo)
124.             this_thread::sleep_for(chrono::seconds(1));
125.         }
126. 
127.         cout << "\nSimulation Complete. Review data with healthcare provider.\n";
128.     }
129. };
130. 
131. int main() {
132.     srand(static_cast<unsigned>(time(nullptr))); // Seed random number generator
133. 
134.     InsulinSmartSystem system;
135.     system.runSimulation(24); // Simulate 24 hours
136. 
137.     return 0;
138. }