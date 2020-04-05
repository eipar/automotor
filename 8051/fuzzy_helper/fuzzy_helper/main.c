#include <stdio.h>
#include <stdlib.h>

int main(void) {
    unsigned char val[4], output[4], ans, flag;
    do {
        printf("\n\n Enter 4 hex points: ");
        scanf(" %x %x %x %x", &val[0], &val[1], &val[2], &val[3]);
        output[0]=val[0];
        output[2]=val[2];
        if (val[1]-val[0]) {
            output[1]=(0xFF+((val[1]-val[0])/2))/(val[1]-val[0]);
        } else {
            output[1]=0;
        }
        if (val[3]-val[2]) {
            output[3]=(0xFF+((val[3]-val[2])/2))/(val[3]-val[2]);
        } else {
            output[3]=0x00;
        }
        printf("\n The point-slope values are: %02X %02X %02X %02X\n\n",output[0], output[1], output[2], output[3]);
        do {
            flag=1;
            printf("run another set of numbers? ");
            while (!kbhit());
            ans=getch();
            if (ans!='y' && ans!='Y' && ans!='n' && ans!='N') {
                flag=0;
                printf("\nhuh?\n");
            }
        } while (!flag);
    } while (ans=='y' || ans=='Y');
    printf("\n later, hosehead!\n");
    return 0;
}
