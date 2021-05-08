#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int PAGE_SIZE = 256;
const int TABLE_SIZE = 16;

int main(int argc, char* argv[]) {

    char *value = NULL;
	size_t len = 0;
	size_t read;
	long long page_number, offset, page_table, totalHits = 0, fault = 0, pages = 0;
	
	int position = 0, physical = 0, frame, logical;
	
	int table[TABLE_SIZE][2];
	int pageTable[PAGE_SIZE];

    FILE *fileDescriptor;

    if (argc < 2) {
        printf("Missing arguments...Exiting\n");
        return 0;
    }

    fileDescriptor = fopen(argv[1], "r");
    if (fileDescriptor == NULL) {
        printf("Cannot open file\n");
        return 0;
    }

    memset(table,-1,TABLE_SIZE*2*sizeof(table[0][0]));
	memset(pageTable,-1,sizeof(pageTable));

    int mask = 255, hit, i;

    while((read = getline(&value, &len, fileDescriptor)) != -1) {
        pages++;
        page_number = atoi(value);
        page_number = page_number >> 8;
        page_number = page_number & mask;

        offset = atoi(value);
        offset = offset & mask;

        logical = atoi(value);
        frame = 0, physical = 0;

        hit = 0;

        for (i = 0; i < TABLE_SIZE; ++i) {
            if (table[i][0] == page_number) {
                hit = 1;
                totalHits++;
                frame = table[i][1];
                break;
            }
        }

        if (hit == 1) {
            printf("Table Hit Confirmed\n");
        }

        else {
            int x = 0;
            for (i = 0; i < PAGE_SIZE; ++i) {
                if(pageTable[i] == page_number) {
                    frame = i;
                    fault++;
                    break;
                }
            }

            if (x == i) {
                pageTable[i] = page_number;
                frame = i;
            }

            table[position][0] = page_number;
            table[position][1] = i;
            position++;
            position = position%15;
        }
        if(logical < 10000) {
            printf("Virtual Address = %d \t\t\t", logical);
        }
        else {
            printf("Virtual Address = %d \t\t", logical);

            physical = frame * PAGE_SIZE + offset;
            printf("Physical Address = %d\n", physical);
        }

        double hitRate = (double)totalHits/pages*100;
        double faultRate = (double)fault/pages*100;

        printf("\nTable Hit Rate = %.2f %c", hitRate, '%');
        printf("\nTable Miss Rate = %.2f %c",(100 - hitRate),'%');
	    printf("\nPage Table Hit Rate = %.2f %c", faultRate,'%');
	    printf("\nPage Table Miss Rate = %.2f %c\n", (100 - faultRate),'%');  
    }
}