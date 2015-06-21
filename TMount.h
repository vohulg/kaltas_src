#ifndef TMOUNT_H
#define TMOUNT_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <QString>


#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif

#ifndef __USE_LARGEFILE64
#define __USE_LARGEFIEL64
#endif

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/mount.h>
#include <sys/mount.h>
#include <iostream>
#include <sys/stat.h>


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;


/* Размер элемента таблицы разделов */
#define PT_SIZE 0x10

/* Флаг загрузочного раздела */
#define BOOTABLE 0x80

#define SIGNATURE 0xAA55
 #define MAX_PART 20


struct systypes {
    char part_type;
    char* part_name;
};


struct pt_struct {
        u8 bootable;
        u8 start_part[3];
        u8 type_part;
        u8 end_part[3];
        u32 sect_before;
        u32 sect_total;
    } ;



class TMount
{

public:
    explicit TMount();
    int info(std::string , std::string );
    int check_sign();
    void read_main_ptable();
    void read_ext_ptable(u64 );
    void pt_info(std::string, std::string);



private:
    int Error;
    systypes i386_sys_types[13];
    int PART_NUM;
    pt_struct pt_t[MAX_PART];
    int hard;
    char mbr[512];


};

#endif // TMOUNT_H
