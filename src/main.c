// Karimou Djibo 1002280646
#include <stdio.h>

// Function to Determine if the temperature is within the acceptable range (68F - 76F)
int temperature_check(int temperature) {
    if (temperature >= 68 && temperature <= 76) {
        printf("Status: NORMAL, the temperature is within the acceptable range.\n");
        return 0;
    }
    else {
        printf("Status: WARNING, the temperature is NOT within the acceptable range.\n");
    }   return 1;
}

int main() {
    int temperature;
    int current_number;
    int status_code;
    int total_sum = 0;
    int count = 0;
    int warning = 0;
    FILE *fptr = fopen("temperature_log.csv", "a");

    printf("Hello, welcome to the Smart Classroom Comfort Monitor (SCCM)\n");
    printf("Please enter the current temperature, ensure you only use whole numerical values: ");
    scanf(" %d", &temperature);
    
    if (fptr == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    // Add the warning status and write to the temperature log, 1 means warning, 0 means normal
    if (temperature_check(temperature)) {
        warning = 1;
        fprintf(fptr, "%d 1\n", temperature);
    } else {
        warning = 0;
        fprintf(fptr, "%d 0\n", temperature);
    }
    printf("Temperature and status logged successfully.\n");
    fclose(fptr);

    FILE *file = fopen("temperature_log.csv", "r");

    
    // scan temperature_log.csv for the temperature number and status 
    printf("Temperature \t System status \t Warning LED\n");
    while (fscanf(file, "%d %d", &current_number, &status_code) == 2) {
        if (status_code == 0) {
            printf("%10dF \t NORMAL \t     OFF\n", current_number);
        }
        else {
            printf("%10dF \t WARNING \t     ON\n", current_number);
        }
        // add the temperature number to total_sum and increase the count for the divisor when caculating the average
        total_sum += current_number;
        count++;
    }
    int average = total_sum / count;
    printf("The current whole number average of the recorded temperatures is %dF\n", average);
    fclose(file);
    return 0;
}
