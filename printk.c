#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *strtokstr_r(char *s, char *delim, char **save_ptr){
    char *end;
    if (s == NULL)
        s = *save_ptr;

    if (s == NULL || *s == '\0'){
        *save_ptr = s;
        return NULL;
    }

    while(strstr(s,delim)==s)
        s += strlen(delim);
    if (*s == '\0'){
        *save_ptr = s;
        return NULL;
    }

    end = strstr (s, delim);
    if (end == NULL){
        *save_ptr = s + strlen(s);
        return s;
    }

    memset(end, 0, strlen(delim));
    *save_ptr = end + strlen(delim);
    return s;
}

int main(int argc, char **argv){
    FILE *dev, *proc;
    bool inserted;
    char line[256];
    char *str;

    proc = fopen("/proc/modules", "r");
    if(!proc){
        perror("/proc/modules");
        return 1;
    }
    inserted = false;
    while(fgets(line, sizeof(line), proc)){
        if(strstr(line, "devprintk") != NULL){
            inserted = true;
            break;
        }
    }
    if(!inserted){
        fprintf(stderr, "devprintk module is not inserted\n");
        fclose(proc);
        return 1;
    }

    dev = fopen("/dev/printk", "w");
    if(!dev){
        perror("/dev/printk");
        fclose(proc);
        return 1;
    }

    for(int i = 1; i < argc; i++){
       while((str = strtokstr_r(argv[i], "\\n", &argv[i])) != NULL){
        fprintf(dev, "%s", str);
        fflush(dev);
      }
}


    fclose(dev);
    fclose(proc);
    return 0;
}
