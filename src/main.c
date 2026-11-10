#include <stdio.h>
#include <math.h>
#include <pigpio.h>

#define PIN 27
#define READING_COUNT 5
#define MIN_TEMP 68.0
#define MAX_TEMP 76.0
#define LOG_FILE "temperature_log.txt"

void lightSwitch(int on){
	gpioWrite(PIN, on);
}

int isOutOfRange(double temperature){
	return temperature < MIN_TEMP || temperature > MAX_TEMP;
}

const char *getStatus(double temperature){
	return isOutOfRange(temperature) ? "WARNING" : "NORMAL";
}

double calculateAverage(double readings[], int count){
	double total = 0;
	for (int i = 0; i < count; i++) total += readings[i];
	return total / count;
}

void displayReadings(double readings[], int count){
	printf("\nTemperature  Status\n");
	for (int i = 0; i < count; i++)
		printf("%.1f F       %s\n", readings[i], getStatus(readings[i]));
}

int saveReadings(double readings[], int count){
	FILE *fp = fopen(LOG_FILE, "w");
	if (fp == NULL){ printf("Could not open %s\n", LOG_FILE); return 0; }
	for (int i = 0; i < count; i++)
		fprintf(fp, "%.1f F %s\n", readings[i], getStatus(readings[i]));
	fprintf(fp, "Average: %.1f F\n", calculateAverage(readings, count));
	return fclose(fp) == 0;
}

int main(void){
	double readings[READING_COUNT];
	int warning = 0;

	if (gpioInitialise() < 0){
		printf("pigpio failed (run with sudo)\n");
		return 1;
	}
	if (gpioSetMode(PIN, PI_OUTPUT) < 0){
		printf("Could not set GPIO %d\n", PIN);
		gpioTerminate();
		return 1;
	}
	lightSwitch(0);

	printf("\nCLASSROOM TEMPERATURE MONITOR (%.0f-%.0f F)\n", MIN_TEMP, MAX_TEMP);

	for (int i = 0; i < READING_COUNT; i++){
		char line[100], extra;
		printf("\nEnter reading %d (F): ", i + 1);
		fflush(stdout);

		while (1){
			if (fgets(line, sizeof(line), stdin) == NULL){
				lightSwitch(0);
				gpioTerminate();
				return 1;
			}
			if (sscanf(line, " %lf %c", &readings[i], &extra) == 1 && isfinite(readings[i])) break;
			printf("Invalid number, try again: ");
			fflush(stdout);
		}
		if (isOutOfRange(readings[i])) warning = 1;
	}

	lightSwitch(warning); 
	displayReadings(readings, READING_COUNT);
	printf("\nAverage temperature: %.1f F\n", calculateAverage(readings, READING_COUNT));
	printf("Warning LED: %s\n", warning ? "ON" : "OFF");
	if (saveReadings(readings, READING_COUNT)) printf("Saved to %s\n", LOG_FILE);

	printf("\nPress Enter to turn off LED and exit...");
	fflush(stdout);
	getchar();
	lightSwitch(0);
	gpioTerminate();
	return 0;
}
