#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define likely(x)  __builtin_expect((x), 1)
#define unlikely(x)  __builtin_expect((x), 0)

// enum of states
typedef enum StateEnum {
    INITIAL,
    RECORDS,
    RECORD,
    KEY,
    NUMBER
} State;


typedef struct RecordStruct {
    double x0;
    double y0;
    double x1;
    double y1;
} Record;


void print_state(State state, char c) {
    if (state == INITIAL) {
        printf("INITIAL: %c\n", c);
    }
    else if (state == RECORDS) {
        printf("RECORDS: %c\n", c);
    }
    else if (state == RECORD) {
        printf("RECORD: %c\n", c);
    }
    else if (state == KEY) {
        printf("KEY: %c\n", c);
    }
    else if (state == NUMBER) {
        printf("NUMBER: %c\n", c);
    }
}


/*
 * A fast float parsing implementation
*/
double fast_atof(const char *p) {
    double out = 0.0;
    int sign = 1;
    int e = 0;
    int c;
    if (*p == '-') {
        p++;
        sign = -1;
    }
    while ((c = *p++) != '\0' && c != '.') {
        if (c != 'e' && c != 'E') {
            out = out * 10.0 + (c - '0');
        }
        else {
            goto exp;
        }
    }
    if (c == '.') {
        while ((c = *p++) != '\0') {
            if (c != 'e' && c != 'E') {
                out = out * 10.0 + (c - '0');
                e = e - 1;
            }
            else {
                goto exp;
            }
        }
    }
exp:
    if (c == 'e' || c == 'E') {
        int exp_sign = 1;
        int i = 0;
        c = *p++;
        if (c == '+')
            c = *p++;
        else if (c == '-') {
            c = *p++;
            exp_sign = -1;
        }
        while (c >= '0' && c <= '9') {
            i = i * 10 + (c - '0');
            c = *p++;
        }
        e += i * exp_sign;
    }
    while (e > 0) {
        out *= 10.0;
        e--;
    }
    while (e < 0) {
        out *= 0.1;
        e++;
    }
    return out * sign;
}


double radians(double degrees)
{
    return degrees * M_PI / 180.0;
}


double HaversineOfDegrees(double X0, double Y0, double X1, double Y1, double R)
{
    double dY = radians(Y1 - Y0);
    double dX = radians(X1 - X0);
    double radY0 = radians(Y0);
    double radY1 = radians(Y1);

    double RootTerm = pow(sin(dY / 2), 2) + cos(radY0) * cos(radY1) * pow(sin(dX / 2), 2);
    double Result = 2 * R * asin(sqrt(RootTerm));
    return Result;
}


void update_record(Record* record, char* key, char* value) {
    //printf("\tCOUNT: %d", record_count);
    //printf("\tKEY: %c%c\n", key[0], key[1]);
    //printf("\tVALUE: %s\n", value);
    double val = fast_atof(value);
    if (key[0] == 'x') {
        if (key[1] == '0') {
            record->x0 = val;
        }
        else if (key[1] == '1') {
            record->x1 = val;
        }
    }
    else if (key[0] == 'y') {
        if (key[1] == '0')
        {
            record->y0 = val;
        }
        else if (key[1] == '1')
        {
            record->y1 = val;
        }
    }
}

int parse_json(Record* records)
{
    // open the json file
    FILE *f = fopen("./data_10000000_flex.json", "r");
    if (f == NULL)
    {
        printf("Error: could not open file\n");
        exit(1);
    }

    int record_count = 0;

    char pos = 0;
    char key[2];
    char value[32];
    State state = INITIAL;
    Record *record = &records[record_count];

    int bytes_read;
    char buffer[32768];
    // read the file in chunks
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0)
    {
        for (int i = 0; i < bytes_read; i++)
        {
            char c = buffer[i];
            if (likely(state == NUMBER))
            {
                if (c >= '0' && c <= '9')
                {
                    value[pos++] = c;
                }
                else if (c == ',')
                {
                    value[pos] = '\0';
                    state = RECORD;
                    update_record(record, key, value);
                }
                else if (c == '}')
                {
                    value[pos] = '\0';
                    update_record(record, key, value);
                    record_count++;
                    state = RECORDS;
                    record = &records[record_count];
                }
                else if (c == '.' || c == 'e' || c == '-')
                {
                    // we're not actually validating the number here,
                    // there could be character in invalid places.
                    value[pos++] = c;
                }
            }
            else if (state == INITIAL)
            {
                if (c == '[')
                {
                    state = RECORDS;
                }
            }
            else if (state == RECORDS)
            {
                if (c == '{')
                {
                    state = RECORD;
                }
                else if (c == ']')
                {
                    break;
                }
            }
            else if (state == RECORD)
            {
                if (c == '"')
                {
                    pos = 0;
                    state = KEY;
                }
                else if (c == '}')
                {
                    state = RECORDS;
                }
            }
            else if (state == KEY)
            {
                if (c == '"')
                {
                    pos = 0;
                    state = NUMBER;
                }
                else
                {
                    key[pos++] = c;
                }
            }

            //print_state(state, c);
        }
    }
    fclose(f);

    return record_count;
}


double calc_haversine_average(Record* records, int record_count) {
    double total = 0;
    for (int i = 0; i < record_count; i++)
    {
        Record *record = &records[i];
        double distance = HaversineOfDegrees(record->x0, record->y0, record->x1, record->y1, 6371.0);
        total += distance;
    }
    return total / record_count;
}


int main(int argc, char** argv) {
    int num_records = 10000000;
    Record *records = (Record *)malloc(num_records * sizeof(Record));

    int t0 = clock();
    int record_count = parse_json(records);
    int t1 = clock();
    double average = calc_haversine_average(records, record_count);
    int t2 = clock();

    free(records);
    double total_elapsed = (t2 - t0) / (double)CLOCKS_PER_SEC;
    printf("# of records: %d\n", record_count);
    printf("Result: %f\n", average);
    printf("Input: %fs\n", (t1 - t0) / (double)CLOCKS_PER_SEC);
    printf("Math: %fs\n", (t2 - t1) / (double)CLOCKS_PER_SEC);
    printf("Total: %fs\n", total_elapsed);
    printf("Throughput: %f %s", record_count / total_elapsed, " haversines/second");
}
